#include "ast.hpp"

ProgramNode::ProgramNode() : hasFuncDecl(false), global(nullptr) {}

void ProgramNode::setSymbolTable(SymbolTable* symtab) { global = symtab; }

bool ProgramNode::validateNode() {
    if (hasFuncDecl) return false;
    if (funcs.size() != 1) return false;
    return funcs[0]->validateNode();
}

void ProgramNode::printTree(std::ostream& out, int tab) {
    for (auto& func : funcs) {
        func->printTree(out, tab);
    }
}

FunctionNode::FunctionNode() : local(nullptr) {}

void FunctionNode::setSymbolTable(SymbolTable* symtab) { local = symtab; }

bool FunctionNode::validateNode() {
    if (name != "main") return false;
    if (returnType != DataType::VOID) return false;

    for (auto& stmt_node : statements)
        if (!stmt_node->validateNode()) return false;

    return true;
}

void FunctionNode::printTree(std::ostream& out, int tab) {
    out << std::string(tab, ' ') << "**PROCEDURE: " << name << std::endl;
    out << std::string(tab, ' ') << "\tReturn Type: " << type_to_string(returnType) << std::endl;
    out << std::string(tab, ' ') << "\tFormal Parameters:" << std::endl;
    for (auto& [nam, typ] : parameters) {
        out << std::string(tab, ' ') << "\t\t" << nam << "_ Type:" << type_to_string(typ) << std::endl;
    }
    out << std::string(tab, ' ') << "**BEGIN: Abstract Syntax Tree" << std::endl;
    for (auto& stmt : statements) {
        stmt->printTree(out, tab + 9);
    }
    out << std::string(tab, ' ') << "**END: Abstract Syntax Tree" << std::endl;
}

AssignStmtNode::AssignStmtNode(std::unique_ptr<VariableExprNode> t, std::unique_ptr<ExprNode> v){
    target = std::move(t);
    value = std::move(v);
}

bool AssignStmtNode::validateNode() {
    if (!target->validateNode() || !value->validateNode()) return false;
    return target->steEntry->get_type() == value->exprType;
}

void AssignStmtNode::printTree(std::ostream& out, int tab) {
    out << std::string(tab, ' ') << "Asgn:" << std::endl;
    out << std::string(tab, ' ') << "\tLHS (";
    target->printTree(out, tab + 2);
    out << ")" << std::endl;
    out << std::string(tab, ' ') << "\tRHS (";
    value->printTree(out, tab + 2);
    out << ")" << std::endl;
}

ReadStmtNode::ReadStmtNode(std::unique_ptr<VariableExprNode> t){
    target = std::move(t);
}

bool ReadStmtNode::validateNode(){
    if (!target->validateNode()) return false;

    return target->exprType == DataType::INT || target->exprType == DataType::FLOAT;
}

void ReadStmtNode::printTree(std::ostream& out, int tab) {
    out << std::string(tab, ' ') << "Read: ";
    target->printTree(out, tab + 2);
    out << std::endl;
}

PrintStmtNode::PrintStmtNode(std::unique_ptr<ExprNode> t) {
    target = std::move(t);
}

bool PrintStmtNode::validateNode(){
    if (!target->validateNode()) return false;

    return target->exprType == DataType::INT || target->exprType == DataType::FLOAT || target->exprType == DataType::STRING;
}

void PrintStmtNode::printTree(std::ostream& out, int tab) {
    out << std::string(tab, ' ') << "Write: ";
    target->printTree(out, tab + 2);
    out << std::endl;
}

BinaryExprNode::BinaryExprNode(BinaryOperator oper, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right) {
    op = oper;
    leftOp = std::move(left);
    rightOp = std::move(right);
}

bool BinaryExprNode::validateNode(){
    if (!leftOp->validateNode() || !rightOp->validateNode()) return false;

    if (leftOp->exprType != rightOp->exprType) return false;
    exprType = leftOp->exprType;

    if (op == BinaryOperator::OR || op == BinaryOperator::AND) {
        return leftOp->exprType == DataType::BOOL;
    } else {
        return leftOp->exprType == DataType::INT || leftOp->exprType == DataType::FLOAT;
    }
}

