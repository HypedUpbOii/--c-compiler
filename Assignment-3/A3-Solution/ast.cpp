#include "ast.hpp"
#include "common_utils.hpp"

extern SymbolTable* local_sym_tab;

ExprNode::~ExprNode() {
    delete place;
}

ProgramNode::ProgramNode() : global(nullptr) {
    funcs = std::vector<std::unique_ptr<FunctionNode>>();
    func_decls = std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>();
}

void ProgramNode::setSymbolTable(SymbolTable* symtab) { global = symtab; }

bool ProgramNode::validateNode() {
    if (global->hasDuplicate()) {
        std::cerr << "sclp error: Redeclaration of variable" << std::endl;
        exit(1);
    }
    std::vector<std::tuple<DataType, std::string, std::vector<DataType>>> func_defs;
    for (auto& func : funcs) {
        std::vector<DataType> params;
        for (auto& [var, type] : func->parameters) {
            params.push_back(type);
        }
        func_defs.emplace_back(func->returnType, func->name, params);
    }
    if (global->hasFuncVarConflict(func_decls) || global->hasFuncVarConflict(func_defs)) {
        std::cerr << "sclp error: Variable has the same name as a procedure" << std::endl;
        exit(1);
    }
    for (auto& func : funcs) {
        if (func->checkFuncVarConflict(func_decls) || func->checkFuncVarConflict(func_defs)) {
            std::cerr << "sclp error: Variable has the same name as a procedure" << std::endl;
            exit(1);
        }
    }
    if (func_decls.size() > 1) {
        std::cerr << "sclp error: Higher level feature detected: with function declaration" << std::endl;
        exit(1);
    }
    if (funcs.size() != 1) {
        std::cerr << "sclp error: More than one function definition" << std::endl;
        exit(1);
    }
    if (func_decls.size() == 1) {
        if (funcs[0]->name == std::get<1>(func_decls[0]) && funcs[0]->returnType == std::get<0>(func_decls[0])) {
            if (std::get<2>(func_decls[0]).size() != funcs[0]->parameters.size()) {
                std::cerr << "sclp error: Signature of declaration and definition do not match" << std::endl;
                exit(1);
            }
            if (funcs[0]->parameters.size() != 0) {
                for (int i = 0; i < funcs[0]->parameters.size(); ++i) {
                    if (std::get<2>(func_decls[0])[i] != funcs[0]->parameters[i].second) {
                        std::cerr << "sclp error: Signature of declaration and definition do not match" << std::endl;
                        exit(1);
                    }
                }
            }
        } else {
            std::cerr << "sclp error: Name and return type of declaration not the same as definition" << std::endl;
            exit(1);
        }
    }
    return funcs[0]->validateNode();
}

void ProgramNode::printTree(std::ostream& out, int tab) {
    for (auto& func : funcs) {
        func->printTree(out, tab);
    }
}

void ProgramNode::generateTAC() {
    for (auto & func : funcs) {
        // each func has its own TAC object (temp numbers are shared tho)
        func->generateTAC();
    }
}

void ProgramNode::printTAC(std::ostream& out) {
    for (auto& func : funcs) {
        func->printTAC(out);
    }
}

ProgramNode::~ProgramNode() {
    delete global;
}

FunctionNode::FunctionNode(DataType ret, std::vector<std::pair<std::string, DataType>> params, std::string nam, std::vector<std::unique_ptr<StmtNode>> stmts, SymbolTable* loc) 
    : returnType(ret), parameters(params), name(nam), statements(std::move(stmts)), local(loc) {}

bool FunctionNode::checkFuncVarConflict(const std::vector<std::tuple<DataType, std::string, std::vector<DataType>>>& decls) {
    return local->hasFuncVarConflict(decls);
}

