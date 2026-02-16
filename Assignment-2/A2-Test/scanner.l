%option noyywrap
%option yylineno
%option warn nodefault

%{
    #include "parser.tab.hh"
    #include "lexer.hpp"

    #ifdef YY_DECL
    #undef YY_DECL
    #endif
    #define YY_DECL parser::Parser::symbol_type yylex(Lexer& lexer)
    #define yyterminate() return parser::Parser::make_YYEOF(lexer.loc)

    using parser::Parser;
%}

digit [0-9]
letter [a-zA-Z_]

%%
[ \t\r]+                            ;
\n                                  ;
"//"[^\n]*                          ;

">="				                {lexer.write_token("GREATER_THAN_EQUAL"); return Parser::make_GREATER_THAN_EQUAL(lexer.loc);}
"<="				                {lexer.write_token("LESS_THAN_EQUAL"); return Parser::make_LESS_THAN_EQUAL(lexer.loc);}
"=="				                {lexer.write_token("EQUAL"); return Parser::make_EQUAL(lexer.loc);}
"!="				                {lexer.write_token("NOT_EQUAL"); return Parser::make_NOT_EQUAL(lexer.loc);}
"&&"		    		            {lexer.write_token("AND"); return Parser::make_AND(lexer.loc);}
"||"	    			            {lexer.write_token("OR"); return Parser::make_OR(lexer.loc);}

">"				                    {lexer.write_token("GREATER_THAN"); return Parser::make_GREATER_THAN(lexer.loc);}
"<"				                    {lexer.write_token("LESS_THAN"); return Parser::make_LESS_THAN(lexer.loc);}
"?"				                    {lexer.write_token("QUESTION_MARK"); return Parser::make_QUESTION_MARK(lexer.loc);}
":"				                    {lexer.write_token("COLON"); return Parser::make_COLON(lexer.loc);}
"!"	    			                {lexer.write_token("NOT"); return Parser::make_NOT(lexer.loc);}
"&" 				                {lexer.write_token("ADDRESSOF"); return Parser::make_ADDRESSOF(lexer.loc);}
"("                                 {lexer.write_token("LEFT_ROUND_BRACKET"); return Parser::make_LEFT_ROUND_BRACKET(lexer.loc);}
")"                                 {lexer.write_token("RIGHT_ROUND_BRACKET"); return Parser::make_RIGHT_ROUND_BRACKET(lexer.loc);}
"{"                                 {lexer.write_token("LEFT_CURLY_BRACKET"); return Parser::make_LEFT_CURLY_BRACKET(lexer.loc);}
"}"                                 {lexer.write_token("RIGHT_CURLY_BRACKET"); return Parser::make_RIGHT_CURLY_BRACKET(lexer.loc);}
","                                 {lexer.write_token("COMMA"); return Parser::make_COMMA(lexer.loc);}
";"                                 {lexer.write_token("SEMICOLON"); return Parser::make_SEMICOLON(lexer.loc);}
"="                                 {lexer.write_token("ASSIGN_OP"); return Parser::make_ASSIGN_OP(lexer.loc);}
"-"                                 {lexer.write_token("MINUS"); return Parser::make_MINUS(lexer.loc);}
"*"                                 {lexer.write_token("MULT"); return Parser::make_MULT(lexer.loc);}
"+"                                 {lexer.write_token("PLUS"); return Parser::make_PLUS(lexer.loc);}
"/"                                 {lexer.write_token("DIV"); return Parser::make_DIV(lexer.loc);}

int                                 {lexer.write_token("INTEGER"); return Parser::make_INTEGER(lexer.loc);}
bool                                {lexer.write_token("BOOL"); return Parser::make_BOOL(lexer.loc);}
void                                {lexer.write_token("VOID"); return Parser::make_VOID(lexer.loc);}
string                              {lexer.write_token("STRING"); return Parser::make_STRING(lexer.loc);}
float                               {lexer.write_token("FLOAT"); return Parser::make_FLOAT(lexer.loc);}

print                               {lexer.write_token("WRITE"); return Parser::make_WRITE(lexer.loc);}
read                                {lexer.write_token("READ"); return Parser::make_READ(lexer.loc);}
return                              {lexer.write_token("RET"); return Parser::make_RET(lexer.loc);}

({digit}+\.{digit}*)|(\.{digit}+)   {lexer.write_token("FLOAT_NUM"); return Parser::make_FLOAT_NUM(std::stof(yytext), lexer.loc);}
{digit}+                            {lexer.write_token("INT_NUM"); return Parser::make_INT_NUM(std::stoi(yytext), lexer.loc);}

{letter}({letter}|{digit})*         {lexer.write_token("NAME"); return Parser::make_NAME(std::string(yytext), lexer.loc);}
\"([^\"\n])*\"                      {lexer.write_token("STR_CONST"); return Parser::make_STR_CONST(std::string(yytext), lexer.loc);}

.                                   {lexer.lexerror();}
%%

Lexer::Lexer(const std::string& input_file, std::ostream& stream)
    : input_file(input_file), outstream(stream) {}

void Lexer::write_token(const string& type) {
    outstream << "\tToken Name: " << type 
                << "\tLexeme: " << yytext
                << "\tLineno: " << yylineno << endl;
    next_token = type;
}

void Lexer::lexerror() {
    cerr << "sclp error: File: " << input_file
            << ", Line: " << yylineno
            << ", Next token: " << next_token
            << ", Lexeme: \"" << yytext
            << "\"\n\t    Description: Illegal character \'" << yytext << "\'" << endl;
    exit(1);
}