void BinaryExprNode::printTree(std::ostream& out, int tab) {
    out << std::endl << std::string(tab, ' ');
    switch(op) {
        case BinaryOperator::PLUS :
        case BinaryOperator::MINUS :
        case BinaryOperator::MULT :
        case BinaryOperator::DIVIDE : {
            out << "Arith: " << op_to_string(op) << type_to_string(exprType) << std::endl;
            break;
        }

        default :
            out << "Condition: " << op_to_string(op) << type_to_string(exprType) << std::endl;
    }
    out << std::string(tab + 2, ' ') << "L_Opd (";
    leftOp->printTree(out, tab + 4);
    out << ")" << std::endl;
    out << std::string(tab + 2, ' ') << "R_Opd (";
    rightOp->printTree(out, tab + 4);
    out << ")";
}

UnaryExprNode::UnaryExprNode(UnaryOperator o, std::unique_ptr<ExprNode> oper) {
    op = o;
    operand = std::move(oper);
}

bool UnaryExprNode::validateNode() {
    if (!operand->validateNode()) return false;

    if (op == UnaryOperator::NOT) {
        exprType = operand->exprType;
        return operand->exprType == DataType::BOOL;
    }

    if (op == UnaryOperator::UMINUS) {
        exprType = operand->exprType;
        return operand->exprType == DataType::INT || operand->exprType == DataType::FLOAT;
    }
}

void UnaryExprNode::printTree(std::ostream& out, int tab) {
    out << std::endl << std::string(tab, ' ');
    if (op == UnaryOperator::UMINUS) {
        out << "Arith: Uminus" << type_to_string(exprType) << std::endl;
        out << "  L_Opd (";
        operand->printTree(out, tab + 4);
        out << ")";
    } else {
        out << "Condition: NOT" << type_to_string(exprType) << std::endl;
        out << "  L_Opd (";
        operand->printTree(out, tab + 4);
        out << ")";
    }
}

TernaryExprNode::TernaryExprNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ExprNode> tExpr, std::unique_ptr<ExprNode> fExpr) {
    condition = std::move(cond);
    trueExpr = std::move(tExpr);
    falseExpr = std::move(fExpr);
}

bool TernaryExprNode::validateNode(){
    if (!condition->validateNode() || !trueExpr->validateNode() || !falseExpr->validateNode()) return false;
    
    if (condition->exprType != DataType::BOOL) return false;

    exprType = trueExpr->exprType;
    return trueExpr->exprType == falseExpr->exprType;
}

void TernaryExprNode::printTree(std::ostream& out, int tab) {
    condition->printTree(out, tab + 4);
    out << std::endl << std::string(tab + 4, ' ') << "True_Part (";
    trueExpr->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab + 4, ' ') << "False_Part (";
    falseExpr->printTree(out, tab + 6);
    out << ")";
}

bool LiteralExprNode::validateNode() {return true;}

IntExprNode::IntExprNode(int i) : value(i) {exprType = DataType::INT;}

void IntExprNode::printTree(std::ostream& out, int tab) {
    out << "Num : " << value << type_to_string(exprType);
}

FloatExprNode::FloatExprNode(float f) : value(f) {exprType = DataType::FLOAT;}

void FloatExprNode::printTree(std::ostream& out, int tab) {
    out << "Num : " << std::fixed << std::setprecision(2) << value << type_to_string(exprType);
}

StringExprNode::StringExprNode(std::string s) : value(s) {exprType = DataType::STRING;}

void StringExprNode::printTree(std::ostream& out, int tab) {
    out << "String : " << value << type_to_string(exprType);
}

VariableExprNode::VariableExprNode(std::string name, SymbolTableEntry* steEntry) {
    if (steEntry == nullptr) return;
    exprType = steEntry->get_type();
}

bool VariableExprNode::validateNode(){
    return steEntry != nullptr;
}

void VariableExprNode::printTree(std::ostream& out, int tab) {
    out << "Name : " << name << "_" << type_to_string(exprType);
}
