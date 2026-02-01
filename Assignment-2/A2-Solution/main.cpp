#include <iostream>
#include <cstdlib>
#include <argp.h>
#include <cstring>
using namespace std;

extern FILE *yyin;
extern "C" {
    int yylex();
    int yyparse();
}
extern unsigned int line_number;
extern char next_token[20];

int show_tokens = false;
int stop_after_scan = false;
int demo_mode = false;
char input_file[1024] = "";
char toks_file[1024];
FILE* fp = nullptr;

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
        strcpy(input_file, args.input_file);
    }

    if (show_tokens && !demo_mode && args.input_file) {
        snprintf(toks_file, sizeof(toks_file), "%s.toks", args.input_file);
        fp = freopen(toks_file, "w", stdout);
    }
}

int main(int argc, char* argv[]) {
    process_command_options(argc, argv);

    if (stop_after_scan) {
        while (yylex() != 0) {}
        fflush(stdout);
        return 0;
    }

    return yyparse();
}
