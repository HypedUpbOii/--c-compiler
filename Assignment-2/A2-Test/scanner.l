%option c++
%option noyywrap
%option yylineno
%option warn nodefault
%option yyclass="Lexer"

%{
    #include "parser.tab.hh"
    #include "lexer.hpp"

    #define YY_DECL parser::Parser::symbol_type Lexer::yylex()

    using parser::Parser;
%}

digit [0-9]
letter [a-zA-Z_]

%%
[ \t\r]+                            ;
\n                                  ;
"//"[^\n]*                          ;

">="				                {write_token("GREATER_THAN_EQUAL"); return Parser::make_GREATER_THAN_EQUAL(loc);}
"<="				                {write_token("LESS_THAN_EQUAL"); return Parser::make_LESS_THAN_EQUAL(loc);}
"=="				                {write_token("EQUAL"); return Parser::make_EQUAL(loc);}
"!="				                {write_token("NOT_EQUAL"); return Parser::make_NOT_EQUAL(loc);}
"&&"		    		            {write_token("AND"); return Parser::make_AND(loc);}
"||"	    			            {write_token("OR"); return Parser::make_OR(loc);}

">"				                    {write_token("GREATER_THAN"); return Parser::make_GREATER_THAN(loc);}
"<"				                    {write_token("LESS_THAN"); return Parser::make_LESS_THAN(loc);}
"?"				                    {write_token("QUESTION_MARK"); return Parser::make_QUESTION_MARK(loc);}
":"				                    {write_token("COLON"); return Parser::make_COLON(loc);}
"!"	    			                {write_token("NOT"); return Parser::make_NOT(loc);}
"&" 				                {write_token("ADDRESSOF"); return Parser::make_ADDRESSOF(loc);}
"("                                 {write_token("LEFT_ROUND_BRACKET"); return Parser::make_LEFT_ROUND_BRACKET(loc);}
")"                                 {write_token("RIGHT_ROUND_BRACKET"); return Parser::make_RIGHT_ROUND_BRACKET(loc);}
"{"                                 {write_token("LEFT_CURLY_BRACKET"); return Parser::make_LEFT_CURLY_BRACKET(loc);}
"}"                                 {write_token("RIGHT_CURLY_BRACKET"); return Parser::make_RIGHT_CURLY_BRACKET(loc);}
","                                 {write_token("COMMA"); return Parser::make_COMMA(loc);}
";"                                 {write_token("SEMICOLON"); return Parser::make_SEMICOLON(loc);}
"="                                 {write_token("ASSIGN_OP"); return Parser::make_ASSIGN_OP(loc);}
"-"                                 {write_token("MINUS"); return Parser::make_MINUS(loc);}
"*"                                 {write_token("MULT"); return Parser::make_MULT(loc);}
"+"                                 {write_token("PLUS"); return Parser::make_PLUS(loc);}
"/"                                 {write_token("DIV"); return Parser::make_DIV(loc);}

int                                 {write_token("INTEGER"); return Parser::make_INTEGER(loc);}
bool                                {write_token("BOOL"); return Parser::make_BOOL(loc);}
void                                {write_token("VOID"); return Parser::make_VOID(loc);}
string                              {write_token("STRING"); return Parser::make_STRING(loc);}
float                               {write_token("FLOAT"); return Parser::make_FLOAT(loc);}

print                               {write_token("WRITE"); return Parser::make_WRITE(loc);}
read                                {write_token("READ"); return Parser::make_READ(loc);}
return                              {write_token("RET"); return Parser::make_RET(loc);}

({digit}+\.{digit}*)|(\.{digit}+)   {write_token("FLOAT_NUM"); return Parser::make_FLOAT_NUM(std::stof(YYText()), loc);}
{digit}+                            {write_token("INT_NUM"); return Parser::make_INT_NUM(std::stoi(YYText()), loc);}

{letter}({letter}|{digit})*         {write_token("NAME"); return Parser::make_NAME(std::string(YYText()), loc);}
\"([^\"\n])*\"                      {write_token("STR_CONST"); return Parser::make_STR_CONST(std::string(YYText()), loc);}

.                                   {lexerror();}
%%

Lexer::Lexer(std::istream* in, std::string input_file std::ostream& stream)
    : yyFlexLexer(in), input_file(input_file), outstream(stream) {}

void Lexer::write_token(const string& type) {
    outstream << "\tToken Name: " << type 
                << "\tLexeme: " << YYText()
                << "\tLineno: " << lineno() << endl;
    next_token = type;
}

void Lexer::lexerror() {
    cerr << "sclp error: File: " << input_file
            << ", Line: " << lineno()
            << ", Next token: " << next_token
            << ", Lexeme: \"" << YYText()
            << "\"\n\t    Description: Illegal character \'" << YYText() << "\'" << endl;
    exit(1);
}
