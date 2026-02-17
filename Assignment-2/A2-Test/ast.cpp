#include "ast.hpp"

ProgramNode::ProgramNode(){

}

AssignStmtNode::AssignStmtNode(std::unique_ptr<VariableExprNode> t, std::unique_ptr<ExprNode> v){
    target = std::move(t);
    value = std::move(v);
}

bool AssignStmtNode::validateNode() {
    if (!target->validateNode() || !value->validateNode()) return false;
    return target->steEntry->type == value->exprType;
}

ReadStmtNode::ReadStmtNode(std::unique_ptr<VariableExprNode> t){
    target = std::move(t);
}

bool ReadStmtNode::validateNode(){
    if (!target->validateNode()) return false;

    return target->exprType == DataType::INT || target->exprType == DataType::FLOAT;
}

PrintStmtNode::PrintStmtNode(std::unique_ptr<ExprNode> t) {
    target = std::move(t);
}

bool PrintStmtNode::validateNode(){
    if (!target->validateNode()) return false;

    return target->exprType == DataType::INT || target->exprType == DataType::FLOAT || target->exprType == DataType::STRING;
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

bool VariableExprNode::validateNode(){
    return steEntry != nullptr;
}

bool LiteralExprNode::validateNode(){
    return true;
}

bool TernaryExprNode::validateNode(){
    if (!condition->validateNode() || !trueExpr->validateNode() || !falseExpr->validateNode()) return false;
    
    if (condition->exprType != DataType::BOOL) return false;

    return trueExpr->exprType == falseExpr->exprType;
}

