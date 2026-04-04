#pragma once
#include "tac_operand.hpp"

class RegisterDescriptor {
  public:
    Register reg_id;
    std::string reg_name;
    RegisterType reg_type;
    RegisterUseCategory reg_use_cat;

    bool used_for_expr_result;
    bool reg_occupied;
    bool used_for_fn_result;
    RegisterDescriptor(Register reg, std::string _name, RegisterType type,
                       RegisterUseCategory use_cat);

    RegisterUseCategory get_use_category();
    Register get_register();
    std::string get_name();

    bool is_register_occupied();
    void set_register_occupied();
    void reset_register_occupied();

    bool is_used_for_fn_return();
    void set_used_for_fn_return();
    void reset_used_for_fn_return();

    bool is_used_for_expr_return();
    void set_used_for_expr_return();
    void reset_used_for_expr_return();

    template <RegisterUseCategory dt> bool is_free();
};

class MachineDescriptor {
  public:
    std::map<Register, RegisterDescriptor *> register_table;

    // std::shared_ptr<TAC_Opd> -> RegisterDescriptor *
    std::map<std::shared_ptr<TAC_Opd>, RegisterDescriptor *> tac_opd_to_rd;

    MachineDescriptor();
    ~MachineDescriptor();

    void initialize_register_table();

    void clear_reg_not_used_for_expr_result();

    RegisterDescriptor *get_register(Register r);

    RegisterDescriptor *
    allocate_rd_for_tac_opd(std::shared_ptr<TAC_Opd> tac_opd);
    RegisterDescriptor *get_rd_for_tac_opd(std::shared_ptr<TAC_Opd> tac_opd);
    void unset_rd_for_tac_opd(std::shared_ptr<TAC_Opd> tac_opd);
    void clear_tac_opd_to_rd();

    template <RegisterUseCategory dt> int count_free_register();

    template <RegisterUseCategory dt> RegisterDescriptor *get_new_register();
};
