#include "tac.hpp"
#include <iostream>

unsigned int TAC::label_number = 0;

std::shared_ptr<Label_TAC_Opd> TAC::getRetLabel() {
    return std::make_shared<Label_TAC_Opd>(label_number++);
}

std::shared_ptr<Temporary_TAC_Opd> TAC::genNewTemporary(bool need_float) {
    return std::make_shared<Temporary_TAC_Opd>(temp_number++, need_float);
}

std::shared_ptr<Variable_TAC_Opd>
TAC::genNewSTemporary(DataType dt, SymbolTable *local, bool is_ret) {
    std::string stemp_name = "stemp" + std::to_string(stemp_number++);
    if (is_ret)
        return std::make_shared<Variable_TAC_Opd>(local->return_stemp);
    else {
        local->insert_stemp(stemp_name, dt);
        return std::make_shared<Variable_TAC_Opd>(
            local->stemp_lookup(stemp_name));
    }
}

std::shared_ptr<Label_TAC_Opd> TAC::genNewLabel() {
    return std::make_shared<Label_TAC_Opd>(label_number++);
}

TAC::TAC() : temp_number(0), stemp_number(0) {}

void TAC::addTACStatements(const std::vector<TAC_Stmt *> &stmts) {
    for (auto stmt : stmts)
        tac_code.push_back(stmt);
}

void TAC::print(std::ostream &out) {
    for (auto stmt : tac_code)
        stmt->print(out);
}

bool TAC::isEmpty() { return tac_code.empty(); }

void TAC::generateRTL(RTL &rtl) {
    for (auto &stmt : tac_code)
        stmt->generateRTL(rtl);
}

TAC::~TAC() {
    for (auto ptr : tac_code)
        delete ptr;
}

OpdType TAC_Opd::get_opd_type() const { return opd_type; }

Double_Const_TAC_Opd::Double_Const_TAC_Opd(double val) : value(val) {
    opd_type = OpdType::DOUBLE_CONST;
}

std::string Double_Const_TAC_Opd::get_name() {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.2f", value);
    return std::string(buffer);
}

double Double_Const_TAC_Opd::get_value() const { return value; }

Int_Const_TAC_Opd::Int_Const_TAC_Opd(int val) : value(val) {
    opd_type = OpdType::INT_CONST;
}

std::string Int_Const_TAC_Opd::get_name() { return std::to_string(value); }

int Int_Const_TAC_Opd::get_value() const { return value; }

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

unsigned int Label_TAC_Opd::get_label_num() const { return label_num; }

Temporary_TAC_Opd::Temporary_TAC_Opd(unsigned int num, bool need_float)
    : temp_num(num) {
    opd_type = OpdType::TEMPORARY;
    needfloat = need_float;
}

std::string Temporary_TAC_Opd::get_name() {
    return "temp" + std::to_string(temp_num);
}

unsigned int Temporary_TAC_Opd::get_temp_num() const { return temp_num; }

bool Temporary_TAC_Opd::get_need_float() const { return needfloat; }

Variable_TAC_Opd::Variable_TAC_Opd(SymbolTableEntry *entry)
    : symtab_entry(entry) {
    opd_type = OpdType::VARIABLE;
}

std::string Variable_TAC_Opd::get_name() { return symtab_entry->get_name(); }

SymbolTableEntry *Variable_TAC_Opd::get_sym_tab_entry() const {
    return symtab_entry;
}

Function_TAC_Opd::Function_TAC_Opd(SymbolTableFunction *entry,
                                   std::vector<std::shared_ptr<TAC_Opd>> args)
    : symtab_entry(entry), params(args) {
    opd_type = OpdType::FUNCTION;
}

std::string Function_TAC_Opd::get_name() {
    std::string ret = "";
    ret += symtab_entry->get_name() + "(";
    if (!params.empty()) {
        for (auto it = params.begin(); it != std::prev(params.end()); ++it)
            ret += (*it)->get_name() + ", ";
        ret += params.back()->get_name();
    }
    ret += ")";
    return ret;
}

SymbolTableFunction *Function_TAC_Opd::get_sym_tab_func() const {
    return symtab_entry;
}

std::vector<std::shared_ptr<TAC_Opd>> Function_TAC_Opd::get_params() {
    return params;
}

// ------------------------------------------------------------------------------

Asgn_TAC_Stmt::Asgn_TAC_Stmt(std::shared_ptr<TAC_Opd> dest,
                             std::shared_ptr<TAC_Opd> src) {
    result = dest;
    oper1 = src;
    oper2 = nullptr;
}

void Asgn_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name()
        << std::endl;
}

