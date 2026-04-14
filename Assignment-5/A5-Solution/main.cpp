#include "argument_handler.hpp"
#include "lexer.hpp"
#include "output_handler.hpp"
#include <fstream>

int main(int argc, char *argv[]) {
    ArgumentHandler args_handler(argc, argv);
    const Arguments args = args_handler.return_arguments();
    OutputHandler out_handler(args);
    std::ostream &tok_stream = out_handler.tokenStream();
    std::ostream &ast_stream = out_handler.astStream();
    std::ostream &tac_stream = out_handler.tacStream();
    std::ostream &rtl_stream = out_handler.rtlStream();
    std::ostream &asm_stream = out_handler.asmStream();

    std::ifstream in(args.input_file);
    if (!in) {
        std::cerr << "sclp error: File: " << args.input_file
                  << " Cannot open the input file" << std::endl;
        return 1;
    }

    Lexer lexer(&in, args.input_file, tok_stream);
    if (args.stop_after_scan) {
        while (true) {
            auto sym = lexer.nextToken();
            if (sym.kind() == parser::Parser::symbol_kind::S_YYEOF)
                break;
        }
        out_handler.commitTokens();

        return 0;
    }

    Program program = Program();
    parser::Parser parser(lexer, program);

    int result = parser.parse();
    out_handler.commitTokens();

    if (args.stop_after_parse)
        return result;

    program.validateProgram();
    program.print(ast_stream);
    out_handler.commitAst();

    if (args.stop_after_ast)
        return 0;

    program.generateTAC();
    program.printTAC(tac_stream);
    out_handler.commitTac();

    if (args.stop_after_tac)
        return 0;

    program.generateRTL();
    program.printRTL(rtl_stream);
    out_handler.commitRtl();

    if (args.stop_after_rtl)
        return 0;

    program.generateSPIM();
    program.printSPIM(asm_stream);
    out_handler.commitAsm();
    return 0;
}
