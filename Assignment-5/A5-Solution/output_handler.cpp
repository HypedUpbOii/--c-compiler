#include "output_handler.hpp"
#include <fstream>
#include <iostream>

int NullBuffer::overflow(int c) { return c; }

NullStream::NullStream() : std::ostream(&buffer) {}

static NullStream null_stream;

OutputHandler::OutputHandler(const Arguments &args)
    : demo_mode(args.demo_mode), token_stream(&null_stream),
      ast_stream(&null_stream), tac_stream(&null_stream),
      rtl_stream(&null_stream), asm_stream(&null_stream) {
    if (args.demo_mode) {
        if (args.show_tokens)
            token_stream = &std::cout;
        if (args.show_ast)
            ast_stream = &std::cout;
        if (args.show_tac)
            tac_stream = &std::cout;
        if (args.show_rtl)
            rtl_stream = &std::cout;
        if (args.show_asm)
            asm_stream = &std::cout;
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

    if (args.show_tac) {
        tac_file = args.input_file + ".tac";
        std::ofstream(tac_file, std::ios::trunc);
        tac_stream = &tac_buffer;
    }

    if (args.show_rtl) {
        rtl_file = args.input_file + ".rtl";
        std::ofstream(rtl_file, std::ios::trunc);
        rtl_stream = &rtl_buffer;
    }

    if (args.show_asm) {
        asm_file = args.input_file + ".spim";
        std::ofstream(asm_file, std::ios::trunc);
        asm_stream = &asm_buffer;
    }
}

std::ostream &OutputHandler::tokenStream() { return *token_stream; }

std::ostream &OutputHandler::astStream() { return *ast_stream; }

std::ostream &OutputHandler::tacStream() { return *tac_stream; }

std::ostream &OutputHandler::rtlStream() { return *rtl_stream; }

std::ostream &OutputHandler::asmStream() { return *asm_stream; }

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

    if (!ast_file.empty()) {
        std::ofstream ast(ast_file, std::ios::trunc);
        ast << ast_buffer.str();
    }
}

void OutputHandler::commitTac() {
    if (demo_mode)
        return;

    if (!tac_file.empty()) {
        std::ofstream tac(tac_file, std::ios::trunc);
        tac << tac_buffer.str();
    }
}

void OutputHandler::commitRtl() {
    if (demo_mode)
        return;

    if (!rtl_file.empty()) {
        std::ofstream rtl(rtl_file, std::ios::trunc);
        rtl << rtl_buffer.str();
    }
}

void OutputHandler::commitAsm() {
    if (demo_mode)
        return;

    if (!asm_file.empty()) {
        std::ofstream spim(asm_file, std::ios::trunc);
        spim << asm_buffer.str();
    }
}
