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

void Function_Call_Ast::validateNode(SymbolTable *sym_tab) {
    funcEntry = sym_tab->func_lookup(name);
    if (funcEntry == nullptr)
        exit_with_err_msg("sclp error: Could not look up function " + name);

    if (arguments.size() != funcEntry->get_params().size())
        exit_with_err_msg("sclp error: Number of arguments mismatch");

    for (int i = 0; i < arguments.size(); ++i) {
        arguments[i]->validateNode(sym_tab);
        if (arguments[i]->exprType != funcEntry->get_params()[i])
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

std::vector<TAC_Stmt *> Function_Call_Ast::generateTAC(TAC &tac,
                                                       SymbolTable *local) {
    std::vector<std::shared_ptr<TAC_Opd>> params;
    std::vector<TAC_Stmt *> result;
    if (funcEntry->get_return_type() != BaseType::VOID)
        place = tac.genNewTemporary();
    for (auto &ptr : arguments) {
        for (auto &stmt : ptr->generateTAC(tac, local))
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

void Name_Expr_Ast::validateNode(SymbolTable *sym_tab) {
    steEntry = sym_tab->lookup(name);
    if (steEntry == nullptr)
        exit_with_err_msg("sclp error: Could not look up variable " + name);

    exprType = steEntry->get_type();
}

void Name_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << "Name : " << name << type_to_string(exprType);
}

std::vector<TAC_Stmt *> Name_Expr_Ast::generateTAC(TAC &tac,
                                                   SymbolTable *local) {
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

template <> void Literal_Expr_Ast<int>::validateNode(SymbolTable *sym_tab) {
    exprType = BaseType::INT;
}

template <> void Literal_Expr_Ast<double>::validateNode(SymbolTable *sym_tab) {
    exprType = BaseType::FLOAT;
}

template <>
void Literal_Expr_Ast<std::string>::validateNode(SymbolTable *sym_tab) {
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
                                                           SymbolTable *local) {
    place = std::make_shared<Int_Const_TAC_Opd>(value);
    return std::vector<TAC_Stmt *>();
}

template <>
std::vector<TAC_Stmt *>
Literal_Expr_Ast<double>::generateTAC(TAC &tac, SymbolTable *local) {
    place = std::make_shared<Double_Const_TAC_Opd>(value);
    return std::vector<TAC_Stmt *>();
}

template <>
std::vector<TAC_Stmt *>
Literal_Expr_Ast<std::string>::generateTAC(TAC &tac, SymbolTable *local) {
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
    : name(nam), steEntry(nullptr), dims(std::move(dim_accessed)) {}

bool Array_Access_Expr_Ast::isLvalue() { return true; }

// Boolean Expression class
Boolean_Expr_Ast::Boolean_Expr_Ast(std::unique_ptr<Expression_Ast> oper1,
                                   BooleanOperator oper,
                                   std::unique_ptr<Expression_Ast> oper2)
    : op(oper) {
    leftOp = std::move(oper1);
    rightOp = std::move(oper2);
}

void Boolean_Expr_Ast::validateNode(SymbolTable *sym_tab) {
    leftOp->validateNode(sym_tab);
    rightOp->validateNode(sym_tab);
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

std::vector<TAC_Stmt *> Boolean_Expr_Ast::generateTAC(TAC &tac,
                                                      SymbolTable *local) {
    auto left_tac = leftOp->generateTAC(tac, local);
    auto right_tac = rightOp->generateTAC(tac, local);
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

void Arithmetic_Expr_Ast::validateNode(SymbolTable *sym_tab) {
    leftOp->validateNode(sym_tab);
    rightOp->validateNode(sym_tab);
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
                                                         SymbolTable *local) {
    auto left_tac = leftOp->generateTAC(tac, local);
    auto right_tac = rightOp->generateTAC(tac, local);
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

void Relational_Expr_Ast::validateNode(SymbolTable *sym_tab) {
    leftOp->validateNode(sym_tab);
    rightOp->validateNode(sym_tab);
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
                                                         SymbolTable *local) {
    auto left_tac = leftOp->generateTAC(tac, local);
    auto right_tac = rightOp->generateTAC(tac, local);
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

void Ternary_Expr_Ast::validateNode(SymbolTable *sym_tab) {
    condition->validateNode(sym_tab);
    trueExpr->validateNode(sym_tab);
    falseExpr->validateNode(sym_tab);

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

std::vector<TAC_Stmt *> Ternary_Expr_Ast::generateTAC(TAC &tac,
                                                      SymbolTable *local) {
    auto cond_tac = condition->generateTAC(tac, local);
    place = tac.genNewSTemporary(exprType, local);
    std::shared_ptr<Label_TAC_Opd> false_label = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> exit_label = tac.genNewLabel();
    auto true_tac = trueExpr->generateTAC(tac, local);
    auto false_tac = falseExpr->generateTAC(tac, local);

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
Address_Expr_Ast::Address_Expr_Ast(std::unique_ptr<Expression_Ast> oper)
    : operand(std::move(oper)) {}

// Pointer Deref Expression
Pointer_Deref_Expr_Ast::Pointer_Deref_Expr_Ast(std::string nam, int level)
    : name(nam), steEntry(nullptr), pointerLevel(level) {}

bool Pointer_Deref_Expr_Ast::isLvalue() { return true; }

// Unary Minus Expression
UMinus_Expr_Ast::UMinus_Expr_Ast(std::unique_ptr<Expression_Ast> oper)
    : operand(std::move(oper)) {}

void UMinus_Expr_Ast::validateNode(SymbolTable *sym_tab) {
    operand->validateNode(sym_tab);

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

std::vector<TAC_Stmt *> UMinus_Expr_Ast::generateTAC(TAC &tac,
                                                     SymbolTable *local) {
    auto oper_tac = operand->generateTAC(tac, local);
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

void Not_Expr_Ast::validateNode(SymbolTable *sym_tab) {
    operand->validateNode(sym_tab);

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

std::vector<TAC_Stmt *> Not_Expr_Ast::generateTAC(TAC &tac,
                                                  SymbolTable *local) {
    auto oper_tac = operand->generateTAC(tac, local);
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

void Assignment_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    target->validateNode(sym_tab);
    value->validateNode(sym_tab);

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

std::vector<TAC_Stmt *>
Assignment_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                                 std::shared_ptr<Variable_TAC_Opd> ret_var,
                                 std::shared_ptr<Label_TAC_Opd> ret_label) {
    auto target_tac = target->generateTAC(tac, local);
    auto value_tac = value->generateTAC(tac, local);
    Asgn_TAC_Stmt *stmt = new Asgn_TAC_Stmt(target->place, value->place);

    std::vector<TAC_Stmt *> result;
    for (auto s : target_tac)
        result.push_back(s);
    for (auto s : value_tac)
        result.push_back(s);
    result.push_back(stmt);
    return result;
}

// Iteration Statement class
Iteration_Stmt_Ast::Iteration_Stmt_Ast(std::unique_ptr<Expression_Ast> cond,
                                       std::unique_ptr<Statement_Ast> bod)
    : condition(std::move(cond)), body(std::move(bod)) {}

void Iteration_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    condition->validateNode(sym_tab);
    body->validateNode(sym_tab);

    if (condition->exprType != BaseType::BOOL)
        exit_with_err_msg("sclp error: Loop condition must be a boolean value");
}

bool Iteration_Stmt_Ast::hasReturn() { return body->hasReturn(); }

void While_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "While:" << std::endl
        << std::string(tab, ' ') << "  Condition (";
    condition->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  Body (" << std::endl;
    body->printTree(out, tab + 6);
    out << ")";
}

std::vector<TAC_Stmt *>
While_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                            std::shared_ptr<Variable_TAC_Opd> ret_var,
                            std::shared_ptr<Label_TAC_Opd> ret_label) {
    auto cond_tac = condition->generateTAC(tac, local);
    auto body_tac = body->generateTAC(tac, local, ret_var, ret_label);
    std::shared_ptr<Temporary_TAC_Opd> opp_cond = tac.genNewTemporary();
    std::shared_ptr<Label_TAC_Opd> check_cond = tac.genNewLabel();
    std::shared_ptr<Label_TAC_Opd> exit_label = tac.genNewLabel();

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
    out << std::string(tab, ' ') << "Do:" << std::endl
        << std::string(tab, ' ') << "  Body (" << std::endl;
    body->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  While Condition (";
    condition->printTree(out, tab + 6);
    out << ")";
}

std::vector<TAC_Stmt *>
Do_While_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                               std::shared_ptr<Variable_TAC_Opd> ret_var,
                               std::shared_ptr<Label_TAC_Opd> ret_label) {
    auto body_tac = body->generateTAC(tac, local, ret_var, ret_label);
    auto cond_tac = condition->generateTAC(tac, local);

    std::shared_ptr<Label_TAC_Opd> begin = tac.genNewLabel();
    Label_TAC_Stmt *begin_stmt = new Label_TAC_Stmt(begin);
    If_Goto_TAC_Stmt *jump_to_start =
        new If_Goto_TAC_Stmt(condition->place, begin);

    std::vector<TAC_Stmt *> result;
    result.push_back(begin_stmt);
    for (auto s : body_tac)
        result.push_back(s);
    for (auto s : cond_tac)
        result.push_back(s);
    result.push_back(jump_to_start);
    return result;
}

// Read Statement class
Read_Stmt_Ast::Read_Stmt_Ast(std::unique_ptr<Name_Expr_Ast> t)
    : target(std::move(t)) {}

void Read_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    target->validateNode(sym_tab);

    if (target->exprType != BaseType::INT &&
        target->exprType != BaseType::FLOAT)
        exit_with_err_msg("sclp error: Can read only int / float values");
}

void Read_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Read: ";
    target->printTree(out, tab + 2);
    out;
}

std::vector<TAC_Stmt *>
Read_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                           std::shared_ptr<Variable_TAC_Opd> ret_var,
                           std::shared_ptr<Label_TAC_Opd> ret_label) {
    auto target_tac = target->generateTAC(tac, local);
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

void Return_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    return_value->validateNode(sym_tab);
    if (return_value->exprType !=
        (sym_tab->func_lookup(funcName))->get_return_type())
        exit_with_err_msg("sclp error: Return type mistmatch");
}

void Return_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Return: ";
    return_value->printTree(out, tab + 2);
}

std::vector<TAC_Stmt *>
Return_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                             std::shared_ptr<Variable_TAC_Opd> ret_var,
                             std::shared_ptr<Label_TAC_Opd> ret_label) {
    auto ret_val_tac = return_value->generateTAC(tac, local);
    Asgn_TAC_Stmt *set_stemp = new Asgn_TAC_Stmt(ret_var, return_value->place);
    Goto_TAC_Stmt *go_func_end = new Goto_TAC_Stmt(ret_label);

    std::vector<TAC_Stmt *> result;
    for (auto s : ret_val_tac)
        result.push_back(s);
    result.push_back(set_stemp);
    result.push_back(go_func_end);
    return result;
}

bool Return_Stmt_Ast::hasReturn() { return true; }

// Selection Statement class
Selection_Stmt_Ast::Selection_Stmt_Ast(std::unique_ptr<Expression_Ast> cond,
                                       std::unique_ptr<Statement_Ast> then,
                                       std::unique_ptr<Statement_Ast> els)
    : condition(std::move(cond)), then_stmt(std::move(then)),
      else_stmt(std::move(els)) {}

void Selection_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    condition->validateNode(sym_tab);
    then_stmt->validateNode(sym_tab);
    if (else_stmt != nullptr)
        else_stmt->validateNode(sym_tab);

    if (condition->exprType != BaseType::BOOL)
        exit_with_err_msg("sclp error: If condition must be a bool");
}

void Selection_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "If:" << std::endl
        << std::string(tab, ' ') << "  Condition (";
    condition->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  Then (" << std::endl;
    then_stmt->printTree(out, tab + 6);
    out << ")";
    if (else_stmt != nullptr) {
        out << std::endl << std::string(tab, ' ') << "  Else (" << std::endl;
        else_stmt->printTree(out, tab + 6);
        out << ")";
    }
}

std::vector<TAC_Stmt *>
Selection_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                                std::shared_ptr<Variable_TAC_Opd> ret_var,
                                std::shared_ptr<Label_TAC_Opd> ret_label) {
    auto cond_tac = condition->generateTAC(tac, local);
    auto then_tac = then_stmt->generateTAC(tac, local, ret_var, ret_label);

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
        auto else_tac = else_stmt->generateTAC(tac, local, ret_var, ret_label);
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

void Sequence_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    for (auto &ptr : statement_list) {
        ptr->validateNode(sym_tab);
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

std::vector<TAC_Stmt *>
Sequence_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                               std::shared_ptr<Variable_TAC_Opd> ret_var,
                               std::shared_ptr<Label_TAC_Opd> ret_label) {
    std::vector<TAC_Stmt *> result;
    for (auto &stmt : statement_list) {
        auto stmt_tac = stmt->generateTAC(tac, local, ret_var, ret_label);
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

void Write_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    target->validateNode(sym_tab);

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

std::vector<TAC_Stmt *>
Write_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                            std::shared_ptr<Variable_TAC_Opd> ret_var,
                            std::shared_ptr<Label_TAC_Opd> ret_label) {
    auto target_tac = target->generateTAC(tac, local);
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

void Call_Stmt_Ast::validateNode(SymbolTable *sym_tab) {
    func_call->validateNode(sym_tab);
    if (func_call->exprType != BaseType::VOID)
        exit_with_err_msg("sclp error: Return type of function ignored");
}

void Call_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "FN CALL: " << func_call->name << "(";
    for (auto &ptr : func_call->arguments) {
        out << std::endl;
        ptr->printTree(out, tab);
    }
    out << ")";
}

std::vector<TAC_Stmt *>
Call_Stmt_Ast::generateTAC(TAC &tac, SymbolTable *local,
                           std::shared_ptr<Variable_TAC_Opd> ret_var,
                           std::shared_ptr<Label_TAC_Opd> ret_label) {
    func_call->generateTAC(tac, local);
    Call_TAC_Stmt *call_stmt = new Call_TAC_Stmt(func_call->place);
    std::vector<TAC_Stmt *> result = {call_stmt};
    return result;
}

// Function Ast class
Function_Ast::Function_Ast(DataType ret,
                           std::vector<std::pair<std::string, DataType>> params,
                           std::string nam,
                           std::vector<std::unique_ptr<Statement_Ast>> stmts,
                           SymbolTable *loc)
    : returnType(ret), parameters(params), name(nam),
      statements(std::move(stmts)), local(loc) {
    name = (nam == "main") ? "main" : nam + "_";
}

void Function_Ast::validateFunction() {
    if (local->hasDuplicate())
        exit_with_err_msg(
            "sclp error: Redeclaration of variable outside function");
    for (auto &stmt_node : statements)
        stmt_node->validateNode(local);

    if (returnType != BaseType::VOID) {
        for (auto &stmt_node : statements)
            if (stmt_node->hasReturn())
                return;

        exit_with_err_msg(
            "sclp error: No return statement in a non void function");
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
        (name == "main") ? nullptr
                         : tac.genNewSTemporary(returnType, local, true);
    for (auto &stmt : statements)
        tac.addTACStatements(
            stmt->generateTAC(tac, local, return_stemp, return_label));

    if (returnType != BaseType::VOID) {
        std::vector<TAC_Stmt *> ret_statements;
        Label_TAC_Stmt *ret_label = new Label_TAC_Stmt(return_label);
        Return_TAC_Stmt *ret_stmt = new Return_TAC_Stmt(return_stemp);
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
    int stack_space = local->getStackSpace();
    // Prologue
    out << "\tsw $ram 0($sp)" << std::endl;
    out << "\tsw $fp, -4($sp)" << std::endl;
    out << "\tsub $fp, $sp, 4" << std::endl;
    out << "\tsub $sp, $sp, " << stack_space << std::endl;

    spim.print(out); // treat return as jump to epilogue

    // Epilogue
    out << "epilogue_" << name << ":" << std::endl;
    out << "\tadd $sp, $sp, " << stack_space << std::endl;
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
        exit_with_err_msg("sclp error: Redeclaration of variable / function");
    // Check if main type exists
    if (funcs.find("main") == funcs.end())
        exit_with_err_msg("sclp error: No main function defined");
    for (const auto &[name, func] : funcs)
        func->validateFunction();
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
    if (!global_vars.empty() || !string_consts.empty())
        out << "\t.data" << std::endl;
    for (auto const &[dt, name] : global_vars) {
        if (dt == BaseType::FLOAT)
            out << name << "_\t.double 0.0" << std::endl;
        else
            out << name << "_\t.word 0" << std::endl;
    }
    int i = 0;
    for (auto const &str : string_consts)
        out << "_str_" << i++ << "\t.asciiz " << str << std::endl;

    for (auto const &[name, func] : funcs)
        func->printSPIM(out);
}

Program::~Program() { delete global; }
