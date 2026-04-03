#include "spim.hpp"

void SPIM::print(std::ostream &out) {
    for (auto &stmt : assembly_code)
        stmt->print(out);
}