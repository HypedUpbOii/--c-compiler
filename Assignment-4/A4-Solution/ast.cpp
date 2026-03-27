#include "ast.hpp"

Expression_Ast::~Expression_Ast() { delete place; }

bool Expression_Ast::isLvalue() { return false; }

// Function call
Function_Call_Ast::Function_Call_Ast(
    std::string nam, SymbolTableEntry *entry,
    std::vector<std::unique_ptr<Expression_Ast>> args)
    : name(nam), funcEntry(entry), arguments(std::move(args)) {}

// Name/Variable class
Name_Expr_Ast::Name_Expr_Ast(std::string nam, SymbolTableEntry *ste)
    : name(nam), steEntry(ste) {}

void Name_Expr_Ast::validateNode() {
    if (steEntry == nullptr) {
        exit_with_err_msg("sclp error: Could not look up variable " + name);
    }
    exprType = steEntry->get_type();
}

void Name_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << "Name : " << name << "_" << type_to_string(exprType);
}

std::vector<TAC_Stmt *> Name_Expr_Ast::generateTAC(TAC &) {
    place = new Variable_TAC_Opd(steEntry);
    return std::vector<TAC_Stmt *>();
}

bool Name_Expr_Ast::isLvalue() { return true; }

// Literal class
template class Literal_Expr_Ast<int>;
template class Literal_Expr_Ast<double>;
template class Literal_Expr_Ast<std::string>;

template <typename T>
Literal_Expr_Ast<T>::Literal_Expr_Ast(T val) : value(val) {}

template <> void Literal_Expr_Ast<int>::validateNode() {
    exprType = BaseType::INT;
}

template <> void Literal_Expr_Ast<double>::validateNode() {
    exprType = BaseType::FLOAT;
}

template <> void Literal_Expr_Ast<std::string>::validateNode() {
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
std::vector<TAC_Stmt *> Literal_Expr_Ast<int>::generateTAC(TAC &tac) {
    place = new Int_Const_TAC_Opd(value);
    return std::vector<TAC_Stmt *>();
}

template <>
std::vector<TAC_Stmt *> Literal_Expr_Ast<double>::generateTAC(TAC &tac) {
    place = new Double_Const_TAC_Opd(value);
    return std::vector<TAC_Stmt *>();
}

template <>
std::vector<TAC_Stmt *> Literal_Expr_Ast<std::string>::generateTAC(TAC &tac) {
    place = new String_Const_TAC_Opd(value);
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
    std::string nam, SymbolTableEntry *entry,
    std::vector<std::unique_ptr<Expression_Ast>> &dim_accessed)
    : name(nam), steEntry(entry), dims(std::move(dim_accessed)) {}

bool Array_Access_Expr_Ast::isLvalue() { return true; }

// Boolean Expression class
Boolean_Expr_Ast::Boolean_Expr_Ast(std::unique_ptr<Expression_Ast> oper1,
                                   BooleanOperator oper,
                                   std::unique_ptr<Expression_Ast> oper2)
    : op(oper) {
    leftOp = std::move(oper1);
    rightOp = std::move(oper2);
}

void Boolean_Expr_Ast::validateNode() {
    leftOp->validateNode();
    rightOp->validateNode();
    if (leftOp->exprType != BaseType::BOOL ||
        rightOp->exprType != BaseType::BOOL) {
        exit_with_err_msg(
            "sclp error: Incompatible type with boolean operator");
    }
    exprType = BaseType::BOOL;
}

void Boolean_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Condition: " << bool_op_to_string(op)
        << type_to_string(exprType) << std::endl;

    printChildren(out, tab);
}

std::vector<TAC_Stmt *> Boolean_Expr_Ast::generateTAC(TAC &tac) {
    auto left_tac = leftOp->generateTAC(tac);
    auto right_tac = rightOp->generateTAC(tac);
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

void Arithmetic_Expr_Ast::validateNode() {
    leftOp->validateNode();
    rightOp->validateNode();
    if (leftOp->exprType != rightOp->exprType) {
        exit_with_err_msg("sclp error: Operand types must match");
    } else if (leftOp->exprType != BaseType::INT &&
               leftOp->exprType != BaseType::FLOAT) {
        exit_with_err_msg(
            "sclp error: Incompatible type with arithmetic operator");
    }

    exprType = leftOp->exprType;
}

void Arithmetic_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Arith: " << arith_op_to_string(op)
        << type_to_string(exprType) << std::endl;

    printChildren(out, tab);
}