void Asgn_TAC_Stmt::generateRTL(RTL &__rtl) {
    bool is_result_temp = result->get_opd_type() == OpdType::TEMPORARY;
    bool is_result_var = result->get_opd_type() == OpdType::VARIABLE;

    bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
    bool is_oper1_int_const = oper1->get_opd_type() == OpdType::INT_CONST;
    bool is_oper1_float_const = oper1->get_opd_type() == OpdType::DOUBLE_CONST;
    bool is_oper1_string_const = oper1->get_opd_type() == OpdType::STRING_CONST;

    RegisterDescriptor *oper1_rd;
    if (is_oper1_temp) {
        oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
    } else if (is_oper1_var) {
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<Variable_TAC_Opd> var =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(oper1);
        std::shared_ptr<RTL_Var_Opd> var_opd =
            std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());
        std::shared_ptr<RTL_Register_Opd> reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);
        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(reg_opd, var_opd);

        __rtl.addRTLStatement(load_stmt);
    } else if (is_oper1_int_const) {
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<Int_Const_TAC_Opd> intconst =
            std::dynamic_pointer_cast<Int_Const_TAC_Opd>(oper1);
        std::shared_ptr<RTL_Int_Const_Opd> intconst_opd =
            std::make_shared<RTL_Int_Const_Opd>(intconst->get_value());
        std::shared_ptr<RTL_Register_Opd> reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);
        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(reg_opd, intconst_opd);

        __rtl.addRTLStatement(load_stmt);
    } else if (is_oper1_float_const) {
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<Double_Const_TAC_Opd> floatconst =
            std::dynamic_pointer_cast<Double_Const_TAC_Opd>(oper1);
        std::shared_ptr<RTL_Double_Const_Opd> floatconst_opd =
            std::make_shared<RTL_Double_Const_Opd>(floatconst->get_value());
        std::shared_ptr<RTL_Register_Opd> reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);
        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(reg_opd, floatconst_opd);

        __rtl.addRTLStatement(load_stmt);
    } else if (is_oper1_string_const) {
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<String_Const_TAC_Opd> strconst =
            std::dynamic_pointer_cast<String_Const_TAC_Opd>(oper1);
        unsigned int str_num = __rtl.getStringConstNum(strconst->get_name());
        std::shared_ptr<RTL_String_Const_Opd> strconst_opd =
            std::make_shared<RTL_String_Const_Opd>(str_num,
                                                   strconst->get_name());
        std::shared_ptr<RTL_Register_Opd> reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);
        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(reg_opd, strconst_opd);

        __rtl.addRTLStatement(load_stmt);
    }

    std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
        std::make_shared<RTL_Register_Opd>(oper1_rd);

    RegisterDescriptor *res_rd;
    // to be safe, don't think this case would occur
    if (is_result_temp) {
        res_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(result);
        std::shared_ptr<RTL_Register_Opd> res_reg_opd =
            std::make_shared<RTL_Register_Opd>(res_rd);

        std::shared_ptr<Transfer_RTL_Stmt> assn_stmt =
            std::make_shared<Transfer_RTL_Stmt>(res_reg_opd, oper1_reg_opd);
        __rtl.addRTLStatement(assn_stmt);

        res_rd->set_used_for_expr_return();
    } else if (is_result_var) {
        std::shared_ptr<Variable_TAC_Opd> var =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(result);
        std::shared_ptr<RTL_Var_Opd> var_opd =
            std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());

        std::shared_ptr<Transfer_RTL_Stmt> assn_stmt =
            std::make_shared<Transfer_RTL_Stmt>(var_opd, oper1_reg_opd);
        __rtl.addRTLStatement(assn_stmt);
    }

    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
    oper1_rd->reset_used_for_expr_return();
}

Call_TAC_Stmt::Call_TAC_Stmt(std::shared_ptr<TAC_Opd> func) {
    result = func;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Call_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << std::endl;
}

void Call_TAC_Stmt::generateRTL(RTL &__rtl) {
    // first load and push all required variables onto the stack
    std::shared_ptr<Function_TAC_Opd> fnCall =
        std::dynamic_pointer_cast<Function_TAC_Opd>(result);
    for (auto it = fnCall->get_params().rbegin();
         it != fnCall->get_params().rend(); it++) {
        auto &opd = *it;
        // add logic to transfer opd to v0/f2
        // TODO: Add logic here
        bool isTemp;
        bool isFloat;
        bool isVar;
        RegisterDescriptor *rd;

        std::shared_ptr<RTL_Register_Opd> reg =
            std::make_shared<RTL_Register_Opd>(rd);
        // find size of push (8 for floats, 4 for rest)
        unsigned int sz =
            (rd->get_use_category() == RegisterUseCategory::float_reg) ? 8 : 4;
        // push register onto the stack
        std::shared_ptr<Push_RTL_Stmt> stmt =
            std::make_shared<Push_RTL_Stmt>(reg, sz);
    }
    // then "call" the function (while assigning v1/f0/nullptr the return)
    std::shared_ptr<Call_RTL_Stmt> call =
        std::make_shared<Call_RTL_Stmt>(fnCall->get_sym_tab_func());
    __rtl.addRTLStatement(call);
    // pop all parameters from the stack
    for (auto &opd : fnCall->get_sym_tab_func()->get_params()) {
        unsigned int sz = (opd == BaseType::FLOAT) ? 8 : 4;
        std::shared_ptr<Pop_RTL_Stmt> stmt = std::make_shared<Pop_RTL_Stmt>(sz);
        __rtl.addRTLStatement(stmt);
    }
}

Bool_Comp_TAC_Stmt::Bool_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> res,
                                       std::shared_ptr<TAC_Opd> op1,
                                       BooleanOperator b_op,
                                       std::shared_ptr<TAC_Opd> op2)
    : op(b_op) {
    result = res;
    oper1 = op1;
    oper2 = op2;
}

void Bool_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name() << " "
        << bool_op_to_symbol(op) << " " << oper2->get_name() << std::endl;
}

void Bool_Comp_TAC_Stmt::generateRTL(RTL &__rtl) {
    bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;

    RegisterDescriptor *oper1_rd;

    if (is_oper1_temp) {
        oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
    } else {
        // load
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper1_rd);

        std::shared_ptr<Variable_TAC_Opd> var =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(oper1);
        std::shared_ptr<RTL_Var_Opd> var_opd =
            std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
        std::make_shared<RTL_Register_Opd>(oper1_rd);

    RegisterDescriptor *result_rd =
        __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
    std::shared_ptr<RTL_Register_Opd> result_reg_opd =
        std::make_shared<RTL_Register_Opd>(result_rd);

    RegisterDescriptor *oper2_rd;
    if (is_oper2_temp) {
        oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
    } else {
        // load
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper2_rd);

        std::shared_ptr<Variable_TAC_Opd> var =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(oper2);
        std::shared_ptr<RTL_Var_Opd> var_opd =
            std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
        std::make_shared<RTL_Register_Opd>(oper2_rd);

    std::shared_ptr<Boolean_RTL_Stmt> bool_comp_stmt =
        std::make_shared<Boolean_RTL_Stmt>(result_reg_opd, oper1_reg_opd,
                                           oper2_reg_opd, op);
    __rtl.addRTLStatement(bool_comp_stmt);

    result_rd->set_used_for_expr_return();
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
    oper1_rd->reset_used_for_expr_return();
    oper2_rd->reset_used_for_expr_return();
}

