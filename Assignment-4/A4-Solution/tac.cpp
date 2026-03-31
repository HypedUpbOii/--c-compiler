#include "tac.hpp"
#include "rtl.hpp"

unsigned int TAC::label_number = 0;

Label_TAC_Opd *TAC::getRetLabel() {
    return new Label_TAC_Opd(label_number++);
}

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

OpdType TAC_Opd::get_opd_type() const {
    return opd_type;
}

Double_Const_TAC_Opd::Double_Const_TAC_Opd(double val) : value(val) {
    opd_type = OpdType::DOUBLE_CONST;
}

std::string Double_Const_TAC_Opd::get_name() {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    return std::string(buffer);
}

double Double_Const_TAC_Opd::get_value() const {
    return value;
}

Int_Const_TAC_Opd::Int_Const_TAC_Opd(int val) : value(val) {
    opd_type = OpdType::INT_CONST;
}

std::string Int_Const_TAC_Opd::get_name() { return std::to_string(value); }

int Int_Const_TAC_Opd::get_value() const {
    return value;
}

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

unsigned int Label_TAC_Opd::get_label_num() const {
    return label_num;
}

Temporary_TAC_Opd::Temporary_TAC_Opd(unsigned int num, bool special, bool need_float)
    : temp_num(num), is_special(special) {
    opd_type = OpdType::TEMPORARY;
    needfloat = need_float;
}

std::string Temporary_TAC_Opd::get_name() {
    return (is_special ? "stemp" : "temp") + std::to_string(temp_num);
}

unsigned int Temporary_TAC_Opd::get_temp_num() const {
    return temp_num;
}

bool Temporary_TAC_Opd::get_need_float() const {
    return needfloat;
}

Variable_TAC_Opd::Variable_TAC_Opd(SymbolTableEntry *entry)
    : symtab_entry(entry) {
    opd_type = OpdType::VARIABLE;
}

std::string Variable_TAC_Opd::get_name() {
    return symtab_entry->get_name();
}

Function_TAC_Opd::Function_TAC_Opd(SymbolTableFunction *entry,
                                   std::vector<TAC_Opd *> args)
    : symtab_entry(entry), params(args) {
    opd_type = OpdType::FUNCTION;
}

std::string Function_TAC_Opd::get_name() {
    std::string ret = "";
    ret += symtab_entry->get_name() + "(";
    if (!params.empty()) {
        for (auto it = params.begin(); it != std::prev(params.end()); ++it) {
            ret += (*it)->get_name() + ", ";
        }
        ret += params.back()->get_name();
    }
    ret += ")";
    return ret;
}

