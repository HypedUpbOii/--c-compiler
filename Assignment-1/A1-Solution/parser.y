%{
    #include <stdio.h>
    #include <stdlib.h>
%}
%token LEFT_ROUND_BRACKET
%token RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET
%token RIGHT_CURLY_BRACKET
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
%token DOUBLE
%token FLOAT
%token WRITE
%token READ
%token RET
%token INT_NUM
%token NAME
%token STR_CONST
%left PLUS MINUS
%left MULT DIV
%%
program
    : 
    ;

statement
    :
    ;

func_declaration
    : func_type func_name LEFT_ROUND_BRACKET func_args RIGHT_ROUND_BRACKET SEMICOLON
    ;

func_definition
    : func_type func_name LEFT_ROUND_BRACKET func_args RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET statement_list RIGHT_CURLY_BRACKET
    ;

var_declaration
    : var_type var_list SEMICOLON
    | var_type var_name ASSIGN_OP expr SEMICOLON
    ;

assign_statement
    : NAME ASSIGN_OP expr SEMICOLON
    ;

print_statement
    : WRITE expr SEMICOLON
    ;

read_statement
    : READ variable_name SEMICOLON
    ;

variable_name
    : NAME
    ;

constant
    : INT_NUM
    | STR_CONST
    ;

expr
    : arithmetic_expr
    | constant
    ;

arithmetic_expr
    :
    ;

func_types
    :
    ;

var_types
    :
    ;

func_args
    : 
    | %empty
    ;
%%
