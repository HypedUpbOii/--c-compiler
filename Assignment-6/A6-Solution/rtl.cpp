#include "rtl.hpp"
#include <iostream>

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

void Arithmetic_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> res = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(result))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op1 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op2 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper2))->reg_desc);

    std::shared_ptr<Arithmetic_ASM_Stmt> stmt =
        std::make_shared<Arithmetic_ASM_Stmt>(res, op1, op2, oper);

    spim.addSPIM(stmt);
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

void Boolean_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> res = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(result))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op1 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op2 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper2))->reg_desc);

    std::shared_ptr<Boolean_ASM_Stmt> stmt =
        std::make_shared<Boolean_ASM_Stmt>(res, op1, op2, oper);

    spim.addSPIM(stmt);
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

void Relational_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> op1 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op2 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper2))->reg_desc);

    std::shared_ptr<Relational_ASM_Stmt> stmt;

    if (isfloat) {
        stmt = std::make_shared<Relational_ASM_Stmt>(op1, op2, oper);
    } else {
        std::shared_ptr<ASM_Register_Opd> res =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(result))
                    ->reg_desc);

        stmt = std::make_shared<Relational_ASM_Stmt>(res, op1, op2, oper);
    }

    spim.addSPIM(stmt);
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

void UMinus_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> res = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(result))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op1 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

    std::shared_ptr<UMinus_ASM_Stmt> stmt =
        std::make_shared<UMinus_ASM_Stmt>(res, op1);

    spim.addSPIM(stmt);
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

void Not_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> res = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(result))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op1 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

    std::shared_ptr<Not_ASM_Stmt> stmt =
        std::make_shared<Not_ASM_Stmt>(res, op1);

    spim.addSPIM(stmt);
}

Call_RTL_Stmt::Call_RTL_Stmt(SymbolTableFunction *ste,
                             std::shared_ptr<RTL_Register_Opd> reg_op) {
    entry = ste;
    reg_opd = reg_op;

    result = nullptr;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Call_RTL_Stmt::print(std::ostream &out) {
    switch (entry->get_return_type().base) {
    case BaseType::VOID: {
        out << "\tcall " << entry->get_name() << std::endl;
        return;
    }
    default: {
        out << "\t" << reg_opd->get_name() << " = call " << entry->get_name()
            << std::endl;
        return;
    }
    }
}

void Call_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Label_Opd> label =
        std::make_shared<ASM_Label_Opd>(entry->get_name());

    std::shared_ptr<Call_ASM_Stmt> stmt =
        std::make_shared<Call_ASM_Stmt>(label);

    spim.addSPIM(stmt);
}

Goto_RTL_Stmt::Goto_RTL_Stmt(std::shared_ptr<RTL_Label_Opd> l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Goto_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "goto:\t\t" << result->get_name() << std::endl;
}

void Goto_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Label_Opd> label = std::make_shared<ASM_Label_Opd>(
        std::dynamic_pointer_cast<RTL_Label_Opd>(result)->label_num);

    std::shared_ptr<Goto_ASM_Stmt> stmt =
        std::make_shared<Goto_ASM_Stmt>(label);

    spim.addSPIM(stmt);
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

void If_Goto_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Label_Opd> label = std::make_shared<ASM_Label_Opd>(
        std::dynamic_pointer_cast<RTL_Label_Opd>(result)->label_num);

    std::shared_ptr<ASM_Register_Opd> op1 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

    std::shared_ptr<If_Goto_ASM_Stmt> stmt =
        std::make_shared<If_Goto_ASM_Stmt>(op1, label);

    spim.addSPIM(stmt);
}

Return_RTL_Stmt::Return_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> reg,
                                 std::string name)
    : func_name(name) {
    result = reg;
}

void Return_RTL_Stmt::print(std::ostream &out) {
    out << "\treturn\t" << result->get_name() << std::endl;
}

void Return_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Label_Opd> label =
        std::make_shared<ASM_Label_Opd>("epilogue_" + func_name);

    std::shared_ptr<Goto_ASM_Stmt> stmt =
        std::make_shared<Goto_ASM_Stmt>(label);

    spim.addSPIM(stmt);
}

