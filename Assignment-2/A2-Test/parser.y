%require "3.2"
%language "c++"

%define api.namespace {parser}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.constructor

%locations

%code requires {
    #include <string>
    class Lexer;
}

%code {
    #include "lexer.hpp"
    static parser::Parser::symbol_type
    yylex(Lexer& lexer) {
        return lexer.nextToken();
    }
}

%param { Lexer& lexer }

%token ERROR

%token <int> INT_NUM
%token <float> FLOAT_NUM
%token <std::string> NAME
%token <std::string> STR_CONST

%token PLUS MINUS MULT DIV
%token ASSIGN_OP
%token GREATER_THAN LESS_THAN
%token GREATER_THAN_EQUAL LESS_THAN_EQUAL
%token EQUAL NOT_EQUAL
%token AND OR NOT
%token SEMICOLON COMMA
%token LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
%token QUESTION_MARK COLON

%token INTEGER BOOL VOID STRING FLOAT
%token WRITE READ RET

%right ASSIGN_OP
%nonassoc QUESTION_MARK COLON
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left GREATER_THAN_EQUAL LESS_THAN_EQUAL
%left GREATER_THAN LESS_THAN

%left PLUS MINUS
%left MULT DIV
%right UMINUS NOT
%%

program
    : func_def
    | func_decl func_def
    | var_list func_def
    | func_decl var_list func_def
    | var_list func_decl var_list func_def
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
    : var_list var_decl_stmt
    | var_decl_stmt
;

func_decl
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET SEMICOLON
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON
;

/*
func_def_list
    : func_def_list func_def
    | func_def
;
*/

func_header
    : named_type NAME
;

func_def
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET optional_var_decl_stmt_list statement_list RIGHT_CURLY_BRACKET
;

formal_param_list
    : formal_param_list COMMA formal_param
    | formal_param
;

formal_param
    : param_type NAME
;

param_type
    : INTEGER
    | FLOAT
    | BOOL
    | STRING
;

statement_list
    : statement_list statement
    | %empty
;

statement
    : assignment_statement
    | print_statement
    | read_statement
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
    : named_type var_decl_item_list SEMICOLON
;

var_decl_item_list
    : var_decl_item_list COMMA NAME
    | NAME
;

named_type
    : INTEGER
    | FLOAT
    | VOID
    | STRING
    | BOOL
;

assignment_statement
    : NAME ASSIGN_OP expression SEMICOLON
;

print_statement
    : WRITE expression SEMICOLON
;

read_statement
    : READ NAME SEMICOLON
;

expression
    : expression PLUS expression
    | expression MINUS expression
    | expression MULT expression
    | expression DIV expression
    | MINUS expression {}
    | LEFT_ROUND_BRACKET expression RIGHT_ROUND_BRACKET
    | NAME
    | constant_as_operand
    | expression QUESTION_MARK expression COLON expression
    | expression AND expression
    | expression OR expression
    | NOT expression
    | rel_expression
;

rel_expression
    : expression LESS_THAN expression
    | expression LESS_THAN_EQUAL expression
    | expression GREATER_THAN expression
    | expression GREATER_THAN_EQUAL expression
    | expression EQUAL expression
    | expression NOT_EQUAL expression
;

constant_as_operand
    : INT_NUM
    | FLOAT_NUM
    | STR_CONST
;

%%
