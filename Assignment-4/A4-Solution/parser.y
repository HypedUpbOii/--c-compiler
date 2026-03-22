%require "3.2"
%language "c++"

%define api.namespace {parser}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.constructor

%locations

%code requires {
    #include <iostream>
    #include "common_utils.hpp"
    #include "ast.hpp"
    #include "symbol_table.hpp"
    class Lexer;
}

%code {
    #include "lexer.hpp"
    static parser::Parser::symbol_type
    yylex(Lexer& lexer, ProgramNode& ast) {
        return lexer.nextToken();
    }

    SymbolTable* global_sym_tab = new SymbolTable();
    SymbolTable* local_sym_tab = new SymbolTable(global_sym_tab);
}

%param { Lexer& lexer }
%param { ProgramNode& ast }

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

%nonassoc ASSIGN_OP
%right QUESTION_MARK COLON
%left OR
%left AND
%right NOT
%nonassoc EQUAL NOT_EQUAL GREATER_THAN_EQUAL LESS_THAN_EQUAL GREATER_THAN LESS_THAN
%left PLUS MINUS
%left MULT DIV
%right UMINUS

%%
program
    : global_decl_statement_list func_def_list
    | func_def_list
;

global_decl_statement_list
    : global_decl_statement_list func_decl
    | global_decl_statement_list var_decl_stmt
    | func_decl
    | var_decl_stmt
;

func_decl
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET SEMICOLON {
        std::vector<DataType> param_types = std::vector<DataType>();
        for (auto [s, t] : $3) {
            param_types.push_back(t);
        }
        $$ = std::make_tuple($1.first, $1.second, param_types);
    }
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON {
        $$ = std::make_tuple($1.first, $1.second, std::vector<DataType>());
    }
;

func_def_list
    : func_def_list func_def
    | func_def
;

func_header
    : named_type NAME {
        $$ = std::make_pair($1, $2);
    }
;

func_def
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_local_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET {
        $$ = std::make_unique<FunctionNode>($1.first, $3, $1.second, std::move($7), local_sym_tab);
        local_sym_tab = new SymbolTable(global_sym_tab);
    }
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_local_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET {
        $$ = std::make_unique<FunctionNode>($1.first, std::vector<std::pair<std::string, DataType>>(), $1.second, std::move($6), local_sym_tab);
        local_sym_tab = new SymbolTable(global_sym_tab);
    }
;

formal_param_list
    : formal_param_list COMMA formal_param {
        $1.push_back($3);
        $$ = $1;
    }
    | formal_param {
        $$ = std::vector<std::pair<std::string, DataType>>({$1});
    }
;

formal_param
    : param_type NAME {
        local_sym_tab->insert($2, $1);
        $$ = std::make_pair($2, $1);
    }
;

param_type
    : INTEGER   { $$ = DataType::INT; }
    | FLOAT     { $$ = DataType::FLOAT; }
    | BOOL      { $$ = DataType::BOOL; }
    | STRING    { $$ = DataType::STRING; }
;

statement_list
    : statement_list statement {
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | %empty {
        $$ = std::vector<std::unique_ptr<StmtNode>>();
    }
;

statement
    : assignment_statement {
        $$ = std::move($1);
    }
    | if_statement {

    }
    | do_while_statement {

    }
    | while_statement {

    }
    | compound_statement {

    }
    | print_statement {
        $$ = std::move($1);
    }
    | read_statement {
        $$ = std::move($1);
    }
    | call_statement {

    }
    | return_statement {

    }
;

call_statement
    : func_call SEMICOLON {

    }
;

func_call
    : NAME LEFT_ROUND_BRACKET actual_arg_list RIGHT_ROUND_BRACKET {

    }
;

actual_arg_list
    : non_empty_arg_list
    | %empty
;

non_empty_arg_list
    : non_empty_arg_list COMMA actual_arg
    | actual_arg
;

actual_arg
    : expression
;

return_statement
    : RETURN expression SEMICOLON
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
        for (const auto & var_name : $2){
            local_sym_tab->insert(var_name, $1);
        }
    }
;

var_decl_item_list
    : var_decl_item_list COMMA var_decl_item {
        $$ = std::move($1);
        $$.push_back($3);
    }
    | var_decl_item { 
        $$ = std::vector<std::string>({ $1 });
    }
;

var_decl_item
    : NAME
    | NAME array_decl
    | pointer_decl NAME
;

pointer_decl
    : MULT
    | MULT pointer_decl
;

