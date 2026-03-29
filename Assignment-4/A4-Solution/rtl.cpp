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
    return std::to_string(value);
}

RTL_Label_Opd::RTL_Label_Opd(int num) : label_num(num) {
    opd_type = OpdType::LABEL;
}

std::string RTL_Label_Opd::get_name() {
    return "Label" + std::to_string(label_num);
}

RTL_Register_Opd::RTL_Register_Opd(RegisterDescriptor * rd) : reg_desc(rd) {
    opd_type = OpdType::REGISTER;
}

std::string RTL_Register_Opd::get_name() {
    return reg_desc->get_name();
}

RTL_String_Const_Opd::RTL_String_Const_Opd(std::string s) : value(s) {
    opd_type = OpdType::STRING_CONST;

    rtl.addNewStringConst(s);
}

std::string RTL_String_Const_Opd::get_name() {

    return "_str_" + std::to_string(rtl.getStringConstNum(value));
}

RTL_Var_Opd::RTL_Var_Opd(SymbolTableEntry * e) : entry(e) {
    opd_type = OpdType::VARIABLE;
}

std::string RTL_Var_Opd::get_name() {
    return entry->get_name() + "_";
}

// ---------------------------------------------------------------

Arithmetic_RTL_Stmt::Arithmetic_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op1, RTL_Register_Opd * op2, ArithmeticOperator opr, bool is_float) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = is_float;
}

void Arithmetic_RTL_Stmt::print(std::ostream & out) {
    std::string operator_string = arith_op_to_rtl_string(oper);
    if (isfloat)
        operator_string = operator_string + ".d";

    out << operator_string << ":\t\t" << result->get_name() << " <- " 
        << oper1->get_name() << " , " << oper2->get_name() << std::endl;
}

Boolean_RTL_Stmt::Boolean_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op1, RTL_Register_Opd * op2, BooleanOperator opr) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
}

void Boolean_RTL_Stmt::print(std::ostream & out) {
    out << bool_op_to_rtl_string(oper) << ":\t\t" << result->get_name() << " - > "
        << oper1->get_name() << " , " << oper2->get_name() << std::endl;
}

Relational_RTL_Stmt::Relational_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op1, RTL_Register_Opd * op2, RelationalOperator opr, bool is_float) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = is_float;
}

void Relational_RTL_Stmt::print(std::ostream & out) {
    std::string operator_string = rel_op_to_rtl_string(oper);
    if (isfloat)
        operator_string = operator_string + ".d";

    out << operator_string << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << " , " << oper2->get_name() << std::endl;
}

UMinus_RTL_Stmt::UMinus_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op, bool is_float) {
    result = res;
    oper1 = op;
    oper2 = nullptr;

    isfloat = is_float;
}

void UMinus_RTL_Stmt::print(std::ostream & out) {
    std::string operator_string = isfloat ? "uminus.d" : "uminus";

    out << operator_string << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << std::endl;
}

Not_RTL_Stmt::Not_RTL_Stmt(RTL_Register_Opd * res, RTL_Register_Opd * op) {
    result = res;
    oper1 = op;
    oper2 = nullptr;
}

void Not_RTL_Stmt::print(std::ostream & out) {
    out << "not" << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << std::endl;
}

Goto_RTL_Stmt::Goto_RTL_Stmt(RTL_Label_Opd * l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Goto_RTL_Stmt::print(std::ostream & out) {
    out << "goto:\t\t" << result->get_name() << std::endl;
}

If_Goto_RTL_Stmt::If_Goto_RTL_Stmt(RTL_Register_Opd * r, RTL_Label_Opd * l) {
    result = l;
    oper1 = r;
    oper2 = nullptr;
}

void If_Goto_RTL_Stmt::print(std::ostream & out) {
    out << "bgtz:\t\t" << oper1->get_name() << " , " << result->get_name() << std::endl;
}

Label_RTL_Stmt::Label_RTL_Stmt(RTL_Label_Opd * l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Label_RTL_Stmt::print(std::ostream & out) {
    out << result->get_name() << ":" << std::endl;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Register_Opd * src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    isfloat = (src->reg_desc->reg_type == RegisterType::float_num);

    dest_type = OpdType::REGISTER;
    src_type = OpdType::REGISTER;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(RTL_Var_Opd * dest, RTL_Register_Opd * src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    SymbolTableEntry * sym_entry = dest->entry;
    isfloat = (sym_entry->type.base == BaseType::FLOAT);

    dest_type = OpdType::VARIABLE;
    src_type = OpdType::REGISTER;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Var_Opd * src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    SymbolTableEntry * sym_entry = src->entry;
    isfloat = (sym_entry->type.base == BaseType::FLOAT);

    dest_type = OpdType::REGISTER;
    src_type = OpdType::VARIABLE;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Int_Const_Opd * src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    dest_type = OpdType::REGISTER;
    src_type = OpdType::INT_CONST;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(RTL_Register_Opd * dest, RTL_Double_Const_Opd * src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    dest_type = OpdType::REGISTER;
    src_type = OpdType::DOUBLE_CONST;
}

void Transfer_RTL_Stmt::print(std::ostream & out) {
    std::string instruction_name;

    if (dest_type == OpdType::REGISTER && src_type == OpdType::REGISTER){
        instruction_name = "move";
        if (isfloat) instruction_name += ".d";
    }
    else if (dest_type == OpdType::VARIABLE && src_type == OpdType::REGISTER){
        instruction_name = "store";
        if (isfloat) instruction_name += ".d";
    }
    else if (dest_type == OpdType::REGISTER && src_type == OpdType::VARIABLE){
        instruction_name = "load";
        if (isfloat) instruction_name += ".d";
    }
    else if (dest_type == OpdType::REGISTER && src_type == OpdType::INT_CONST)
        instruction_name = "iLoad";
    else if (dest_type == OpdType::REGISTER && src_type == OpdType::DOUBLE_CONST)
        instruction_name = "iLoad.d";
    else
        exit_with_err_msg("kys");

    out << instruction_name << ":\t\t" << result->get_name() << " <- " << oper1->get_name() << std::endl;
}

Read_RTL_Stmt::Read_RTL_Stmt() {

}

void Read_RTL_Stmt::print(std::ostream & out) {
    out << "read" << std::endl;
}

Write_RTL_Stmt::Write_RTL_Stmt() {

}

void Write_RTL_Stmt::print(std::ostream & out) {
    out << "write" << std::endl;
}

RTL::RTL() {
    string_const_num = 0;
    machine_descriptor = new MachineDescriptor();
}

RTL::~RTL() {
    for (RTL_Stmt * stmt : rtl_code) delete stmt;

    delete machine_descriptor;
}

void RTL::addNewStringConst(std::string s) {
    if (string_to_int.find(s) != string_to_int.end()) return;

    string_to_int[s] = string_const_num++;
}

unsigned int RTL::getStringConstNum(std::string s) {
    return string_to_int[s];
}

void RTL::addRTLStatement(RTL_Stmt * stmt) {
    rtl_code.push_back(stmt);
}