Arith_Comp_TAC_Stmt::Arith_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> res,
                                         std::shared_ptr<TAC_Opd> op1,
                                         ArithmeticOperator a_op,
                                         std::shared_ptr<TAC_Opd> op2)
    : op(a_op) {
    result = res;
    oper1 = op1;
    oper2 = op2;
}

void Arith_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name() << " "
        << arith_op_to_symbol(op) << " " << oper2->get_name() << std::endl;
}

void Arith_Comp_TAC_Stmt::generateRTL(RTL &__rtl) {
    bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
    bool is_oper1_int_const = oper1->get_opd_type() == OpdType::INT_CONST;
    bool is_oper1_float_const = oper1->get_opd_type() == OpdType::DOUBLE_CONST;

    bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;
    bool is_oper2_var = oper2->get_opd_type() == OpdType::VARIABLE;
    bool is_oper2_int_const = oper2->get_opd_type() == OpdType::INT_CONST;
    bool is_oper2_float_const = oper2->get_opd_type() == OpdType::DOUBLE_CONST;

    RegisterDescriptor *oper1_rd;
    if (is_oper1_temp) {
        oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
    } else if (is_oper1_var) {
        // load from variable
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper1_rd);

        std::shared_ptr<Variable_TAC_Opd> var =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(oper1);
        std::shared_ptr<RTL_Var_Opd> var_opd =
            std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    } else if (is_oper1_int_const) {
        // load from int const
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper1_rd);

        std::shared_ptr<Int_Const_TAC_Opd> intconst =
            std::dynamic_pointer_cast<Int_Const_TAC_Opd>(oper1);
        std::shared_ptr<RTL_Int_Const_Opd> intconst_opd =
            std::make_shared<RTL_Int_Const_Opd>(intconst->get_value());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, intconst_opd);
        __rtl.addRTLStatement(load_stmt);
    } else if (is_oper1_float_const) {
        // load from float const
        oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper1_rd);

        std::shared_ptr<Double_Const_TAC_Opd> floatconst =
            std::dynamic_pointer_cast<Double_Const_TAC_Opd>(oper1);
        std::shared_ptr<RTL_Double_Const_Opd> floatconst_opd =
            std::make_shared<RTL_Double_Const_Opd>(floatconst->get_value());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, floatconst_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
        std::make_shared<RTL_Register_Opd>(oper1_rd);

    RegisterDescriptor *result_rd =
        __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
    std::shared_ptr<RTL_Register_Opd> result_reg_opd =
        std::make_shared<RTL_Register_Opd>(result_rd);

    RegisterDescriptor *oper2_rd;
    if (is_oper2_temp) {
        oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
    } else if (is_oper2_var) {
        // load from variable
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper2_rd);

        std::shared_ptr<Variable_TAC_Opd> var =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(oper2);
        std::shared_ptr<RTL_Var_Opd> var_opd =
            std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, var_opd);
        __rtl.addRTLStatement(load_stmt);
    } else if (is_oper2_int_const) {
        // load from int const
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper2_rd);

        std::shared_ptr<Int_Const_TAC_Opd> intconst =
            std::dynamic_pointer_cast<Int_Const_TAC_Opd>(oper2);
        std::shared_ptr<RTL_Int_Const_Opd> intconst_opd =
            std::make_shared<RTL_Int_Const_Opd>(intconst->get_value());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, intconst_opd);
        __rtl.addRTLStatement(load_stmt);
    } else if (is_oper2_float_const) {
        // load from float const
        oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
        std::shared_ptr<RTL_Register_Opd> load_value_reg =
            std::make_shared<RTL_Register_Opd>(oper2_rd);

        std::shared_ptr<Double_Const_TAC_Opd> floatconst =
            std::dynamic_pointer_cast<Double_Const_TAC_Opd>(oper2);
        std::shared_ptr<RTL_Double_Const_Opd> floatconst_opd =
            std::make_shared<RTL_Double_Const_Opd>(floatconst->get_value());

        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(load_value_reg, floatconst_opd);
        __rtl.addRTLStatement(load_stmt);
    }

    std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
        std::make_shared<RTL_Register_Opd>(oper2_rd);

    std::shared_ptr<Arithmetic_RTL_Stmt> arith_comp_stmt =
        std::make_shared<Arithmetic_RTL_Stmt>(result_reg_opd, oper1_reg_opd,
                                              oper2_reg_opd, op);
    __rtl.addRTLStatement(arith_comp_stmt);

    result_rd->set_used_for_expr_return();
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
    __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
    oper1_rd->reset_used_for_expr_return();
    oper2_rd->reset_used_for_expr_return();
}

Rel_Comp_TAC_Stmt::Rel_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> res,
                                     std::shared_ptr<TAC_Opd> op1,
                                     RelationalOperator r_op,
                                     std::shared_ptr<TAC_Opd> op2)
    : op(r_op) {
    result = res;
    oper1 = op1;
    oper2 = op2;

    bool is_temp = op1->get_opd_type() == OpdType::TEMPORARY;
    bool is_var = op1->get_opd_type() == OpdType::VARIABLE;
    bool is_float_const = op1->get_opd_type() == OpdType::DOUBLE_CONST;

    needfloat = (is_temp && (std::dynamic_pointer_cast<Temporary_TAC_Opd>(op1))
                                ->get_need_float()) ||
                (is_var && (std::dynamic_pointer_cast<Variable_TAC_Opd>(op1))
                               ->get_sym_tab_entry()
                               ->get_need_float()) ||
                is_float_const;
}