Label_RTL_Stmt::Label_RTL_Stmt(std::shared_ptr<RTL_Label_Opd> l) {
    result = l;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Label_RTL_Stmt::print(std::ostream &out) {
    out << std::endl << result->get_name() << ":" << std::endl;
}

void Label_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Label_Opd> label = std::make_shared<ASM_Label_Opd>(
        std::dynamic_pointer_cast<RTL_Label_Opd>(result)->label_num);

    std::shared_ptr<Label_ASM_Stmt> stmt =
        std::make_shared<Label_ASM_Stmt>(label);

    spim.addSPIM(stmt);
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
    } else if (dest_type == OpdType::VARIABLE &&
               src_type == OpdType::REGISTER) {
        instruction_name = "store";
        if (isfloat)
            instruction_name += ".d";
    } else if (dest_type == OpdType::REGISTER &&
               src_type == OpdType::VARIABLE) {
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

void Transfer_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    if (dest_type == OpdType::REGISTER && src_type == OpdType::REGISTER) {
        std::shared_ptr<ASM_Register_Opd> res =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(result))
                    ->reg_desc);

        std::shared_ptr<ASM_Register_Opd> op1 =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

        std::shared_ptr<Move_ASM_Stmt> stmt =
            std::make_shared<Move_ASM_Stmt>(res, op1);

        spim.addSPIM(stmt);
    } else if (dest_type == OpdType::VARIABLE &&
               src_type == OpdType::REGISTER) {
        std::shared_ptr<RTL_Var_Opd> var =
            std::dynamic_pointer_cast<RTL_Var_Opd>(result);
        std::shared_ptr<ASM_Mem_Opd> res;
        if (var->entry->is_global)
            res = std::make_shared<ASM_Mem_Opd>(var->entry->get_name());
        else
            res = std::make_shared<ASM_Mem_Opd>(var->entry->stack_position,
                                                md->get_register(Register::fp));

        std::shared_ptr<ASM_Register_Opd> op1 =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

        std::shared_ptr<Store_Mem_ASM_Stmt> stmt =
            std::make_shared<Store_Mem_ASM_Stmt>(res, op1);

        spim.addSPIM(stmt);
    } else if (dest_type == OpdType::REGISTER &&
               src_type == OpdType::VARIABLE) {
        std::shared_ptr<ASM_Register_Opd> res =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(result))
                    ->reg_desc);

        std::shared_ptr<RTL_Var_Opd> var =
            std::dynamic_pointer_cast<RTL_Var_Opd>(oper1);
        std::shared_ptr<ASM_Mem_Opd> op1;
        if (var->entry->is_global)
            op1 = std::make_shared<ASM_Mem_Opd>(var->entry->get_name());
        else
            op1 = std::make_shared<ASM_Mem_Opd>(var->entry->stack_position,
                                                md->get_register(Register::fp));

        std::shared_ptr<Load_Mem_ASM_Stmt> stmt =
            std::make_shared<Load_Mem_ASM_Stmt>(res, op1);

        spim.addSPIM(stmt);
    } else if (dest_type == OpdType::REGISTER &&
               src_type == OpdType::INT_CONST) {
        std::shared_ptr<ASM_Register_Opd> res =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(result))
                    ->reg_desc);

        std::shared_ptr<ASM_Int_Const_Opd> op1 =
            std::make_shared<ASM_Int_Const_Opd>(
                (std::dynamic_pointer_cast<RTL_Int_Const_Opd>(oper1))->value);

        std::shared_ptr<Load_Int_Immediate_ASM_Stmt> stmt =
            std::make_shared<Load_Int_Immediate_ASM_Stmt>(res, op1);

        spim.addSPIM(stmt);
    } else if (dest_type == OpdType::REGISTER &&
               src_type == OpdType::DOUBLE_CONST) {
        std::shared_ptr<ASM_Register_Opd> res =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(result))
                    ->reg_desc);

        std::shared_ptr<ASM_Double_Const_Opd> op1 =
            std::make_shared<ASM_Double_Const_Opd>(
                (std::dynamic_pointer_cast<RTL_Double_Const_Opd>(oper1))
                    ->value);

        std::shared_ptr<Load_Double_Immediate_ASM_Stmt> stmt =
            std::make_shared<Load_Double_Immediate_ASM_Stmt>(res, op1);

        spim.addSPIM(stmt);
    } else if (dest_type == OpdType::REGISTER &&
               src_type == OpdType::STRING_CONST) {
        std::shared_ptr<ASM_Register_Opd> res =
            std::make_shared<ASM_Register_Opd>(
                (std::dynamic_pointer_cast<RTL_Register_Opd>(result))
                    ->reg_desc);

        std::shared_ptr<ASM_String_Const_Opd> op1 =
            std::make_shared<ASM_String_Const_Opd>(
                (std::dynamic_pointer_cast<RTL_String_Const_Opd>(oper1))
                    ->string_num,
                (std::dynamic_pointer_cast<RTL_String_Const_Opd>(oper1))
                    ->value);

        std::shared_ptr<Load_Address_ASM_Stmt> stmt =
            std::make_shared<Load_Address_ASM_Stmt>(res, op1);

        spim.addSPIM(stmt);
    } else
        exit_with_err_msg("kys");
}

