#pragma once
#include <argp.h>
#include <iostream>
#include "common_utils.hpp"

class ArgumentHandler {
private:
    static error_t parse_opt(int key, char* arg, struct argp_state* state);
    static struct argp_option options[];
    static struct argp argp;

    Arguments args;
public:
    ArgumentHandler(int argc, char* argv[]);
    const Arguments& return_arguments() const;
};