void Rel_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << oper1->get_name() << " "
        << rel_op_to_symbol(op) << " " << oper2->get_name() << std::endl;
}

void Rel_Comp_TAC_Stmt::generateRTL(RTL &__rtl) {
    if (needfloat) {
        bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
        bool is_oper1_float_const =
            oper1->get_opd_type() == OpdType::DOUBLE_CONST;

        RegisterDescriptor *oper1_rd;
        if (is_oper1_temp) {
            oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
        } else if (is_oper1_var) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<Variable_TAC_Opd> var =
                std::dynamic_pointer_cast<Variable_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Var_Opd> var_opd =
                std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());
            std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper1_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        } else if (is_oper1_float_const) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<Double_Const_TAC_Opd> floatconst =
                std::dynamic_pointer_cast<Double_Const_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Double_Const_Opd> floatconst_opd =
                std::make_shared<RTL_Double_Const_Opd>(floatconst->get_value());
            std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper1_reg_opd,
                                                    floatconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }

        bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper2_var = oper2->get_opd_type() == OpdType::VARIABLE;
        bool is_oper2_float_const =
            oper2->get_opd_type() == OpdType::DOUBLE_CONST;

        RegisterDescriptor *oper2_rd;
        if (is_oper2_temp) {
            oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
        } else if (is_oper2_var) {
            oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            std::shared_ptr<Variable_TAC_Opd> var =
                std::dynamic_pointer_cast<Variable_TAC_Opd>(oper2);
            std::shared_ptr<RTL_Var_Opd> var_opd =
                std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());
            std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper2_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper2_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        } else if (is_oper2_float_const) {
            oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            std::shared_ptr<Double_Const_TAC_Opd> floatconst =
                std::dynamic_pointer_cast<Double_Const_TAC_Opd>(oper2);
            std::shared_ptr<RTL_Double_Const_Opd> floatconst_opd =
                std::make_shared<RTL_Double_Const_Opd>(floatconst->get_value());
            std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper2_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper2_reg_opd,
                                                    floatconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }

        std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);
        std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
            std::make_shared<RTL_Register_Opd>(oper2_rd);

        RegisterDescriptor *rd =
            __rtl.machine_descriptor
                ->get_new_register<RegisterUseCategory::int_reg>();
        RegisterDescriptor *res_rd =
            __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);

        // add the relational statement based on case
        switch (op) {
        case RelationalOperator::LESS_THAN:
        case RelationalOperator::LESS_THAN_EQUAL:
        case RelationalOperator::EQUAL: {
            std::shared_ptr<Relational_RTL_Stmt> rel_stmt =
                std::make_shared<Relational_RTL_Stmt>(oper1_reg_opd,
                                                      oper2_reg_opd, op);
            __rtl.addRTLStatement(rel_stmt);

            std::shared_ptr<RTL_Register_Opd> reg1 =
                std::make_shared<RTL_Register_Opd>(rd);
            std::shared_ptr<RTL_Register_Opd> reg2 =
                std::make_shared<RTL_Register_Opd>(res_rd);

            std::shared_ptr<RTL_Int_Const_Opd> one =
                std::make_shared<RTL_Int_Const_Opd>(1);
            RegisterDescriptor *zero =
                __rtl.machine_descriptor->get_register(Register::zero);
            std::shared_ptr<RTL_Register_Opd> zero_reg =
                std::make_shared<RTL_Register_Opd>(zero);

            // iLoad
            std::shared_ptr<Transfer_RTL_Stmt> iload_stmt =
                std::make_shared<Transfer_RTL_Stmt>(reg1, one);
            __rtl.addRTLStatement(iload_stmt);
            // move
            std::shared_ptr<Transfer_RTL_Stmt> move_stmt =
                std::make_shared<Transfer_RTL_Stmt>(reg2, zero_reg);
            __rtl.addRTLStatement(move_stmt);
            // movt
            std::shared_ptr<Mov_RTL_Stmt> movt_stmt =
                std::make_shared<Mov_RTL_Stmt>(reg2, reg1, 0, true);
            __rtl.addRTLStatement(movt_stmt);

            break;
        }
        case RelationalOperator::GREATER_THAN:
        case RelationalOperator::GREATER_THAN_EQUAL:
        case RelationalOperator::NOT_EQUAL: {
            std::shared_ptr<Relational_RTL_Stmt> rel_stmt =
                std::make_shared<Relational_RTL_Stmt>(
                    oper1_reg_opd, oper2_reg_opd, anti_op(op));
            __rtl.addRTLStatement(rel_stmt);

            std::shared_ptr<RTL_Register_Opd> reg1 =
                std::make_shared<RTL_Register_Opd>(rd);
            std::shared_ptr<RTL_Register_Opd> reg2 =
                std::make_shared<RTL_Register_Opd>(res_rd);

            std::shared_ptr<RTL_Int_Const_Opd> one =
                std::make_shared<RTL_Int_Const_Opd>(1);
            RegisterDescriptor *zero =
                __rtl.machine_descriptor->get_register(Register::zero);
            std::shared_ptr<RTL_Register_Opd> zero_reg =
                std::make_shared<RTL_Register_Opd>(zero);

            // iLoad
            std::shared_ptr<Transfer_RTL_Stmt> iload_stmt =
                std::make_shared<Transfer_RTL_Stmt>(reg1, one);
            __rtl.addRTLStatement(iload_stmt);
            // move
            std::shared_ptr<Transfer_RTL_Stmt> move_stmt =
                std::make_shared<Transfer_RTL_Stmt>(reg2, zero_reg);
            __rtl.addRTLStatement(move_stmt);
            // movf
            std::shared_ptr<Mov_RTL_Stmt> movf_stmt =
                std::make_shared<Mov_RTL_Stmt>(reg2, reg1, 0, false);
            __rtl.addRTLStatement(movf_stmt);

            break;
        }
        }

        res_rd->set_used_for_expr_return();
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
        oper1_rd->reset_used_for_expr_return();
        oper2_rd->reset_used_for_expr_return();
        rd->reset_used_for_expr_return();
    } else {
        bool is_oper1_temp = oper1->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper1_var = oper1->get_opd_type() == OpdType::VARIABLE;
        bool is_oper1_int_const = oper1->get_opd_type() == OpdType::INT_CONST;

        RegisterDescriptor *oper1_rd;
        if (is_oper1_temp) {
            oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
        } else if (is_oper1_var) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<Variable_TAC_Opd> var =
                std::dynamic_pointer_cast<Variable_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Var_Opd> var_opd =
                std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());
            std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper1_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        } else if (is_oper1_int_const) {
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<Int_Const_TAC_Opd> intconst =
                std::dynamic_pointer_cast<Int_Const_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Int_Const_Opd> intconst_opd =
                std::make_shared<RTL_Int_Const_Opd>(intconst->get_value());
            std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper1_reg_opd,
                                                    intconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }

        RegisterDescriptor *res_rd =
            __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);

        bool is_oper2_temp = oper2->get_opd_type() == OpdType::TEMPORARY;
        bool is_oper2_var = oper2->get_opd_type() == OpdType::VARIABLE;
        bool is_oper2_int_const = oper2->get_opd_type() == OpdType::INT_CONST;

        RegisterDescriptor *oper2_rd;
        if (is_oper2_temp) {
            oper2_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper2);
        } else if (is_oper2_var) {
            oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            std::shared_ptr<Variable_TAC_Opd> var =
                std::dynamic_pointer_cast<Variable_TAC_Opd>(oper2);
            std::shared_ptr<RTL_Var_Opd> var_opd =
                std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());
            std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper2_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper2_reg_opd, var_opd);
            __rtl.addRTLStatement(load_stmt);
        } else if (is_oper2_int_const) {
            oper2_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper2);
            std::shared_ptr<Int_Const_TAC_Opd> intconst =
                std::dynamic_pointer_cast<Int_Const_TAC_Opd>(oper2);
            std::shared_ptr<RTL_Int_Const_Opd> intconst_opd =
                std::make_shared<RTL_Int_Const_Opd>(intconst->get_value());
            std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper2_rd);
            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(oper2_reg_opd,
                                                    intconst_opd);
            __rtl.addRTLStatement(load_stmt);
        }

        std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);
        std::shared_ptr<RTL_Register_Opd> res_reg_opd =
            std::make_shared<RTL_Register_Opd>(res_rd);
        std::shared_ptr<RTL_Register_Opd> oper2_reg_opd =
            std::make_shared<RTL_Register_Opd>(oper2_rd);

        std::shared_ptr<Relational_RTL_Stmt> rel_stmt =
            std::make_shared<Relational_RTL_Stmt>(res_reg_opd, oper1_reg_opd,
                                                  oper2_reg_opd, op);
        __rtl.addRTLStatement(rel_stmt);

        res_rd->set_used_for_expr_return();
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper2);
        oper1_rd->reset_used_for_expr_return();
        oper2_rd->reset_used_for_expr_return();
    }
}

