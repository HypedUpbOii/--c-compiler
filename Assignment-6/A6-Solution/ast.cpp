#include "ast.hpp"
#include <algorithm>
#include <iomanip>

bool Expression_Ast::isLvalue() { return false; }

// Function call
Function_Call_Ast::Function_Call_Ast(
    std::string nam, std::vector<std::unique_ptr<Expression_Ast>> args)
    : funcEntry(nullptr), arguments(std::move(args)) {
    name = (nam == "main") ? "main" : nam + "_";
}

void Function_Call_Ast::validateNode(Context &ctx) {
    funcEntry = (ctx.local)->func_lookup(name);
    if (funcEntry == nullptr)
        exit_with_err_msg("sclp error: Could not look up function " + name);

    if (arguments.size() != funcEntry->get_params().size())
        exit_with_err_msg("sclp error: Number of arguments mismatch");

    for (int i = 0; i < arguments.size(); ++i) {
        arguments[i]->validateNode(ctx);
        if (arguments[i]->exprType != funcEntry->get_params()[i].second)
            exit_with_err_msg("sclp error: Argument type mismatch");
    }
    exprType = funcEntry->get_return_type();
}

void Function_Call_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl << std::string(tab, ' ') << "FN CALL: " << name << "(";
    for (auto &ptr : arguments) {
        out << std::endl;
        ptr->printTree(out, tab);
    }
    out << ")";
}

std::vector<TAC_Stmt *> Function_Call_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::vector<std::shared_ptr<TAC_Opd>> params;
    std::vector<TAC_Stmt *> result;
    if (funcEntry->get_return_type() != BaseType::VOID) {
        bool needfloat = funcEntry->get_return_type() == BaseType::FLOAT;
        place = tac.genNewTemporary(needfloat);
    }

    for (auto &ptr : arguments) {
        for (auto &stmt : ptr->generateTAC(tac, ctx))
            result.push_back(stmt);
        params.push_back(ptr->place);
    }
    func_call_place = std::make_shared<Function_TAC_Opd>(funcEntry, params);
    if (funcEntry->get_return_type() != BaseType::VOID) {
        Asgn_TAC_Stmt *stmt = new Asgn_TAC_Stmt(place, func_call_place);
        result.push_back(stmt);
    } else {
        place = func_call_place;
    }
    return result;
}

// Name/Variable class
Name_Expr_Ast::Name_Expr_Ast(std::string nam)
    : name(nam + "_"), steEntry(nullptr) {}

void Name_Expr_Ast::validateNode(Context &ctx) {
    steEntry = (ctx.local)->lookup(name);
    if (steEntry == nullptr)
        exit_with_err_msg("sclp error: Could not look up variable " + name);

    exprType = steEntry->get_type();
}

void Name_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << "Name : " << name << type_to_string(exprType);
}

std::vector<TAC_Stmt *> Name_Expr_Ast::generateTAC(TAC &tac, Context &ctx) {
    place = std::make_shared<Variable_TAC_Opd>(steEntry);
    return std::vector<TAC_Stmt *>();
}

bool Name_Expr_Ast::isLvalue() { return true; }

// Literal class
template class Literal_Expr_Ast<int>;
template class Literal_Expr_Ast<double>;
template class Literal_Expr_Ast<std::string>;

template <typename T>
Literal_Expr_Ast<T>::Literal_Expr_Ast(T val) : value(val) {}

template <> void Literal_Expr_Ast<int>::validateNode(Context &ctx) {
    exprType = BaseType::INT;
}

template <> void Literal_Expr_Ast<double>::validateNode(Context &ctx) {
    exprType = BaseType::FLOAT;
}

template <> void Literal_Expr_Ast<std::string>::validateNode(Context &ctx) {
    exprType = BaseType::STRING;
}

template <> void Literal_Expr_Ast<int>::printTree(std::ostream &out, int tab) {
    out << "Num : " << value << type_to_string(exprType);
}

template <>
void Literal_Expr_Ast<double>::printTree(std::ostream &out, int tab) {
    out << "Num : " << std::fixed << std::setprecision(2) << value
        << type_to_string(exprType);
}

template <>
void Literal_Expr_Ast<std::string>::printTree(std::ostream &out, int tab) {
    out << "String : " << value << type_to_string(exprType);
}

template <>
std::vector<TAC_Stmt *> Literal_Expr_Ast<int>::generateTAC(TAC &tac,
                                                           Context &ctx) {
    place = std::make_shared<Int_Const_TAC_Opd>(value);
    return std::vector<TAC_Stmt *>();
}

template <>
std::vector<TAC_Stmt *> Literal_Expr_Ast<double>::generateTAC(TAC &tac,
                                                              Context &ctx) {
    place = std::make_shared<Double_Const_TAC_Opd>(value);
    return std::vector<TAC_Stmt *>();
}

template <>
std::vector<TAC_Stmt *>
Literal_Expr_Ast<std::string>::generateTAC(TAC &tac, Context &ctx) {
    place = std::make_shared<String_Const_TAC_Opd>(value);
    return std::vector<TAC_Stmt *>();
}

// Binary Expression class
void Binary_Expr_Ast::printChildren(std::ostream &out, int tab) {
    out << std::string(tab + 2, ' ') << "L_Opd (";
    leftOp->printTree(out, tab + 4);
    out << ")" << std::endl;
    out << std::string(tab + 2, ' ') << "R_Opd (";
    rightOp->printTree(out, tab + 4);
    out << ")";
}

// Array Access class
Array_Access_Expr_Ast::Array_Access_Expr_Ast(
    std::string nam, std::vector<std::unique_ptr<Expression_Ast>> &dim_accessed)
    : name(nam + "_"), dims(std::move(dim_accessed)) {}

bool Array_Access_Expr_Ast::isLvalue() { return true; }

