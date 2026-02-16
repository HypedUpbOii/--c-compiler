#include "defs.hpp"

extern FILE* yyin;
parser::Parser::symbol_type yylex(Lexer&);

int main(int argc, char* argv[]) {
    ArgumentHandler args_handler(argc, argv);
    const Arguments args = args_handler.return_arguments();
    OutputHandler out_handler(args);
    std::ostream& tok_stream = out_handler.tokenStream();
    std::ostream& ast_stream = out_handler.astStream();

    FILE* file = fopen(args.input_file.c_str(), "r");
    if (!file) {
        std::cerr << "sclp error: File: " << args.input_file << " Cannot open the input file" << std::endl;
        return 1;
    }
    yyin = file;

    Lexer lexer(args.input_file, tok_stream);
    if (args.stop_after_scan) {
        while (true) {
            auto sym = yylex(lexer);
            if (sym.kind() == parser::Parser::symbol_kind::S_YYEOF)
                break;
        }
        out_handler.commitTokens();
        fclose(file);
        return 0;
    }

    parser::Parser parser(lexer);

    int result = parser.parse();
    if (args.stop_after_parse) {
        fclose(file);
        return result;
    }

    out_handler.commitAst();
    fclose(file);
    return 0;
}