Unary_Comp_TAC_Stmt::Unary_Comp_TAC_Stmt(std::shared_ptr<TAC_Opd> res,
                                         UnaryOperator u_op,
                                         std::shared_ptr<TAC_Opd> oper)
    : op(u_op) {
    result = res;
    oper1 = oper;
    oper2 = nullptr;
}

void Unary_Comp_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << result->get_name() << " = " << unary_op_to_symbol(op) << " "
        << oper1->get_name() << std::endl;
}

void Unary_Comp_TAC_Stmt::generateRTL(RTL &__rtl) {
    bool is_temp = (oper1->get_opd_type() == OpdType::TEMPORARY);
    bool is_var = (oper1->get_opd_type() == OpdType::VARIABLE);
    bool is_int_const = (oper1->get_opd_type() == OpdType::INT_CONST);
    bool is_float_const = (oper1->get_opd_type() == OpdType::DOUBLE_CONST);
    switch (op) {
    case UnaryOperator::UMINUS: {
        RegisterDescriptor *oper1_rd;
        if (is_temp) {
            oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
        } else if (is_var) {
            // load into register from variable
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<RTL_Register_Opd> load_temp_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);
            std::shared_ptr<Variable_TAC_Opd> var =
                std::dynamic_pointer_cast<Variable_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Var_Opd> var_opd =
                std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());

            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(load_temp_reg_opd, var_opd);

            __rtl.addRTLStatement(load_stmt);
        } else if (is_int_const) {
            // load into register from variable
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<RTL_Register_Opd> load_temp_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);
            std::shared_ptr<Int_Const_TAC_Opd> int_const =
                std::dynamic_pointer_cast<Int_Const_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Int_Const_Opd> int_const_opd =
                std::make_shared<RTL_Int_Const_Opd>(int_const->get_value());

            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(load_temp_reg_opd,
                                                    int_const_opd);

            __rtl.addRTLStatement(load_stmt);
        } else if (is_float_const) {
            // load into register from variable
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<RTL_Register_Opd> load_temp_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);
            std::shared_ptr<Double_Const_TAC_Opd> float_const =
                std::dynamic_pointer_cast<Double_Const_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Double_Const_Opd> float_const_opd =
                std::make_shared<RTL_Double_Const_Opd>(
                    float_const->get_value());

            std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(load_temp_reg_opd,
                                                    float_const_opd);

            __rtl.addRTLStatement(load_stmt);
        }

        // uminus
        RegisterDescriptor *res_rd =
            __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
        std::shared_ptr<RTL_Register_Opd> res_opd =
            std::make_shared<RTL_Register_Opd>(res_rd);
        std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);

        std::shared_ptr<UMinus_RTL_Stmt> uminus_stmt =
            std::make_shared<UMinus_RTL_Stmt>(res_opd, oper1_reg_opd);

        __rtl.addRTLStatement(uminus_stmt);

        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
        res_rd->set_used_for_expr_return();
        oper1_rd->reset_used_for_expr_return();

        break;
    }

    case UnaryOperator::NOT: {
        RegisterDescriptor *oper1_rd;

        if (is_temp) {
            oper1_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(oper1);
        } else {
            // load value into temp
            oper1_rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(oper1);
            std::shared_ptr<RTL_Register_Opd> load_oper1_reg_opd =
                std::make_shared<RTL_Register_Opd>(oper1_rd);

            std::shared_ptr<Variable_TAC_Opd> var_tac_opd =
                std::dynamic_pointer_cast<Variable_TAC_Opd>(oper1);
            std::shared_ptr<RTL_Var_Opd> var_opd =
                std::make_shared<RTL_Var_Opd>(var_tac_opd->get_sym_tab_entry());

            std::shared_ptr<Transfer_RTL_Stmt> load_into_register_stmt =
                std::make_shared<Transfer_RTL_Stmt>(load_oper1_reg_opd,
                                                    var_opd);
            __rtl.addRTLStatement(load_into_register_stmt);
        }

        // not instruction
        RegisterDescriptor *res_rd =
            __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
        std::shared_ptr<RTL_Register_Opd> res_reg_opd =
            std::make_shared<RTL_Register_Opd>(res_rd);
        std::shared_ptr<RTL_Register_Opd> oper1_reg_opd =
            std::make_shared<RTL_Register_Opd>(oper1_rd);
        std::shared_ptr<Not_RTL_Stmt> not_stmt =
            std::make_shared<Not_RTL_Stmt>(res_reg_opd, oper1_reg_opd);

        __rtl.addRTLStatement(not_stmt);

        __rtl.machine_descriptor->unset_rd_for_tac_opd(oper1);
        res_rd->set_used_for_expr_return();
        oper1_rd->reset_used_for_expr_return();

        break;
    }
    }
}

