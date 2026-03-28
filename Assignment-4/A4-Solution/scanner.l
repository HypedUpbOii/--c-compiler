%option c++
%option noyywrap yylineno
%option warn nodefault

%{
    #include "parser.tab.hh"
    #include "lexer.hpp"
%}

digit [0-9]
letter [a-zA-Z_]

%%
[ \t\r]+                            ;
\n                                  ;
"//"[^\n]*                          ;

">="				                {return parser::Parser::token::GREATER_THAN_EQUAL;}
"<="				                {return parser::Parser::token::LESS_THAN_EQUAL;}
"=="				                {return parser::Parser::token::EQUAL;}
"!="				                {return parser::Parser::token::NOT_EQUAL;}
"&&"		    		            {return parser::Parser::token::AND;}
"||"	    			            {return parser::Parser::token::OR;}

">"				                    {return parser::Parser::token::GREATER_THAN;}
"<"				                    {return parser::Parser::token::LESS_THAN;}
"?"				                    {return parser::Parser::token::QUESTION_MARK;}
":"				                    {return parser::Parser::token::COLON;}
"!"	    			                {return parser::Parser::token::NOT;}
"("                                 {return parser::Parser::token::LEFT_ROUND_BRACKET;}
")"                                 {return parser::Parser::token::RIGHT_ROUND_BRACKET;}
"{"                                 {return parser::Parser::token::LEFT_CURLY_BRACKET;}
"}"                                 {return parser::Parser::token::RIGHT_CURLY_BRACKET;}
"["                                 {std::cerr << "Higher level language feature" << std::endl; exit(1); return parser::Parser::token::LEFT_SQUARE_BRACKET;}
"]"                                 {std::cerr << "Higher level language feature" << std::endl; exit(1); return parser::Parser::token::RIGHT_SQUARE_BRACKET;}
"&"                                 {std::cerr << "Higher level language feature" << std::endl; exit(1); return parser::Parser::token::ADDRESSOF;} // L6
","                                 {return parser::Parser::token::COMMA;}
";"                                 {return parser::Parser::token::SEMICOLON;}
"="                                 {return parser::Parser::token::ASSIGN_OP;}
"-"                                 {return parser::Parser::token::MINUS;}
"*"                                 {return parser::Parser::token::MULT;}
"+"                                 {return parser::Parser::token::PLUS;}
"/"                                 {return parser::Parser::token::DIV;}

int                                 {return parser::Parser::token::INTEGER;}
bool                                {return parser::Parser::token::BOOL;}
void                                {return parser::Parser::token::VOID;}
string                              {return parser::Parser::token::STRING;}
float                               {return parser::Parser::token::FLOAT;}

print                               {return parser::Parser::token::WRITE;}
read                                {return parser::Parser::token::READ;}
return                              {std::cerr << "Higher level language feature" << std::endl; exit(1); return parser::Parser::token::RETURN;} //L5
if                                  {return parser::Parser::token::IF;}
else                                {return parser::Parser::token::ELSE;}
while                               {return parser::Parser::token::WHILE;}
do                                  {return parser::Parser::token::DO;}

({digit}+\.{digit}*)|(\.{digit}+)   {return parser::Parser::token::FLOAT_NUM;}
{digit}+                            {return parser::Parser::token::INT_NUM;}

{letter}({letter}|{digit})*         {return parser::Parser::token::NAME;}
\"([^\"\n])*\"                      {return parser::Parser::token::STR_CONST;}

.                                   {return parser::Parser::token::ERROR;}
%%
