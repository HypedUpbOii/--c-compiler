#include "spim.hpp"

ASM_Double_Const_Opd::ASM_Double_Const_Opd(double v) : value(v) {
    opd_type = OpdType::DOUBLE_CONST;
}

std::string ASM_Double_Const_Opd::get_name() {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    return std::string(buffer);
}

ASM_Int_Const_Opd::ASM_Int_Const_Opd(int v) : value(v) {
    opd_type = OpdType::INT_CONST;
}

std::string ASM_Int_Const_Opd::get_name() { return std::to_string(value); }

ASM_Label_Opd::ASM_Label_Opd(unsigned int num) : label_num(num) {
    opd_type = OpdType::LABEL;
}

ASM_Label_Opd::ASM_Label_Opd(std::string name) : label_name(name) {
    opd_type = OpdType::LABEL;
}

std::string ASM_Label_Opd::get_name() {
    if (label_name.empty())
        return "Label" + std::to_string(label_num);
    else
        return label_name;
}

ASM_Register_Opd::ASM_Register_Opd(RegisterDescriptor *rd) : reg_desc(rd) {
    opd_type = OpdType::REGISTER;
}

std::string ASM_Register_Opd::get_name() { return "$" + reg_desc->get_name(); }

ASM_String_Const_Opd::ASM_String_Const_Opd(unsigned int str_num, std::string s)
    : string_num(str_num), value(s) {
    opd_type = OpdType::STRING_CONST;
}

std::string ASM_String_Const_Opd::get_name() {
    return "_str_" + std::to_string(string_num);
}

ASM_Mem_Opd::ASM_Mem_Opd(int offset, RegisterDescriptor *r)
    : fp_offset(offset), rd(r) {
    opd_type = OpdType::VARIABLE;
}

ASM_Mem_Opd::ASM_Mem_Opd(std::string nam) : name(nam) {}

std::string ASM_Mem_Opd::get_name() {
    if (name.empty())
        return std::to_string(fp_offset) + "($" + rd->get_name() + ")";
    else
        return name;
}

// ------------------------------------------------------------------------

Arithmetic_ASM_Stmt::Arithmetic_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                                         std::shared_ptr<ASM_Register_Opd> op1,
                                         std::shared_ptr<ASM_Opd> op2,
                                         ArithmeticOperator opr) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = op1->reg_desc->reg_type == RegisterType::float_num;
}

void Arithmetic_ASM_Stmt::print(std::ostream &out) {
    std::string operator_string = arith_op_to_rtl_string(oper);
    if (isfloat)
        operator_string = operator_string + ".d";

    out << "\t" << operator_string << " " << result->get_name() << ", "
        << oper1->get_name() << ", " << oper2->get_name() << std::endl;
}

Boolean_ASM_Stmt::Boolean_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                                   std::shared_ptr<ASM_Register_Opd> op1,
                                   std::shared_ptr<ASM_Register_Opd> op2,
                                   BooleanOperator opr) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
}

void Boolean_ASM_Stmt::print(std::ostream &out) {
    out << "\t" << bool_op_to_rtl_string(oper) << " " << result->get_name()
        << ", " << oper1->get_name() << ", " << oper2->get_name() << std::endl;
}

Relational_ASM_Stmt::Relational_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                                         std::shared_ptr<ASM_Register_Opd> op1,
                                         std::shared_ptr<ASM_Register_Opd> op2,
                                         RelationalOperator opr) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = false;
}

Relational_ASM_Stmt::Relational_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> op1,
                                         std::shared_ptr<ASM_Register_Opd> op2,
                                         RelationalOperator opr) {
    result = nullptr;
    oper1 = op1;
    oper2 = op2;

    oper = opr;
    isfloat = true;
}

void Relational_ASM_Stmt::print(std::ostream &out) {
    std::string operator_string = rel_op_to_rtl_string(oper);
    if (isfloat) {
        switch (oper) {
        case RelationalOperator::EQUAL:
        case RelationalOperator::NOT_EQUAL: {
            operator_string = "c.eq.d";
            break;
        }
        case RelationalOperator::LESS_THAN:
        case RelationalOperator::GREATER_THAN_EQUAL: {
            operator_string = "c.lt.d";
            break;
        }
        default: {
            operator_string = "c.le.d";
        }
        }
    }

    if (!isfloat) {
        out << "\t" << operator_string << " " << result->get_name() << ", "
            << oper1->get_name() << ", " << oper2->get_name() << std::endl;
    } else {
        out << "\t" << operator_string << " " << oper1->get_name() << ", "
            << oper2->get_name() << std::endl;
    }
}

UMinus_ASM_Stmt::UMinus_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                                 std::shared_ptr<ASM_Register_Opd> op) {
    result = res;
    oper1 = op;
    oper2 = nullptr;

    isfloat = op->reg_desc->reg_type == RegisterType::float_num;
}

void UMinus_ASM_Stmt::print(std::ostream &out) {
    std::string operator_string = isfloat ? "neg.d" : "neg";

    out << "\t" << operator_string << " " << result->get_name() << ", "
        << oper1->get_name() << std::endl;
}

Not_ASM_Stmt::Not_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> res,
                           std::shared_ptr<ASM_Register_Opd> op) {
    result = res;
    oper1 = op;
    oper2 = nullptr;
}

void Not_ASM_Stmt::print(std::ostream &out) {
    out << "\txori " << result->get_name() << ", " << oper1->get_name() << ", 1"
        << std::endl;
}