Read_RTL_Stmt::Read_RTL_Stmt() {}

void Read_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "read" << std::endl;
}

void Read_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<Syscall_ASM_Stmt> stmt =
        std::make_shared<Syscall_ASM_Stmt>();

    spim.addSPIM(stmt);
}

Write_RTL_Stmt::Write_RTL_Stmt() {}

void Write_RTL_Stmt::print(std::ostream &out) {
    out << "\t" << "write" << std::endl;
}

void Write_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<Syscall_ASM_Stmt> stmt =
        std::make_shared<Syscall_ASM_Stmt>();

    spim.addSPIM(stmt);
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

void Mov_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> res = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(result))->reg_desc);

    std::shared_ptr<ASM_Register_Opd> op1 = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(oper1))->reg_desc);

    std::shared_ptr<Move_If_ASM_Stmt> stmt =
        std::make_shared<Move_If_ASM_Stmt>(res, op1, flag, movt);

    spim.addSPIM(stmt);
}

Load_Address_RTL_Stmt::Load_Address_RTL_Stmt(
    std::shared_ptr<RTL_Register_Opd> res, std::shared_ptr<RTL_Var_Opd> opd) {
    result = res;
    oper1 = opd;
}

void Load_Address_RTL_Stmt::print(std::ostream &out) {
    out << "\tload_addr:\t" << result->get_name() << " <- " << oper1->get_name()
        << std::endl;
}

void Load_Address_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> reg = std::make_shared<ASM_Register_Opd>(
        std::dynamic_pointer_cast<RTL_Register_Opd>(result)->reg_desc);
    std::shared_ptr<RTL_Var_Opd> var =
        std::dynamic_pointer_cast<RTL_Var_Opd>(oper1);
    std::shared_ptr<ASM_Mem_Opd> mem;
    if (var->entry->is_global) {
        mem = std::make_shared<ASM_Mem_Opd>(var->entry->get_name());
    } else {
        mem = std::make_shared<ASM_Mem_Opd>(var->entry->stack_position,
                                            md->get_register(Register::fp));
    }
    std::shared_ptr<Load_Address_ASM_Stmt> stmt =
        std::make_shared<Load_Address_ASM_Stmt>(reg, mem);
    spim.addSPIM(stmt);
}

Indirect_Op_RTL_Stmt::Indirect_Op_RTL_Stmt(
    std::shared_ptr<RTL_Register_Opd> res,
    std::shared_ptr<RTL_Register_Opd> opd, bool load)
    : isload(load) {
    result = res;
    oper1 = opd;
}

void Indirect_Op_RTL_Stmt::print(std::ostream &out) {
    std::string ins_name = isload ? "load_ind" : "store_ind";
    std::shared_ptr<RTL_Register_Opd> reg1 =
        std::dynamic_pointer_cast<RTL_Register_Opd>(result);
    std::shared_ptr<RTL_Register_Opd> reg2 =
        std::dynamic_pointer_cast<RTL_Register_Opd>(oper1);
    if (reg1->reg_desc->get_use_category() == RegisterUseCategory::float_reg ||
        reg2->reg_desc->get_use_category() == RegisterUseCategory::float_reg)
        ins_name += ".d";
    out << "\t" << ins_name << ":\t" << result->get_name() << " <- "
        << oper1->get_name() << std::endl;
}

