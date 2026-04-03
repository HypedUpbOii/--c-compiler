%require "3.2"
%language "c++"

%define api.namespace {parser}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.constructor

%locations

%code requires {
    #include <iostream>
    #include <stack>
    #include "common_utils.hpp"
    #include "ast.hpp"
    #include "symbol_table.hpp"
    class Lexer;
}

%code {
    #include "lexer.hpp"
    static parser::Parser::symbol_type
    yylex(Lexer& lexer, Program& ast) {
        return lexer.nextToken();
    }

    std::string scope_name;
    std::stack<SymbolTable*> symbolTableStack;
}

%param { Lexer& lexer }
%param { Program& ast }

%token GREATER_THAN_EQUAL
%token LESS_THAN_EQUAL
%token EQUAL
%token NOT_EQUAL
%token AND
%token OR
%token GREATER_THAN
%token LESS_THAN
%token QUESTION_MARK
%token COLON
%token NOT
%token LEFT_ROUND_BRACKET
%token RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET
%token RIGHT_CURLY_BRACKET
%token LEFT_SQUARE_BRACKET
%token RIGHT_SQUARE_BRACKET
%token ADDRESSOF
%token COMMA
%token SEMICOLON
%token ASSIGN_OP
%token MINUS
%token MULT
%token PLUS
%token DIV
%token INTEGER
%token BOOL
%token VOID
%token STRING
%token FLOAT
%token WRITE
%token READ
%token RETURN
%token IF
%token ELSE
%token WHILE
%token DO
%token <double> FLOAT_NUM
%token <int> INT_NUM
%token <std::string> NAME
%token <std::string> STR_CONST
%token ERROR

%type <DataType> param_type
%type <DataType> named_type
%type <std::pair<DataType, std::string>> func_header
%type <std::pair<std::string, DataType>> formal_param
%type <std::vector<std::pair<std::string, DataType>>> formal_param_list

%type <std::unique_ptr<Assignment_Stmt_Ast>> assignment_statement
%type <std::unique_ptr<While_Stmt_Ast>> while_statement
%type <std::unique_ptr<Do_While_Stmt_Ast>> do_while_statement
%type <std::unique_ptr<Read_Stmt_Ast>> read_statement
%type <std::unique_ptr<Return_Stmt_Ast>> return_statement
%type <std::unique_ptr<Selection_Stmt_Ast>> if_statement
%type <std::unique_ptr<Sequence_Stmt_Ast>> compound_statement
%type <std::unique_ptr<Write_Stmt_Ast>> print_statement
%type <std::unique_ptr<Call_Stmt_Ast>> call_statement
%type <std::unique_ptr<Statement_Ast>> statement
%type <std::vector<std::unique_ptr<Statement_Ast>>> statement_list

%type <std::unique_ptr<Expression_Ast>> expression
%type <std::unique_ptr<Expression_Ast>> if_condition
%type <std::unique_ptr<Relational_Expr_Ast>> rel_expression
%type <std::unique_ptr<Expression_Ast>> variable_as_operand
%type <std::unique_ptr<Base_Expr_Ast>> constant_as_operand
%type <std::unique_ptr<Function_Call_Ast>> func_call
%type <std::string> variable_name
%type <std::vector<std::unique_ptr<Expression_Ast>>> array_dimensions
%type <std::pair<std::string, std::vector<std::unique_ptr<Expression_Ast>>>> array_access
%type <std::pair<std::string, int>> pointer_access

%type <std::vector<int>> array_decl
%type <int> pointer_decl
%type <std::pair<std::string, DataType>> var_decl_item
%type <std::vector<std::pair<std::string, DataType>>> var_decl_item_list
%type <std::pair<DataType, std::vector<std::string>>> var_decl_stmt
%type <std::unique_ptr<Expression_Ast>> actual_arg
%type <std::vector<std::unique_ptr<Expression_Ast>>> non_empty_arg_list
%type <std::vector<std::unique_ptr<Expression_Ast>>> actual_arg_list
%type <std::unique_ptr<Function_Ast>> func_def
%type <std::vector<std::unique_ptr<Function_Ast>>> func_def_list

%nonassoc THEN
%nonassoc ELSE
%nonassoc ASSIGN_OP
%right QUESTION_MARK COLON
%left OR
%left AND
%right NOT
%nonassoc EQUAL NOT_EQUAL GREATER_THAN_EQUAL LESS_THAN_EQUAL GREATER_THAN LESS_THAN
%left PLUS MINUS
%left MULT DIV
%right UMINUS