void Array_Access_Expr_Ast::validateNode(Context &ctx) {
    int dims_size = dims.size();
    SymbolTableEntry *steEntry = ctx.local->lookup(name);

    if (steEntry == nullptr)
        exit_with_err_msg("sclp error: variable not found");

    std::vector<int> arr_dims = steEntry->get_type().array_dimensions;
    int arr_dims_size = arr_dims.size();

    // dims must be = dimensions of the array
    if (dims_size != arr_dims_size)
        exit_with_err_msg("sclp error: accessing different dimensions than "
                          "present in array " +
                          name);

    for (auto &ptr : dims) {
        ptr->validateNode(ctx);
        if (ptr->exprType != BaseType::INT)
            exit_with_err_msg(
                "sclp error: array access dimension not an integer");
    }

    // ensure the types are int expressions
    for (int i = 0; i < dims_size; i++) {
        auto &ptr = dims[i];
        // if the access values are integer constants at compile time, ensure
        // they are valid (< size of that dim)
        Literal_Expr_Ast<int> *literal_ptr =
            dynamic_cast<Literal_Expr_Ast<int> *>(ptr.get());
        if (literal_ptr && literal_ptr->value >= arr_dims[i])
            exit_with_err_msg(
                "sclp error: array access out of bounds at compile time");
    }

    // set exprType
    exprType = DataType(steEntry->get_type().base);
}

void Array_Access_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Array_Access: " << type_to_string(exprType)
        << std::endl
        << std::string(tab + 2, ' ') << "Base (Name : " << name
        << type_to_string(exprType) << ")";

    int dim_num = 0;
    for (auto &expr : dims) {
        out << std::endl
            << std::string(tab + 2, ' ') << "Dim " << dim_num++ << " : (";
        expr->printTree(out, tab + 4);
        out << ")";
    }
}

std::vector<TAC_Stmt *> Array_Access_Expr_Ast::generateTAC(TAC &tac,
                                                           Context &ctx) {
    std::vector<TAC_Stmt *> ans;
    SymbolTableEntry *steEntry = ctx.local->lookup(name);
    std::vector<int> sizes = steEntry->get_type().array_dimensions;
    int num_dims = sizes.size();

    std::vector<TAC_Stmt *> first_code = dims[0]->generateTAC(tac, ctx);
    for (auto stmt : first_code)
        ans.push_back(stmt);
    place = dims[0]->place;

    for (int i = 0; i < num_dims - 1; i++) {
        std::shared_ptr<Int_Const_TAC_Opd> next_dim_tac_opd =
            std::make_shared<Int_Const_TAC_Opd>(sizes[i + 1]);
        std::shared_ptr<Temporary_TAC_Opd> mult_temp = tac.genNewTemporary();
        Arith_Comp_TAC_Stmt *mult_stmt = new Arith_Comp_TAC_Stmt(
            mult_temp, place, ArithmeticOperator::MULT, next_dim_tac_opd);
        ans.push_back(mult_stmt);

        std::vector<TAC_Stmt *> dim_code = dims[i + 1]->generateTAC(tac, ctx);
        for (auto stmt : dim_code)
            ans.push_back(stmt);
        std::shared_ptr<Temporary_TAC_Opd> add_temp = tac.genNewTemporary();
        Arith_Comp_TAC_Stmt *add_stmt = new Arith_Comp_TAC_Stmt(
            add_temp, mult_temp, ArithmeticOperator::PLUS, dims[i + 1]->place);
        ans.push_back(add_stmt);

        place = add_temp;
    }

    int sz = (steEntry->get_type().base == BaseType::FLOAT) ? 8 : 4;
    std::shared_ptr<Int_Const_TAC_Opd> size_opd =
        std::make_shared<Int_Const_TAC_Opd>(sz);
    std::shared_ptr<Temporary_TAC_Opd> offset = tac.genNewTemporary();
    Arith_Comp_TAC_Stmt *size_mult_stmt = new Arith_Comp_TAC_Stmt(
        offset, place, ArithmeticOperator::MULT, size_opd);
    ans.push_back(size_mult_stmt);

    std::shared_ptr<Address_Of_TAC_Opd> address_var_tac_opd =
        std::make_shared<Address_Of_TAC_Opd>(steEntry);
    std::shared_ptr<Temporary_TAC_Opd> arr_loc_temp = tac.genNewTemporary();
    Asgn_TAC_Stmt *address_asgn =
        new Asgn_TAC_Stmt(arr_loc_temp, address_var_tac_opd);
    ans.push_back(address_asgn);

    std::shared_ptr<Temporary_TAC_Opd> final_loc = tac.genNewTemporary();
    Arith_Comp_TAC_Stmt *add_offset_stmt = new Arith_Comp_TAC_Stmt(
        final_loc, arr_loc_temp, ArithmeticOperator::PLUS, offset);
    ans.push_back(add_offset_stmt);

    place = std::make_shared<Pointer_Deref_TAC_Opd>(final_loc);

    return ans;
}

// Boolean Expression class
Boolean_Expr_Ast::Boolean_Expr_Ast(std::unique_ptr<Expression_Ast> oper1,
                                   BooleanOperator oper,
                                   std::unique_ptr<Expression_Ast> oper2)
    : op(oper) {
    leftOp = std::move(oper1);
    rightOp = std::move(oper2);
}

void Boolean_Expr_Ast::validateNode(Context &ctx) {
    leftOp->validateNode(ctx);
    rightOp->validateNode(ctx);
    if (leftOp->exprType != BaseType::BOOL ||
        rightOp->exprType != BaseType::BOOL)
        exit_with_err_msg(
            "sclp error: Incompatible type with boolean operator");

    exprType = BaseType::BOOL;
}

void Boolean_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Condition: " << bool_op_to_string(op)
        << type_to_string(exprType) << std::endl;

    printChildren(out, tab);
}