bool FunctionNode::validateNode() {
    if (local->hasDuplicate()) {
        std::cerr << "sclp error: Redeclaration of variable" << std::endl;
        exit(1);
    }
    if (name != "main") {
        std::cerr << "sclp error: No function with name main found" << std::endl;
        exit(1);
    }
    if (returnType != DataType::VOID) {
        std::cerr << "sclp error: Function named main is not returning void" << std::endl;  
        exit(1);      
    }

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

void FunctionNode::generateTAC() {
    for (auto& stmt : statements) {
        stmt->generateTAC(tac);
    }
}

void FunctionNode::printTAC(std::ostream& out) {
    if (tac.isEmpty()) return;
    
    out << "**PROCEDURE: " << name << std::endl;
    out << "**BEGIN: Three Address Code Statements" << std::endl;
    tac.print(out);
    out << "**END: Three Address Code Statements" << std::endl;
}

AssignStmtNode::AssignStmtNode(std::unique_ptr<VariableExprNode> t, std::unique_ptr<ExprNode> v){
    target = std::move(t);
    value = std::move(v);
}

bool AssignStmtNode::validateNode() {
    if (!target->validateNode() || !value->validateNode()) {
        std::cerr << "sclp error: Target/Value are invalid" << std::endl;
        exit(1);
    }
    if (target->steEntry->get_type() == value->exprType) {
        return true;
    } else {
        std::cerr << "sclp error: LHS and RHS have different types" << std::endl;
        exit(1);
    }
}

void AssignStmtNode::printTree(std::ostream& out, int tab) {
    out << std::string(tab, ' ') << "Asgn:" << std::endl;
    out << std::string(tab, ' ') << "  LHS (";
    target->printTree(out, tab + 6);
    out << ")" << std::endl;
    out << std::string(tab, ' ') << "  RHS (";
    value->printTree(out, tab + 6);
    out << ")" << std::endl;
}

void AssignStmtNode::generateTAC(TAC& tac) {
    target->generateTAC(tac);
    value->generateTAC(tac);
    Asgn_TAC_Stmt* stmt = new Asgn_TAC_Stmt(target->place, value->place);
    tac.addTACStatements(value->code);
    value->code.clear();
    tac.addTACStatement(stmt);
}

ReadStmtNode::ReadStmtNode(std::unique_ptr<VariableExprNode> t){
    target = std::move(t);
}

bool ReadStmtNode::validateNode(){
    if (!target->validateNode()) return false;

    if (target->exprType == DataType::INT || target->exprType == DataType::FLOAT) {
        return true;
    } else {
        std::cerr << "sclp error: Can read only int / float values" << std::endl;
        exit(1);
    }
}

void ReadStmtNode::printTree(std::ostream& out, int tab) {
    out << std::string(tab, ' ') << "Read: ";
    target->printTree(out, tab + 2);
    out << std::endl;
}

void ReadStmtNode::generateTAC(TAC& tac) {
    target->generateTAC(tac);
    IO_TAC_Stmt* stmt = new IO_TAC_Stmt(false, target->place);
    tac.addTACStatements(target->code);
    target->code.clear();
    tac.addTACStatement(stmt);
}

PrintStmtNode::PrintStmtNode(std::unique_ptr<ExprNode> t) {
    target = std::move(t);
}

bool PrintStmtNode::validateNode(){
    if (!target->validateNode()) return false;

    if (target->exprType == DataType::INT || target->exprType == DataType::FLOAT || target->exprType == DataType::STRING) {
        return true;
    } else {
        std::cerr << "sclp error: Can't print bools or other weird types" << std::endl;
        exit(1);
    }
}

void PrintStmtNode::printTree(std::ostream& out, int tab) {
    out << std::string(tab, ' ') << "Write: ";
    target->printTree(out, tab + 2);
    out << std::endl;
}

void PrintStmtNode::generateTAC(TAC& tac) {
    target->generateTAC(tac);
    IO_TAC_Stmt* stmt = new IO_TAC_Stmt(true, target->place);
    tac.addTACStatements(target->code);
    target->code.clear();
    tac.addTACStatement(stmt);
}

BinaryExprNode::BinaryExprNode(BinaryOperator oper, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right) {
    op = oper;
    leftOp = std::move(left);
    rightOp = std::move(right);
}

bool BinaryExprNode::validateNode(){
    if (!leftOp->validateNode() || !rightOp->validateNode()) return false;

    if (leftOp->exprType != rightOp->exprType) {
        std::cerr << "sclp error: Operand types must match" << std::endl;
        exit(1);
    }

    switch(op) {
        case BinaryOperator::PLUS :
        case BinaryOperator::MINUS :
        case BinaryOperator::MULT :
        case BinaryOperator::DIVIDE : {
            exprType = leftOp->exprType;
            break;
        }

        default :
            exprType = DataType::BOOL;
    }

    if (op == BinaryOperator::OR || op == BinaryOperator::AND) {
        if (leftOp->exprType == DataType::BOOL) {
            return true;
        } else {
            std::cerr << "sclp error: Incompatible type with boolean operator" << std::endl;
            exit(1);
        }
    } else {
        if (leftOp->exprType == DataType::INT || leftOp->exprType == DataType::FLOAT) {
            return true;
        } else {
            std::cerr << "sclp error: Incompatible type with arithmetic operator" << std::endl;
            exit(1);
        }
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

void BinaryExprNode::generateTAC(TAC& tac) {
    leftOp->generateTAC(tac);
    rightOp->generateTAC(tac);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt* stmt = new Compute_TAC_Stmt(place, leftOp->place, op, rightOp->place);

    for (auto s : leftOp->code) code.push_back(s);
    leftOp->code.clear();
    for (auto s : rightOp->code) code.push_back(s);
    rightOp->code.clear();
    code.push_back(stmt);
}

UnaryExprNode::UnaryExprNode(UnaryOperator o, std::unique_ptr<ExprNode> oper) {
    op = o;
    operand = std::move(oper);
}

bool UnaryExprNode::validateNode() {
    if (!operand->validateNode()) return false;

    exprType = operand->exprType;
    if (op == UnaryOperator::NOT) {
        if (operand->exprType == DataType::BOOL) {
            return true;
        } else {
            std::cerr << "sclp error: Incompatible unary operand with NOT operator" << std::endl;
            exit(1);
        }
    } else {
        if (operand->exprType == DataType::INT || operand->exprType == DataType::FLOAT) {
            return true;
        } else {
            std::cerr << "sclp error: Incompatible unary operand with UMINUS operator" << std::endl;
            exit(1);
        }
    }
}

void UnaryExprNode::printTree(std::ostream& out, int tab) {
    out << std::endl << std::string(tab, ' ');
    if (op == UnaryOperator::UMINUS) {
        out << "Arith: Uminus" << type_to_string(exprType) << std::endl;
        out << std::string(tab, ' ') << "  L_Opd (";
        operand->printTree(out, tab + 4);
        out << ")";
    } else {
        out << "Condition: NOT" << type_to_string(exprType) << std::endl;
        out << std::string(tab, ' ') << "  L_Opd (";
        operand->printTree(out, tab + 4);
        out << ")";
    }
}

void UnaryExprNode::generateTAC(TAC& tac) {
    operand->generateTAC(tac);
    place = tac.genNewTemporary();
    Compute_TAC_Stmt* stmt = new Compute_TAC_Stmt(place, op, operand->place);

    for (auto s : operand->code) code.push_back(s);
    operand->code.clear();
    code.push_back(stmt);
}

TernaryExprNode::TernaryExprNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ExprNode> tExpr, std::unique_ptr<ExprNode> fExpr) {
    condition = std::move(cond);
    trueExpr = std::move(tExpr);
    falseExpr = std::move(fExpr);
}

bool TernaryExprNode::validateNode(){
    if (!condition->validateNode() || !trueExpr->validateNode() || !falseExpr->validateNode()) return false;
    
    if (condition->exprType != DataType::BOOL) {
        std::cerr << "sclp error: Condition must be of type bool" << std::endl;
        exit(1);
    }

    exprType = trueExpr->exprType;
    if (trueExpr->exprType == falseExpr->exprType) {
        return true;
    } else {
        std::cerr << "sclp error: Both branches have different types" << type_to_string(trueExpr->exprType) << " and " << type_to_string(falseExpr->exprType) << std::endl;
        exit(1);
    }
}

void TernaryExprNode::printTree(std::ostream& out, int tab) {
    condition->printTree(out, tab + 4);
    out << std::endl << std::string(tab + 4, ' ') << "True_Part (";
    trueExpr->printTree(out, tab + 6);
    out << ")" << std::endl << std::string(tab + 4, ' ') << "False_Part (";
    falseExpr->printTree(out, tab + 6);
    out << ")";
}

void TernaryExprNode::generateTAC(TAC& tac) {
    condition->generateTAC(tac);
    place = tac.genNewSTemporary();
    Label_TAC_Opd* false_label = tac.genNewLabel();
    Label_TAC_Opd* exit_label = tac.genNewLabel();
    trueExpr->generateTAC(tac);
    falseExpr->generateTAC(tac);

    Temporary_TAC_Opd* opp_cond = tac.genNewTemporary();
    Compute_TAC_Stmt* negate_stmt = new Compute_TAC_Stmt(opp_cond, UnaryOperator::NOT, condition->place);
    If_Goto_TAC_Stmt* go_to_false = new If_Goto_TAC_Stmt(opp_cond, false_label);
    Goto_TAC_Stmt* go_to_exit = new Goto_TAC_Stmt(exit_label);
    Asgn_TAC_Stmt* set_true_val = new Asgn_TAC_Stmt(place, trueExpr->place);
    Asgn_TAC_Stmt* set_false_val = new Asgn_TAC_Stmt(place, falseExpr->place);
    Label_TAC_Stmt* false_branch = new Label_TAC_Stmt(false_label);
    Label_TAC_Stmt* exit_branch = new Label_TAC_Stmt(exit_label);

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

bool LiteralExprNode::validateNode() {return true;}

IntExprNode::IntExprNode(int i) : value(i) {exprType = DataType::INT;}

void IntExprNode::printTree(std::ostream& out, int tab) {
    out << "Num : " << value << type_to_string(exprType);
}

void IntExprNode::generateTAC(TAC& tac) {
    place = new Int_Const_TAC_Opd(value);
}

FloatExprNode::FloatExprNode(double f) : value(f) {exprType = DataType::FLOAT;}

void FloatExprNode::printTree(std::ostream& out, int tab) {
    out << "Num : " << std::fixed << std::setprecision(2) << value << type_to_string(exprType);
}

void FloatExprNode::generateTAC(TAC& tac) {
    place = new Double_Const_TAC_Opd(value);
}

StringExprNode::StringExprNode(std::string s) : value(s) {exprType = DataType::STRING;}

void StringExprNode::printTree(std::ostream& out, int tab) {
    out << "String : " << value << type_to_string(exprType);
}

void StringExprNode::generateTAC(TAC& tac) {
    place = new String_Const_TAC_Opd(value);
}

VariableExprNode::VariableExprNode(std::string nam, SymbolTableEntry* entry) : name(nam), steEntry(entry) {
    if (steEntry == nullptr) return;
    exprType = steEntry->get_type();
}

bool VariableExprNode::validateNode(){
    if (name == "main") {
        std::cerr << "sclp error: Cannot have variable named main" << std::endl;
        exit(1);
    }
    if (steEntry != nullptr) {
        return true;
    } else {
        std::cerr << "sclp error: Could not look up variable " << name << std::endl;
        exit(1);
    }
}

void VariableExprNode::printTree(std::ostream& out, int tab) {
    out << "Name : " << name << "_" << type_to_string(exprType);
}

void VariableExprNode::generateTAC(TAC& tac) {
    place = new Variable_TAC_Opd(steEntry);
}
