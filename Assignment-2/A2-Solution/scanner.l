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
letter [a-zA-Z_]

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

">"				                {write_tokens(">", "GREATER_THAN"); return GREATER_THAN;}
">="				            {write_tokens(">=", "GREATER_THAN_EQUAL"); return GREATER_THAN_EQUAL;}
"<"				                {write_tokens("<", "LESS_THAN"); return LESS_THAN;}
"<="				            {write_tokens("<=", "LESS_THAN_EQUAL"); return LESS_THAN_EQUAL;}
"=="				            {write_tokens("==", "EQUAL"); return EQUAL;}
"!="				            {write_tokens("!=", "NOT_EQUAL"); return NOT_EQUAL;}
"?"				                {write_tokens("?", "QUESTION_MARK"); return QUESTION_MARK;}
":"				                {write_tokens(":", "COLON"); return COLON;}
"&&"				            {write_tokens("&&", "AND"); return AND;}
"||"				            {write_tokens("||", "OR"); return OR;}
"!"				                {write_tokens("!", "NOT"); return NOT;}
"&"				                {write_tokens("&", "ADDRESSOF"); return ADDRESSOF;}

int                             {write_tokens("int", "INTEGER"); return INTEGER;}
bool                            {write_tokens("bool", "BOOL"); return BOOL;}
void                            {write_tokens("void", "VOID"); return VOID;}
string                          {write_tokens("string", "STRING"); return STRING;}
float                           {write_tokens("float", "FLOAT"); return FLOAT;}

print                           {write_tokens("print", "WRITE"); return WRITE;}
read                            {write_tokens("read", "READ"); return READ;}
return                          {write_tokens("return", "RET"); return RET;}

[ \t]                           {}
({digit})+\.({digit})*          {
                                    write_tokens(yytext, "FLOAT_NUM");
                                    yylval.fval = stof(string(yytext));
                                    return FLOAT_NUM;
                                }
{digit}+                        {
                                    write_tokens(yytext, "INT_NUM");
                                    yylval.ival = stoi(string(yytext));
                                    return INT_NUM;
                                }
\.({digit})+                    {
                                    write_tokens(yytext, "FLOAT_NUM");
                                    yylval.fval = stof(string(yytext));
                                    return FLOAT_NUM;
                                }
{letter}({letter}|{digit}|)*    {
                                    write_tokens(yytext, "NAME");
                                    yylval.sval = string(yytext);
                                    return NAME;
                                }
\"([^\"\n])*\"                  {
                                    write_tokens(yytext, "STR_CONST");
                                    std::string quotes = string(yytext);
                                    yylval.sval = quotes.substr(1, quotes.length() - 2);
                                    return STR_CONST;
                                }
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
