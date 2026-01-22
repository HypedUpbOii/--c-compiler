%{
    #include <stdio.h>
    #define DEBUG_PRINT 0
    void print(char* match, char* type);
    void debug_print(char* msg);
    unsigned int line_number = 1;
%}

digit [0-9]
operator [-+*/]
letter [a-zA-Z]
quote ["]
character [.\n]

%%
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


print {print("print", "PRINT");}
read {print("read", "READ");}
return {print("return", "RET");}

[ \t] {debug_print("\nIgnoring for now\n");}
{digit}+ {print(yytext, "INT_NUM");}
{letter}({letter}|{digit}|"_")* {print(yytext, "NAME");} 
\"(\\.|[^"\\])*\" {print(yytext, "STRING_LITERAL");}
\n {line_number++;}  
%%

void print(char* match, char* type) {
    printf("\tToken Name: %s\tLexeme: %s\tLineno: %d\n", type, match, line_number);
}

void debug_print(char* msg) {
    if (DEBUG_PRINT) {
        printf("%s", msg);
    }
}
