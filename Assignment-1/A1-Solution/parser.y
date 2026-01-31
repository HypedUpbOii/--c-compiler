%{
    #include <stdio.h>
    #include <stdlib.h>
    int yylex(void);
    int yyerror(char* s);
    extern char input_file[1024];
    extern unsigned int line_number;
    extern char next_token[20];
    extern char* yytext;
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
%token FLOAT_NUM
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
    : func_header LEFT_ROUND_BRACKET formal_param_list RIGHT_ROUND_BRACKET SEMICOLON
    | func_header LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET SEMICOLON
;

func_def_list
    : func_def_list func_def
    | func_def
;

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
;

constant_as_operand
    : INT_NUM
    | FLOAT_NUM
    | STR_CONST
;
%%

int yyerror(char* s) {
    fprintf(stderr, "syntax error\n");
    fprintf(stderr, "sclp error: File: %s, Line: %d, Next token: %s, Lexeme: %s\n\t    Description: Cannot parse the input program\n", input_file, line_number, next_token, yytext);
    exit(1);
    return 1;
}