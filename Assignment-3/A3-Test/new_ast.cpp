#include "new_ast.hpp"

void AST::error(const std::string& err_msg) {
    std::cerr << err_msg << std::endl;
    exit(1);
}

