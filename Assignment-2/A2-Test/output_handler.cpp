#include "output_hander.hpp"

int NullBuffer::overflow(int c) {
    return c;
}

NullStream::NullStream() : std::ostream(&buffer) {}

static NullStream null_stream;

OutputHandler::OutputHandler(const Arguments& args)
    : demo_mode(args.demo_mode), token_stream(&null_stream), ast_stream(&null_stream) {
    if (args.demo_mode) {
        if (args.show_tokens) {
            token_stream = &std::cout;
        }
        if (args.show_ast)
            ast_stream = &std::cout;
        return;
    }

    if (args.show_tokens) {
        toks_file = args.input_file + ".toks";
        std::ofstream(toks_file, std::ios::trunc);
        token_stream = &token_buffer;
    }

    if (args.show_ast) {
        ast_file = args.input_file + ".ast";
        std::ofstream(ast_file, std::ios::trunc);
        ast_stream = &ast_buffer;
    }
}

std::ostream& OutputHandler::tokenStream() {
    return *token_stream;
}

std::ostream& OutputHandler::astStream() {
    return *ast_stream;
}

void OutputHandler::commitTokens() {
    if (demo_mode)
        return;

    if (!toks_file.empty()) {
        std::ofstream toks(toks_file, std::ios::trunc);
        toks << token_buffer.str();
    }
}

void OutputHandler::commitAst() {
    if (demo_mode)
        return;

    if (!toks_file.empty()) {
        std::ofstream ast(ast_file, std::ios::trunc);
        ast << ast_buffer.str();
    }
}