std::vector<TAC_Stmt *> Boolean_Expr_Ast::generateTAC(TAC &tac, Context &ctx) {
    auto left_tac = leftOp->generateTAC(tac, ctx);
    auto right_tac = rightOp->generateTAC(tac, ctx);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Bool_Comp_TAC_Stmt(place, leftOp->place, op, rightOp->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : left_tac)
        result.push_back(s);
    for (auto s : right_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Arithmetic Expression class
Arithmetic_Expr_Ast::Arithmetic_Expr_Ast(std::unique_ptr<Expression_Ast> oper1,
                                         ArithmeticOperator oper,
                                         std::unique_ptr<Expression_Ast> oper2)
    : op(oper) {
    leftOp = std::move(oper1);
    rightOp = std::move(oper2);
}

void Arithmetic_Expr_Ast::validateNode(Context &ctx) {
    leftOp->validateNode(ctx);
    rightOp->validateNode(ctx);
    if (leftOp->exprType != rightOp->exprType)
        exit_with_err_msg("sclp error: Operand types must match");
    else if (leftOp->exprType != BaseType::INT &&
             leftOp->exprType != BaseType::FLOAT)
        exit_with_err_msg(
            "sclp error: Incompatible type with arithmetic operator");

    exprType = leftOp->exprType;
}

void Arithmetic_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Arith: " << arith_op_to_string(op)
        << type_to_string(exprType) << std::endl;

    printChildren(out, tab);
}

std::vector<TAC_Stmt *> Arithmetic_Expr_Ast::generateTAC(TAC &tac,
                                                         Context &ctx) {
    auto left_tac = leftOp->generateTAC(tac, ctx);
    auto right_tac = rightOp->generateTAC(tac, ctx);
    // may need float
    place = tac.genNewTemporary(leftOp->exprType.base == BaseType::FLOAT);
    Compute_TAC_Stmt *stmt =
        new Arith_Comp_TAC_Stmt(place, leftOp->place, op, rightOp->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : left_tac)
        result.push_back(s);
    for (auto s : right_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Relational Expression class
Relational_Expr_Ast::Relational_Expr_Ast(std::unique_ptr<Expression_Ast> oper1,
                                         RelationalOperator oper,
                                         std::unique_ptr<Expression_Ast> oper2)
    : op(oper) {
    leftOp = std::move(oper1);
    rightOp = std::move(oper2);
}

void Relational_Expr_Ast::validateNode(Context &ctx) {
    leftOp->validateNode(ctx);
    rightOp->validateNode(ctx);
    if (leftOp->exprType != rightOp->exprType)
        exit_with_err_msg("sclp error: Operand types must match");
    else if (leftOp->exprType != BaseType::INT &&
             leftOp->exprType != BaseType::FLOAT)
        exit_with_err_msg(
            "sclp error: Incompatible type with relational operator");

    exprType = BaseType::BOOL;
}

void Relational_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Condition: " << rel_op_to_string(op)
        << type_to_string(exprType) << std::endl;

    printChildren(out, tab);
}

std::vector<TAC_Stmt *> Relational_Expr_Ast::generateTAC(TAC &tac,
                                                         Context &ctx) {
    auto left_tac = leftOp->generateTAC(tac, ctx);
    auto right_tac = rightOp->generateTAC(tac, ctx);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Rel_Comp_TAC_Stmt(place, leftOp->place, op, rightOp->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : left_tac)
        result.push_back(s);
    for (auto s : right_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Ternary Expression class
Ternary_Expr_Ast::Ternary_Expr_Ast(std::unique_ptr<Expression_Ast> cond,
                                   std::unique_ptr<Expression_Ast> tExpr,
                                   std::unique_ptr<Expression_Ast> fExpr)
    : condition(std::move(cond)), trueExpr(std::move(tExpr)),
      falseExpr(std::move(fExpr)) {}

void Ternary_Expr_Ast::validateNode(Context &ctx) {
    condition->validateNode(ctx);
    trueExpr->validateNode(ctx);
    falseExpr->validateNode(ctx);

    if (condition->exprType != BaseType::BOOL)
        exit_with_err_msg("sclp error: Condition in the ternary expression "
                          "must be of type bool");
    else if (trueExpr->exprType != falseExpr->exprType)
        exit_with_err_msg(
            "sclp error: Both branches in the ternary have different types " +
            type_to_string(trueExpr->exprType) + " and " +
            type_to_string(falseExpr->exprType));

    exprType = trueExpr->exprType;
}

void Ternary_Expr_Ast::printTree(std::ostream &out, int tab) {
    condition->printTree(out, tab + 4);
    out << std::endl << std::string(tab + 4, ' ') << "True_Part (";
    trueExpr->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab + 4, ' ') << "False_Part (";
    falseExpr->printTree(out, tab + 6);
    out << ")";
}

std::vector<TAC_Stmt *> Ternary_Expr_Ast::generateTAC(TAC &tac, Context &ctx) {
    auto cond_tac = condition->generateTAC(tac, ctx);
    place = tac.genNewSTemporary(exprType, ctx.local);
    std::shared_ptr<Label_TAC_Opd> false_label = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> exit_label = tac.genNewLabel();
    auto true_tac = trueExpr->generateTAC(tac, ctx);
    auto false_tac = falseExpr->generateTAC(tac, ctx);

    std::shared_ptr<Temporary_TAC_Opd> opp_cond = tac.genNewTemporary();
    Compute_TAC_Stmt *negate_stmt =
        new Unary_Comp_TAC_Stmt(opp_cond, UnaryOperator::NOT, condition->place);
    If_Goto_TAC_Stmt *go_to_false = new If_Goto_TAC_Stmt(opp_cond, false_label);
    Goto_TAC_Stmt *go_to_exit = new Goto_TAC_Stmt(exit_label);
    Asgn_TAC_Stmt *set_true_val = new Asgn_TAC_Stmt(place, trueExpr->place);
    Asgn_TAC_Stmt *set_false_val = new Asgn_TAC_Stmt(place, falseExpr->place);
    Label_TAC_Stmt *false_branch = new Label_TAC_Stmt(false_label);
    Label_TAC_Stmt *exit_branch = new Label_TAC_Stmt(exit_label);

    std::vector<TAC_Stmt *> result;
    for (auto stmt : cond_tac) {
        result.push_back(stmt);
    }
    result.push_back(negate_stmt);
    result.push_back(go_to_false);
    for (auto stmt : true_tac) {
        result.push_back(stmt);
    }
    result.push_back(set_true_val);
    result.push_back(go_to_exit);
    result.push_back(false_branch);
    for (auto stmt : false_tac) {
        result.push_back(stmt);
    }
    result.push_back(set_false_val);
    result.push_back(exit_branch);
    return result;
}

// Unary Expressions
// Address Expression
Address_Expr_Ast::Address_Expr_Ast(std::unique_ptr<Name_Expr_Ast> oper)
    : operand(std::move(oper)) {}

void Address_Expr_Ast::validateNode(Context &ctx) {
    operand->validateNode(ctx);
    exprType =
        DataType(operand->exprType.base, operand->exprType.pointer_level + 1);
}

void Address_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Address_Of: " << type_to_string(exprType)
        << std::endl
        << std::string(tab + 2, ' ') << "Base (";
    operand->printTree(out, tab);

    out << ")";
}

std::vector<TAC_Stmt *> Address_Expr_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::vector<TAC_Stmt *> ans;
    std::shared_ptr<Address_Of_TAC_Opd> add_tac_opd =
        std::make_shared<Address_Of_TAC_Opd>(operand->steEntry);
    std::shared_ptr<Temporary_TAC_Opd> new_temp = tac.genNewTemporary();
    place = new_temp;

    Asgn_TAC_Stmt *asgn_stmt = new Asgn_TAC_Stmt(new_temp, add_tac_opd);
    ans.push_back(asgn_stmt);
    return ans;
}

// Pointer Deref Expression
Pointer_Deref_Expr_Ast::Pointer_Deref_Expr_Ast(std::string nam, int level)
    : name(nam + "_"), pointerLevel(level) {}

bool Pointer_Deref_Expr_Ast::isLvalue() { return true; }

void Pointer_Deref_Expr_Ast::validateNode(Context &ctx) {
    SymbolTableEntry *steEntry = ctx.local->lookup(name);
    if (steEntry == nullptr)
        exit_with_err_msg("sclp error: variable not found");
    DataType var_type = steEntry->get_type();

    int var_ptr_level = var_type.pointer_level;
    if (pointerLevel > var_ptr_level)
        exit_with_err_msg("sclp error: derefencing more than allowed");

    exprType = DataType(var_type.base, var_ptr_level - pointerLevel);
}

void Pointer_Deref_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ')
        << "Pointer_Deref: " << type_to_string(exprType) << std::endl
        << std::string(tab + 2, ' ') << "Operand (Name : " << name
        << type_to_string(exprType) << ")" << std::endl
        << std::string(tab + 2, ' ') << "Degree : " << pointerLevel;
}

