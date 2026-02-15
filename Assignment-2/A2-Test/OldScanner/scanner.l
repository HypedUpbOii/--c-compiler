%option c++
%option noyywrap
%option yylineno
%option warn nodefault
%option yyclass="Lexer"

%{
    #include "lexer.hpp"
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
%}

digit [0-9]
letter [a-zA-Z_]

%%
[ \t\r]+                            ;
\n                                  ;
"//"[^\n]*                          ;

">="				                {write_token("GREATER_THAN_EQUAL");}
"<="				                {write_token("LESS_THAN_EQUAL");}
"=="				                {write_token("EQUAL");}
"!="				                {write_token("NOT_EQUAL");}
"&&"		    		            {write_token("AND");}
"||"	    			            {write_token("OR");}

">"				                    {write_token("GREATER_THAN");}
"<"				                    {write_token("LESS_THAN");}
"?"				                    {write_token("QUESTION_MARK");}
":"				                    {write_token("COLON");}
"!"	    			                {write_token("NOT");}
"&" 				                {write_token("ADDRESSOF");}
"("                                 {write_token("LEFT_ROUND_BRACKET");}
")"                                 {write_token("RIGHT_ROUND_BRACKET");}
"{"                                 {write_token("LEFT_CURLY_BRACKET");}
"}"                                 {write_token("RIGHT_CURLY_BRACKET");}
","                                 {write_token("COMMA");}
";"                                 {write_token("SEMICOLON");}
"="                                 {write_token("ASSIGN_OP");}
"-"                                 {write_token("MINUS");}
"*"                                 {write_token("MULT");}
"+"                                 {write_token("PLUS");}
"/"                                 {write_token("DIV");}

int                                 {write_token("INTEGER");}
bool                                {write_token("BOOL");}
void                                {write_token("VOID");}
string                              {write_token("STRING");}
float                               {write_token("FLOAT");}

print                               {write_token("WRITE");}
read                                {write_token("READ");}
return                              {write_token("RET");}

({digit}+\.{digit}*)|(\.{digit}+)   {
                                        write_token("FLOAT_NUM");
                                        // yylval->emplace<float>(stof(yytext));
                                    }
{digit}+                            {
                                        write_token("INT_NUM");
                                        // yylval->emplace<int>(atoi(yytext));
                                    }

{letter}({letter}|{digit})*         {write_token("NAME");}
\"([^\"\n])*\"                      {write_token("STR_CONST");}

.                                   {lexerror();}
%%