Goto_TAC_Stmt::Goto_TAC_Stmt(std::shared_ptr<Label_TAC_Opd> l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Goto_TAC_Stmt::print(std::ostream &out) {
    out << "\tgoto " << result->get_name() << std::endl;
}

void Goto_TAC_Stmt::generateRTL(RTL &__rtl) {
    std::shared_ptr<Label_TAC_Opd> l =
        std::dynamic_pointer_cast<Label_TAC_Opd>(result);
    std::shared_ptr<RTL_Label_Opd> l_rtl_opd =
        std::make_shared<RTL_Label_Opd>(l->get_label_num());
    std::shared_ptr<Goto_RTL_Stmt> goto_stmt =
        std::make_shared<Goto_RTL_Stmt>(l_rtl_opd);
    __rtl.addRTLStatement(goto_stmt);
}

If_Goto_TAC_Stmt::If_Goto_TAC_Stmt(std::shared_ptr<TAC_Opd> cond,
                                   std::shared_ptr<Label_TAC_Opd> label) {
    result = cond;
    oper1 = label;
    oper2 = nullptr;
}

void If_Goto_TAC_Stmt::print(std::ostream &out) {
    out << "\tif(" << result->get_name() << ") goto " << oper1->get_name()
        << std::endl;
}

void If_Goto_TAC_Stmt::generateRTL(RTL &__rtl) {
    bool istemp = result->get_opd_type() == OpdType::TEMPORARY;

    std::shared_ptr<Label_TAC_Opd> label_tac_opd =
        std::dynamic_pointer_cast<Label_TAC_Opd>(oper1);
    std::shared_ptr<RTL_Label_Opd> l =
        std::make_shared<RTL_Label_Opd>(label_tac_opd->get_label_num());
    std::shared_ptr<If_Goto_RTL_Stmt> if_goto_stmt;
    RegisterDescriptor *rd;
    if (!istemp) {
        // load into temp
        rd = __rtl.machine_descriptor->allocate_rd_for_tac_opd(result);
        std::shared_ptr<RTL_Register_Opd> reg_opd =
            std::make_shared<RTL_Register_Opd>(rd);

        std::shared_ptr<Variable_TAC_Opd> var_tac_opd =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(result);
        std::shared_ptr<RTL_Var_Opd> var_opd =
            std::make_shared<RTL_Var_Opd>(var_tac_opd->get_sym_tab_entry());

        // load statement
        std::shared_ptr<Transfer_RTL_Stmt> load_stmt =
            std::make_shared<Transfer_RTL_Stmt>(reg_opd, var_opd);
        __rtl.addRTLStatement(load_stmt);

        // construct the if-goto statement
        std::shared_ptr<RTL_Register_Opd> reg =
            std::make_shared<RTL_Register_Opd>(rd);
        if_goto_stmt = std::make_shared<If_Goto_RTL_Stmt>(reg, l);
    } else {
        // need to get register descriptor corresponding to that temp number
        rd = __rtl.machine_descriptor->get_rd_for_tac_opd(result);

        std::shared_ptr<RTL_Register_Opd> reg_opd =
            std::make_shared<RTL_Register_Opd>(rd);

        if_goto_stmt = std::make_shared<If_Goto_RTL_Stmt>(reg_opd, l);
    }

    __rtl.addRTLStatement(if_goto_stmt);

    __rtl.machine_descriptor->unset_rd_for_tac_opd(result);
    rd->reset_used_for_expr_return();
}

IO_TAC_Stmt::IO_TAC_Stmt(bool write, std::shared_ptr<TAC_Opd> oper)
    : is_write(write) {
    result = oper;
    oper1 = nullptr;
    oper2 = nullptr;
}

void IO_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << (is_write ? "write  " : "read  ") << result->get_name()
        << std::endl;
}