std::vector<TAC_Stmt *> Pointer_Deref_Expr_Ast::generateTAC(TAC &tac,
                                                            Context &ctx) {
    std::vector<TAC_Stmt *> ans;
    SymbolTableEntry *steEntry = ctx.local->lookup(name);
    std::shared_ptr<Variable_TAC_Opd> var_tac_opd =
        std::make_shared<Variable_TAC_Opd>(steEntry);

    place = var_tac_opd;
    for (int i = 0; i < pointerLevel - 1; i++) {
        std::shared_ptr<Pointer_Deref_TAC_Opd> deref =
            std::make_shared<Pointer_Deref_TAC_Opd>(place);
        std::shared_ptr<Temporary_TAC_Opd> temp = tac.genNewTemporary();
        Asgn_TAC_Stmt *asgn = new Asgn_TAC_Stmt(temp, deref);
        ans.push_back(asgn);
        place = temp;
    }
    place = std::make_shared<Pointer_Deref_TAC_Opd>(place);
    return ans;
}

// Unary Minus Expression
UMinus_Expr_Ast::UMinus_Expr_Ast(std::unique_ptr<Expression_Ast> oper)
    : operand(std::move(oper)) {}

void UMinus_Expr_Ast::validateNode(Context &ctx) {
    operand->validateNode(ctx);

    if (operand->exprType != BaseType::INT &&
        operand->exprType != BaseType::FLOAT)
        exit_with_err_msg(
            "sclp error: Incompatible unary operand with UMINUS operator");
    exprType = operand->exprType;
}

void UMinus_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Arith: Uminus" << type_to_string(exprType)
        << std::endl
        << std::string(tab, ' ') << "  L_Opd (";
    operand->printTree(out, tab + 4);
    out << ")";
}

