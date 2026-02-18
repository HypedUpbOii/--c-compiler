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

%token ERROR

%token <int> INT_NUM
%token <double> FLOAT_NUM
%token <std::string> NAME
%token <std::string> STR_CONST

%token PLUS MINUS MULT DIV
%token ASSIGN_OP
%token GREATER_THAN LESS_THAN
%token GREATER_THAN_EQUAL LESS_THAN_EQUAL
%token SEMICOLON COMMA
%token LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET

%token INTEGER BOOL VOID STRING FLOAT
%token WRITE READ RET

%type <DataType> named_type
%type <DataType> param_type
%type <std::vector<std::string>> var_decl_item_list
%type <std::pair<DataType, std::string>> func_header
%type <std::unique_ptr<FunctionNode>> func_def
%type <std::vector<std::pair<std::string, DataType>>> formal_param_list
%type <std::vector<std::unique_ptr<StmtNode>>> statement_list
%type <std::pair<std::string, DataType>> formal_param
%type <std::unique_ptr<StmtNode>> statement
%type <std::unique_ptr<AssignStmtNode>> assignment_statement
%type <std::unique_ptr<ReadStmtNode>> read_statement
%type <std::unique_ptr<PrintStmtNode>> print_statement
%type <std::unique_ptr<ExprNode>> expression
%type <std::unique_ptr<ExprNode>> constant_as_operand
%type <std::unique_ptr<ExprNode>> rel_expression
%type <std::tuple<DataType, std::string, std::vector<DataType>>> func_decl

%nonassoc ASSIGN_OP
%right QUESTION_MARK COLON
%left OR
%left AND
%right NOT
%nonassoc EQUAL NOT_EQUAL
%nonassoc GREATER_THAN_EQUAL LESS_THAN_EQUAL
%nonassoc GREATER_THAN LESS_THAN

%left PLUS MINUS
%left MULT DIV
%right UMINUS
%%

program
    : func_def {
        ast.funcs.push_back(std::move($1));
        ast.setSymbolTable(global_sym_tab);
    }
    | func_decl func_def {
        ast.func_decls.push_back($1);
        ast.funcs.push_back(std::move($2));
        ast.setSymbolTable(global_sym_tab);
    }
    | var_list func_def {
        ast.funcs.push_back(std::move($2));
        ast.setSymbolTable(global_sym_tab);
    }
    | func_decl var_list func_def {
        ast.func_decls.push_back($1);
        ast.funcs.push_back(std::move($3));
        ast.setSymbolTable(global_sym_tab);
    }
    | var_list func_decl var_list func_def {
        ast.func_decls.push_back($2);
        ast.funcs.push_back(std::move($4));
        ast.setSymbolTable(global_sym_tab);
    }
;

/*
global_decl_statement_list
    : global_decl_statement_list func_decl 
    | global_decl_statement_list var_decl_stmt
    | func_decl
    | var_decl_stmt
;
*/

var_list 
    : var_list global_var_decl_stmt
    | global_var_decl_stmt
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

/*
func_def_list
    : func_def_list func_def
    | func_def
;
*/

func_header
    : named_type NAME {
        $$ = std::make_pair($1, $2);
    }
;

func_def
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET {
        $$ = std::make_unique<FunctionNode>($1.first, $3, $1.second, std::move($7), local_sym_tab);
        local_sym_tab = new SymbolTable(global_sym_tab);
    }
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET {
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
    | print_statement {
        $$ = std::move($1);
    }
    | read_statement {
        $$ = std::move($1);
    }
;

optional_var_decl_stmt_list
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

global_var_decl_stmt
    : named_type var_decl_item_list SEMICOLON {
        for (const auto & var_name : $2){
            global_sym_tab->insert(var_name, $1);
        }
    }

var_decl_item_list
    : var_decl_item_list COMMA NAME {
        $$ = std::move($1);
        $$.push_back($3);
    }
    | NAME { 
        $$ = std::vector<std::string>({ $1 });
    }
;

named_type
    : INTEGER   { $$ = DataType::INT; }
    | FLOAT     { $$ = DataType::FLOAT; }
    | VOID      { $$ = DataType::VOID; }
    | STRING    { $$ = DataType::STRING; }
    | BOOL      { $$ = DataType::BOOL; }
;

assignment_statement
    : NAME ASSIGN_OP expression SEMICOLON {
        std::unique_ptr<VariableExprNode> temp = std::make_unique<VariableExprNode>($1, local_sym_tab->lookup($1));
        $$ = std::make_unique<AssignStmtNode>(std::move(temp), std::move($3));
    }
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
    | NAME {
        $$ = std::make_unique<VariableExprNode>($1, local_sym_tab->lookup($1));
    }
    | constant_as_operand {
        $$ = std::move($1);
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
    | expression EQUAL expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::EQUAL, std::move($1), std::move($3));
    }
    | expression NOT_EQUAL expression {
        $$ = std::make_unique<BinaryExprNode>(BinaryOperator::NOT_EQUAL, std::move($1), std::move($3));
    }
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