%initial-action {
    symbolTableStack = std::stack<SymbolTable*>();
    symbolTableStack.push(ast.global);

    scope_name = "";
}

%%
program
    : global_decl_statement_list func_def_list {
        ast.addFunctions(std::move($2));
    }
    | func_def_list {
        ast.addFunctions(std::move($1));
    }
;

global_decl_statement_list
    : global_decl_statement_list func_decl
    | global_decl_statement_list var_decl_stmt {
        ast.addGlobal($2);
    }
    | func_decl
    | var_decl_stmt {
        ast.addGlobal($1);
    }
;

func_decl
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET SEMICOLON {
        std::vector<DataType> param_types = std::vector<DataType>();
        for (auto [s, t] : $3) {
            param_types.push_back(t);
        }
        symbolTableStack.top()->insert_func($1.second, $1.first, param_types);
        std::string name = ($1.second == "main") ? "main" : $1.second + "_";
        ast.addFuncDef(name, $1.first);
    }
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON {
        symbolTableStack.top()->insert_func($1.second, $1.first, std::vector<DataType>());
        std::string name = ($1.second == "main") ? "main" : $1.second + "_";
        ast.addFuncDef(name, $1.first);
    }
;

func_def_list
    : func_def_list func_def {
        ast.addFuncDef($2->name, $2->returnType);
        $$ = std::move($1);
        $$.push_back(std::move($2));
    }
    | func_def {
        ast.addFuncDef($1->name, $1->returnType);
        $$ = std::vector<std::unique_ptr<Function_Ast>>();
        $$.push_back(std::move($1));
    }
;

func_header
    : named_type NAME {
        $$ = std::make_pair($1, $2);
    }
;

func_def
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET {
        scope_name = $1.second;
        SymbolTable* local = new SymbolTable(symbolTableStack.top(), $1.first);
        std::vector<DataType> sub_signature;
        for (const auto & ptr : $3) {
            sub_signature.push_back(ptr.second);
            local->insert(ptr.first, ptr.second, true);    
        }
        symbolTableStack.top()->insert_func($1.second, $1.first, sub_signature, true);
        symbolTableStack.push(local);
    } optional_local_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET {
        $$ = std::make_unique<Function_Ast>($1.first, $3, $1.second, std::move($8), symbolTableStack.top());
        symbolTableStack.pop();
    }
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET {
        scope_name = $1.second;
        SymbolTable* local = new SymbolTable(symbolTableStack.top());
        symbolTableStack.top()->insert_func($1.second, $1.first, std::vector<DataType>(), true);
        symbolTableStack.push(local);
    } optional_local_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET {
        $$ = std::make_unique<Function_Ast>($1.first, std::vector<std::pair<std::string, DataType>>(), $1.second, std::move($7), symbolTableStack.top());
        symbolTableStack.pop();
    }
;

formal_param_list
    : formal_param_list COMMA formal_param {
        $$ = std::move($1);
        $$.push_back($3);
    }
    | formal_param {
        $$ = std::vector<std::pair<std::string, DataType>>({ $1 });
    }
;

formal_param
    : param_type NAME {
        $$ = std::make_pair($2, $1);
    }
;

param_type
    : INTEGER   { $$ = DataType(BaseType::INT); }
    | FLOAT     { $$ = DataType(BaseType::FLOAT); }
    | BOOL      { $$ = DataType(BaseType::BOOL); }
    | STRING    { $$ = DataType(BaseType::STRING); }
;

