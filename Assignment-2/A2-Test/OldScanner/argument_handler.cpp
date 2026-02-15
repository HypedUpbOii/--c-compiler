#include "argument_handler.hpp"

const char* argp_program_version = "Sclp Version: A3";
const char* argp_program_bug_address = "<23b1006@iitb.ac.in/23b1073@iitb.ac.in>";

static char doc[] = "A language processor for C like language";
static char args_doc[] = "[FILE]";

argp_option ArgumentHandler::options[] = {
    {"sa-scan", 1000, 0, 0, "Stop after scanning"},
    {"sa-parse", 1001, 0, 0, "Stop after parsing"},
    {"show-tokens", 1002, 0, 0, "Show the tokens in FILE.toks (or out.toks)"},
    {"show-ast", 1003, 0, 0, "Show abstract syntax trees in FILE.ast (or out.ast)"},
    {"demo", 'd', 0, 0, "Demo version. Use stdout for the output instead of files"},
    {0}
};

error_t ArgumentHandler::parse_opt(int key, char* arg, struct argp_state* state) {
    ArgumentHandler* self = static_cast<ArgumentHandler*>(state->input);

    switch (key) {
        case 1000:
            self->args.stop_after_scan = true;
            break;
        
        case 1001:
            self->args.stop_after_parse = true;
            break;

        case 1002:
            self->args.show_tokens = true;
            break;

        case 1003:
            self->args.show_ast = true;
            break;

        case 'd':
            self->args.demo_mode = true;
            break;

        case ARGP_KEY_ARG:
            if (state->arg_num > 0) {
                std::cerr << "Only one file name can be provided." << std::endl;
                argp_usage(state);
            }
            self->args.input_file = arg;
            break;

        case ARGP_KEY_END:
            if (state->arg_num == 0)
                argp_usage(state);
            break;
            
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

argp ArgumentHandler::argp = {
    options,
    parse_opt,
    args_doc,
    doc
};

ArgumentHandler::ArgumentHandler(int argc, char* argv[]) {
    argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, this);
}

const Arguments& ArgumentHandler::return_arguments() const {
    return args;
}