#include "ast.hpp"

Expression_Ast::~Expression_Ast() {
    delete place;
    code.clear();
}

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

void Name_Expr_Ast::generateTAC(TAC &) {
    place = new Variable_TAC_Opd(steEntry);
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

template <> void Literal_Expr_Ast<int>::generateTAC(TAC &tac) {
    place = new Int_Const_TAC_Opd(value);
}

template <> void Literal_Expr_Ast<double>::generateTAC(TAC &tac) {
    place = new Double_Const_TAC_Opd(value);
}

template <> void Literal_Expr_Ast<std::string>::generateTAC(TAC &tac) {
    place = new String_Const_TAC_Opd(value);
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

void Boolean_Expr_Ast::generateTAC(TAC &tac) {
    leftOp->generateTAC(tac);
    rightOp->generateTAC(tac);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Bool_Comp_TAC_Stmt(place, leftOp->place, op, rightOp->place);

    for (auto s : leftOp->code)
        code.push_back(s);
    leftOp->code.clear();
    for (auto s : rightOp->code)
        code.push_back(s);
    rightOp->code.clear();
    code.push_back(stmt);
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

void Arithmetic_Expr_Ast::generateTAC(TAC &tac) {
    leftOp->generateTAC(tac);
    rightOp->generateTAC(tac);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Arith_Comp_TAC_Stmt(place, leftOp->place, op, rightOp->place);

    for (auto s : leftOp->code)
        code.push_back(s);
    leftOp->code.clear();
    for (auto s : rightOp->code)
        code.push_back(s);
    rightOp->code.clear();
    code.push_back(stmt);
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

void Relational_Expr_Ast::generateTAC(TAC &tac) {
    leftOp->generateTAC(tac);
    rightOp->generateTAC(tac);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Rel_Comp_TAC_Stmt(place, leftOp->place, op, rightOp->place);

    for (auto s : leftOp->code)
        code.push_back(s);
    leftOp->code.clear();
    for (auto s : rightOp->code)
        code.push_back(s);
    rightOp->code.clear();
    code.push_back(stmt);
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

void Ternary_Expr_Ast::generateTAC(TAC &tac) {
    condition->generateTAC(tac);
    place = tac.genNewSTemporary();
    Label_TAC_Opd *false_label = tac.genNewLabel();
    Label_TAC_Opd *exit_label = tac.genNewLabel();
    trueExpr->generateTAC(tac);
    falseExpr->generateTAC(tac);

    Temporary_TAC_Opd *opp_cond = tac.genNewTemporary();
    Compute_TAC_Stmt *negate_stmt =
        new Unary_Comp_TAC_Stmt(opp_cond, UnaryOperator::NOT, condition->place);
    If_Goto_TAC_Stmt *go_to_false = new If_Goto_TAC_Stmt(opp_cond, false_label);
    Goto_TAC_Stmt *go_to_exit = new Goto_TAC_Stmt(exit_label);
    Asgn_TAC_Stmt *set_true_val = new Asgn_TAC_Stmt(place, trueExpr->place);
    Asgn_TAC_Stmt *set_false_val = new Asgn_TAC_Stmt(place, falseExpr->place);
    Label_TAC_Stmt *false_branch = new Label_TAC_Stmt(false_label);
    Label_TAC_Stmt *exit_branch = new Label_TAC_Stmt(exit_label);

    // Clears done to avoid exponential memory blowup
    // Only clear children once used
    for (auto stmt : condition->code) {
        code.push_back(stmt);
    }
    condition->code.clear();
    code.push_back(negate_stmt);
    code.push_back(go_to_false);
    for (auto stmt : trueExpr->code) {
        code.push_back(stmt);
    }
    trueExpr->code.clear();
    code.push_back(set_true_val);
    code.push_back(go_to_exit);
    code.push_back(false_branch);
    for (auto stmt : falseExpr->code) {
        code.push_back(stmt);
    }
    falseExpr->code.clear();
    code.push_back(set_false_val);
    code.push_back(exit_branch);
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

void UMinus_Expr_Ast::generateTAC(TAC &tac) {
    operand->generateTAC(tac);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Unary_Comp_TAC_Stmt(place, UnaryOperator::UMINUS, operand->place);

    for (auto s : operand->code)
        code.push_back(s);
    operand->code.clear();
    code.push_back(stmt);
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

void Not_Expr_Ast::generateTAC(TAC &tac) {
    operand->generateTAC(tac);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt *stmt =
        new Unary_Comp_TAC_Stmt(place, UnaryOperator::NOT, operand->place);

    for (auto s : operand->code)
        code.push_back(s);
    operand->code.clear();
    code.push_back(stmt);
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

void Assignment_Stmt_Ast::generateTAC(TAC &tac) {
    target->generateTAC(tac);
    value->generateTAC(tac);
    Asgn_TAC_Stmt *stmt = new Asgn_TAC_Stmt(target->place, value->place);
    tac.addTACStatements(value->code);
    value->code.clear();
    tac.addTACStatement(stmt);
}

// Iteration Statement class

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

void Read_Stmt_Ast::generateTAC(TAC &tac) {
    target->generateTAC(tac);
    IO_TAC_Stmt *stmt = new IO_TAC_Stmt(false, target->place);
    tac.addTACStatements(target->code);
    target->code.clear();
    tac.addTACStatement(stmt);
}

// Return Statement class

// Selection Statement class

// Sequence Statement class

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

void Write_Stmt_Ast::generateTAC(TAC &tac) {
    target->generateTAC(tac);
    IO_TAC_Stmt *stmt = new IO_TAC_Stmt(true, target->place);
    tac.addTACStatements(target->code);
    target->code.clear();
    tac.addTACStatement(stmt);
}