std::vector<TAC_Stmt *> UMinus_Expr_Ast::generateTAC(TAC &tac, Context &ctx) {
    auto oper_tac = operand->generateTAC(tac, ctx);
    // may need float
    place = tac.genNewTemporary(operand->exprType.base == BaseType::FLOAT);
    Compute_TAC_Stmt *stmt =
        new Unary_Comp_TAC_Stmt(place, UnaryOperator::UMINUS, operand->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : oper_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Not Expression class
Not_Expr_Ast::Not_Expr_Ast(std::unique_ptr<Expression_Ast> expr)
    : operand(std::move(expr)) {}

void Not_Expr_Ast::validateNode(Context &ctx) {
    operand->validateNode(ctx);

    if (operand->exprType != BaseType::BOOL)
        exit_with_err_msg(
            "sclp error: Incompatible unary operand with NOT operator");
    exprType = operand->exprType;
}

void Not_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Condition: NOT" << type_to_string(exprType)
        << std::endl
        << std::string(tab, ' ') << "  L_Opd (";
    operand->printTree(out, tab + 4);
    out << ")";
}

std::vector<TAC_Stmt *> Not_Expr_Ast::generateTAC(TAC &tac, Context &ctx) {
    auto oper_tac = operand->generateTAC(tac, ctx);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Unary_Comp_TAC_Stmt(place, UnaryOperator::NOT, operand->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : oper_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Statements
bool Statement_Ast::hasReturn() { return false; }

// Assignment Statment class
Assignment_Stmt_Ast::Assignment_Stmt_Ast(std::unique_ptr<Expression_Ast> dst,
                                         std::unique_ptr<Expression_Ast> src)
    : target(std::move(dst)), value(std::move(src)) {}

void Assignment_Stmt_Ast::validateNode(Context &ctx) {
    target->validateNode(ctx);
    value->validateNode(ctx);

    if (!target->isLvalue() || target->exprType != value->exprType)
        exit_with_err_msg("sclp error: LHS and RHS have different types");
}

void Assignment_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Asgn:" << std::endl
        << std::string(tab, ' ') << "  LHS (";
    target->printTree(out, tab + 4);
    out << ")" << std::endl << std::string(tab, ' ') << "  RHS (";
    value->printTree(out, tab + 4);
    out << ")";
}

std::vector<TAC_Stmt *> Assignment_Stmt_Ast::generateTAC(TAC &tac,
                                                         Context &ctx) {
    auto value_tac = value->generateTAC(tac, ctx);
    std::vector<TAC_Stmt *> result;
    for (auto s : value_tac)
        result.push_back(s);
    auto target_tac = target->generateTAC(tac, ctx);

    Asgn_TAC_Stmt *stmt;
    if (target->place->get_opd_type() == OpdType::POINTER &&
        value->place->get_opd_type() == OpdType::POINTER) {
        auto temp = tac.genNewTemporary();
        Asgn_TAC_Stmt *shift = new Asgn_TAC_Stmt(temp, value->place);
        result.push_back(shift);
        stmt = new Asgn_TAC_Stmt(target->place, temp);
    } else
        stmt = new Asgn_TAC_Stmt(target->place, value->place);

    for (auto s : target_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Iteration Statement class
Iteration_Stmt_Ast::Iteration_Stmt_Ast(std::unique_ptr<Expression_Ast> cond,
                                       std::unique_ptr<Statement_Ast> bod)
    : condition(std::move(cond)), body(std::move(bod)) {}

void Iteration_Stmt_Ast::validateNode(Context &ctx) {
    condition->validateNode(ctx);
    ctx.loop_depth++;
    body->validateNode(ctx);
    ctx.loop_depth--;

    if (condition->exprType != BaseType::BOOL)
        exit_with_err_msg("sclp error: Loop condition must be a boolean value");
}

bool Iteration_Stmt_Ast::hasReturn() { return body->hasReturn(); }

void While_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "While:" << std::endl
        << std::string(tab, ' ') << "  Condition (";
    condition->printTree(out, tab + 6);
    auto new_body = dynamic_cast<Sequence_Stmt_Ast *>(body.get());
    if (new_body != nullptr && new_body->statement_list.empty()) {
        out << ")" << std::endl << std::string(tab, ' ') << "  Body ()";
    } else {
        out << ")" << std::endl
            << std::string(tab, ' ') << "  Body (" << std::endl;
        body->printTree(out, tab + 6);
        out << ")";
    }
}

std::vector<TAC_Stmt *> While_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::shared_ptr<Label_TAC_Opd> check_cond = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> exit_label = tac.genNewLabel();

    ctx.loop_start.push(check_cond);
    ctx.loop_end.push(exit_label);
    ctx.loop_depth++;
    auto cond_tac = condition->generateTAC(tac, ctx);
    auto body_tac = body->generateTAC(tac, ctx);
    ctx.loop_start.pop();
    ctx.loop_end.pop();
    ctx.loop_depth--;

    std::shared_ptr<Temporary_TAC_Opd> opp_cond = tac.genNewTemporary();
    Compute_TAC_Stmt *negate_stmt =
        new Unary_Comp_TAC_Stmt(opp_cond, UnaryOperator::NOT, condition->place);
    If_Goto_TAC_Stmt *go_to_exit = new If_Goto_TAC_Stmt(opp_cond, exit_label);
    Goto_TAC_Stmt *go_to_cond = new Goto_TAC_Stmt(check_cond);
    Label_TAC_Stmt *check_cond_stmt = new Label_TAC_Stmt(check_cond);
    Label_TAC_Stmt *exit_label_stmt = new Label_TAC_Stmt(exit_label);

    std::vector<TAC_Stmt *> result;
    result.push_back(check_cond_stmt);
    for (auto s : cond_tac)
        result.push_back(s);
    result.push_back(negate_stmt);
    result.push_back(go_to_exit);
    for (auto s : body_tac)
        result.push_back(s);
    result.push_back(go_to_cond);
    result.push_back(exit_label_stmt);
    return result;
}

void Do_While_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Do:" << std::endl;
    auto new_body = dynamic_cast<Sequence_Stmt_Ast *>(body.get());
    if (new_body != nullptr && new_body->statement_list.empty()) {
        out << std::string(tab, ' ') << "  Body ()" << std::endl;
    } else {
        out << std::string(tab, ' ') << "  Body (" << std::endl;
        body->printTree(out, tab + 6);
        out << ")" << std::endl;
    }
    out << std::string(tab, ' ') << "  While Condition (";
    condition->printTree(out, tab + 6);
    out << ")";
}

std::vector<TAC_Stmt *> Do_While_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::shared_ptr<Label_TAC_Opd> start_label = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> check_cond = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> exit_label = tac.genNewLabel();

    ctx.loop_start.push(check_cond);
    ctx.loop_end.push(exit_label);
    ctx.loop_depth++;
    auto body_tac = condition->generateTAC(tac, ctx);
    auto cond_tac = body->generateTAC(tac, ctx);
    ctx.loop_start.pop();
    ctx.loop_end.pop();
    ctx.loop_depth--;

    Label_TAC_Stmt *begin_stmt = new Label_TAC_Stmt(start_label);
    If_Goto_TAC_Stmt *jump_to_start =
        new If_Goto_TAC_Stmt(condition->place, start_label);
    Label_TAC_Stmt *cond_stmt = new Label_TAC_Stmt(check_cond);
    Label_TAC_Stmt *exit_stmt = new Label_TAC_Stmt(exit_label);

    std::vector<TAC_Stmt *> result;
    result.push_back(begin_stmt);
    for (auto s : body_tac)
        result.push_back(s);
    result.push_back(cond_stmt);
    for (auto s : cond_tac)
        result.push_back(s);
    result.push_back(jump_to_start);
    result.push_back(exit_stmt);
    return result;
}

// For statement class
For_Stmt_Ast::For_Stmt_Ast(std::unique_ptr<Assignment_Stmt_Ast> init,
                           std::unique_ptr<Expression_Ast> cond,
                           std::unique_ptr<Assignment_Stmt_Ast> update,
                           std::unique_ptr<Statement_Ast> bod)
    : initialize_stmt(std::move(init)), condition(std::move(cond)),
      update_stmt(std::move(update)), body(std::move(bod)) {}

void For_Stmt_Ast::validateNode(Context &ctx) {
    initialize_stmt->validateNode(ctx);
    condition->validateNode(ctx);
    update_stmt->validateNode(ctx);

    ctx.loop_depth++;
    body->validateNode(ctx);
    ctx.loop_depth--;

    if (condition->exprType != BaseType::BOOL)
        exit_with_err_msg("sclp error: Loop condition must be a boolean value");
}