void IO_TAC_Stmt::generateRTL(RTL &__rtl) {
    RegisterDescriptor *rd =
        __rtl.machine_descriptor->get_register(Register::v0);
    RegisterDescriptor *extra_reg = nullptr;
    bool v0_free = __rtl.machine_descriptor->get_register(Register::v0)
                       ->is_free<RegisterUseCategory::int_reg>();
    // first check if v0 is free
    if (!v0_free) {
        // get a new int_reg
        extra_reg = __rtl.machine_descriptor
                        ->get_new_register<RegisterUseCategory::int_reg>();
        // move v0 into that reg
        std::shared_ptr<RTL_Register_Opd> src =
            std::make_shared<RTL_Register_Opd>(rd);
        std::shared_ptr<RTL_Register_Opd> dest =
            std::make_shared<RTL_Register_Opd>(extra_reg);
        std::shared_ptr<Transfer_RTL_Stmt> mov =
            std::make_shared<Transfer_RTL_Stmt>(dest, src);

        __rtl.addRTLStatement(mov);
    }
    if (is_write) {
        bool is_temp = result->get_opd_type() == OpdType::TEMPORARY;
        bool is_var = (result->get_opd_type() == OpdType::VARIABLE) &&
                      (std::dynamic_pointer_cast<Variable_TAC_Opd>(result)
                           ->get_sym_tab_entry()
                           ->get_type() != BaseType::STRING);
        bool is_int_const = result->get_opd_type() == OpdType::INT_CONST;
        bool is_float_const = result->get_opd_type() == OpdType::DOUBLE_CONST;
        bool is_string_const = result->get_opd_type() == OpdType::STRING_CONST;
        bool is_string_var =
            ((result->get_opd_type() == OpdType::VARIABLE) &&
             (std::dynamic_pointer_cast<Variable_TAC_Opd>(result)
                  ->get_sym_tab_entry()
                  ->get_type() == BaseType::STRING));

        std::shared_ptr<Variable_TAC_Opd> var =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(result);
        std::shared_ptr<Temporary_TAC_Opd> temp =
            std::dynamic_pointer_cast<Temporary_TAC_Opd>(result);
        std::shared_ptr<Int_Const_TAC_Opd> int_const =
            std::dynamic_pointer_cast<Int_Const_TAC_Opd>(result);
        std::shared_ptr<Double_Const_TAC_Opd> float_const =
            std::dynamic_pointer_cast<Double_Const_TAC_Opd>(result);
        std::shared_ptr<String_Const_TAC_Opd> string_const =
            std::dynamic_pointer_cast<String_Const_TAC_Opd>(result);

        bool needfloat =
            (is_var && var->get_sym_tab_entry()->get_need_float()) ||
            (is_temp && temp->get_need_float()) || is_float_const;

        // load correct value into v0
        std::shared_ptr<RTL_Int_Const_Opd> num;
        std::shared_ptr<RTL_Register_Opd> reg =
            std::make_shared<RTL_Register_Opd>(rd);
        if (needfloat) {
            num = std::make_shared<RTL_Int_Const_Opd>(3);
        } else if (is_var || is_temp || is_int_const) {
            num = std::make_shared<RTL_Int_Const_Opd>(1);
        } else if (is_string_const || is_string_var) {
            num = std::make_shared<RTL_Int_Const_Opd>(4);
        }

        std::shared_ptr<Transfer_RTL_Stmt> iload_stmt =
            std::make_shared<Transfer_RTL_Stmt>(reg, num);
        __rtl.addRTLStatement(iload_stmt);

        // load into correct register
        if (needfloat) {
            rd = __rtl.machine_descriptor->get_register(Register::f12);
        } else if (!needfloat) {
            rd = __rtl.machine_descriptor->get_register(Register::a0);
        }

        reg = std::make_shared<RTL_Register_Opd>(rd);
        std::shared_ptr<Transfer_RTL_Stmt> load_stmt;

        RegisterDescriptor *temp_rd;
        if (is_temp) {
            temp_rd = __rtl.machine_descriptor->get_rd_for_tac_opd(result);
            std::shared_ptr<RTL_Register_Opd> reg_op =
                std::make_shared<RTL_Register_Opd>(temp_rd);
            load_stmt = std::make_shared<Transfer_RTL_Stmt>(reg, reg_op);
        } else if (is_var || is_string_var) {
            std::shared_ptr<RTL_Var_Opd> var_opd =
                std::make_shared<RTL_Var_Opd>(var->get_sym_tab_entry());
            load_stmt = std::make_shared<Transfer_RTL_Stmt>(reg, var_opd);
        } else if (is_int_const) {
            std::shared_ptr<RTL_Int_Const_Opd> int_const_opd =
                std::make_shared<RTL_Int_Const_Opd>(int_const->get_value());
            load_stmt = std::make_shared<Transfer_RTL_Stmt>(reg, int_const_opd);
        } else if (is_float_const) {
            std::shared_ptr<RTL_Double_Const_Opd> float_const_opd =
                std::make_shared<RTL_Double_Const_Opd>(
                    float_const->get_value());
            load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(reg, float_const_opd);
        } else if (is_string_const) {
            // add string const to string const table of rtl
            std::string str_value = string_const->get_name();

            unsigned int str_num = __rtl.getStringConstNum(str_value);

            std::shared_ptr<RTL_String_Const_Opd> string_const_opd =
                std::make_shared<RTL_String_Const_Opd>(str_num, str_value);
            load_stmt =
                std::make_shared<Transfer_RTL_Stmt>(reg, string_const_opd);
        }

        __rtl.addRTLStatement(load_stmt);

        __rtl.machine_descriptor->unset_rd_for_tac_opd(result);

        if (is_temp)
            temp_rd->reset_used_for_expr_return();

        // write instruction
        std::shared_ptr<Write_RTL_Stmt> write_stmt =
            std::make_shared<Write_RTL_Stmt>();
        __rtl.addRTLStatement(write_stmt);
    } else {
        std::shared_ptr<Variable_TAC_Opd> var_tac_opd =
            std::dynamic_pointer_cast<Variable_TAC_Opd>(result);

        std::shared_ptr<RTL_Int_Const_Opd> num;
        std::shared_ptr<RTL_Register_Opd> reg =
            std::make_shared<RTL_Register_Opd>(rd);

        // choose appropriate value to load into v0
        if (var_tac_opd->get_sym_tab_entry()->get_need_float()) {
            num = std::make_shared<RTL_Int_Const_Opd>(7);
        } else {
            num = std::make_shared<RTL_Int_Const_Opd>(5);
        }

        std::shared_ptr<Transfer_RTL_Stmt> iload_stmt =
            std::make_shared<Transfer_RTL_Stmt>(reg, num);
        __rtl.addRTLStatement(iload_stmt);

        std::shared_ptr<Read_RTL_Stmt> read_stmt =
            std::make_shared<Read_RTL_Stmt>();
        __rtl.addRTLStatement(read_stmt);

        std::shared_ptr<RTL_Var_Opd> var_rtl_opd =
            std::make_shared<RTL_Var_Opd>(var_tac_opd->get_sym_tab_entry());

        // select appropriate register to move result from
        if (var_tac_opd->get_sym_tab_entry()->get_need_float()) {
            rd = __rtl.machine_descriptor->get_register(Register::f0);
        } else {
            rd = __rtl.machine_descriptor->get_register(Register::v0);
        }

        std::shared_ptr<RTL_Register_Opd> reg_opd =
            std::make_shared<RTL_Register_Opd>(rd);

        std::shared_ptr<Transfer_RTL_Stmt> store_stmt =
            std::make_shared<Transfer_RTL_Stmt>(var_rtl_opd, reg_opd);
        __rtl.addRTLStatement(store_stmt);
    }
    if (!v0_free) {
        // move extra_reg stuff back to rd
        std::shared_ptr<RTL_Register_Opd> dest =
            std::make_shared<RTL_Register_Opd>(rd);
        std::shared_ptr<RTL_Register_Opd> src =
            std::make_shared<RTL_Register_Opd>(extra_reg);
        std::shared_ptr<Transfer_RTL_Stmt> mov =
            std::make_shared<Transfer_RTL_Stmt>(dest, src);

        __rtl.addRTLStatement(mov);

        extra_reg->reset_used_for_expr_return();
    }
}

