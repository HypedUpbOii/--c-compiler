%{
    #include <iostream>
    using namespace std;
    
    int yylex(void);
    int yyerror(char* s);
    
    extern char* yytext;
    extern string next_token;
    extern unsigned int line_number;
    extern string input_file;
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
%token GREATER_THAN
%token GREATER_THAN_EQUAL
%token LESS_THAN
%token LESS_THAN_EQUAL
%token EQUAL
%token NOT_EQUAL
%token QUESTION_MARK
%token COLON
%token AND
%token OR
%token NOT
%token ADDRESSOF
%token INTEGER
%token BOOL
%token VOID
%token STRING
%token FLOAT
%token WRITE
%token READ
%token RET
%token FLOAT_NUM
%token INT_NUM
%token FLOAT_NUM
%token NAME
%token STR_CONST

%left OR
%left AND

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

int yyerror(char* s) {
    cerr << "syntax error" << endl;
    cerr << "sclp error: File: " << input_file << ", Line: " << line_number << ", Next token: " << next_token << ", Lexeme \"" << yytext << "\"\n\t    Description: Cannot parse the input program" << endl;
    exit(1);
    return 1;
}
