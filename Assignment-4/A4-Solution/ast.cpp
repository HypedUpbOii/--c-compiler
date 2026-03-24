#include "ast.hpp"

Expression_Ast::~Expression_Ast() { delete place; }

// Function call

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

// Literal class
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
    out << std::endl << std::string(tab + 4, ' ') << "True__Part (";
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

// Pointer Deref Expression

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

// Assignment Statment class
Assignment_Stmt_Ast::Assignment_Stmt_Ast(std::unique_ptr<Name_Expr_Ast> dst,
                                         std::unique_ptr<Expression_Ast> src)
    : target(std::move(dst)), value(std::move(src)) {}

void Assignment_Stmt_Ast::validateNode() {
    target->validateNode();
    value->validateNode();

    if (target->steEntry->get_type() != value->exprType) {
        exit_with_err_msg("sclp error: LHS and RHS have different types");
    }
}

void Assignment_Stmt_Ast::printTree(std::ostream &out, int tab) {
    out << std::string(tab, ' ') << "Asgn:" << std::endl
        << std::string(tab, ' ') << "  LHS (";
    target->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab, ' ') << "  RHS (";
    value->printTree(out, tab + 6);
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
    : condition(std::move(cond)), body(std::move(body)) {}

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
    out << ")" << std::endl << std::string(tab, ' ') << "  Body (";
    body->printTree(out, tab + 6);
    out << ")" << std::endl;
}

std::vector<TAC_Stmt *> While_Stmt_Ast::generateTAC(TAC &tac) {
    auto cond_tac = condition->generateTAC(tac);
    auto body_tac = body->generateTAC(tac);
    Temporary_TAC_Opd *opp_cond = tac.genNewTemporary();
    Label_TAC_Opd *check_cond = tac.genNewLabel();
    Label_TAC_Opd *exit_label = tac.genNewLabel();

    Temporary_TAC_Opd *opp_cond = tac.genNewTemporary();
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
        << std::string(tab, ' ') << "  Body (";
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
    out << ")" << std::endl << std::string(tab, ' ') << "  Then (";
    then_stmt->printTree(out, tab + 6);
    out << ")" << std::endl;
    if (else_stmt != nullptr) {
        out << std::string(tab, ' ') << "  Else (";
        else_stmt->printTree(out, tab + 6);
        out << ")" << std::endl;
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
    std::vector<std::unique_ptr<Statement_Ast>> &statements) {
    for (auto &ptr : statements) {
        statement_list.push_back(std::move(ptr));
    }
}

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

// Function Ast class

Function_Ast::Function_Ast(DataType ret,
                           std::vector<std::pair<std::string, DataType>> params,
                           std::string nam,
                           std::vector<std::unique_ptr<Statement_Ast>> stmts,
                           SymbolTable *loc)
    : returnType(ret), parameters(params), name(nam),
      statements(std::move(stmts)), local(loc) {}

bool Function_Ast::checkFuncVarConflict(
    const std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>
        &decls) {
    return local->hasFuncVarConflict(decls);
}

void Function_Ast::validateNode() {
    if (local->hasDuplicate())
        exit_with_err_msg("sclp error: Redeclaration of variable");
    if (name != "main")
        exit_with_err_msg("sclp error: No function with name main found");
    if (returnType != BaseType::VOID)
        exit_with_err_msg(
            "sclp error: Function named main is not returning void");

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

Program::Program() : global(nullptr) {
    funcs = std::vector<std::unique_ptr<Function_Ast>>();
    func_decls =
        std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>();
}

void Program::setSymbolTable(SymbolTable *symtab) { global = symtab; }

void Program::validateProgram() {
    if (global->hasDuplicate()) {
        exit_with_err_msg("sclp error: Redeclaration of variable");
    }
    std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>
        func_defs;
    for (auto &func : funcs) {
        std::vector<DataType> params;
        for (auto &[var, type] : func->parameters) {
            params.push_back(type);
        }
        func_defs.emplace_back(func->returnType, func->name, params);
    }
    if (global->hasFuncVarConflict(func_decls) ||
        global->hasFuncVarConflict(func_defs)) {
        exit_with_err_msg(
            "sclp error: Variable has the same name as a procedure");
    }
    for (auto &func : funcs) {
        if (func->checkFuncVarConflict(func_decls) ||
            func->checkFuncVarConflict(func_defs)) {
            exit_with_err_msg(
                "sclp error: Variable has the same name as a procedure");
        }
    }
    if (func_decls.size() > 1) {
        exit_with_err_msg("sclp error: Higher level feature detected: with "
                          "function declaration");
    }
    if (funcs.size() != 1) {
        exit_with_err_msg("sclp error: More than one function definition");
    }
    if (func_decls.size() == 1) {
        if (funcs[0]->name == std::get<1>(func_decls[0]) &&
            funcs[0]->returnType == std::get<0>(func_decls[0])) {
            if (std::get<2>(func_decls[0]).size() !=
                funcs[0]->parameters.size()) {
                exit_with_err_msg("sclp error: Signature of declaration and "
                                  "definition do not match");
            }
            if (funcs[0]->parameters.size() != 0) {
                for (int i = 0; i < funcs[0]->parameters.size(); ++i) {
                    if (std::get<2>(func_decls[0])[i] !=
                        funcs[0]->parameters[i].second) {
                        exit_with_err_msg(
                            "sclp error: Signature of declaration and "
                            "definition do not match");
                    }
                }
            }
        } else {
            exit_with_err_msg("sclp error: Name and return type of declaration "
                              "not the same as definition");
        }
    }
    funcs[0]->validateNode();
}

void Program::print(std::ostream &out) {
    for (auto &func : funcs) {
        func->printTree(out);
    }
}

void Program::generateTAC() {
    for (auto &func : funcs) {
        // each func has its own TAC object (temp numbers are shared tho)
        func->generateTAC();
    }
}

void Program::printTAC(std::ostream &out) {
    for (auto &func : funcs) {
        func->printTAC(out);
    }
}

Program::~Program() { delete global; }