Label_TAC_Stmt::Label_TAC_Stmt(std::shared_ptr<Label_TAC_Opd> label) {
    result = label;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Label_TAC_Stmt::print(std::ostream &out) {
    out << result->get_name() << ":" << std::endl;
}

void Label_TAC_Stmt::generateRTL(RTL &__rtl) {
    std::shared_ptr<Label_TAC_Opd> temp =
        std::dynamic_pointer_cast<Label_TAC_Opd>(result);
    std::shared_ptr<RTL_Label_Opd> l =
        std::make_shared<RTL_Label_Opd>(temp->get_label_num());

    std::shared_ptr<Label_RTL_Stmt> stmt = std::make_shared<Label_RTL_Stmt>(l);

    __rtl.addRTLStatement(stmt);
}

Return_TAC_Stmt::Return_TAC_Stmt(std::shared_ptr<Variable_TAC_Opd> stemp,
                                 std::string name)
    : func_name(name) {
    result = stemp;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Return_TAC_Stmt::print(std::ostream &out) {
    out << "\t" << "return " << result->get_name() << std::endl;
}

void Return_TAC_Stmt::generateRTL(RTL &__rtl) {
    // get register v1/f0, and store variable in v1/f0
    std::shared_ptr<Variable_TAC_Opd> res =
        std::dynamic_pointer_cast<Variable_TAC_Opd>(result);
    std::shared_ptr<RTL_Var_Opd> stemp =
        std::make_shared<RTL_Var_Opd>(res->get_sym_tab_entry());
    RegisterDescriptor *rd =
        (res->get_sym_tab_entry()->get_need_float())
            ? __rtl.machine_descriptor->get_register(Register::f0)
            : __rtl.machine_descriptor->get_register(Register::v1);
    std::shared_ptr<RTL_Register_Opd> reg =
        std::make_shared<RTL_Register_Opd>(rd);
    std::shared_ptr<Transfer_RTL_Stmt> stmt =
        std::make_shared<Transfer_RTL_Stmt>(reg, stemp);
    __rtl.addRTLStatement(stmt);

    // make return_rtl statement and return v1/f0
    // rtl_return translates to go_to_epilogue
    std::shared_ptr<Return_RTL_Stmt> ret =
        std::make_shared<Return_RTL_Stmt>(reg, func_name);
    __rtl.addRTLStatement(ret);
}
