%{
    #include <stdio.h>
    #include <assert.h>
    #include <string.h>
    #define DEBUG_PRINT 0
    void print(char* match, const char* type);
    void debug_print(const char* msg);
    void yyerror(char* err_match);
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
"(" {print("(", "LEFT_ROUND_BRACKET");}
")" {print(")", "RIGHT_ROUND_BRACKET");}
"{" {print("{", "LEFT_CURLY_BRACKET");}
"}" {print("}", "RIGHT_CURLY_BRACKET");}
, {print(",", "COMMA");}
; {print(";", "SEMICOLON");}
= {print("=", "ASSIGN_OP");}
- {print("-", "MINUS");}
"*" {print("*", "MULT");}
"+" {print("+", "PLUS");}
"/" {print("/", "DIV");}

int {print("int", "INTEGER");}
bool {print("bool", "BOOL");}
void {print("void", "VOID");}
string {print("string", "STRING");}
double {print("double", "DOUBLE");}
float {print("float", "FLOAT");}

print {print("print", "WRITE");}
read {print("read", "READ");}
return {print("return", "RET");}

[ \t] {debug_print("\nIgnoring for now\n");}
({digit})+\.({digit})* {print(yytext, "FLOAT_NUM");}
{digit}+ {print(yytext, "INT_NUM");}
\.({digit})+ {print(yytext, "FLOAT_NUM");}
{letter}({letter}|{digit}|"_")* {print(yytext, "NAME");} 
\"([^\"\n])*\" {print(yytext, "STR_CONST");}
. {yyerror(yytext);}
%%

void print(char* match, const char* type) {
    printf("\tToken Name: %s\tLexeme: %s\tLineno: %d\n", type, match, line_number);
    strcpy(next_token, type);
}

void debug_print(const char* msg) {
    if (DEBUG_PRINT) {
        printf("%s", msg);
    }
}

void yyerror(char* match) {
    printf("sclp error: File: ,Line: %d, Next token: %s, Lexeme: %s\n\tDescription: Illegal character \'%c\'\n", line_number, next_token, match, match[0]);
    exit(1);
}