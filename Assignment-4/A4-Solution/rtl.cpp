#include "rtl.hpp"
#include <cstdio>

RTL_Double_Const_Opd::RTL_Double_Const_Opd(double v) : value(v) {
    opd_type = OpdType::DOUBLE_CONST;
}

std::string RTL_Double_Const_Opd::get_name() {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    return std::string(buffer);
}

RTL_Int_Const_Opd::RTL_Int_Const_Opd(int v) : value(v) {
    opd_type = OpdType::INT_CONST;
}

std::string RTL_Int_Const_Opd::get_name() {
    return to_string(value);
}

RTL_Label_Opd::RTL_Label_Opd(int num) : label_num(num) {
    opd_type = OpdType::LABEL;
}

std::string RTL_Label_Opd::get_name() {
    return "Label" + std::to_string(label_num);
}

RTL_Register_Opd::RTL_Register_Opd(Register_Descriptor * rd) : reg_desc(rd) {
    opd_type = OpdType::REGISTER;
}

std::string RTL_Register_Opd::get_name() {
    return reg_to_symbols(reg_desc->reg);
}

RTL_String_Const_Opd::RTL_String_Const_Opd(std::string s) : value(s) {
    opd_type = OpdType::STRING_CONST;

    // handle insert into the string table
}

std::string RTL_String_Const_Opd::get_name() {
    // handle retrieval from the string table
    return "";
}

RTL_Var_Opd::RTL_Var_Opd(SymbolTableEntry * e) : entry(e) {
    opd_type = OpdType::VARIABLE;
}

std::string RTL_Var_Opd::get_name() {
    return entry->get_name() + "_";
}

// ---------------------------------------------------------------

Compute_RTL_Stmt::Compute_RTL_Stmt(RTL_Opd * res, RTL_Opd * op1, RTL_Opd * op2) {
    result = res;
    oper1 = op1;
    oper2 = op2;
}

void Compute_RTL_Stmt::print(std::ostream & out) {
    // TODO : write the statement based on the type of operator
}
