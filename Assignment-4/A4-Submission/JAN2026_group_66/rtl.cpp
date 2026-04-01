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

std::string RTL_Int_Const_Opd::get_name() { return std::to_string(value); }

RTL_Label_Opd::RTL_Label_Opd(int num) : label_num(num) {
    opd_type = OpdType::LABEL;
}

std::string RTL_Label_Opd::get_name() {
    return "Label" + std::to_string(label_num);
}

RTL_Register_Opd::RTL_Register_Opd(RegisterDescriptor *rd) : reg_desc(rd) {
    opd_type = OpdType::REGISTER;
}

std::string RTL_Register_Opd::get_name() { return reg_desc->get_name(); }

RTL_String_Const_Opd::RTL_String_Const_Opd(unsigned int str_num, std::string s)
    : string_num(str_num), value(s) {
    opd_type = OpdType::STRING_CONST;
}

std::string RTL_String_Const_Opd::get_name() {
    return "_str_" + std::to_string(string_num);
}

unsigned int RTL_String_Const_Opd::get_string_num() const { return string_num; }

RTL_Var_Opd::RTL_Var_Opd(SymbolTableEntry *e) : entry(e) {
    opd_type = OpdType::VARIABLE;
}

std::string RTL_Var_Opd::get_name() { return entry->get_name(); }

// ---------------------------------------------------------------

Arithmetic_RTL_Stmt::Arithmetic_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                                         std::shared_ptr<RTL_Register_Opd> op1,
                                         std::shared_ptr<RTL_Register_Opd> op2,
                                         ArithmeticOperator opr) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = op1->reg_desc->reg_type == RegisterType::float_num;
}

void Arithmetic_RTL_Stmt::print(std::ostream &out) {
    std::string operator_string = arith_op_to_rtl_string(oper);
    if (isfloat)
        operator_string = operator_string + ".d";

    out << "\t" << operator_string << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << " , " << oper2->get_name() << std::endl;
}

Boolean_RTL_Stmt::Boolean_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                                   std::shared_ptr<RTL_Register_Opd> op1,
                                   std::shared_ptr<RTL_Register_Opd> op2,
                                   BooleanOperator opr) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
}

void Boolean_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << bool_op_to_rtl_string(oper) << ":\t\t" << result->get_name()
        << " <- " << oper1->get_name() << " , " << oper2->get_name()
        << std::endl;
}

Relational_RTL_Stmt::Relational_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                                         std::shared_ptr<RTL_Register_Opd> op1,
                                         std::shared_ptr<RTL_Register_Opd> op2,
                                         RelationalOperator opr) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = false;
}

Relational_RTL_Stmt::Relational_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> op1,
                                         std::shared_ptr<RTL_Register_Opd> op2,
                                         RelationalOperator opr) {
    result = nullptr;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = true;
}

void Relational_RTL_Stmt::print(std::ostream &out) {
    std::string operator_string = rel_op_to_rtl_string(oper);
    if (isfloat)
        operator_string = operator_string + ".d";

    if (!isfloat) {
        out << "\t" << operator_string << ":\t\t" << result->get_name()
            << " <- " << oper1->get_name() << " , " << oper2->get_name()
            << std::endl;
    } else {
        out << "\t" << operator_string << ":\t\t" << oper1->get_name() << " , "
            << oper2->get_name() << std::endl;
    }
}

UMinus_RTL_Stmt::UMinus_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                                 std::shared_ptr<RTL_Register_Opd> op) {
    result = res;
    oper1 = op;
    oper2 = nullptr;

    isfloat = op->reg_desc->reg_type == RegisterType::float_num;
}

void UMinus_RTL_Stmt::print(std::ostream &out) {
    std::string operator_string = isfloat ? "uminus.d" : "uminus";

    out << "\t" << operator_string << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << std::endl;
}

Not_RTL_Stmt::Not_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                           std::shared_ptr<RTL_Register_Opd> op) {
    result = res;
    oper1 = op;
    oper2 = nullptr;
}

void Not_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "not" << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << std::endl;
}

Goto_RTL_Stmt::Goto_RTL_Stmt(std::shared_ptr<RTL_Label_Opd> l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Goto_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "goto:\t\t" << result->get_name() << std::endl;
}

If_Goto_RTL_Stmt::If_Goto_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> r,
                                   std::shared_ptr<RTL_Label_Opd> l) {
    result = l;
    oper1 = r;
    oper2 = nullptr;
}

void If_Goto_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "bgtz:\t\t" << oper1->get_name() << " , "
        << result->get_name() << std::endl;
}