void For_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "For:" << std::endl
        << std::string(tab, ' ') << "  Initialize (";
    initialize_stmt->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  Condition (";
    condition->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  Body (" << std::endl;
    body->printTree(out, tab + 6);
    out << ")" << std::endl
        << std::string(tab, ' ') << "  Update (" << std::endl;
    update_stmt->printTree(out, tab + 6);
    out << ")";
}

std::vector<TAC_Stmt *> For_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::shared_ptr<Label_TAC_Opd> check_cond = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> update_label = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> exit_label = tac.genNewLabel();

    ctx.loop_start.push(update_label);
    ctx.loop_end.push(exit_label);
    ctx.loop_depth++;
    auto init_tac = initialize_stmt->generateTAC(tac, ctx);
    auto update_tac = update_stmt->generateTAC(tac, ctx);
    auto cond_tac = condition->generateTAC(tac, ctx);
    auto body_tac = body->generateTAC(tac, ctx);
    ctx.loop_start.pop();
    ctx.loop_end.pop();
    ctx.loop_depth--;

    std::shared_ptr<Temporary_TAC_Opd> opp_cond = tac.genNewTemporary();
    Compute_TAC_Stmt *negate_stmt =
        new Unary_Comp_TAC_Stmt(opp_cond, UnaryOperator::NOT, condition->place);
    If_Goto_TAC_Stmt *go_to_exit = new If_Goto_TAC_Stmt(opp_cond, exit_label);
    Goto_TAC_Stmt *go_to_cond = new Goto_TAC_Stmt(check_cond);
    Label_TAC_Stmt *check_cond_stmt = new Label_TAC_Stmt(check_cond);
    Label_TAC_Stmt *update_stmt = new Label_TAC_Stmt(update_label);
    Label_TAC_Stmt *exit_label_stmt = new Label_TAC_Stmt(exit_label);

    std::vector<TAC_Stmt *> result;
    for (auto s : init_tac)
        result.push_back(s);
    result.push_back(check_cond_stmt);
    for (auto s : cond_tac)
        result.push_back(s);
    result.push_back(negate_stmt);
    result.push_back(go_to_exit);
    for (auto s : body_tac)
        result.push_back(s);
    result.push_back(update_stmt);
    for (auto s : update_tac)
        result.push_back(s);
    result.push_back(go_to_cond);
    result.push_back(exit_label_stmt);
    return result;
}

bool For_Stmt_Ast::hasReturn() { return body->hasReturn(); }

// Read Statement class
Read_Stmt_Ast::Read_Stmt_Ast(std::unique_ptr<Name_Expr_Ast> t)
    : target(std::move(t)) {}

void Read_Stmt_Ast::validateNode(Context &ctx) {
    target->validateNode(ctx);

    if (target->exprType != BaseType::INT &&
        target->exprType != BaseType::FLOAT)
        exit_with_err_msg("sclp error: Can read only int / float values");
}

void Read_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Read: ";
    target->printTree(out, tab + 2);
    out;
}

std::vector<TAC_Stmt *> Read_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    auto target_tac = target->generateTAC(tac, ctx);
    IO_TAC_Stmt *stmt = new IO_TAC_Stmt(false, target->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : target_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Return Statement class
Return_Stmt_Ast::Return_Stmt_Ast(std::unique_ptr<Expression_Ast> ret,
                                 std::string func)
    : return_value(std::move(ret)) {
    funcName = (func == "main") ? "main" : func + "_";
}

void Return_Stmt_Ast::validateNode(Context &ctx) {
    return_value->validateNode(ctx);
    if (return_value->exprType !=
        ((ctx.local)->func_lookup(funcName))->get_return_type())
        exit_with_err_msg("sclp error: Return type mistmatch");
}

void Return_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Return: ";
    return_value->printTree(out, tab + 2);
}

std::vector<TAC_Stmt *> Return_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    auto ret_val_tac = return_value->generateTAC(tac, ctx);
    Asgn_TAC_Stmt *set_stemp =
        new Asgn_TAC_Stmt(ctx.return_stemp, return_value->place);
    Goto_TAC_Stmt *go_func_end = new Goto_TAC_Stmt(ctx.return_label);

    std::vector<TAC_Stmt *> result;
    for (auto s : ret_val_tac)
        result.push_back(s);
    result.push_back(set_stemp);
    result.push_back(go_func_end);
    return result;
}

bool Return_Stmt_Ast::hasReturn() { return true; }

// Break Statement class
void Break_Stmt_Ast::validateNode(Context &ctx) {
    if (ctx.loop_depth == 0)
        exit_with_err_msg("sclp error: Break cannot be used outside a loop");
}

void Break_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Break";
}

std::vector<TAC_Stmt *> Break_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::vector<TAC_Stmt *> result;
    Goto_TAC_Stmt *stmt = new Goto_TAC_Stmt(ctx.loop_end.top());
    result.push_back(stmt);
    return result;
}

// Continue Statement class
void Continue_Stmt_Ast::validateNode(Context &ctx) {
    if (ctx.loop_depth == 0)
        exit_with_err_msg("sclp error: Break cannot be used outside a loop");
}

void Continue_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Continue";
}

std::vector<TAC_Stmt *> Continue_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::vector<TAC_Stmt *> result;
    Goto_TAC_Stmt *stmt = new Goto_TAC_Stmt(ctx.loop_start.top());
    result.push_back(stmt);
    return result;
}

// Selection Statement class
Selection_Stmt_Ast::Selection_Stmt_Ast(std::unique_ptr<Expression_Ast> cond,
                                       std::unique_ptr<Statement_Ast> then,
                                       std::unique_ptr<Statement_Ast> els)
    : condition(std::move(cond)), then_stmt(std::move(then)),
      else_stmt(std::move(els)) {}

void Selection_Stmt_Ast::validateNode(Context &ctx) {
    condition->validateNode(ctx);
    then_stmt->validateNode(ctx);
    if (else_stmt != nullptr)
        else_stmt->validateNode(ctx);

    if (condition->exprType != BaseType::BOOL)
        exit_with_err_msg("sclp error: If condition must be a bool");
}

void Selection_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "If:" << std::endl
        << std::string(tab, ' ') << "  Condition (";
    condition->printTree(out, tab + 6);
    auto new_then_stmt = dynamic_cast<Sequence_Stmt_Ast *>(then_stmt.get());
    if (new_then_stmt != nullptr && new_then_stmt->statement_list.empty()) {
        out << ")" << std::endl << std::string(tab, ' ') << "  Then ()";
    } else {
        out << ")" << std::endl
            << std::string(tab, ' ') << "  Then (" << std::endl;
        then_stmt->printTree(out, tab + 6);
        out << ")";
    }
    if (else_stmt != nullptr) {
        auto new_else_stmt = dynamic_cast<Sequence_Stmt_Ast *>(else_stmt.get());
        if (new_else_stmt != nullptr && new_else_stmt->statement_list.empty()) {
            out << std::endl << std::string(tab, ' ') << "  Else ()";
        } else {
            out << std::endl
                << std::string(tab, ' ') << "  Else (" << std::endl;
            else_stmt->printTree(out, tab + 6);
            out << ")";
        }
    }
}