void Indirect_Op_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    if (isload) {
        std::shared_ptr<ASM_Register_Opd> dst =
            std::make_shared<ASM_Register_Opd>(
                std::dynamic_pointer_cast<RTL_Register_Opd>(result)->reg_desc);
        std::shared_ptr<ASM_Mem_Opd> src = std::make_shared<ASM_Mem_Opd>(
            0, std::dynamic_pointer_cast<RTL_Register_Opd>(oper1)->reg_desc);
        std::shared_ptr<Load_Mem_ASM_Stmt> stmt =
            std::make_shared<Load_Mem_ASM_Stmt>(dst, src);
        spim.addSPIM(stmt);
    } else {
        std::shared_ptr<ASM_Mem_Opd> dst = std::make_shared<ASM_Mem_Opd>(
            0, std::dynamic_pointer_cast<RTL_Register_Opd>(result)->reg_desc);
        std::shared_ptr<ASM_Register_Opd> src =
            std::make_shared<ASM_Register_Opd>(
                std::dynamic_pointer_cast<RTL_Register_Opd>(oper1)->reg_desc);
        std::shared_ptr<Store_Mem_ASM_Stmt> stmt =
            std::make_shared<Store_Mem_ASM_Stmt>(dst, src);
        spim.addSPIM(stmt);
    }
}

Push_RTL_Stmt::Push_RTL_Stmt(std::shared_ptr<RTL_Register_Opd> res,
                             unsigned int sz) {
    size = sz;
    result = res;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Push_RTL_Stmt::print(std::ostream &out) {
    out << "\tpush:\t" << result->get_name() << std::endl;
}

void Push_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> res = std::make_shared<ASM_Register_Opd>(
        (std::dynamic_pointer_cast<RTL_Register_Opd>(result))->reg_desc);

    std::shared_ptr<ASM_Mem_Opd> sp_mem =
        std::make_shared<ASM_Mem_Opd>(4 - size, md->get_register(Register::sp));

    std::shared_ptr<ASM_Register_Opd> sp =
        std::make_shared<ASM_Register_Opd>(md->get_register(Register::sp));

    std::shared_ptr<Store_Mem_ASM_Stmt> stmt =
        std::make_shared<Store_Mem_ASM_Stmt>(sp_mem, res);

    spim.addSPIM(stmt);

    std::shared_ptr<ASM_Int_Const_Opd> offset =
        std::make_shared<ASM_Int_Const_Opd>(size);

    std::shared_ptr<Arithmetic_ASM_Stmt> smt =
        std::make_shared<Arithmetic_ASM_Stmt>(sp, sp, offset,
                                              ArithmeticOperator::MINUS);

    spim.addSPIM(smt);
}

Pop_RTL_Stmt::Pop_RTL_Stmt(unsigned int sz) {
    size = sz;
    result = nullptr;
    oper1 = nullptr;
    oper2 = nullptr;
}

void Pop_RTL_Stmt::print(std::ostream &out) { out << "\tpop" << std::endl; }

void Pop_RTL_Stmt::generateSPIM(SPIM &spim, MachineDescriptor *md) {
    std::shared_ptr<ASM_Register_Opd> sp =
        std::make_shared<ASM_Register_Opd>(md->get_register(Register::sp));

    std::shared_ptr<ASM_Int_Const_Opd> offset =
        std::make_shared<ASM_Int_Const_Opd>(size);

    std::shared_ptr<Arithmetic_ASM_Stmt> stmt =
        std::make_shared<Arithmetic_ASM_Stmt>(sp, sp, offset,
                                              ArithmeticOperator::PLUS);

    spim.addSPIM(stmt);
}

// RTL class
std::map<std::string, unsigned int> RTL::string_to_int;

RTL::RTL() { machine_descriptor = new MachineDescriptor(); }

RTL::~RTL() { delete machine_descriptor; }

void RTL::set_string_to_int_map(std::map<std::string, unsigned int> mp) {
    string_to_int = mp;
}

void RTL::print(std::ostream &out) {
    for (auto &stmt : rtl_code)
        stmt->print(out);
}

unsigned int RTL::getStringConstNum(std::string s) { return string_to_int[s]; }

void RTL::addRTLStatement(std::shared_ptr<RTL_Stmt> stmt) {
    rtl_code.push_back(stmt);
}

bool RTL::isEmpty() { return rtl_code.empty(); }

void RTL::generateSPIM(SPIM &spim) {
    for (auto &stmt : rtl_code)
        stmt->generateSPIM(spim, machine_descriptor);
}
