#include "common_utils.hpp"
#include "symbol_table.hpp"
#include "tac.hpp"
#include <map>
#include <list>

typedef enum {
    int_num,
    float_num,
} RegisterType;

typedef enum {
    fixed_reg,
    int_reg,
    fn_result,
    argument,
    pointer,
    ret_address,
    float_reg
} RegisterUseCategory;

class RegisterDescriptor {
    Register reg_id;
    std::string reg_name;
    RegisterType reg_type;
    RegisterUseCategory reg_use_cat;

    bool used_for_expr_result;
    bool reg_occupied;
    bool used_for_fn_result;

public:
    RegisterDescriptor(Register reg, std::string _name, RegisterType type, RegisterUseCategory use_cat);

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

    template<RegisterUseCategory dt>
    bool is_free();
};

class MachineDescriptor {
public:
    std::map<Register, RegisterDescriptor *> register_table;

    // temp number -> RegisterDescriptor *
    std::map<unsigned int, RegisterDescriptor *> temp_to_rd;

    MachineDescriptor();
    ~MachineDescriptor();

    void initialize_register_table();

    void clear_reg_not_used_for_expr_result();

    RegisterDescriptor * get_register(Register r);

    RegisterDescriptor * get_rd_for_temp(unsigned int temp_num);
    void unset_rd_for_temp(unsigned int temp_num);

    template <RegisterUseCategory dt>
    int count_free_register();

    template <RegisterUseCategory dt>
    RegisterDescriptor * get_new_register();
};