std::vector<TAC_Stmt *> Selection_Stmt_Ast::generateTAC(TAC &tac,
                                                        Context &ctx) {
    auto cond_tac = condition->generateTAC(tac, ctx);
    auto then_tac = then_stmt->generateTAC(tac, ctx);

    std::shared_ptr<Temporary_TAC_Opd> opp_cond = tac.genNewTemporary();
    Compute_TAC_Stmt *negate_cond =
        new Unary_Comp_TAC_Stmt(opp_cond, UnaryOperator::NOT, condition->place);
    std::shared_ptr<Label_TAC_Opd> exit_label = tac.genNewLabel();
    Label_TAC_Stmt *exit_label_stmt = new Label_TAC_Stmt(exit_label);
    Goto_TAC_Stmt *jump_to_exit = new Goto_TAC_Stmt(exit_label);
    std::shared_ptr<Label_TAC_Opd> else_label =
        (else_stmt == nullptr) ? exit_label : tac.genNewLabel();
    If_Goto_TAC_Stmt *jump_to_false =
        new If_Goto_TAC_Stmt(opp_cond, else_label);

    std::vector<TAC_Stmt *> result;
    for (auto s : cond_tac)
        result.push_back(s);
    result.push_back(negate_cond);
    result.push_back(jump_to_false);
    for (auto s : then_tac)
        result.push_back(s);
    result.push_back(jump_to_exit);

    if (else_stmt != nullptr) {
        Label_TAC_Stmt *else_label_stmt = new Label_TAC_Stmt(else_label);
        result.push_back(else_label_stmt);
        auto else_tac = else_stmt->generateTAC(tac, ctx);
        for (auto s : else_tac)
            result.push_back(s);
    }
    result.push_back(exit_label_stmt);
    return result;
}

bool Selection_Stmt_Ast::hasReturn() {
    return then_stmt->hasReturn() ||
           ((else_stmt != nullptr) && else_stmt->hasReturn());
}

// Sequence Statement class
Sequence_Stmt_Ast::Sequence_Stmt_Ast(
    std::vector<std::unique_ptr<Statement_Ast>> &statements)
    : statement_list(std::move(statements)) {}

void Sequence_Stmt_Ast::validateNode(Context &ctx) {
    for (auto &ptr : statement_list) {
        ptr->validateNode(ctx);
    }
}

void Sequence_Stmt_Ast::printTree(std::ostream &out, int tab) {
    if (statement_list.empty())
        return;

    for (auto it = statement_list.begin();
         it != std::prev(statement_list.end()); ++it) {
        (*it)->printTree(out, tab);
        out << std::endl;
    }
    statement_list.back()->printTree(out, tab);
}

std::vector<TAC_Stmt *> Sequence_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    std::vector<TAC_Stmt *> result;
    for (auto &stmt : statement_list) {
        auto stmt_tac = stmt->generateTAC(tac, ctx);
        for (auto s : stmt_tac)
            result.push_back(s);
    }

    return result;
}

bool Sequence_Stmt_Ast::hasReturn() {
    for (auto &stmt : statement_list)
        if (stmt->hasReturn())
            return true;

    return false;
}

// Write Statement class
Write_Stmt_Ast::Write_Stmt_Ast(std::unique_ptr<Expression_Ast> t)
    : target(std::move(t)) {}

void Write_Stmt_Ast::validateNode(Context &ctx) {
    target->validateNode(ctx);

    if (target->exprType != BaseType::INT &&
        target->exprType != BaseType::FLOAT &&
        target->exprType != BaseType::STRING) {
        exit_with_err_msg("sclp error: Can't print bools or other weird types");
    }
}

void Write_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Write: ";
    target->printTree(out, tab + 2);
}

std::vector<TAC_Stmt *> Write_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    auto target_tac = target->generateTAC(tac, ctx);
    IO_TAC_Stmt *stmt = new IO_TAC_Stmt(true, target->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : target_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Call Stmt class
Call_Stmt_Ast::Call_Stmt_Ast(std::unique_ptr<Function_Call_Ast> call)
    : func_call(std::move(call)) {}

void Call_Stmt_Ast::validateNode(Context &ctx) {
    func_call->validateNode(ctx);
    if (func_call->exprType != BaseType::VOID)
        exit_with_err_msg("sclp error: Return type "
                          "of function ignored");
}

void Call_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "FN CALL: " << func_call->name << "(";
    for (auto &ptr : func_call->arguments) {
        out << std::endl;
        ptr->printTree(out, tab);
    }
    out << ")";
}

std::vector<TAC_Stmt *> Call_Stmt_Ast::generateTAC(TAC &tac, Context &ctx) {
    func_call->generateTAC(tac, ctx);
    Call_TAC_Stmt *call_stmt = new Call_TAC_Stmt(func_call->place);
    std::vector<TAC_Stmt *> result = {call_stmt};
    return result;
}

// Function Ast class
Function_Ast::Function_Ast(DataType ret,
                           std::vector<std::pair<std::string, DataType>> params,
                           std::string nam,
                           std::vector<std::unique_ptr<Statement_Ast>> stmts,
                           SymbolTable *loc, bool add_underscore)
    : returnType(ret), parameters(params), name(nam),
      statements(std::move(stmts)), local(loc) {
    if (add_underscore)
        name = (nam == "main") ? "main" : nam + "_";
}

void Function_Ast::validateFunction() {
    if (local->hasDuplicate())
        exit_with_err_msg("sclp error: Redeclaration of variable "
                          "outside function");
    Context ctx;
    ctx.local = this->local;
    ctx.loop_depth = 0;
    for (auto &stmt_node : statements)
        stmt_node->validateNode(ctx);

    if (returnType != BaseType::VOID) {
        for (auto &stmt_node : statements)
            if (stmt_node->hasReturn())
                return;

        exit_with_err_msg("sclp error: No return statement in a "
                          "non void function");
    }
}

