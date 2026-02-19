#include "lexer.hpp"

Lexer::Lexer(std::istream *in, std::string input_file, std::ostream &stream)
    : yyFlexLexer(in), input_file(input_file), outstream(stream) {}

parser::Parser::symbol_type Lexer::nextToken() {
    int token = yylex();

    switch (token) {
        case parser::Parser::token::GREATER_THAN_EQUAL:
            write_token("GREATER_THAN_EQUAL");
            return parser::Parser::make_GREATER_THAN_EQUAL(loc);
        case parser::Parser::token::LESS_THAN_EQUAL:
            write_token("LESS_THAN_EQUAL");
            return parser::Parser::make_LESS_THAN_EQUAL(loc);
        case parser::Parser::token::EQUAL:
            write_token("EQUAL");
            return parser::Parser::make_EQUAL(loc);
        case parser::Parser::token::NOT_EQUAL:
            write_token("NOT_EQUAL");
            return parser::Parser::make_NOT_EQUAL(loc);
        case parser::Parser::token::AND:
            write_token("AND");
            return parser::Parser::make_AND(loc);
        case parser::Parser::token::OR:
            write_token("OR");
            return parser::Parser::make_OR(loc);
        case parser::Parser::token::GREATER_THAN:
            write_token("GREATER_THAN");
            return parser::Parser::make_GREATER_THAN(loc);
        case parser::Parser::token::LESS_THAN:
            write_token("LESS_THAN");
            return parser::Parser::make_LESS_THAN(loc);
        case parser::Parser::token::QUESTION_MARK:
            write_token("QUESTION_MARK");
            return parser::Parser::make_QUESTION_MARK(loc);
        case parser::Parser::token::COLON:
            write_token("COLON");
            return parser::Parser::make_COLON(loc);
        case parser::Parser::token::NOT:
            write_token("NOT");
            return parser::Parser::make_NOT(loc);
        case parser::Parser::token::LEFT_ROUND_BRACKET:
            write_token("LEFT_ROUND_BRACKET");
            return parser::Parser::make_LEFT_ROUND_BRACKET(loc);
        case parser::Parser::token::RIGHT_ROUND_BRACKET:
            write_token("RIGHT_ROUND_BRACKET");
            return parser::Parser::make_RIGHT_ROUND_BRACKET(loc);
        case parser::Parser::token::LEFT_CURLY_BRACKET:
            write_token("LEFT_CURLY_BRACKET");
            return parser::Parser::make_LEFT_CURLY_BRACKET(loc);
        case parser::Parser::token::RIGHT_CURLY_BRACKET:
            write_token("RIGHT_CURLY_BRACKET");
            return parser::Parser::make_RIGHT_CURLY_BRACKET(loc);
        case parser::Parser::token::LEFT_SQUARE_BRACKET:
            write_token("LEFT_SQUARE_BRACKET");
            return parser::Parser::make_LEFT_SQUARE_BRACKET(loc);
        case parser::Parser::token::RIGHT_SQUARE_BRACKET:
            write_token("RIGHT_SQUARE_BRACKET");
            return parser::Parser::make_RIGHT_SQUARE_BRACKET(loc);
        case parser::Parser::token::ADDRESS_OF:
            write_token("ADDRESS_OF");
            return parser::Parser::make_ADDRESS_OF(loc);
        case parser::Parser::token::COMMA:
            write_token("COMMA");
            return parser::Parser::make_COMMA(loc);
        case parser::Parser::token::SEMICOLON:
            write_token("SEMICOLON");
            return parser::Parser::make_SEMICOLON(loc);
        case parser::Parser::token::ASSIGN_OP:
            write_token("ASSIGN_OP");
            return parser::Parser::make_ASSIGN_OP(loc);
        case parser::Parser::token::MINUS:
            write_token("MINUS");
            return parser::Parser::make_MINUS(loc);
        case parser::Parser::token::MULT:
            write_token("MULT");
            return parser::Parser::make_MULT(loc);
        case parser::Parser::token::PLUS:
            write_token("PLUS");
            return parser::Parser::make_PLUS(loc);
        case parser::Parser::token::DIV:
            write_token("DIV");
            return parser::Parser::make_DIV(loc);
        case parser::Parser::token::INTEGER:
            write_token("INTEGER");
            return parser::Parser::make_INTEGER(loc);
        case parser::Parser::token::BOOL:
            write_token("BOOL");
            return parser::Parser::make_BOOL(loc);
        case parser::Parser::token::VOID:
            write_token("VOID");
            return parser::Parser::make_VOID(loc);
        case parser::Parser::token::STRING:
            write_token("STRING");
            return parser::Parser::make_STRING(loc);
        case parser::Parser::token::FLOAT:
            write_token("FLOAT");
            return parser::Parser::make_FLOAT(loc);
        case parser::Parser::token::WRITE:
            write_token("WRITE");
            return parser::Parser::make_WRITE(loc);
        case parser::Parser::token::READ:
            write_token("READ");
            return parser::Parser::make_READ(loc);
        case parser::Parser::token::RETURN:
            write_token("RETURN");
            return parser::Parser::make_RETURN(loc);
        case parser::Parser::token::IF:
            write_token("IF");
            return parser::Parser::make_IF(loc);
        case parser::Parser::token::ELSE:
            write_token("ELSE");
            return parser::Parser::make_ELSE(loc);
        case parser::Parser::token::WHILE:
            write_token("WHILE");
            return parser::Parser::make_WHILE(loc);
        case parser::Parser::token::DO_WHILE:
            write_token("DO_WHILE");
            return parser::Parser::make_DO_WHILE(loc);
        case parser::Parser::token::FLOAT_NUM:
            write_token("FLOAT_NUM");
            return parser::Parser::make_FLOAT_NUM(std::strtod(YYText()), loc);
        case parser::Parser::token::INT_NUM:
            write_token("INT_NUM");
            return parser::Parser::make_INT_NUM(std::atoi(YYText()), loc);
        case parser::Parser::token::NAME:
            write_token("NAME");
            return parser::Parser::make_NAME(std::string(YYText()), loc);
        case parser::Parser::token::STR_CONST:
            write_token("STR_CONST");
            return parser::Parser::make_STR_CONST(std::string(YYText()), loc);

        case parser::Parser::token::ERROR:
            lexerror();

    }

    return parser::Parser::make_YYEOF(loc);
}

void Lexer::write_token(const std::string &type) {
    outstream << "\tToken Name: " << type
              << "\tLexeme: " << YYText()
              << "\tLineno: " << lineno() << std::endl;
    next_token = type;
}

void Lexer::lexerror() {
    std::cerr << "sclp error: File: " << input_file
              << ", Line: " << lineno()
              << ", Next token: " << next_token
              << ", Lexeme: \"" << YYText()
              << "\"\n\t    Description: Illegal character \'" << YYText()
              << "\'" << std::endl;
    exit(1);
}

void parser::Parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << "syntax error" << std::endl;
    std::cerr << "sclp error: File: " << lexer.input_file
              << ", Line: " << lexer.lineno()
              << ", Next token: " << lexer.next_token
              << ", Lexeme: \"" << lexer.YYText()
              << "\"\n\t    Description: Cannot parse the input program" << std::endl;
    exit(1);
}