statement_list
    : statement_list statement {
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | %empty {
        $$ = std::vector<std::unique_ptr<Statement_Ast>>();
    }
;

statement
    : assignment_statement {
        $$ = std::move($1);
    }
    | if_statement {
        $$ = std::move($1);    
    }
    | do_while_statement {
        $$ = std::move($1);
    }
    | while_statement {
        $$ = std::move($1);
    }
    | compound_statement {
        $$ = std::move($1);
    }
    | print_statement {
        $$ = std::move($1);
    }
    | read_statement {
        $$ = std::move($1);
    }
    | call_statement {
        $$ = std::move($1);
    }
    | return_statement {
        $$ = std::move($1);
    }
;

call_statement
    : func_call SEMICOLON {
        $$ = std::make_unique<Call_Stmt_Ast>(std::move($1));
    }
;

func_call
    : NAME LEFT_ROUND_BRACKET actual_arg_list RIGHT_ROUND_BRACKET {
        $$ = std::make_unique<Function_Call_Ast>($1, std::move($3));
    }
;

actual_arg_list
    : non_empty_arg_list {
        $$ = std::move($1);
    }
    | %empty {
        $$ = std::vector<std::unique_ptr<Expression_Ast>>();
    }
;

non_empty_arg_list
    : non_empty_arg_list COMMA actual_arg {
        $$ = std::move($1);
        $$.push_back(std::move($3));
    }
    | actual_arg {
        $$ = std::vector<std::unique_ptr<Expression_Ast>>();
        $$.push_back(std::move($1));
    }
;

actual_arg
    : expression {
        $$ = std::move($1);
    }
;

return_statement
    : RETURN expression SEMICOLON {
        $$ = std::make_unique<Return_Stmt_Ast>(std::move($2), scope_name);
    }
;

optional_local_var_decl_stmt_list
    : %empty
    | var_decl_stmt_list
;

var_decl_stmt_list
    : var_decl_stmt_list var_decl_stmt
    | var_decl_stmt
;

var_decl_stmt
    : named_type var_decl_item_list SEMICOLON {
        std::vector<std::string> names;
        for (const auto & var : $2){
            DataType dt = var.second;
            dt.base = $1.base;
            symbolTableStack.top()->insert(var.first, dt);
            names.push_back(var.first);
        }
        $$ = std::make_pair($1, names);
    }
;

var_decl_item_list
    : var_decl_item_list COMMA var_decl_item {
        $$ = std::move($1);
        $$.push_back($3);
    }
    | var_decl_item { 
        $$ = std::vector<std::pair<std::string, DataType>>({ $1 });
    }
;

var_decl_item
    : NAME {
        $$ = std::make_pair($1, DataType());
    }
    | NAME array_decl {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::make_pair($1, DataType(BaseType::VOID, 0, $2));
    }
    | pointer_decl NAME {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::make_pair($2, DataType(BaseType::VOID, $1));
    }
;

pointer_decl
    : MULT {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = 1;
    }
    | MULT pointer_decl {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = $2 + 1;
    }
;

array_decl
    : LEFT_SQUARE_BRACKET INT_NUM RIGHT_SQUARE_BRACKET {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::vector<int>({ $2 });
    }
    | LEFT_SQUARE_BRACKET INT_NUM RIGHT_SQUARE_BRACKET array_decl {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::move($4);
        // $$.push_back($2);
    }
;

named_type
    : INTEGER   { $$ = DataType(BaseType::INT); }
    | FLOAT     { $$ = DataType(BaseType::FLOAT); }
    | VOID      { $$ = DataType(BaseType::VOID); }
    | STRING    { $$ = DataType(BaseType::STRING); }
    | BOOL      { $$ = DataType(BaseType::BOOL); }
;

assignment_statement
    : variable_as_operand ASSIGN_OP expression SEMICOLON {
        $$ = std::make_unique<Assignment_Stmt_Ast>(std::move($1), std::move($3));
    }
    | variable_as_operand ASSIGN_OP func_call SEMICOLON {
        $$ = std::make_unique<Assignment_Stmt_Ast>(std::move($1), std::move($3));
    }
    | variable_as_operand ASSIGN_OP ADDRESSOF variable_name SEMICOLON {
        exit_with_err_msg("sclp error: L6 feature");
        // std::unique_ptr<Address_Expr_Ast> temp = std::make_unique<Address_Expr_Ast>(std::make_unique<Name_Expr_Ast>($4);
        // $$ = std::make_unique<Assignment_Stmt_Ast>(std::move($1), std::move(temp));
    }
;

if_condition
    : LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET {
        $$ = std::move($2);
    }
;

if_statement
    : IF if_condition statement ELSE statement {
        $$ = std::make_unique<Selection_Stmt_Ast>(std::move($2), std::move($3), std::move($5));
    }
    | IF if_condition statement %prec THEN {
        $$ = std::make_unique<Selection_Stmt_Ast>(std::move($2), std::move($3), nullptr);
    }
;

do_while_statement
    : DO statement WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET SEMICOLON {
        $$ = std::make_unique<Do_While_Stmt_Ast>(std::move($5), std::move($2));
    }
;

while_statement
    : WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement {
        $$ = std::make_unique<While_Stmt_Ast>(std::move($3), std::move($5));
    }
;

compound_statement
    : LEFT_CURLY_BRACKET statement_list RIGHT_CURLY_BRACKET {
        $$ = std::make_unique<Sequence_Stmt_Ast>($2);
    }
;

print_statement
    : WRITE expression SEMICOLON {
        $$ = std::make_unique<Write_Stmt_Ast>(std::move($2));
    }
;

read_statement
    : READ NAME SEMICOLON {
        $$ = std::make_unique<Read_Stmt_Ast>(std::move(std::make_unique<Name_Expr_Ast>($2)));
    }
;

expression
    : expression PLUS expression {
        $$ = std::make_unique<Arithmetic_Expr_Ast>(std::move($1), ArithmeticOperator::PLUS, std::move($3));
    }
    | expression MINUS expression {
        $$ = std::make_unique<Arithmetic_Expr_Ast>(std::move($1), ArithmeticOperator::MINUS, std::move($3));
    }
    | expression MULT expression {
        $$ = std::make_unique<Arithmetic_Expr_Ast>(std::move($1), ArithmeticOperator::MULT, std::move($3));
    }
    | expression DIV expression {
        $$ = std::make_unique<Arithmetic_Expr_Ast>(std::move($1), ArithmeticOperator::DIVIDE, std::move($3));
    }
    | MINUS expression %prec UMINUS {
        $$ = std::make_unique<UMinus_Expr_Ast>(std::move($2));
    }
    | LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET {
        $$ = std::move($2);
    }
    | expression QUESTION_MARK expression COLON expression {
        $$ = std::make_unique<Ternary_Expr_Ast>(std::move($1), std::move($3), std::move($5));
    }
    | expression AND expression {
        $$ = std::make_unique<Boolean_Expr_Ast>(std::move($1), BooleanOperator::AND, std::move($3));
    }
    | expression OR expression {
        $$ = std::make_unique<Boolean_Expr_Ast>(std::move($1), BooleanOperator::OR, std::move($3));
    }
    | NOT expression {
        $$ = std::make_unique<Not_Expr_Ast>(std::move($2));
    }
    | rel_expression {
        $$ = std::move($1);
    }
    | variable_as_operand {
        $$ = std::move($1);
    }
    | constant_as_operand {
        $$ = std::move($1);
    }
;

rel_expression
    : expression LESS_THAN expression {
        $$ = std::make_unique<Relational_Expr_Ast>(std::move($1), RelationalOperator::LESS_THAN, std::move($3));
    }
    | expression LESS_THAN_EQUAL expression {
        $$ = std::make_unique<Relational_Expr_Ast>(std::move($1), RelationalOperator::LESS_THAN_EQUAL, std::move($3));
    }
    | expression GREATER_THAN expression {
        $$ = std::make_unique<Relational_Expr_Ast>(std::move($1), RelationalOperator::GREATER_THAN, std::move($3));
    }
    | expression GREATER_THAN_EQUAL expression {
        $$ = std::make_unique<Relational_Expr_Ast>(std::move($1), RelationalOperator::GREATER_THAN_EQUAL, std::move($3));
    }
    | expression NOT_EQUAL expression {
        $$ = std::make_unique<Relational_Expr_Ast>(std::move($1), RelationalOperator::NOT_EQUAL, std::move($3));
    }
    | expression EQUAL expression {
        $$ = std::make_unique<Relational_Expr_Ast>(std::move($1), RelationalOperator::EQUAL, std::move($3));
    }
;

variable_as_operand
    : variable_name {
        $$ = std::make_unique<Name_Expr_Ast>($1);
    }
    | array_access {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::make_unique<Array_Access_Expr_Ast>($1.first, $1.second);
    }
    | pointer_access {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::make_unique<Pointer_Deref_Expr_Ast>($1.first, $1.second);
    }
;

variable_name
    : NAME {
        $$ = $1;
    }
;

array_access
    : variable_name array_dimensions {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::make_pair($1, $2);
    }
;

pointer_access
    : MULT variable_name {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::make_pair($2, 1);
    }
    | MULT pointer_access {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::make_pair($2.first, $2.second + 1);
    }
;

array_dimensions
    : LEFT_SQUARE_BRACKET expression RIGHT_SQUARE_BRACKET {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::vector<std::unique_ptr<Expression_Ast>>({ std::move($2) });
    }
    | array_dimensions LEFT_SQUARE_BRACKET expression RIGHT_SQUARE_BRACKET {
        exit_with_err_msg("sclp error: L6 feature");
        // $$ = std::move($1);
        // $$.push_back(std::move($3));
    }
;

constant_as_operand
    : INT_NUM {
        $$ = std::make_unique<Literal_Expr_Ast<int>>($1);
    }
    | FLOAT_NUM {
        $$ = std::make_unique<Literal_Expr_Ast<double>>($1);
    }
    | STR_CONST {
        $$ = std::make_unique<Literal_Expr_Ast<std::string>>($1);
        ast.addStringConst($1);
    }
;
%%
