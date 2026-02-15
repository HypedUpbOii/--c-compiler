#include "defs.hpp"

int check_ast() {
    return 0;
}

int main(int argc, char* argv[]) {
    ArgumentHandler args_handler(argc, argv);
    const Arguments args = args_handler.return_arguments();
    OutputHandler out_handler(args);
    std::ostream& tok_stream = out_handler.tokenStream();
    std::ostream& ast_stream = out_handler.astStream();

    std::ifstream in(args.input_file);
    if (!in) {
        std::cerr << "sclp error: File: " << args.input_file << " Cannot open the input file" << std::endl;
        return 1;
    }

    Lexer lexer(&in, tok_stream);
    if (args.stop_after_scan) {
        while (true) {
            auto sym = lexer.yylex();
            if (sym.kind() == parser::Parser::symbol_kind::S_YYEOF)
                break;
        }
        out_handler.commitTokens();
        return 0;
    }

    parser::Parser parser(lexer);

    int result = parser.parse();
    if (args.stop_after_parse) {
        return result;
    }

    out_handler.commitAst();
    return check_ast();
}
