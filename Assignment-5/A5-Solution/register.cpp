#include "register.hpp"

RegisterDescriptor::RegisterDescriptor(Register reg, std::string _name,
                                       RegisterType type,
                                       RegisterUseCategory use_cat) {
    reg_id = reg;
    reg_name = _name;
    reg_type = type;
    reg_use_cat = use_cat;

    used_for_expr_result = false;
    reg_occupied = false;
    used_for_fn_result = false;
}

RegisterUseCategory RegisterDescriptor::get_use_category() {
    return reg_use_cat;
}
Register RegisterDescriptor::get_register() { return reg_id; }
std::string RegisterDescriptor::get_name() { return reg_name; }

bool RegisterDescriptor::is_register_occupied() { return reg_occupied; }
void RegisterDescriptor::set_register_occupied() { reg_occupied = true; }
void RegisterDescriptor::reset_register_occupied() { reg_occupied = false; }

bool RegisterDescriptor::is_used_for_fn_return() { return used_for_fn_result; }
void RegisterDescriptor::set_used_for_fn_return() { used_for_fn_result = true; }
void RegisterDescriptor::reset_used_for_fn_return() {
    used_for_fn_result = false;
}

bool RegisterDescriptor::is_used_for_expr_return() {
    return used_for_expr_result;
}
void RegisterDescriptor::set_used_for_expr_return() {
    used_for_expr_result = true;
}
void RegisterDescriptor::reset_used_for_expr_return() {
    used_for_expr_result = false;
    reset_register_occupied();
}

template <RegisterUseCategory dt> bool RegisterDescriptor::is_free() {
    return reg_use_cat == dt && !is_used_for_expr_return() &&
           !is_used_for_fn_return();
}

template bool RegisterDescriptor::is_free<RegisterUseCategory::int_reg>();
template bool RegisterDescriptor::is_free<RegisterUseCategory::float_reg>();

// ---------------------------------------------------------------------------------------------------------------

MachineDescriptor::MachineDescriptor() { initialize_register_table(); }

MachineDescriptor::~MachineDescriptor() {
    for (auto [_, rd] : register_table)
        delete rd;
}

void MachineDescriptor::initialize_register_table() {
    register_table[Register::zero] =
        new RegisterDescriptor(Register::zero, "zero", RegisterType::int_num,
                               RegisterUseCategory::fixed_reg);
    register_table[Register::v0] =
        new RegisterDescriptor(Register::v0, "v0", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::v1] =
        new RegisterDescriptor(Register::v1, "v1", RegisterType::int_num,
                               RegisterUseCategory::fn_result);
    register_table[Register::a0] =
        new RegisterDescriptor(Register::a0, "a0", RegisterType::int_num,
                               RegisterUseCategory::argument);
    register_table[Register::a1] =
        new RegisterDescriptor(Register::a1, "a1", RegisterType::int_num,
                               RegisterUseCategory::argument);
    register_table[Register::a2] =
        new RegisterDescriptor(Register::a2, "a2", RegisterType::int_num,
                               RegisterUseCategory::argument);
    register_table[Register::a3] =
        new RegisterDescriptor(Register::a3, "a3", RegisterType::int_num,
                               RegisterUseCategory::argument);
    register_table[Register::t0] =
        new RegisterDescriptor(Register::t0, "t0", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t1] =
        new RegisterDescriptor(Register::t1, "t1", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t2] =
        new RegisterDescriptor(Register::t2, "t2", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t3] =
        new RegisterDescriptor(Register::t3, "t3", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t4] =
        new RegisterDescriptor(Register::t4, "t4", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t5] =
        new RegisterDescriptor(Register::t5, "t5", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t6] =
        new RegisterDescriptor(Register::t6, "t6", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t7] =
        new RegisterDescriptor(Register::t7, "t7", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t8] =
        new RegisterDescriptor(Register::t8, "t8", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::t9] =
        new RegisterDescriptor(Register::t9, "t9", RegisterType::int_num,
                               RegisterUseCategory::int_reg);

    register_table[Register::s0] =
        new RegisterDescriptor(Register::s0, "s0", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::s1] =
        new RegisterDescriptor(Register::s1, "s1", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::s2] =
        new RegisterDescriptor(Register::s2, "s2", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::s3] =
        new RegisterDescriptor(Register::s3, "s3", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::s4] =
        new RegisterDescriptor(Register::s4, "s4", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::s5] =
        new RegisterDescriptor(Register::s5, "s5", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::s6] =
        new RegisterDescriptor(Register::s6, "s6", RegisterType::int_num,
                               RegisterUseCategory::int_reg);
    register_table[Register::s7] =
        new RegisterDescriptor(Register::s7, "s7", RegisterType::int_num,
                               RegisterUseCategory::int_reg);

    register_table[Register::f0] =
        new RegisterDescriptor(Register::f0, "f0", RegisterType::float_num,
                               RegisterUseCategory::fn_result);
    register_table[Register::f2] =
        new RegisterDescriptor(Register::f2, "f2", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f4] =
        new RegisterDescriptor(Register::f4, "f4", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f6] =
        new RegisterDescriptor(Register::f6, "f6", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f8] =
        new RegisterDescriptor(Register::f8, "f8", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f10] =
        new RegisterDescriptor(Register::f10, "f10", RegisterType::float_num,
                               RegisterUseCategory::fn_result);
    register_table[Register::f12] =
        new RegisterDescriptor(Register::f12, "f12", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f14] =
        new RegisterDescriptor(Register::f14, "f14", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f16] =
        new RegisterDescriptor(Register::f16, "f16", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f18] =
        new RegisterDescriptor(Register::f18, "f18", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f20] =
        new RegisterDescriptor(Register::f20, "f20", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f22] =
        new RegisterDescriptor(Register::f22, "f22", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f24] =
        new RegisterDescriptor(Register::f24, "f24", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f26] =
        new RegisterDescriptor(Register::f26, "f26", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f28] =
        new RegisterDescriptor(Register::f28, "f28", RegisterType::float_num,
                               RegisterUseCategory::float_reg);
    register_table[Register::f30] =
        new RegisterDescriptor(Register::f30, "f30", RegisterType::float_num,
                               RegisterUseCategory::float_reg);

    register_table[Register::gp] =
        new RegisterDescriptor(Register::gp, "gp", RegisterType::int_num,
                               RegisterUseCategory::pointer);
    register_table[Register::sp] =
        new RegisterDescriptor(Register::sp, "sp", RegisterType::int_num,
                               RegisterUseCategory::pointer);
    register_table[Register::fp] =
        new RegisterDescriptor(Register::fp, "fp", RegisterType::int_num,
                               RegisterUseCategory::pointer);
    register_table[Register::ra] =
        new RegisterDescriptor(Register::ra, "ra", RegisterType::int_num,
                               RegisterUseCategory::ret_address);
}