SymbolTableEntry * Variable_TAC_Opd::get_sym_tab_entry() const {
    return symtab_entry;
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

Call_TAC_Stmt::Call_TAC_Stmt(TAC_Opd *func) {
    result = func;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Call_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << std::endl;
}

void Asgn_TAC_Stmt::generateRTL(RTL & __rtl) {
    bool is_result_temp = result->get_opd_type() == OpdType::TEMPORARY;

    bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
    bool is_oper1_int_const = oper1->get_opd_type() == OpdType::INT_CONST;
    bool is_oper1_float_const = oper1->get_opd_type() == OpdType::DOUBLE_CONST;

    RegisterDescriptor * oper1_rd;
    if (is_oper1_temp) {
        oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
    }
    else if (is_oper1_var) {
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper1;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());
        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(oper1_rd);
        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(reg_opd, var_opd);

        __rtl.addRTLStatement(load_stmt);
    }
    else if (is_oper1_int_const) {
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        Int_Const_TAC_Opd * intconst = (Int_Const_TAC_Opd *) oper1;
        RTL_Int_Const_Opd * intconst_opd = new RTL_Int_Const_Opd(intconst->get_value());
        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(oper1_rd);
        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(reg_opd, intconst_opd);

        __rtl.addRTLStatement(load_stmt);
    }
    else if (is_oper1_float_const) {
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        Double_Const_TAC_Opd * floatconst = (Double_Const_TAC_Opd *) oper1;
        RTL_Double_Const_Opd * floatconst_opd = new RTL_Double_Const_Opd(floatconst->get_value());
        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(oper1_rd);
        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(reg_opd, floatconst_opd);

        __rtl.addRTLStatement(load_stmt);
    }
    RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);

    RegisterDescriptor * res_rd;
    // to be safe, don't think this case would occur
    if (is_result_temp) {
        res_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(result);
        RTL_Register_Opd * res_reg_opd = new RTL_Register_Opd(res_rd);

        Transfer_RTL_Stmt * assn_stmt = new Transfer_RTL_Stmt(res_reg_opd, oper1_reg_opd);
        __rtl.addRTLStatement(assn_stmt);

        res_rd->set_used_for_expr_return();
    }
    else {
        Variable_TAC_Opd * var = (Variable_TAC_Opd *) result;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());

        Transfer_RTL_Stmt * assn_stmt = new Transfer_RTL_Stmt(var_opd, oper1_reg_opd);
        __rtl.addRTLStatement(assn_stmt);
    }

    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
    oper1_rd->reset_used_for_expr_return();
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
    bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;

    RegisterDescriptor * oper1_rd;

    if (is_oper1_temp) {
        oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
    }
    else {
        // load
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper1_rd);

        Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper1;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);

    RegisterDescriptor * result_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
    RTL_Register_Opd * result_reg_opd = new RTL_Register_Opd(result_rd);

    RegisterDescriptor * oper2_rd;
    if (is_oper2_temp) {
        oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
    }
    else {
        // load
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper2_rd);

        Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper2;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);

    Boolean_RTL_Stmt * bool_comp_stmt = new Boolean_RTL_Stmt(result_reg_opd, oper1_reg_opd, oper2_reg_opd, op);
    __rtl.addRTLStatement(bool_comp_stmt);

    result_rd->set_used_for_expr_return();
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
    oper1_rd->reset_used_for_expr_return();
    oper2_rd->reset_used_for_expr_return();
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
    bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
    bool is_oper1_int_const = oper1->get_opd_type() == OpdType::INT_CONST;
    bool is_oper1_float_const = oper1->get_opd_type() == OpdType::DOUBLE_CONST;

    bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper2_var = oper2->get_opd_type() == OpdType::VARIABLE;
    bool is_oper2_int_const = oper2->get_opd_type() == OpdType::INT_CONST;
    bool is_oper2_float_const = oper2->get_opd_type() == OpdType::DOUBLE_CONST;

    RegisterDescriptor * oper1_rd;
    if (is_oper1_temp) {
        oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
    }
    else if (is_oper1_var) {
        // load from variable
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper1_rd);

        Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper1;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    }
    else if (is_oper1_int_const) {
        // load from int const
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper1_rd);

        Int_Const_TAC_Opd * intconst = (Int_Const_TAC_Opd *) oper1;
        RTL_Int_Const_Opd * intconst_opd = new RTL_Int_Const_Opd(intconst->get_value());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, intconst_opd);
        __rtl.addRTLStatement(load_stmt);
    }
    else if (is_oper1_float_const) {
        // load from float const
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper1_rd);

        Double_Const_TAC_Opd * floatconst = (Double_Const_TAC_Opd *) oper1;
        RTL_Double_Const_Opd * floatconst_opd = new RTL_Double_Const_Opd(floatconst->get_value());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, floatconst_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);

    RegisterDescriptor * result_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
    RTL_Register_Opd * result_reg_opd = new RTL_Register_Opd(result_rd);

    RegisterDescriptor * oper2_rd;
    if (is_oper2_temp) {
        oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
    }
    else if (is_oper2_var) {
        // load from variable
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper2_rd);

        Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper2;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    }
    else if (is_oper2_int_const) {
        // load from int const
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper2_rd);

        Int_Const_TAC_Opd * intconst = (Int_Const_TAC_Opd *) oper2;
        RTL_Int_Const_Opd * intconst_opd = new RTL_Int_Const_Opd(intconst->get_value());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, intconst_opd);
        __rtl.addRTLStatement(load_stmt);
    }
    else if (is_oper2_float_const) {
        // load from float const
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        RTL_Register_Opd * load_value_reg = new RTL_Register_Opd(oper2_rd);

        Double_Const_TAC_Opd * floatconst = (Double_Const_TAC_Opd *) oper2;
        RTL_Double_Const_Opd * floatconst_opd = new RTL_Double_Const_Opd(floatconst->get_value());

        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_value_reg, floatconst_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);

    Arithmetic_RTL_Stmt * arith_comp_stmt = new Arithmetic_RTL_Stmt(result_reg_opd, oper1_reg_opd, oper2_reg_opd, op);
    __rtl.addRTLStatement(arith_comp_stmt);

    result_rd->set_used_for_expr_return();
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
    oper1_rd->reset_used_for_expr_return();
    oper2_rd->reset_used_for_expr_return();

}

