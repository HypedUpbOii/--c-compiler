%language c++

%define api.namespace {parser}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.constructor

%locations

%code requires {
    #include <string>
}

%code {
    #include "lexer.hpp"
}

%param { Lexer& lexer }

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
%token ADDRESSOF
%token SEMICOLON COMMA
%token LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
%token QUESTION_MARK COLON

%token INTEGER BOOL VOID STRING FLOAT
%token WRITE READ RET

%left PLUS MINUS
%left MULT DIV
%right UMINUS
%noassoc GREATER_THAN GREATER_THAN_EQUAL LESS_THAN LESS_THAN_EQUAL

%%


%%

parser::Parser::symbol_type
parser::Parser::yylex() {
    return lexer.yylex();
}