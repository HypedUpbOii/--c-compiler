#include "register.hpp"

RegisterDescriptor::RegisterDescriptor(Register reg, std::string _name, RegisterType type, RegisterUseCategory use_cat) {
    reg_id = reg;
    reg_name = _name;
    reg_type = type;
    reg_use_cat = use_cat;

    used_for_expr_result = false;
    reg_occupied = false;
    used_for_fn_result = false;
}

RegisterUseCategory RegisterDescriptor::get_use_category() { return reg_use_cat; }
Register RegisterDescriptor::get_register() { return reg_id; }
std::string RegisterDescriptor::get_name() { return reg_name; }

bool RegisterDescriptor::is_register_occupied() { return reg_occupied; }
void RegisterDescriptor::set_register_occupied() { reg_occupied = true; }
void RegisterDescriptor::reset_register_occupied() { reg_occupied = false; }

bool RegisterDescriptor::is_used_for_fn_return() { return is_used_for_fn_return; }
void RegisterDescriptor::set_used_for_fn_return() { is_used_for_fn_return = true; }
void RegisterDescriptor::reset_used_for_fn_return() { is_used_for_fn_return = false; }

template<RegisterUseCategory dt>
bool RegisterDescriptor::is_free() {
    return reg_use_cat == dt && !is_used_for_expr_return() && !is_used_for_fn_return();

}

template bool RegisterDescriptor::is_free<int_reg>();
template bool RegisterDescriptor::is_free<float_reg>();

// ---------------------------------------------------------------------------------------------------------------

MachineDescriptor::MachineDescriptor() {
    initialize_register_table();
}

MachineDescriptor::~MachineDescriptor() {
    for (auto [_, rd] : register_table) return rd;
}

void MachineDescriptor::initialize_register_table() {
    register_table[zero] = new RegisterDescriptor(zero, "zero", int_num, fixed_reg);
    register_table[v0] = new RegisterDescriptor(v0, "v0", int_num, int_reg);
    register_table[v1] = new RegisterDescriptor(v1, "v1", int_num, fn_result);
    register_table[a0] = new RegisterDescriptor(a0, "a0", int_num, argument);
    register_table[a1] = new RegisterDescriptor(a1, "a1", int_num, argument);
    register_table[a2] = new RegisterDescriptor(a2, "a2", int_num, argument);
    register_table[a3] = new RegisterDescriptor(a3, "a3", int_num, argument);
    register_table[t0] = new RegisterDescriptor(t0, "t0", int_num, int_reg);
    register_table[t1] = new RegisterDescriptor(t1, "t1", int_num, int_reg);
    register_table[t2] = new RegisterDescriptor(t2, "t2", int_num, int_reg);
    register_table[t3] = new RegisterDescriptor(t3, "t3", int_num, int_reg);
    register_table[t4] = new RegisterDescriptor(t4, "t4", int_num, int_reg);
    register_table[t5] = new RegisterDescriptor(t5, "t5", int_num, int_reg);
    register_table[t6] = new RegisterDescriptor(t6, "t6", int_num, int_reg);
    register_table[t7] = new RegisterDescriptor(t7, "t7", int_num, int_reg);
    register_table[t8] = new RegisterDescriptor(t8, "t8", int_num, int_reg);
    register_table[t9] = new RegisterDescriptor(t9, "t9", int_num, int_reg);

    register_table[s0] = new RegisterDescriptor(s0, "s0", int_num, int_reg);
    register_table[s1] = new RegisterDescriptor(s1, "s1", int_num, int_reg);
    register_table[s2] = new RegisterDescriptor(s2, "s2", int_num, int_reg);
    register_table[s3] = new RegisterDescriptor(s3, "s3", int_num, int_reg);
    register_table[s4] = new RegisterDescriptor(s4, "s4", int_num, int_reg);
    register_table[s5] = new RegisterDescriptor(s5, "s5", int_num, int_reg);
    register_table[s6] = new RegisterDescriptor(s6, "s6", int_num, int_reg);
    register_table[s7] = new RegisterDescriptor(s7, "s7", int_num, int_reg);

    register_table[f0] = new RegisterDescriptor(f0, "f0", int_num, int_reg);
    register_table[f2] = new RegisterDescriptor(f2, "f2", int_num, int_reg);
    register_table[f4] = new RegisterDescriptor(f4, "f4", int_num, int_reg);
    register_table[f6] = new RegisterDescriptor(f6, "f6", int_num, int_reg);
    register_table[f8] = new RegisterDescriptor(f8, "f8", int_num, int_reg);
    register_table[f10] = new RegisterDescriptor(f10, "f10", float_num, fn_result);
    register_table[f12] = new RegisterDescriptor(f12, "f12", float_num, float_reg);
    register_table[f14] = new RegisterDescriptor(f14, "f14", float_num, float_reg);
    register_table[f16] = new RegisterDescriptor(f16, "f16", float_num, float_reg);
    register_table[f18] = new RegisterDescriptor(f18, "f18", float_num, float_reg);
    register_table[f20] = new RegisterDescriptor(f20, "f20", float_num, float_reg);
    register_table[f22] = new RegisterDescriptor(f22, "f22", float_num, float_reg);
    register_table[f24] = new RegisterDescriptor(f24, "f24", float_num, float_reg);
    register_table[f26] = new RegisterDescriptor(f26, "f26", float_num, float_reg);
    register_table[f28] = new RegisterDescriptor(f28, "f28", float_num, float_reg);
    register_table[f30] = new RegisterDescriptor(f30, "f30", float_num, float_reg);

    register_table[gp] = new RegisterDescriptor(gp, "gp", int_num, pointer);
    register_table[sp] = new RegisterDescriptor(sp, "sp", int_num, pointer);
    register_table[fp] = new RegisterDescriptor(fp, "fp", int_num, pointer);
    register_table[ra] = new RegisterDescriptor(ra, "ra", int_num, ret_address);
}