Call_ASM_Stmt::Call_ASM_Stmt(std::shared_ptr<ASM_Label_Opd> func_label) {
    label = func_label;
    reg = nullptr;
}

void Call_ASM_Stmt::print(std::ostream &out) {
    out << "\tjal " << label->get_name() << std::endl;
}

Goto_ASM_Stmt::Goto_ASM_Stmt(std::shared_ptr<ASM_Label_Opd> l) {
    label = l;
    reg = nullptr;
}

void Goto_ASM_Stmt::print(std::ostream &out) {
    out << "\tj " << label->get_name() << std::endl;
}

If_Goto_ASM_Stmt::If_Goto_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> r,
                                   std::shared_ptr<ASM_Label_Opd> l) {
    label = l;
    reg = r;
}

void If_Goto_ASM_Stmt::print(std::ostream &out) {
    out << "\tbgtz " << reg->get_name() << ", " << label->get_name()
        << std::endl;
}

Jump_Reg_ASM_Stmt::Jump_Reg_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> r) {
    label = nullptr;
    reg = r;
}

void Jump_Reg_ASM_Stmt::print(std::ostream &out) {
    out << "\tjr " << reg->get_name() << std::endl;
}

Label_ASM_Stmt::Label_ASM_Stmt(std::shared_ptr<ASM_Label_Opd> l) : label(l) {}

void Label_ASM_Stmt::print(std::ostream &out) {
    out << label->get_name() << ":" << std::endl;
}

Move_If_ASM_Stmt::Move_If_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> dst,
                                   std::shared_ptr<ASM_Register_Opd> src,
                                   unsigned int fl, bool is)
    : flag(fl), ist(is) {
    source = src;
    dest = dst;
}

void Move_If_ASM_Stmt::print(std::ostream &out) {
    out << (ist ? "\tmovt " : "\tmovf ") << dest->get_name() << ", "
        << source->get_name() << ", " << flag << std::endl;
}

Move_ASM_Stmt::Move_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> dst,
                             std::shared_ptr<ASM_Register_Opd> src) {
    source = src;
    dest = dst;
    isFloat = dst->reg_desc->reg_type == RegisterType::float_num;
}

void Move_ASM_Stmt::print(std::ostream &out) {
    out << (isFloat ? "\tmov.d " : "\tmove ") << dest->get_name() << ", "
        << source->get_name() << std::endl;
}

Load_Int_Immediate_ASM_Stmt::Load_Int_Immediate_ASM_Stmt(
    std::shared_ptr<ASM_Register_Opd> dst,
    std::shared_ptr<ASM_Int_Const_Opd> src) {
    source = src;
    dest = dst;
}

void Load_Int_Immediate_ASM_Stmt::print(std::ostream &out) {
    out << "\tli " << dest->get_name() << ", " << source->get_name()
        << std::endl;
}

Load_Double_Immediate_ASM_Stmt::Load_Double_Immediate_ASM_Stmt(
    std::shared_ptr<ASM_Register_Opd> dst,
    std::shared_ptr<ASM_Double_Const_Opd> src) {
    source = src;
    dest = dst;
}

void Load_Double_Immediate_ASM_Stmt::print(std::ostream &out) {
    out << "\tli.d " << dest->get_name() << ", " << source->get_name()
        << std::endl;
}

Load_Mem_ASM_Stmt::Load_Mem_ASM_Stmt(std::shared_ptr<ASM_Register_Opd> dst,
                                     std::shared_ptr<ASM_Mem_Opd> src) {
    source = src;
    dest = dst;
    isFloat = dst->reg_desc->reg_type == RegisterType::float_num;
}

void Load_Mem_ASM_Stmt::print(std::ostream &out) {
    out << (isFloat ? "\tl.d " : "\tlw ") << dest->get_name() << ", "
        << source->get_name() << std::endl;
}

Load_Address_ASM_Stmt::Load_Address_ASM_Stmt(
    std::shared_ptr<ASM_Register_Opd> dst,
    std::shared_ptr<ASM_String_Const_Opd> src) {
    source = src;
    dest = dst;
}

Load_Address_ASM_Stmt::Load_Address_ASM_Stmt(
    std::shared_ptr<ASM_Register_Opd> dst, std::shared_ptr<ASM_Mem_Opd> src) {
    source = src;
    dest = dst;
}

void Load_Address_ASM_Stmt::print(std::ostream &out) {
    out << "\tla " << dest->get_name() << ", " << source->get_name()
        << std::endl;
}

Store_Mem_ASM_Stmt::Store_Mem_ASM_Stmt(std::shared_ptr<ASM_Mem_Opd> dst,
                                       std::shared_ptr<ASM_Register_Opd> src) {
    source = src;
    dest = dst;
    isFloat = src->reg_desc->reg_type == RegisterType::float_num;
}

void Store_Mem_ASM_Stmt::print(std::ostream &out) {
    out << (isFloat ? "\ts.d " : "\tsw ") << source->get_name() << ", "
        << dest->get_name() << std::endl;
}

Syscall_ASM_Stmt::Syscall_ASM_Stmt() {}

void Syscall_ASM_Stmt::print(std::ostream &out) {
    out << "\tsyscall" << std::endl;
}

// -----------------------------------------------------------
void SPIM::addSPIM(std::shared_ptr<ASM_Stmt> code) {
    assembly_code.push_back(code);
}

void SPIM::print(std::ostream &out) {
    for (auto &stmt : assembly_code)
        stmt->print(out);
}
