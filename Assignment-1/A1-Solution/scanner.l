%{
    #include "y.tab.h"
    #include <stdio.h>
    #include <string.h>
    void print(char* match, const char* type);
    void lexerror(char* match);
    extern int show_tokens;
    extern char input_file[1024];
    extern FILE* fp;
    extern char toks_file[1024];
    unsigned int line_number = 1;
    char next_token[20];
%}

digit [0-9]
operator [-+*/]
letter [a-zA-Z]
quote ["]
character [.\n]

%%
\n {line_number++;}  
"(" {print("(", "LEFT_ROUND_BRACKET"); return LEFT_ROUND_BRACKET;}
")" {print(")", "RIGHT_ROUND_BRACKET"); return RIGHT_ROUND_BRACKET;}
"{" {print("{", "LEFT_CURLY_BRACKET"); return LEFT_CURLY_BRACKET;}
"}" {print("}", "RIGHT_CURLY_BRACKET"); return RIGHT_CURLY_BRACKET;}
, {print(",", "COMMA"); return COMMA;}
; {print(";", "SEMICOLON"); return SEMICOLON;}
= {print("=", "ASSIGN_OP"); return ASSIGN_OP;}
- {print("-", "MINUS"); return MINUS;}
"*" {print("*", "MULT"); return MULT;}
"+" {print("+", "PLUS"); return PLUS;}
"/" {print("/", "DIV"); return DIV;}
"//"[^\n]* {}

int {print("int", "INTEGER"); return INTEGER;}
bool {print("bool", "BOOL"); return BOOL;}
void {print("void", "VOID"); return VOID;}
string {print("string", "STRING"); return STRING;}
float {print("float", "FLOAT"); return FLOAT;}

print {print("print", "WRITE"); return WRITE;}
read {print("read", "READ"); return READ;}
return {print("return", "RET"); return RET;}

[ \t] {}
({digit})+\.({digit})* {print(yytext, "FLOAT_NUM"); return FLOAT_NUM;}
{digit}+ {print(yytext, "INT_NUM"); return INT_NUM;}
\.({digit})+ {print(yytext, "FLOAT_NUM"); return FLOAT_NUM;}
{letter}({letter}|{digit}|"_")* {print(yytext, "NAME"); return NAME;}
\"([^\"\n])*\" {print(yytext, "STR_CONST"); return STR_CONST;}
. {lexerror(yytext);}
%%

void print(char* match, const char* type) {
    if (show_tokens) {
        printf("\tToken Name: %s\tLexeme: %s\tLineno: %d\n", type, match, line_number);
    }
    strcpy(next_token, type);
}

void lexerror(char* match) {
    printf("sclp error: File: %s, Line: %d, Next token: %s, Lexeme: \"%s\"\n\t    Description: Illegal character \'%c\'\n", input_file, line_number, next_token, match, match[0]);
    if (fp) {
        fclose(fp);
        fopen(toks_file, "w");
    }
    exit(1);
}