array_decl
    : LEFT_SQUARE_BRACKET INT_NUM RIGHT_SQUARE_BRACKET
    | LEFT_SQUARE_BRACKET INT_NUM RIGHT_SQUARE_BRACKET array_decl
;

named_type
    : INTEGER   { $$ = DataType::INT; }
    | FLOAT     { $$ = DataType::FLOAT; }
    | VOID      { $$ = DataType::VOID; }
    | STRING    { $$ = DataType::STRING; }
    | BOOL      { $$ = DataType::BOOL; }
;

assignment_statement
    : variable_as_operand ASSIGN_OP expression SEMICOLON {
        std::unique_ptr<VariableExprNode> temp = std::make_unique<VariableExprNode>($1, local_sym_tab->lookup($1));
        $$ = std::make_unique<AssignStmtNode>(std::move(temp), std::move($3));
    }
    | variable_as_operand ASSIGN_OP func_call SEMICOLON {

    }
    | variable_as_operand ASSIGN_OP ADDRESSOF variable_name SEMICOLON {
        
    }
;

if_condition
    : LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET
;

if_statement
    : IF if_condition statement ELSE statement
    | IF if_condition statement
;

do_while_statement
    : DO statement WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement
;

while_statement
    : WHILE LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET statement
;

compound_statement
    : LEFT_CURLY_BRACKET statement_list RIGHT_CURLY_BRACKET
;

print_statement
    : WRITE expression SEMICOLON {
        $$ = std::make_unique<PrintStmtNode>(std::move($2));
    }
;

read_statement
    : READ NAME SEMICOLON {
        $$ = std::make_unique<ReadStmtNode>(std::move(std::make_unique<VariableExprNode>($2, local_sym_tab->lookup($2))));
    }
;

expression
    : expression PLUS expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::PLUS, std::move($1), std::move($3));
    }
    | expression MINUS expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::MINUS, std::move($1), std::move($3));
    }
    | expression MULT expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::MULT, std::move($1), std::move($3));
    }
    | expression DIV expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::DIVIDE, std::move($1), std::move($3));
    }
    | MINUS expression %prec UMINUS {
        $$ = std::make_unique<UnaryExprNode>(UnaryOperator::UMINUS, std::move($2));
    }
    | LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET {
        $$ = std::move($2);
    }
    | expression QUESTION_MARK expression COLON expression {
        $$ = std::make_unique<TernaryExprNode>(std::move($1), std::move($3), std::move($5));
    }
    | expression AND expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::AND, std::move($1), std::move($3));
    }
    | expression OR expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::OR, std::move($1), std::move($3));
    }
    | NOT expression {
        $$ = std::make_unique<UnaryExprNode>(UnaryOperator::NOT, std::move($2));
    }
    | rel_expression {
        $$ = std::move($1);
    }
    | variable_as_operand {
        $$ = std::make_unique<VariableExprNode>($1, local_sym_tab->lookup($1));
    }
    | constant_as_operand {
        $$ = std::move($1);
    }
;

rel_expression
    : expression LESS_THAN expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::LESS_THAN, std::move($1), std::move($3));
    }
    | expression LESS_THAN_EQUAL expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::LESS_THAN_EQUAL, std::move($1), std::move($3));
    }
    | expression GREATER_THAN expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::GREATER_THAN, std::move($1), std::move($3));
    }
    | expression GREATER_THAN_EQUAL expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::GREATER_THAN_EQUAL, std::move($1), std::move($3));
    }
    | expression NOT_EQUAL expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::NOT_EQUAL, std::move($1), std::move($3));
    }
    | expression EQUAL expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::EQUAL, std::move($1), std::move($3));
    }
;

variable_as_operand
    : variable_name
    | array_access
    | pointer_access
;

variable_name
    : NAME
;

array_access
    : variable_name array_dimensions
;

pointer_access
    : MULT variable_name
    | MULT pointer_access
;

array_dimensions
    : LEFT_SQUARE_BRACKET expression RIGHT_SQUARE_BRACKET
    | array_dimensions LEFT_SQUARE_BRACKET expression RIGHT_SQUARE_BRACKET
;

constant_as_operand
    : INT_NUM {
        $$ = std::make_unique<IntExprNode>($1);
    }
    | FLOAT_NUM {
        $$ = std::make_unique<FloatExprNode>($1);
    }
    | STR_CONST {
        $$ = std::make_unique<StringExprNode>($1);
    }
;
%%