void MachineDescriptor::clear_reg_not_used_for_expr_result() {
    std::map<Register, RegisterDescriptor *>::iterator i;

    for (i = register_table.begin(); i != register_table.end(); i++) {
        RegisterDescriptor *rd = i->second;

        if (!rd->is_used_for_expr_return()) {
            rd->reset_register_occupied();
            // break; // sus, removed it for now
        }
    }
}

RegisterDescriptor *MachineDescriptor::get_register(Register r) {
    return register_table[r];
}

RegisterDescriptor *
MachineDescriptor::allocate_rd_for_tac_opd(std::shared_ptr<TAC_Opd> tac_opd) {
    bool is_temp = tac_opd->get_opd_type() == OpdType::TEMPORARY;
    bool is_var = tac_opd->get_opd_type() == OpdType::VARIABLE;
    bool is_float_const = tac_opd->get_opd_type() == OpdType::DOUBLE_CONST;
    bool is_func = tac_opd->get_opd_type() == OpdType::FUNCTION;

    bool needfloat =
        (is_temp && (std::dynamic_pointer_cast<Temporary_TAC_Opd>(tac_opd))
                        ->get_need_float()) ||
        (is_var && (std::dynamic_pointer_cast<Variable_TAC_Opd>(tac_opd))
                       ->get_sym_tab_entry()
                       ->get_need_float()) ||
        is_float_const ||
        (is_func && (std::dynamic_pointer_cast<Function_TAC_Opd>(tac_opd))
                        ->get_need_float());

    RegisterDescriptor *new_rd;
    if (needfloat) {
        new_rd = get_new_register<RegisterUseCategory::float_reg>();
    } else {
        new_rd = get_new_register<RegisterUseCategory::int_reg>();
    }
    new_rd->set_used_for_expr_return();
    tac_opd_to_rd[tac_opd] = new_rd;
    return new_rd;
}

RegisterDescriptor *
MachineDescriptor::get_rd_for_tac_opd(std::shared_ptr<TAC_Opd> tac_opd) {
    if (tac_opd_to_rd.find(tac_opd) == tac_opd_to_rd.end()) return nullptr;
    return tac_opd_to_rd[tac_opd];
}

void MachineDescriptor::unset_rd_for_tac_opd(std::shared_ptr<TAC_Opd> tac_opd) {
    if (tac_opd_to_rd.find(tac_opd) != tac_opd_to_rd.end())
        tac_opd_to_rd.erase(tac_opd);
}

void MachineDescriptor::clear_tac_opd_to_rd() { tac_opd_to_rd.clear(); }

RegisterDescriptor * MachineDescriptor::get_rd_for_func(std::shared_ptr<Function_TAC_Opd> func_tac_opd) {
    bool needfloat = func_tac_opd->get_sym_tab_func()->get_return_type() == BaseType::FLOAT;
    Register reg = needfloat ? Register::f0 : Register::v1;
    RegisterDescriptor * rd = register_table[reg];
    rd->set_used_for_expr_return(); // ideally set used for fn return
    return rd;
}

void MachineDescriptor::set_rd_for_tac_opd(std::shared_ptr<TAC_Opd> tac_opd, RegisterDescriptor * rd) {
    tac_opd_to_rd[tac_opd] = rd;
}

template <RegisterUseCategory dt> int MachineDescriptor::count_free_register() {
    std::map<Register, RegisterDescriptor *>::iterator i;

    int count = 0;

    for (i = register_table.begin(); i != register_table.end(); i++) {
        if (i->second->is_free<dt>())
            count++;
    }

    return count;
}

// sus
template <RegisterUseCategory dt>
RegisterDescriptor *MachineDescriptor::get_new_register() {
    RegisterDescriptor *reg_desc;

    std::map<Register, RegisterDescriptor *>::iterator i;
    for (i = register_table.begin(); i != register_table.end(); i++) {
        reg_desc = i->second;

        if (reg_desc->is_free<dt>()) {
            reg_desc->set_register_occupied();
            reg_desc->set_used_for_expr_return();
            return reg_desc;
        }
    }

    exit_with_err_msg("kys");
    return nullptr;
}

template int
MachineDescriptor::count_free_register<RegisterUseCategory::int_reg>();
template int
MachineDescriptor::count_free_register<RegisterUseCategory::float_reg>();

template RegisterDescriptor *
MachineDescriptor::get_new_register<RegisterUseCategory::int_reg>();
template RegisterDescriptor *
MachineDescriptor::get_new_register<RegisterUseCategory::float_reg>();
