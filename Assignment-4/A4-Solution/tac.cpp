#include "tac.hpp"
#include "rtl.hpp"

unsigned int TAC::label_number = 0;

Temporary_TAC_Opd *TAC::genNewTemporary(bool need_float) {
    return new Temporary_TAC_Opd(temp_number++, false, need_float);
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

Temporary_TAC_Opd::Temporary_TAC_Opd(unsigned int num, bool special, bool need_float)
    : temp_num(num), is_special(special) {
    opd_type = OpdType::TEMPORARY;
    needfloat = need_float;
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

// ------------------------------------------------------------------------------

Asgn_TAC_Stmt::Asgn_TAC_Stmt(TAC_Opd *dest, TAC_Opd *src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;
}

void Asgn_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name()
        << std::endl;
}

void Asgn_TAC_Stmt::generateRTL(RTL & __rtl) {

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

void Bool_Comp_TAC_Stmt::generateRTL(RTL & __rtl) {

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

void Arith_Comp_TAC_Stmt::generateRTL(RTL & __rtl) {

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

void Rel_Comp_TAC_Stmt::generateRTL(RTL & __rtl) {

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

void Unary_Comp_TAC_Stmt::generateRTL(RTL & __rtl) {

}

Goto_TAC_Stmt::Goto_TAC_Stmt(Label_TAC_Opd *l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Goto_TAC_Stmt::print(std::ostream &out) {
    out << "\tgoto " << result->get_name() << std::endl;
}

void Goto_TAC_Stmt::generateRTL(RTL & __rtl) {

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

void If_Goto_TAC_Stmt::generateRTL(RTL & __rtl) {
    bool istemp = result->opd_type == OpdType::TEMPROARY;

    Label_TAC_Opd * label_tac_opd = (Label_TAC_Opd *) oper1;
    RTL_Label_Opd * l = new RTL_Label_Opd(label_tac_opd->label_num);

    if (!istemp) {
        // load into temp
        RegisterDescriptor * rd = __rtl.machine_descriptor->get_new_register<int_reg>();
        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(rd);
        
        Variable_TAC_Opd * var_tac_opd = (Variable_TAC_Opd *) result;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(result->symtab_entry);

        // load statement
        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(reg_opd, var_opd);
        __rtl.addRTLStatement(load_stmt);

        // use register to construct the if-goto statement
        RTL_Register_Opd * reg = new RTL_Register_Opd(rd);
        If_Goto_RTL_Stmt * if_goto_stmt = new If_Goto_RTL_Stmt(reg, l);

    }
    else {
        // need to get register descriptor corresponding to that temp number
        Temporary_TAC_Opd * temp_tac_opd = (Temporary_TAC_Opd *) result;
        RegisterDescriptor * rd = __rtl.machine_descriptor->get_rd_for_temp(result->temp_num);

        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(rd);

        If_Goto_RTL_Stmt * if_goto_stmt = new If_Goto_RTL_Stmt(reg_opd, l);

    }
    
    __rtl.addRTLStatement(if_goto_stmt);
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

void IO_TAC_Stmt::generateRTL(RTL & __rtl) {
    if (is_write) {
        bool is_temp = result->opd_type == OpdType::TEMPORARY;
        Variable_TAC_Opd * var = (Variable_TAC_Opd *) result;
        Temporary_TAC_Opd * temp = (Temporary_TAC_Opd *) result;

        bool needfloat = (!is_temp && var->symtab_entry->type.base == BaseType::FLOAT) || 
                         (is_temp && temp->need_float);

        // load correct value into v0
        RTL_Int_Const_Opd * num;
        RegisterDescriptor * rd = __rtl.machine_descriptor->get_register(Register::v0);
        RTL_Register_Opd * reg = new RTL_Register_Opd(rd);
        if (!needfloat) {
            num = new RTL_Int_Const_Opd(1);
        }
        else if (needfloat) {
            num = new RTL_Int_Const_Opd(3);
        }
        Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg, num);
        __rtl.addRTLStatement(iload_stmt);

        // load into correct register
        if (!needfloat) {
            rd = __rtl.machine_descriptor->get_register(Register::a0);
        }
        else if (needfloat) {
            rd = __rtl.machine_descriptor->get_register(Register::f12);
        }
        RTL_Register_Opd * reg_to_load = (is_temp) ? temp : var;
        reg = new RTL_Register_Opd(reg_to_load);
        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(reg, reg_to_load);
        __rtl.addRTLStatement(load_stmt);

        // write instruction
        Write_RTL_Stmt * write_stmt = new Write_RTL_Stmt();
        __rtl.addRTLStatement(write_stmt);  
    }
    else {
        Variable_TAC_Opd * var_tac_opd = (Variable_TAC_Opd *) result;

        RTL_Int_Const_Opd * num;
        RegisterDescriptor * rd = __rtl.machine_descriptor->get_register(Register::v0);
        RTL_Register_Opd * reg = new RTL_Register_Opd(rd);

        // choose appropriate value to load into v0
        if (var_tac_opd->symtab_entry->type.base == BaseType::INT) {
            num = new RTL_Int_Const_Opd(5);
        }
        else if (var_tac_opd->symtab_entry->type.base == BaseType::FLOAT) {
            num = new RTL_Int_Const_Opd(7);
        }

        Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg, num);
        __rtl.addRTLStatement(iload_stmt);


        Read_RTL_Stmt * read_stmt = new Read_RTL_Stmt();
        __rtl.addRTLStatement(read_stmt);
        
        RTL_Var_Opd * var_rtl_opd = new RTL_Var_Opd(var_tac_opd->symtab_entry);

        // select appropriate register to move result from
        if (var_tac_opd->symtab_entry->type.base == BaseType::INT) {
            rd = __rtl.machine_descriptor->get_register(Register::v0);
        }
        else if (var_tac_opd->symtab_entry->type.base == BaseType::FLOAT) {
            rd = __rtl.machine_descriptor->get_register(Register::f0);
        }

        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(rd);

        Transfer_RTL_Stmt * store_stmt = new Transfer_RTL_Stmt(var_rtl_opd, reg_opd);
        __rtl.addRTLStatement(store_stmt);
    }
}

Label_TAC_Stmt::Label_TAC_Stmt(Label_TAC_Opd *label) {
    result = label;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Label_TAC_Stmt::print(std::ostream &out) {
    out << result->get_name() << ":" << std::endl;
}

void Label_TAC_Stmt::generateRTL(RTL & __rtl) {
    Label_TAC_Opd * temp = (Label_TAC_Opd *) result;
    RTL_Label_Opd * l = new RTL_Label_Opd(result->label_num);

    Label_RTL_Stmt * stmt = new Label_RTL_Stmt(l);

    __rtl.addRTLStatement(stmt);
}
