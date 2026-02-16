#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <argp.h>
#include "symbol_table.hpp"
using namespace std;

extern FILE *yyin;
extern int yylex();
extern int yyparse();

bool show_tokens = false;
bool stop_after_scan = false;
bool demo_mode = false;
string input_file;
static ostringstream write_buffer;
static ofstream toks_file;

const char* argp_program_version = "Sclp Version: A2";
const char* argp_program_bug_address = "<23b1006@iitb.ac.in/23b1073@iitb.ac.in>";
static char doc[] = "A language processor for C like language";
static char args_doc[] = "[INPUT_FILE]";
static struct argp_option options[] {
    {"sa-scan", 1000, 0, 0, "Stop after scanning"},
    {"show-tokens", 1001, 0, 0, "Show the tokens in FILE.toks (or out.toks)"},
    {"demo", 'd', 0, 0, "Demo version. Use stdout for the output instead of files"},
    {0}
};

struct arguments {
    const char* input_file;
};

static error_t parse_opt(int key, char* arg, struct argp_state *state) {
    struct arguments* args = (struct arguments*)state->input;
    switch(key) {
        case 1000:
            stop_after_scan = true;
            break;

        case 1001:
            show_tokens = true;
            break;

        case 'd':
            demo_mode = true;
            break;

        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                args->input_file = arg;
            } else {
                argp_usage(state);
            }
            break;

        case ARGP_KEY_END:
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {
    options,
    parse_opt,
    args_doc,
    doc
};

void process_command_options(int argc, char* argv[]) {
    struct arguments args;
    args.input_file = nullptr;
    
    argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, &args);

    if (args.input_file) {
        yyin = fopen(args.input_file, "r");
        if (!yyin) {
            cerr << "sclp error: File: " << input_file << " Cannot open the input file" << endl;
            exit(1);
        }
        input_file = args.input_file;
    }

    if (show_tokens && !demo_mode && args.input_file) {
        string toks_file_name = string(args.input_file) + ".toks";
        toks_file = ofstream(toks_file_name);
        std::streambuf* original_cout_buf = std::cout.rdbuf(write_buffer.rdbuf());
    }
}

int main(int argc, char* argv[]) {
    process_command_options(argc, argv);

    if (stop_after_scan) {
        while (yylex() != 0) {}
        toks_file << write_buffer.str();
        toks_file.flush();
        return 0;
    }

    SymbolTable * global_sym_tab = new SymbolTable();
    SymbolTable * curr_sym_tab = global_sym_tab;

    std::string err_msg = "";

    int status = yyparse();
    if (status == 0) {
        toks_file << write_buffer.str();
        toks_file.flush();
    }
    return status;
}
