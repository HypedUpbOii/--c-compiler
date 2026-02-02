%{
    #include "y.tab.h"
    #include <iostream>
    using namespace std;
    
    void write_tokens(char* match, string type);
    void lexerror(char* match);
    
    extern bool show_tokens;
    extern string input_file;
    
    unsigned int line_number = 1;
    string next_token;
%}

digit [0-9]
letter [a-zA-Z]

%%
\n                              {line_number++;}  
"("                             {write_tokens("(", "LEFT_ROUND_BRACKET"); return LEFT_ROUND_BRACKET;}
")"                             {write_tokens(")", "RIGHT_ROUND_BRACKET"); return RIGHT_ROUND_BRACKET;}
"{"                             {write_tokens("{", "LEFT_CURLY_BRACKET"); return LEFT_CURLY_BRACKET;}
"}"                             {write_tokens("}", "RIGHT_CURLY_BRACKET"); return RIGHT_CURLY_BRACKET;}
,                               {write_tokens(",", "COMMA"); return COMMA;}
;                               {write_tokens(";", "SEMICOLON"); return SEMICOLON;}
=                               {write_tokens("=", "ASSIGN_OP"); return ASSIGN_OP;}
-                               {write_tokens("-", "MINUS"); return MINUS;}
"*"                             {write_tokens("*", "MULT"); return MULT;}
"+"                             {write_tokens("+", "PLUS"); return PLUS;}
"/"                             {write_tokens("/", "DIV"); return DIV;}
"//"[^\n]*                      {}

int                             {write_tokens("int", "INTEGER"); return INTEGER;}
bool                            {write_tokens("bool", "BOOL"); return BOOL;}
void                            {write_tokens("void", "VOID"); return VOID;}
string                          {write_tokens("string", "STRING"); return STRING;}
float                           {write_tokens("float", "FLOAT"); return FLOAT;}

print                           {write_tokens("print", "WRITE"); return WRITE;}
read                            {write_tokens("read", "READ"); return READ;}
return                          {write_tokens("return", "RET"); return RET;}

[ \t]                           {}
({digit})+\.({digit})*          {write_tokens(yytext, "FLOAT_NUM"); return FLOAT_NUM;}
{digit}+                        {write_tokens(yytext, "INT_NUM"); return INT_NUM;}
\.({digit})+                    {write_tokens(yytext, "FLOAT_NUM"); return FLOAT_NUM;}
{letter}({letter}|{digit}|"_")* {write_tokens(yytext, "NAME"); return NAME;}
\"([^\"\n])*\"                  {write_tokens(yytext, "STR_CONST"); return STR_CONST;}
.                               {lexerror(yytext);}
%%

void write_tokens(char* match, string type) {
    if (show_tokens) {
        cout << "\tToken Name: " << type << "\tLexeme: " << match << "\tLineno: " << line_number << endl;
    }
    next_token = type;
}

void lexerror(char* match) {
    cerr << "sclp error: File: " << input_file << ", Line: " << line_number << ", Next token: " << next_token << ", Lexeme: \"" << match << "\"\n\t    Description: Illegal character \'" << match[0] << "\'" << endl;
    exit(1);
}