std::vector<TAC_Stmt *> Arithmetic_Expr_Ast::generateTAC(TAC &tac) {
    auto left_tac = leftOp->generateTAC(tac);
    auto right_tac = rightOp->generateTAC(tac);
    place = tac.genNewTemporary();
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

void Relational_Expr_Ast::validateNode() {
    leftOp->validateNode();
    rightOp->validateNode();
    if (leftOp->exprType != rightOp->exprType) {
        exit_with_err_msg("sclp error: Operand types must match");
    } else if (leftOp->exprType != BaseType::INT &&
               leftOp->exprType != BaseType::FLOAT) {
        exit_with_err_msg(
            "sclp error: Incompatible type with relational operator");
    }

    exprType = BaseType::BOOL;
}

void Relational_Expr_Ast::printTree(std::ostream &out, int tab) {
    out << std::endl
        << std::string(tab, ' ') << "Condition: " << rel_op_to_string(op)
        << type_to_string(exprType) << std::endl;

    printChildren(out, tab);
}

std::vector<TAC_Stmt *> Relational_Expr_Ast::generateTAC(TAC &tac) {
    auto left_tac = leftOp->generateTAC(tac);
    auto right_tac = rightOp->generateTAC(tac);
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

void Ternary_Expr_Ast::validateNode() {
    condition->validateNode();
    trueExpr->validateNode();
    falseExpr->validateNode();

    if (condition->exprType != BaseType::BOOL) {
        exit_with_err_msg("sclp error: Condition in the ternary expression "
                          "must be of type bool");
    } else if (trueExpr->exprType != falseExpr->exprType) {
        exit_with_err_msg(
            "sclp error: Both branches in the ternary have different types " +
            type_to_string(trueExpr->exprType) + " and " +
            type_to_string(falseExpr->exprType));
    }

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

std::vector<TAC_Stmt *> Ternary_Expr_Ast::generateTAC(TAC &tac) {
    auto cond_tac = condition->generateTAC(tac);
    place = tac.genNewSTemporary();
    Label_TAC_Opd *false_label = tac.genNewLabel();
    Label_TAC_Opd *exit_label = tac.genNewLabel();
    auto true_tac = trueExpr->generateTAC(tac);
    auto false_tac = falseExpr->generateTAC(tac);

    Temporary_TAC_Opd *opp_cond = tac.genNewTemporary();
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
Pointer_Deref_Expr_Ast::Pointer_Deref_Expr_Ast(std::string nam,
                                               SymbolTableEntry *entry,
                                               int level)
    : name(nam), steEntry(entry), pointerLevel(level) {}

bool Pointer_Deref_Expr_Ast::isLvalue() { return true; }

// Unary Minus Expression
UMinus_Expr_Ast::UMinus_Expr_Ast(std::unique_ptr<Expression_Ast> oper)
    : operand(std::move(oper)) {}

void UMinus_Expr_Ast::validateNode() {
    operand->validateNode();

    if (operand->exprType != BaseType::INT &&
        operand->exprType != BaseType::FLOAT) {
        exit_with_err_msg(
            "sclp error: Incompatible unary operand with UMINUS operator");
    }
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

std::vector<TAC_Stmt *> UMinus_Expr_Ast::generateTAC(TAC &tac) {
    auto oper_tac = operand->generateTAC(tac);
    place = tac.genNewTemporary();
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

void Not_Expr_Ast::validateNode() {
    operand->validateNode();

    if (operand->exprType != BaseType::BOOL) {
        exit_with_err_msg(
            "sclp error: Incompatible unary operand with NOT operator");
    }
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

std::vector<TAC_Stmt *> Not_Expr_Ast::generateTAC(TAC &tac) {
    auto oper_tac = operand->generateTAC(tac);
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
// Assignment Statment class
Assignment_Stmt_Ast::Assignment_Stmt_Ast(std::unique_ptr<Expression_Ast> dst,
                                         std::unique_ptr<Expression_Ast> src)
    : target(std::move(dst)), value(std::move(src)) {}

void Assignment_Stmt_Ast::validateNode() {
    target->validateNode();
    value->validateNode();

    if (!target->isLvalue() || target->exprType != value->exprType) {
        exit_with_err_msg("sclp error: LHS and RHS have different types");
    }
}

void Assignment_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Asgn:" << std::endl
        << std::string(tab, ' ') << "  LHS (";
    target->printTree(out, tab + 4);
    out << ")" << std::endl << std::string(tab, ' ') << "  RHS (";
    value->printTree(out, tab + 4);
    out << ")" << std::endl;
}

std::vector<TAC_Stmt *> Assignment_Stmt_Ast::generateTAC(TAC &tac) {
    auto target_tac = target->generateTAC(tac);
    auto value_tac = value->generateTAC(tac);
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

void Iteration_Stmt_Ast::validateNode() {
    condition->validateNode();
    body->validateNode();

    if (condition->exprType != BaseType::BOOL) {
        exit_with_err_msg("sclp error: Loop condition must be a boolean value");
    }
}

void While_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "While:" << std::endl
        << std::string(tab, ' ') << "  Condition (";
    condition->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  Body (" << std::endl;
    body->printTree(out, tab + 6);
    out << ")" << std::endl;
}

std::vector<TAC_Stmt *> While_Stmt_Ast::generateTAC(TAC &tac) {
    auto cond_tac = condition->generateTAC(tac);
    auto body_tac = body->generateTAC(tac);
    Temporary_TAC_Opd *opp_cond = tac.genNewTemporary();
    Label_TAC_Opd *check_cond = tac.genNewLabel();
    Label_TAC_Opd *exit_label = tac.genNewLabel();

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
    out << ")" << std::endl;
}

std::vector<TAC_Stmt *> Do_While_Stmt_Ast::generateTAC(TAC &tac) {
    auto body_tac = body->generateTAC(tac);
    auto cond_tac = condition->generateTAC(tac);

    Label_TAC_Opd *begin = tac.genNewLabel();
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

void Read_Stmt_Ast::validateNode() {
    target->validateNode();

    if (target->exprType != BaseType::INT &&
        target->exprType != BaseType::FLOAT) {
        exit_with_err_msg("sclp error: Can read only int / float values");
    }
}

void Read_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Read: ";
    target->printTree(out, tab + 2);
    out << std::endl;
}

std::vector<TAC_Stmt *> Read_Stmt_Ast::generateTAC(TAC &tac) {
    auto target_tac = target->generateTAC(tac);
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
    : return_value(std::move(ret)), func_name(func) {}

// Selection Statement class
Selection_Stmt_Ast::Selection_Stmt_Ast(std::unique_ptr<Expression_Ast> cond,
                                       std::unique_ptr<Statement_Ast> then,
                                       std::unique_ptr<Statement_Ast> els)
    : condition(std::move(cond)), then_stmt(std::move(then)),
      else_stmt(std::move(els)) {}

void Selection_Stmt_Ast::validateNode() {
    condition->validateNode();
    then_stmt->validateNode();
    if (else_stmt != nullptr) {
        else_stmt->validateNode();
    }

    if (condition->exprType != BaseType::BOOL) {
        exit_with_err_msg("sclp error: If condition must be a bool");
    }
}

void Selection_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "If:" << std::endl
        << std::string(tab, ' ') << "  Condition (";
    condition->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  Then (" << std::endl;
    then_stmt->printTree(out, tab + 6);
    out << ")";
    if (else_stmt != nullptr) {
        out << std::string(tab, ' ') << "  Else (" << std::endl;
        else_stmt->printTree(out, tab + 6);
        out << ")";
    }
}

std::vector<TAC_Stmt *> Selection_Stmt_Ast::generateTAC(TAC &tac) {
    auto cond_tac = condition->generateTAC(tac);
    auto then_tac = then_stmt->generateTAC(tac);

    Temporary_TAC_Opd *opp_cond = tac.genNewTemporary();
    Compute_TAC_Stmt *negate_cond =
        new Unary_Comp_TAC_Stmt(opp_cond, UnaryOperator::NOT, condition->place);
    Label_TAC_Opd *exit_label = tac.genNewLabel();
    Label_TAC_Stmt *exit_label_stmt = new Label_TAC_Stmt(exit_label);
    Goto_TAC_Stmt *jump_to_exit = new Goto_TAC_Stmt(exit_label);
    Label_TAC_Opd *else_label =
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
        auto else_tac = else_stmt->generateTAC(tac);
        for (auto s : else_tac)
            result.push_back(s);
    }
    result.push_back(exit_label_stmt);
    return result;
}

// Sequence Statement class
Sequence_Stmt_Ast::Sequence_Stmt_Ast(
    std::vector<std::unique_ptr<Statement_Ast>> &statements)
    : statement_list(std::move(statements)) {}

void Sequence_Stmt_Ast::validateNode() {
    for (auto &ptr : statement_list) {
        ptr->validateNode();
    }
}

void Sequence_Stmt_Ast::printTree(std::ostream &out, int tab) {
    for (auto &ptr : statement_list) {
        ptr->printTree(out, tab);
    }
}

std::vector<TAC_Stmt *> Sequence_Stmt_Ast::generateTAC(TAC &tac) {
    std::vector<TAC_Stmt *> result;
    for (auto &stmt : statement_list) {
        auto stmt_tac = stmt->generateTAC(tac);
        for (auto s : stmt_tac)
            result.push_back(s);
    }

    return result;
}

// Write Statement class
Write_Stmt_Ast::Write_Stmt_Ast(std::unique_ptr<Expression_Ast> t)
    : target(std::move(t)) {}

void Write_Stmt_Ast::validateNode() {
    target->validateNode();

    if (target->exprType != BaseType::INT &&
        target->exprType != BaseType::FLOAT &&
        target->exprType != BaseType::STRING) {
        exit_with_err_msg("sclp error: Can't print bools or other weird types");
    }
}

void Write_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Write: ";
    target->printTree(out, tab + 2);
    out << std::endl;
}

std::vector<TAC_Stmt *> Write_Stmt_Ast::generateTAC(TAC &tac) {
    auto target_tac = target->generateTAC(tac);
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

void Call_Stmt_Ast::validateNode() {
    func_call->validateNode();
    if (func_call->exprType != BaseType::VOID) {
        exit_with_err_msg("sclp error: Return type of function ignored");
    }
}

// Function Ast class
Function_Ast::Function_Ast(DataType ret,
                           std::vector<std::pair<std::string, DataType>> params,
                           std::string nam,
                           std::vector<std::unique_ptr<Statement_Ast>> stmts,
                           SymbolTable *loc)
    : returnType(ret), parameters(params), name(nam),
      statements(std::move(stmts)), local(loc) {}

void Function_Ast::validateFunction() {
    if (local->hasDuplicate())
        exit_with_err_msg(
            "sclp error: Redeclaration of variable outside function");
    for (auto &stmt_node : statements)
        stmt_node->validateNode();
}

void Function_Ast::printTree(std::ostream &out) {
    int tab = 0;
    out << std::string(tab, ' ') << "**PROCEDURE: " << name << std::endl;
    out << std::string(tab, ' ')
        << "\tReturn Type: " << type_to_string(returnType) << std::endl;
    out << std::string(tab, ' ') << "\tFormal Parameters:" << std::endl;
    for (auto &[nam, typ] : parameters) {
        out << std::string(tab, ' ') << "\t\t" << nam
            << "_ Type:" << type_to_string(typ) << std::endl;
    }
    out << std::string(tab, ' ') << "**BEGIN: Abstract Syntax Tree"
        << std::endl;
    for (auto &stmt : statements) {
        stmt->printTree(out, tab + 9);
    }
    out << std::string(tab, ' ') << "**END: Abstract Syntax Tree" << std::endl;
}

void Function_Ast::generateTAC() {
    for (auto &stmt : statements) {
        tac.addTACStatements(stmt->generateTAC(tac));
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

Function_Ast::~Function_Ast() { delete local; }

// Program class (please change in A5)
Program::Program() : global(new SymbolTable()) {}

void Program::addFunctions(
    std::vector<std::unique_ptr<Function_Ast>> func_list) {
    for (auto &ptr : func_list) {
        original_func_order.push_back(ptr->name);
        funcs.emplace(ptr->name, std::move(ptr));
    }
}

void Program::validateProgram() {
    if (global->hasDuplicate()) {
        exit_with_err_msg("sclp error: Redeclaration of variable");
    }
    // Check if main type exists
    if (funcs.find("main") == funcs.end()) {
        exit_with_err_msg("sclp error: No main function defined");
    }
    if (funcs["main"]->returnType != BaseType::VOID) {
        exit_with_err_msg(
            "sclp error: Main function must have return type void");
    }
    for (const auto &[name, func] : funcs)
        func->validateFunction();
}

void Program::print(std::ostream &out) {
    for (auto const &[name, func] : funcs)
        func->printTree(out);
}

// each func has its own TAC object (only labels are shared)
void Program::generateTAC() {
    for (auto const &[name, func] : funcs)
        func->generateTAC();
}

void Program::printTAC(std::ostream &out) {
    for (auto const &[name, func] : funcs)
        func->printTAC(out);
}

Program::~Program() { delete global; }