void Function_Ast::printTree(std::ostream &out) {
    int tab = 0;
    out << std::string(tab, ' ') << "**PROCEDURE: " << name << std::endl;
    out << std::string(tab, ' ')
        << "\tReturn Type: " << type_to_string(returnType) << std::endl;
    out << std::string(tab, ' ') << "\tFormal Parameters:" << std::endl;
    for (auto &[nam, typ] : parameters) {
        out << std::string(tab, ' ') << "\t\t" << nam
            << "_  Type:" << type_to_string(typ) << std::endl;
    }
    out << std::string(tab, ' ') << "**BEGIN: Abstract Syntax Tree"
        << std::endl;
    for (auto &stmt : statements) {
        stmt->printTree(out, tab + 9);
        out << std::endl;
    }
    out << std::string(tab, ' ') << "**END: Abstract Syntax Tree" << std::endl;
}

void Function_Ast::generateTAC(std::shared_ptr<Label_TAC_Opd> return_label) {
    std::shared_ptr<Variable_TAC_Opd> return_stemp =
        (returnType == BaseType::VOID)
            ? nullptr
            : tac.genNewSTemporary(returnType, local, true);
    Context ctx;
    ctx.local = this->local;
    ctx.return_label = return_label;
    ctx.return_stemp = return_stemp;
    ctx.loop_depth = 0;
    for (auto &stmt : statements)
        tac.addTACStatements(stmt->generateTAC(tac, ctx));

    if (returnType != BaseType::VOID) {
        std::vector<TAC_Stmt *> ret_statements;
        Label_TAC_Stmt *ret_label = new Label_TAC_Stmt(return_label);
        Return_TAC_Stmt *ret_stmt = new Return_TAC_Stmt(return_stemp, name);
        ret_statements.push_back(ret_label);
        ret_statements.push_back(ret_stmt);
        tac.addTACStatements(ret_statements);
    }
}

void Function_Ast::printTAC(std::ostream &out) {
    if (tac.isEmpty())
        return;

    out << "**PROCEDURE: " << name << std::endl;
    out << "**BEGIN: Three Address Code Statements" << std::endl;
    tac.print(out);
    out << "**END: Three Address Code Statements" << std::endl;
}

void Function_Ast::generateRTL() { tac.generateRTL(rtl); }

void Function_Ast::printRTL(std::ostream &out) {
    if (rtl.isEmpty())
        return;

    out << "**PROCEDURE: " << name << std::endl;
    out << "**BEGIN: RTL Statements" << std::endl;
    rtl.print(out);
    out << "**END: RTL Statements" << std::endl;
}

void Function_Ast::generateSPIM() { rtl.generateSPIM(spim); }

void Function_Ast::printSPIM(std::ostream &out) {
    out << "\t.text" << std::endl;
    out << "\t.globl " << name << std::endl;
    out << name << ":" << std::endl;
    int stack_space = (local == nullptr) ? 0 : local->getStackSpace();
    // Prologue
    out << "\tsw $ra, 0($sp)" << std::endl;
    out << "\tsw $fp, -4($sp)" << std::endl;
    out << "\tsub $fp, $sp, 4" << std::endl;
    out << "\tsub $sp, $sp, " << 8 - stack_space << std::endl;

    spim.print(out); // treat return as jump to epilogue

    // Epilogue
    out << "epilogue_" << name << ":" << std::endl;
    out << "\tadd $sp, $sp, " << 8 - stack_space << std::endl;
    out << "\tlw $fp, -4($sp)" << std::endl;
    out << "\tlw $ra, 0($sp)" << std::endl;
    out << "\tjr $ra" << std::endl;
}

Function_Ast::~Function_Ast() { delete local; }

// Program Class
Program::Program() : global(new SymbolTable()) {}

void Program::addFuncDef(std::string name, DataType dt) {
    if (std::find(func_order.begin(), func_order.end(), name) ==
        func_order.end()) {
        func_order.push_back(name);
        rets[name] = (dt != BaseType::VOID) ? TAC::getRetLabel() : nullptr;
    }
}

void Program::addFunctions(
    std::vector<std::unique_ptr<Function_Ast>> func_list) {
    for (auto &ptr : func_list)
        funcs.emplace(ptr->name, std::move(ptr));
}

void Program::addStringConst(std::string str) {
    unsigned int sz = global_strings.size();
    if (global_strings.find(str) == global_strings.end()) {
        global_strings[str] = sz;
        string_consts.push_back(str);
    }
}

void Program::addGlobal(std::pair<DataType, std::vector<std::string>> line) {
    for (auto &str : line.second)
        global_vars.emplace_back(line.first, str);
}

void Program::validateProgram() {
    if (global->hasDuplicate())
        exit_with_err_msg("sclp error: Redeclaration of variable "
                          "/ function");
    // Check if main type exists
    if (funcs.find("main") == funcs.end())
        exit_with_err_msg("sclp error: No main function defined");
    for (const auto &[name, func] : funcs)
        func->validateFunction();

    auto defs = global->getOnlyFuncDecls();
    for (auto &def : defs) {
        auto ptr = std::make_unique<Function_Ast>(
            def->get_return_type(), def->get_params(), def->get_name(),
            std::vector<std::unique_ptr<Statement_Ast>>(), nullptr, false);
        funcs.emplace(def->get_name(), std::move(ptr));
    }
}

void Program::print(std::ostream &out) {
    for (auto const &[name, func] : funcs)
        func->printTree(out);
}

void Program::generateTAC() {
    for (auto const &[name, func] : funcs)
        func->generateTAC(rets[name]);
}

void Program::printTAC(std::ostream &out) {
    for (auto const &[name, func] : funcs)
        func->printTAC(out);
}

void Program::generateRTL() {
    RTL::set_string_to_int_map(global_strings);
    for (auto const &[name, func] : funcs)
        func->generateRTL();
}

void Program::printRTL(std::ostream &out) {
    for (auto const &[name, func] : funcs)
        func->printRTL(out);
}

void Program::generateSPIM() {
    for (auto const &[name, func] : funcs)
        func->generateSPIM();
}

void Program::printSPIM(std::ostream &out) {
    // print globals first
    out << std::endl;
    if (!global_vars.empty() || !string_consts.empty())
        out << "\t.data" << std::endl;
    for (auto const &[dt, name] : global_vars) {
        if (dt == BaseType::FLOAT)
            out << name << "_:\t.double 0.0" << std::endl;
        else
            out << name << "_:\t.word 0" << std::endl;
    }
    int i = 0;
    for (auto const &str : string_consts)
        out << "_str_" << i++ << ":\t.asciiz " << str << std::endl;

    for (auto const &[name, func] : funcs)
        func->printSPIM(out);
}

Program::~Program() { delete global; }
