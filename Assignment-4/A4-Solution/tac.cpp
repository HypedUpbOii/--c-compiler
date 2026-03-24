#include "tac.hpp"

unsigned int TAC::label_number = 0;

Temporary_TAC_Opd *TAC::genNewTemporary() {
    return new Temporary_TAC_Opd(temp_number++);
}

Temporary_TAC_Opd *TAC::genNewSTemporary() {
    return new Temporary_TAC_Opd(stemp_number++, true);
}

Label_TAC_Opd *TAC::genNewLabel() { return new Label_TAC_Opd(label_number++); }

TAC::TAC() : temp_number(0), stemp_number(0) {}

void TAC::addTACStatements(const std::vector<TAC_Stmt *> &stmts) {
    for (auto stmt : stmts) {
        tac_code.push_back(stmt);
    }
}

void TAC::print(std::ostream &out) {
    for (auto stmt : tac_code) {
        stmt->print(out);
    }
}

bool TAC::isEmpty() { return tac_code.empty(); }

TAC::~TAC() {
    for (auto ptr : tac_code) {
        delete ptr;
    }
}

Double_Const_TAC_Opd::Double_Const_TAC_Opd(double val) : value(val) {
    opd_type = OpdType::DOUBLE_CONST;
}

std::string Double_Const_TAC_Opd::get_name() {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    return std::string(buffer);
}

Int_Const_TAC_Opd::Int_Const_TAC_Opd(int val) : value(val) {
    opd_type = OpdType::INT_CONST;
}

std::string Int_Const_TAC_Opd::get_name() { return std::to_string(value); }

String_Const_TAC_Opd::String_Const_TAC_Opd(const std::string &val)
    : value(val) {
    opd_type = OpdType::STRING_CONST;
}

std::string String_Const_TAC_Opd::get_name() { return value; }

Label_TAC_Opd::Label_TAC_Opd(unsigned int num) : label_num(num) {
    opd_type = OpdType::LABEL;
}

std::string Label_TAC_Opd::get_name() {
    return "Label" + std::to_string(label_num);
}

Temporary_TAC_Opd::Temporary_TAC_Opd(unsigned int num, bool special)
    : temp_num(num), is_special(special) {
    opd_type = OpdType::TEMPORARY;
}

std::string Temporary_TAC_Opd::get_name() {
    return (is_special ? "stemp" : "temp") + std::to_string(temp_num);
}

Variable_TAC_Opd::Variable_TAC_Opd(SymbolTableEntry *entry)
    : symtab_entry(entry) {
    opd_type = OpdType::VARIABLE;
}

std::string Variable_TAC_Opd::get_name() {
    return symtab_entry->get_name() + "_";
}

Asgn_TAC_Stmt::Asgn_TAC_Stmt(TAC_Opd *dest, TAC_Opd *src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;
}

void Asgn_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name()
        << std::endl;
}

Bool_Comp_TAC_Stmt::Bool_Comp_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1,
                                       BooleanOperator b_op, TAC_Opd *op2)
    : op(b_op) {
    result = res;
    oper1 = op1;
    oper2 = op2;
}

void Bool_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name() << " "
        << bool_op_to_symbol(op) << " " << oper2->get_name() << std::endl;
}

Arith_Comp_TAC_Stmt::Arith_Comp_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1,
                                         ArithmeticOperator a_op, TAC_Opd *op2)
    : op(a_op) {
    result = res;
    oper1 = op1;
    oper2 = op2;
}

void Arith_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name() << " "
        << arith_op_to_symbol(op) << " " << oper2->get_name() << std::endl;
}

Rel_Comp_TAC_Stmt::Rel_Comp_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1,
                                     RelationalOperator r_op, TAC_Opd *op2)
    : op(r_op) {
    result = res;
    oper1 = op1;
    oper2 = op2;
}

void Rel_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name() << " "
        << rel_op_to_symbol(op) << " " << oper2->get_name() << std::endl;
}

Unary_Comp_TAC_Stmt::Unary_Comp_TAC_Stmt(TAC_Opd *res, UnaryOperator u_op,
                                         TAC_Opd *oper)
    : op(u_op) {
    result = res;
    oper1 = oper;
    oper2 = nullptr;
}

void Unary_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << unary_op_to_symbol(op) << " "
        << oper1->get_name() << std::endl;
}

Goto_TAC_Stmt::Goto_TAC_Stmt(Label_TAC_Opd *l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Goto_TAC_Stmt::print(std::ostream &out) {
    out << "\tgoto " << result->get_name() << std::endl;
}

If_Goto_TAC_Stmt::If_Goto_TAC_Stmt(TAC_Opd *cond,
                                   Label_TAC_Opd *label) {
    result = cond;
    oper1 = label;
    oper2 = nullptr;
}

void If_Goto_TAC_Stmt::print(std::ostream &out) {
    out << "\tif(" << result->get_name() << ") goto " << oper1->get_name()
        << std::endl;
}

IO_TAC_Stmt::IO_TAC_Stmt(bool write, TAC_Opd *oper) : is_write(write) {
    result = oper;
    oper1 = nullptr;
    oper2 = nullptr;
}

void IO_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << (is_write ? "write  " : "read  ") << result->get_name()
        << std::endl;
}

Label_TAC_Stmt::Label_TAC_Stmt(Label_TAC_Opd *label) {
    result = label;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Label_TAC_Stmt::print(std::ostream &out) {
    out << result->get_name() << ":" << std::endl;
}