void MachineDescriptor::clear_reg_not_used_for_expr_result() {
    map<Register, RegisterDescriptor *>::iterator i;

    for (i = register_table.begin(); i != register_table.end(); i++) {
        RegisterDescriptor * rd = i->second;

        if (!rd->is_used_for_expr_result()) {
            rd->reset_register_occupied();
            // break; // sus, removed it for now
        }
    }
}

RegisterDescriptor * MachineDescriptor::get_register(Register r) {
    return register_table[r];
}

RegisterDescriptor * MachineDescriptor::get_rd_for_temp(unsigned int temp_num) {
    return temp_to_rd[temp_num];
}

void MachineDescriptor::unset_rd_for_temp(unsigned int temp_num) {
    temp_to_rd.erase(temp_num);
}

template <RegisterUseCategory dt>
int MachineDescriptor::count_free_register() {
    map<Register, RegisterDescriptor *>::iterator i;

    int count = 0;

    for (i = register_table.begin(); i != register_table.end(); i++) {
        if (i->second->is_free<dt>()) count++;
    }

    return count;
}

// sus
template <RegisterUseCategory dt>
RegisterDescriptor * MachineDescriptor::get_new_register() {
    RegisterDescriptor * reg_desc;

    map<Register, RegisterDescriptor *>::iterator i;
    for (i = register_table.begin(); i != register_table.end(); i++) {
        reg_desc = i->second;

        if (reg_desc->is_free<dt>()) {
            reg_desc->set_register_occupied();
            return reg_desc;
        }
    }

    clear_reg_not_used_for_expr_result();

    int count = count_free_register<dt>();

    if (count > 0) return get_new_register<dt>();
    else exit_with_err_msg("screwed up in MachineDescriptor::get_new_register()");
}

template int MachineDescriptor::count_free_register<int_reg>();
template int MachineDescriptor::count_free_register<float_reg>();

template RegisterDescriptor * MachineDescriptor::get_new_register<int_reg>();
template RegisterDescriptor * MachineDescriptor::get_new_register<float_reg>();