Label_RTL_Stmt::Label_RTL_Stmt(std::shared_ptr<RTL_Label_Opd> l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Label_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << ":" << std::endl;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                                     std::shared_ptr<RTL_Register_Opd> src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    isfloat = (src->reg_desc->reg_type == RegisterType::float_num);

    dest_type = OpdType::REGISTER;
    src_type = OpdType::REGISTER;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(std::shared_ptr<RTL_Var_Opd> dest,
                                     std::shared_ptr<RTL_Register_Opd> src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    SymbolTableEntry *sym_entry = dest->entry;
    isfloat = (sym_entry->get_need_float());

    dest_type = OpdType::VARIABLE;
    src_type = OpdType::REGISTER;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                                     std::shared_ptr<RTL_Var_Opd> src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    SymbolTableEntry *sym_entry = src->entry;
    isfloat = (sym_entry->get_need_float());

    dest_type = OpdType::REGISTER;
    src_type = OpdType::VARIABLE;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> dest,
                                     std::shared_ptr<RTL_Int_Const_Opd> src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    isfloat = false;

    dest_type = OpdType::REGISTER;
    src_type = OpdType::INT_CONST;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(
    std::shared_ptr<RTL_Register_Opd> dest,
    std::shared_ptr<RTL_Double_Const_Opd> src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    isfloat = true;

    dest_type = OpdType::REGISTER;
    src_type = OpdType::DOUBLE_CONST;
}

Transfer_RTL_Stmt::Transfer_RTL_Stmt(
    std::shared_ptr<RTL_Register_Opd> dest,
    std::shared_ptr<RTL_String_Const_Opd> src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;

    isfloat = false;

    dest_type = OpdType::REGISTER;
    src_type = OpdType::STRING_CONST;
}

void Transfer_RTL_Stmt::print(std::ostream &out) {
    std::string instruction_name;

    if (dest_type == OpdType::REGISTER && src_type == OpdType::REGISTER) {
        instruction_name = "move";
        if (isfloat)
            instruction_name += ".d";
    } else if ((dest_type == OpdType::VARIABLE || dest_type == OpdType::TEMPORARY) &&
               src_type == OpdType::REGISTER) {
        instruction_name = "store";
        if (isfloat)
            instruction_name += ".d";
    } else if (dest_type == OpdType::REGISTER &&
               (src_type == OpdType::VARIABLE || src_type == OpdType::TEMPORARY)) {
        instruction_name = "load";
        if (isfloat)
            instruction_name += ".d";
    } else if (dest_type == OpdType::REGISTER && src_type == OpdType::INT_CONST)
        instruction_name = "iLoad";
    else if (dest_type == OpdType::REGISTER &&
             src_type == OpdType::DOUBLE_CONST)
        instruction_name = "iLoad.d";
    else if (dest_type == OpdType::REGISTER &&
             src_type == OpdType::STRING_CONST)
        instruction_name = "load_addr";
    else
        exit_with_err_msg("kys");

    out << "\t" << instruction_name << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << std::endl;
}

Read_RTL_Stmt::Read_RTL_Stmt() {}

void Read_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "read" << std::endl;
}

Write_RTL_Stmt::Write_RTL_Stmt() {}

void Write_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "write" << std::endl;
}

Mov_RTL_Stmt::Mov_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                           std::shared_ptr<RTL_Register_Opd> opd,
                           unsigned int _flag, bool _movt) {
    result = res;
    oper1 = opd;
    oper2 = nullptr;

    flag = _flag;
    movt = _movt;
}

void Mov_RTL_Stmt::print(std::ostream &out) {
    std::string instruction_name = (movt) ? "movt" : "movf";
    out << "\t" << instruction_name << ":\t\t" << result->get_name() << " <- "
        << oper1->get_name() << " , " << flag << std::endl;
}

RTL::RTL() {
    string_const_num = 0;
    machine_descriptor = new MachineDescriptor();
}

RTL::~RTL() { delete machine_descriptor; }

void RTL::print(std::ostream &out) {
    for (auto &stmt : rtl_code) {
        stmt->print(out);
    }
}

unsigned int RTL::getStringConstNum(std::string s) {
    if (string_to_int.find(s) == string_to_int.end())
        string_to_int[s] = string_const_num++;

    return string_to_int[s];
}

void RTL::addRTLStatement(std::shared_ptr<RTL_Stmt> stmt) {
    rtl_code.push_back(stmt);
}

bool RTL::isEmpty() { return rtl_code.empty(); }