Rel_Comp_TAC_Stmt::Rel_Comp_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1,
                                     RelationalOperator r_op, TAC_Opd *op2)
    : op(r_op) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    bool is_temp = op1->get_opd_type() == OpdType::TEMPORARY;
    bool is_var = op1->get_opd_type() == OpdType::VARIABLE;
    bool is_float_const = op1->get_opd_type() == OpdType::DOUBLE_CONST;

    needfloat = (is_temp && ((Temporary_TAC_Opd *) op1)->get_need_float())
     || (is_var && ((Variable_TAC_Opd *) op1)->get_sym_tab_entry()->get_need_float())
     || is_float_const;
}

void Rel_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name() << " "
        << rel_op_to_symbol(op) << " " << oper2->get_name() << std::endl;
}

void Rel_Comp_TAC_Stmt::generateRTL(RTL & __rtl) {
    if (needfloat) {
        bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
        bool is_oper1_float_const = oper1->get_opd_type() == OpdType::DOUBLE_CONST;

        RegisterDescriptor * oper1_rd;
        if (is_oper1_temp) {
            oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
        }
        else if (is_oper1_var) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper1;
            RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());
            RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper1_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        }
        else if (is_oper1_float_const) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            Double_Const_TAC_Opd * floatconst = (Double_Const_TAC_Opd *) oper1;
            RTL_Double_Const_Opd * floatconst_opd = new RTL_Double_Const_Opd(floatconst->get_value());
            RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper1_reg_opd, floatconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }

        RegisterDescriptor * res_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);

        bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper2_var = oper2->get_opd_type() == OpdType::VARIABLE;
        bool is_oper2_float_const = oper2->get_opd_type() == OpdType::DOUBLE_CONST;

        RegisterDescriptor * oper2_rd;
        if (is_oper2_temp) {
            oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
        }
        else if (is_oper2_var) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper2;
            RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());
            RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper2_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        }
        else if (is_oper2_float_const) {
            oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            Double_Const_TAC_Opd * floatconst = (Double_Const_TAC_Opd *) oper2;
            RTL_Double_Const_Opd * floatconst_opd = new RTL_Double_Const_Opd(floatconst->get_value());
            RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper2_reg_opd, floatconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }
        
        RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);
        RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);
        RTL_Register_Opd * res_reg_opd = new RTL_Register_Opd(res_rd);
        // add the relational statement based on case
        switch(op) {
            case RelationalOperator::LESS_THAN:
            {
                Relational_RTL_Stmt * rel_stmt = new Relational_RTL_Stmt(oper1_reg_opd, oper2_reg_opd, RelationalOperator::LESS_THAN);
                __rtl.addRTLStatement(rel_stmt);

                RegisterDescriptor * rd1 = __rtl.machine_descriptor->get_new_register<int_reg>();
                RegisterDescriptor * rd2 = __rtl.machine_descriptor->get_new_register<int_reg>();

                RTL_Register_Opd * reg1 = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2 = new RTL_Register_Opd(rd2);

                RTL_Int_Const_Opd * one = new RTL_Int_Const_Opd(1);
                RegisterDescriptor * zero = __rtl.machine_descriptor->get_register(Register::zero);
                RTL_Register_Opd * zero_reg = new RTL_Register_Opd(zero);
                
                // iLoad
                Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg1, one);
                __rtl.addRTLStatement(iload_stmt);
                // move
                Transfer_RTL_Stmt * move_stmt = new Transfer_RTL_Stmt(reg2, zero_reg);
                // movt
                RTL_Register_Opd * reg1_ = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2_ = new RTL_Register_Opd(rd2);
                Mov_RTL_Stmt * movt_stmt = new Mov_RTL_Stmt(reg2_, reg1_, 0, true);
                __rtl.addRTLStatement(movt_stmt);

                break;
            }
            case RelationalOperator::LESS_THAN_EQUAL:
            {
                Relational_RTL_Stmt * rel_stmt = new Relational_RTL_Stmt(oper1_reg_opd, oper2_reg_opd, RelationalOperator::LESS_THAN_EQUAL);
                __rtl.addRTLStatement(rel_stmt);

                RegisterDescriptor * rd1 = __rtl.machine_descriptor->get_new_register<int_reg>();
                RegisterDescriptor * rd2 = __rtl.machine_descriptor->get_new_register<int_reg>();

                RTL_Register_Opd * reg1 = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2 = new RTL_Register_Opd(rd2);

                RTL_Int_Const_Opd * one = new RTL_Int_Const_Opd(1);
                RegisterDescriptor * zero = __rtl.machine_descriptor->get_register(Register::zero);
                RTL_Register_Opd * zero_reg = new RTL_Register_Opd(zero);
                
                // iLoad
                Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg1, one);
                __rtl.addRTLStatement(iload_stmt);
                // move
                Transfer_RTL_Stmt * move_stmt = new Transfer_RTL_Stmt(reg2, zero_reg);
                // movt
                RTL_Register_Opd * reg1_ = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2_ = new RTL_Register_Opd(rd2);
                Mov_RTL_Stmt * movt_stmt = new Mov_RTL_Stmt(reg2_, reg1_, 0, true);
                __rtl.addRTLStatement(movt_stmt);
                break;
            }
            case RelationalOperator::GREATER_THAN:
            {
                Relational_RTL_Stmt * rel_stmt = new Relational_RTL_Stmt(oper1_reg_opd, oper2_reg_opd, RelationalOperator::LESS_THAN_EQUAL);
                __rtl.addRTLStatement(rel_stmt);

                RegisterDescriptor * rd1 = __rtl.machine_descriptor->get_new_register<int_reg>();
                RegisterDescriptor * rd2 = __rtl.machine_descriptor->get_new_register<int_reg>();

                RTL_Register_Opd * reg1 = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2 = new RTL_Register_Opd(rd2);

                RTL_Int_Const_Opd * one = new RTL_Int_Const_Opd(1);
                RegisterDescriptor * zero = __rtl.machine_descriptor->get_register(Register::zero);
                RTL_Register_Opd * zero_reg = new RTL_Register_Opd(zero);
                
                // iLoad
                Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg1, one);
                __rtl.addRTLStatement(iload_stmt);
                // move
                Transfer_RTL_Stmt * move_stmt = new Transfer_RTL_Stmt(reg2, zero_reg);
                // movt
                RTL_Register_Opd * reg1_ = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2_ = new RTL_Register_Opd(rd2);
                Mov_RTL_Stmt * movf_stmt = new Mov_RTL_Stmt(reg2_, reg1_, 0, false);
                __rtl.addRTLStatement(movf_stmt);
                break;
            }
            case RelationalOperator::GREATER_THAN_EQUAL:
            {
                Relational_RTL_Stmt * rel_stmt = new Relational_RTL_Stmt(oper1_reg_opd, oper2_reg_opd, RelationalOperator::LESS_THAN);
                __rtl.addRTLStatement(rel_stmt);

                RegisterDescriptor * rd1 = __rtl.machine_descriptor->get_new_register<int_reg>();
                RegisterDescriptor * rd2 = __rtl.machine_descriptor->get_new_register<int_reg>();

                RTL_Register_Opd * reg1 = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2 = new RTL_Register_Opd(rd2);

                RTL_Int_Const_Opd * one = new RTL_Int_Const_Opd(1);
                RegisterDescriptor * zero = __rtl.machine_descriptor->get_register(Register::zero);
                RTL_Register_Opd * zero_reg = new RTL_Register_Opd(zero);
                
                // iLoad
                Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg1, one);
                __rtl.addRTLStatement(iload_stmt);
                // move
                Transfer_RTL_Stmt * move_stmt = new Transfer_RTL_Stmt(reg2, zero_reg);
                // movt
                RTL_Register_Opd * reg1_ = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2_ = new RTL_Register_Opd(rd2);
                Mov_RTL_Stmt * movf_stmt = new Mov_RTL_Stmt(reg2_, reg1_, 0, false);
                __rtl.addRTLStatement(movf_stmt);

                break;
            }
            case RelationalOperator::EQUAL:
            {
                Relational_RTL_Stmt * rel_stmt = new Relational_RTL_Stmt(oper1_reg_opd, oper2_reg_opd, RelationalOperator::EQUAL);
                __rtl.addRTLStatement(rel_stmt);

                RegisterDescriptor * rd1 = __rtl.machine_descriptor->get_new_register<int_reg>();
                RegisterDescriptor * rd2 = __rtl.machine_descriptor->get_new_register<int_reg>();

                RTL_Register_Opd * reg1 = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2 = new RTL_Register_Opd(rd2);

                RTL_Int_Const_Opd * one = new RTL_Int_Const_Opd(1);
                RegisterDescriptor * zero = __rtl.machine_descriptor->get_register(Register::zero);
                RTL_Register_Opd * zero_reg = new RTL_Register_Opd(zero);
                
                // iLoad
                Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg1, one);
                __rtl.addRTLStatement(iload_stmt);
                // move
                Transfer_RTL_Stmt * move_stmt = new Transfer_RTL_Stmt(reg2, zero_reg);
                // movt
                RTL_Register_Opd * reg1_ = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2_ = new RTL_Register_Opd(rd2);
                Mov_RTL_Stmt * movt_stmt = new Mov_RTL_Stmt(reg2_, reg1_, 0, true);
                __rtl.addRTLStatement(movt_stmt);
                break;
            }
            case RelationalOperator::NOT_EQUAL:
            {
                Relational_RTL_Stmt * rel_stmt = new Relational_RTL_Stmt(oper1_reg_opd, oper2_reg_opd, RelationalOperator::EQUAL);
                __rtl.addRTLStatement(rel_stmt);

                RegisterDescriptor * rd1 = __rtl.machine_descriptor->get_new_register<int_reg>();
                RegisterDescriptor * rd2 = __rtl.machine_descriptor->get_new_register<int_reg>();

                RTL_Register_Opd * reg1 = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2 = new RTL_Register_Opd(rd2);

                RTL_Int_Const_Opd * one = new RTL_Int_Const_Opd(1);
                RegisterDescriptor * zero = __rtl.machine_descriptor->get_register(Register::zero);
                RTL_Register_Opd * zero_reg = new RTL_Register_Opd(zero);
                
                // iLoad
                Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg1, one);
                __rtl.addRTLStatement(iload_stmt);
                // move
                Transfer_RTL_Stmt * move_stmt = new Transfer_RTL_Stmt(reg2, zero_reg);
                // movt
                RTL_Register_Opd * reg1_ = new RTL_Register_Opd(rd1);
                RTL_Register_Opd * reg2_ = new RTL_Register_Opd(rd2);
                Mov_RTL_Stmt * movf_stmt = new Mov_RTL_Stmt(reg2_, reg1_, 0, false);
                __rtl.addRTLStatement(movf_stmt);
                break;
            }
        }

        
        res_rd->set_used_for_expr_return();
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
        oper1_rd->reset_used_for_expr_return();
        oper2_rd->reset_used_for_expr_return();
    }
    else {
        bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
        bool is_oper1_int_const = oper1->get_opd_type() == OpdType::INT_CONST;

        RegisterDescriptor * oper1_rd;
        if (is_oper1_temp) {
            oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
        }
        else if (is_oper1_var) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper1;
            RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());
            RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper1_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        }
        else if (is_oper1_int_const) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            Int_Const_TAC_Opd * intconst = (Int_Const_TAC_Opd *) oper1;
            RTL_Int_Const_Opd * intconst_opd = new RTL_Int_Const_Opd(intconst->get_value());
            RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper1_reg_opd, intconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }

        RegisterDescriptor * res_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);

        bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper2_var = oper2->get_opd_type() == OpdType::VARIABLE;
        bool is_oper2_int_const = oper2->get_opd_type() == OpdType::INT_CONST;

        RegisterDescriptor * oper2_rd;
        if (is_oper2_temp) {
            oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
        }
        else if (is_oper2_var) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper2;
            RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());
            RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper2_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        }
        else if (is_oper2_int_const) {
            oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            Int_Const_TAC_Opd * intconst = (Int_Const_TAC_Opd *) oper2;
            RTL_Int_Const_Opd * intconst_opd = new RTL_Int_Const_Opd(intconst->get_value());
            RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);
            Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(oper2_reg_opd, intconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }

        RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);
        RTL_Register_Opd * res_reg_opd = new RTL_Register_Opd(res_rd);
        RTL_Register_Opd * oper2_reg_opd = new RTL_Register_Opd(oper2_rd);

        Relational_RTL_Stmt * rel_stmt = new Relational_RTL_Stmt(res_reg_opd, oper1_reg_opd, oper2_reg_opd, op);
        __rtl.addRTLStatement(rel_stmt);

        res_rd->set_used_for_expr_return();
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
        oper1_rd->reset_used_for_expr_return();
        oper2_rd->reset_used_for_expr_return();
    }
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
    bool is_temp = (oper1->get_opd_type() == OpdType::TEMPORARY);
    bool is_var = (oper1->get_opd_type() == OpdType::VARIABLE);
    bool is_int_const = (oper1->get_opd_type() == OpdType::INT_CONST);
    bool is_float_const = (oper1->get_opd_type() == OpdType::DOUBLE_CONST);
    switch (op) {
        case UnaryOperator::UMINUS:
            {
                RegisterDescriptor * oper1_rd;
                if (is_temp) {
                    oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
                }
                else if (is_var) {
                    // load into register from variable
                    oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
                    RTL_Register_Opd * load_temp_reg_opd = new RTL_Register_Opd(oper1_rd);
                    Variable_TAC_Opd * var = (Variable_TAC_Opd *) oper1;
                    RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());

                    Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_temp_reg_opd, var_opd);

                    __rtl.addRTLStatement(load_stmt);
                }
                else if (is_int_const) {
                    // load into register from variable
                    oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
                    RTL_Register_Opd * load_temp_reg_opd = new RTL_Register_Opd(oper1_rd);
                    Int_Const_TAC_Opd * int_const = (Int_Const_TAC_Opd *) oper1;
                    RTL_Int_Const_Opd * int_const_opd = new RTL_Int_Const_Opd(int_const->get_value());

                    Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_temp_reg_opd, int_const_opd);

                    __rtl.addRTLStatement(load_stmt);
                }
                else if (is_float_const) {
                    // load into register from variable
                    oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
                    RTL_Register_Opd * load_temp_reg_opd = new RTL_Register_Opd(oper1_rd);
                    Double_Const_TAC_Opd * float_const = (Double_Const_TAC_Opd *) oper1;
                    RTL_Double_Const_Opd * float_const_opd = new RTL_Double_Const_Opd(float_const->get_value());

                    Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(load_temp_reg_opd, float_const_opd);

                    __rtl.addRTLStatement(load_stmt);
                }

                // uminus
                RegisterDescriptor * res_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
                RTL_Register_Opd * res_opd = new RTL_Register_Opd(res_rd);
                RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);

                UMinus_RTL_Stmt * uminus_stmt = new UMinus_RTL_Stmt(res_opd, oper1_reg_opd);

                __rtl.addRTLStatement(uminus_stmt);

                __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
                res_rd->set_used_for_expr_return();
                oper1_rd->reset_used_for_expr_return();
                
                break;
            }

        case UnaryOperator::NOT:
        {            
            RegisterDescriptor * oper1_rd;
                
            if (is_temp) {
                oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
            }
            else {
                // load value into temp
                oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
                RTL_Register_Opd * load_oper1_reg_opd = new RTL_Register_Opd(oper1_rd);

                Variable_TAC_Opd * var_tac_opd = (Variable_TAC_Opd *) oper1;
                RTL_Var_Opd * var_opd = new RTL_Var_Opd(var_tac_opd->get_sym_tab_entry());

                Transfer_RTL_Stmt * load_into_register_stmt = new Transfer_RTL_Stmt(load_oper1_reg_opd, var_opd);
                __rtl.addRTLStatement(load_into_register_stmt);
            }

            // not instruction
            RegisterDescriptor * res_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
            RTL_Register_Opd * res_reg_opd = new RTL_Register_Opd(res_rd);
            RTL_Register_Opd * oper1_reg_opd = new RTL_Register_Opd(oper1_rd);
            Not_RTL_Stmt * not_stmt = new Not_RTL_Stmt(res_reg_opd, oper1_reg_opd);

            __rtl.addRTLStatement(not_stmt);

            __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
            res_rd->set_used_for_expr_return();
            oper1_rd->reset_used_for_expr_return();
            
            break;
        }
    }
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
    Label_TAC_Opd * l = (Label_TAC_Opd *) result;
    RTL_Label_Opd * l_rtl_opd = new RTL_Label_Opd(l->get_label_num());
    Goto_RTL_Stmt * goto_stmt = new Goto_RTL_Stmt(l_rtl_opd);
    __rtl.addRTLStatement(goto_stmt);
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
    bool istemp = result->get_opd_type() == OpdType::TEMPORARY;

    Label_TAC_Opd * label_tac_opd = (Label_TAC_Opd *) oper1;
    RTL_Label_Opd * l = new RTL_Label_Opd(label_tac_opd->get_label_num());
    If_Goto_RTL_Stmt * if_goto_stmt;
    RegisterDescriptor * rd;
    if (!istemp) {
        // load into temp
        rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(rd);
        
        Variable_TAC_Opd * var_tac_opd = (Variable_TAC_Opd *) result;
        RTL_Var_Opd * var_opd = new RTL_Var_Opd(var_tac_opd->get_sym_tab_entry());

        // load statement
        Transfer_RTL_Stmt * load_stmt = new Transfer_RTL_Stmt(reg_opd, var_opd);
        __rtl.addRTLStatement(load_stmt);

        // construct the if-goto statement
        RTL_Register_Opd * reg = new RTL_Register_Opd(rd);
        if_goto_stmt = new If_Goto_RTL_Stmt(reg, l);
    }
    else {
        // need to get register descriptor corresponding to that temp number
        rd = __rtl.machine_descriptor->get_rd_for_tac_opd(result);

        RTL_Register_Opd * reg_opd = new RTL_Register_Opd(rd);

        if_goto_stmt = new If_Goto_RTL_Stmt(reg_opd, l);

    }
    
    __rtl.addRTLStatement(if_goto_stmt);

    __rtl.machine_descriptor->unset_rd_for_tac_opd(result);
    rd->reset_used_for_expr_return();
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
        bool is_temp = result->get_opd_type() == OpdType::TEMPORARY;
        bool is_var = result->get_opd_type() == OpdType::VARIABLE;
        bool is_int_const = result->get_opd_type() == OpdType::INT_CONST;
        bool is_float_const = result->get_opd_type() == OpdType::DOUBLE_CONST;
        bool is_string_const = result->get_opd_type() == OpdType::STRING_CONST;

        Variable_TAC_Opd * var = (Variable_TAC_Opd *) result;
        Temporary_TAC_Opd * temp = (Temporary_TAC_Opd *) result;
        Int_Const_TAC_Opd * int_const = (Int_Const_TAC_Opd *) result;
        Double_Const_TAC_Opd * float_const = (Double_Const_TAC_Opd *) result;
        String_Const_TAC_Opd * string_const = (String_Const_TAC_Opd *) result;

        bool needfloat = (is_var && var->get_sym_tab_entry()->get_need_float()) || 
                         (is_temp && temp->get_need_float()) || is_float_const;

        // load correct value into v0
        RTL_Int_Const_Opd * num;
        RegisterDescriptor * rd = __rtl.machine_descriptor->get_register(Register::v0);
        RTL_Register_Opd * reg = new RTL_Register_Opd(rd);
        if (needfloat) {
            num = new RTL_Int_Const_Opd(3);
        }
        else if (is_var || is_temp || is_int_const) {
            num = new RTL_Int_Const_Opd(1);
        }
        else if (is_string_const) {
            num = new RTL_Int_Const_Opd(4);
        }

        Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg, num);
        __rtl.addRTLStatement(iload_stmt);

        // load into correct register
        if (needfloat) {
            rd = __rtl.machine_descriptor->get_register(Register::f12);
        }
        else if (!needfloat) {
            rd = __rtl.machine_descriptor->get_register(Register::a0);
        }

        reg = new RTL_Register_Opd(rd);
        Transfer_RTL_Stmt * load_stmt;

        RegisterDescriptor * temp_rd;
        if (is_temp) {
            temp_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(result);
            RTL_Register_Opd * reg_op = new RTL_Register_Opd(temp_rd);
            load_stmt = new Transfer_RTL_Stmt(reg, reg_op);
        }
        else if (is_var) {
            RTL_Var_Opd * var_opd = new RTL_Var_Opd(var->get_sym_tab_entry());
            load_stmt = new Transfer_RTL_Stmt(reg, var_opd);
        }
        else if (is_int_const) {
            RTL_Int_Const_Opd * int_const_opd = new RTL_Int_Const_Opd(int_const->get_value());
            load_stmt = new Transfer_RTL_Stmt(reg, int_const_opd);
        }
        else if (is_float_const) {
            RTL_Double_Const_Opd * float_const_opd = new RTL_Double_Const_Opd(float_const->get_value());
            load_stmt = new Transfer_RTL_Stmt(reg, float_const_opd);
        }
        else if (is_string_const) {
            // add string const to string const table of rtl
            std::string str_value = string_const->get_name();
            __rtl.addNewStringConst(str_value);

            unsigned int str_num = __rtl.getStringConstNum(str_value);

            RTL_String_Const_Opd * string_const_opd = new RTL_String_Const_Opd(str_num, str_value);
            load_stmt = new Transfer_RTL_Stmt(reg, string_const_opd);
        }

        __rtl.addRTLStatement(load_stmt);

        __rtl.machine_descriptor->unset_rd_for_tac_opd(result);
        temp_rd->reset_used_for_expr_return();

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
        if (var_tac_opd->get_sym_tab_entry()->get_need_float()) {
            num = new RTL_Int_Const_Opd(7);
        }
        else {
            num = new RTL_Int_Const_Opd(5);
        }

        Transfer_RTL_Stmt * iload_stmt = new Transfer_RTL_Stmt(reg, num);
        __rtl.addRTLStatement(iload_stmt);


        Read_RTL_Stmt * read_stmt = new Read_RTL_Stmt();
        __rtl.addRTLStatement(read_stmt);
        
        RTL_Var_Opd * var_rtl_opd = new RTL_Var_Opd(var_tac_opd->get_sym_tab_entry());

        // select appropriate register to move result from
        if (var_tac_opd->get_sym_tab_entry()->get_need_float()) {
            rd = __rtl.machine_descriptor->get_register(Register::f0);
        }
        else {
            rd = __rtl.machine_descriptor->get_register(Register::v0);
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

Return_TAC_Stmt::Return_TAC_Stmt(Temporary_TAC_Opd *stemp) {
    result = stemp;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Return_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << "return " << result->get_name() << std::endl;
}

void Label_TAC_Stmt::generateRTL(RTL & __rtl) {
    Label_TAC_Opd * temp = (Label_TAC_Opd *) result;
    RTL_Label_Opd * l = new RTL_Label_Opd(temp->get_label_num());

    Label_RTL_Stmt * stmt = new Label_RTL_Stmt(l);

    __rtl.addRTLStatement(stmt);
}
