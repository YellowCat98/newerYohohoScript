#include "interpreter.hpp"
#include <iostream>
#include "../utils.hpp"

using namespace runtime;
using namespace frontend;

values::RuntimeVal* interpreter::evaluate_program(AST::Program* program, Environment* env) {
    values::RuntimeVal* lastEvaluated;

    for (auto& statement : program->body) {
        lastEvaluated = evaluate(statement, env);
    }

    return lastEvaluated;
}

values::NumVal* interpreter::evaluate_numeric_binary_expr(values::NumVal* lhs, values::NumVal* rhs, const std::string& op) {
    int result = 0;

    if (op == "+") result = lhs->value + rhs->value; else
    if (op == "-") result = lhs->value - rhs->value; else
    if (op == "*") result = lhs->value * rhs->value; else
    if (op == "/") result = lhs->value / rhs->value; else
    result = lhs->value % rhs->value;

    auto returnvalue = new values::NumVal();
    returnvalue->value = result;
    return returnvalue;
}

values::RuntimeVal* interpreter::evaluate_binary_expr(AST::BinEx* binop, Environment* env) {
    auto lhs = evaluate(binop->left, env);
    auto rhs = evaluate(binop->right, env);

    if (lhs->type == values::ValueType::Number && rhs->type == values::ValueType::Number) {
        return evaluate_numeric_binary_expr(dynamic_cast<values::NumVal*>(lhs), dynamic_cast<values::NumVal*>(rhs), binop->op);
    }

    return new values::RuntimeVal();
}

values::RuntimeVal* interpreter::evaluate_identifier(AST::Identifier* ident, Environment* env) {
    auto val = env->lookupVar(ident->symbol);
    return val;
}

values::RuntimeVal* interpreter::evaluate_object_expr(AST::ObjectLiteral* obj, Environment* env) {
    auto object = new values::ObjectVal();

    for (auto& prop : obj->properties) {
        auto runtimeVal = (prop->value.value() == nullptr) ? env->lookupVar(prop->key) : evaluate(dynamic_cast<AST::Stmt*>(prop->value.value()), env);

        object->properties.emplace(prop->key, runtimeVal);
    }

    return object;
}

values::RuntimeVal* interpreter::evaluate_call_expr(AST::CallExpr* expr, Environment* env) {
    std::deque<values::RuntimeVal*> args;
    for (auto& arg : expr->args) {
        args.push_back(evaluate(arg, env));
    }
    auto fn = evaluate(expr->caller, env);

    if (fn->type == values::ValueType::NativeFn) {
        auto result = dynamic_cast<values::NativeFnValue*>(fn)->call(args, env);
        return result;
    }

    if (fn->type == values::ValueType::Function) {
        
        auto func = dynamic_cast<values::FunValue*>(fn);
        auto scope = new Environment(func->decEnv);

        for (int i = 0; i < func->params.size(); ++i) {
            auto name = func->params[i];
            scope->declareVar(name, args[i], false);
        }

        auto result = new values::RuntimeVal();
        for (auto& stmt : func->body) {
            result = evaluate(stmt, scope);
        }

        return result;
    }

    throw std::runtime_error("Interpreter: Cannot call value that is not a function.");
}

values::RuntimeVal* interpreter::evaluate_var_declaration(AST::VarDeclare* declaration, Environment* env) {
    auto value = declaration->value ? evaluate(declaration->value.value(), env) : utils::MK_NULL();
    return env->declareVar(declaration->identifier, value, declaration->constant);
}

values::RuntimeVal* interpreter::evaluate_assignment(AST::AssignExpr* node, Environment* env) {
    if (node->assigne->kind != AST::NodeType::Identifier) {
        throw std::runtime_error(fmt::format("Invalid LHS in assignment expression."));
    }
    auto name = dynamic_cast<AST::Identifier*>(node->assigne)->symbol;
    return env->assignVar(name, evaluate(node->value, env));
}

values::RuntimeVal* interpreter::evaluate_fun_declaration(AST::FunDeclare* declaration, Environment* env) {
    auto fn = new values::FunValue();
    fn->name = declaration->name;
    fn->params = declaration->parameters;
    fn->decEnv = env;
    fn->body = declaration->body;

    return env->declareVar(declaration->name, fn, true);
}

values::RuntimeVal* interpreter::evaluate_if_statement(AST::IfStmt* ifstmt, Environment* env) {
    bool condition = dynamic_cast<values::BoolVal*>(evaluate(ifstmt->condition, env))->value;

    bool hasElse = ifstmt->elseStmt.has_value();

    values::RuntimeVal* lastEvaluated = new values::RuntimeVal();

    if (!hasElse) {
        if (condition) {
            for (auto& stmt : ifstmt->body) {
                lastEvaluated = evaluate(stmt, env);
            }
            return lastEvaluated;
        }
    } else {
        if (condition) {
            for (auto& stmt : ifstmt->body) {
                lastEvaluated = evaluate(stmt, env);
            }
            return lastEvaluated;
        } else {
            for (auto& stmt : ifstmt->elseStmt.value()->body) {
                lastEvaluated = evaluate(stmt, env);
            }
            return lastEvaluated;
        }
    }

    return lastEvaluated;
}

values::RuntimeVal* interpreter::evaluate_comparison_expr(AST::CompEx* compEx, Environment* env) {
    auto left = dynamic_cast<values::NumVal*>(evaluate(compEx->left, env));
    auto right = dynamic_cast<values::NumVal*>(evaluate(compEx->right, env));

    bool result;

    if (compEx->op == "<") result = left->value < right->value; else
    if (compEx->op == ">") result = left->value > right->value; else
    if (compEx->op == "==") result = left->value == right->value; else
    if (compEx->op == ">=") result = left->value >= right->value; else
    if (compEx->op == "<=") result = left->value <= right->value;

    return utils::MK_BOOL(result);
}

values::RuntimeVal* interpreter::evaluate(AST::Stmt* astNode, Environment* env) {
    switch (astNode->kind) {
        case AST::NodeType::NumericLiteral: {
            auto value = new values::NumVal();
            value->value = dynamic_cast<AST::NumericLiteral*>(astNode)->value;
            return value;
        }
        case AST::NodeType::Identifier: {
            return evaluate_identifier(dynamic_cast<AST::Identifier*>(astNode), env);
        }
        case AST::NodeType::BinaryExpr: {
            return evaluate_binary_expr(dynamic_cast<AST::BinEx*>(astNode), env);
        }
        case AST::NodeType::Program: {
            return evaluate_program(dynamic_cast<AST::Program*>(astNode), env);
        }
        case AST::NodeType::VarDeclare: {
            return evaluate_var_declaration(dynamic_cast<AST::VarDeclare*>(astNode), env);
        }
        case AST::NodeType::AssignmentExpr: {
            return evaluate_assignment(dynamic_cast<AST::AssignExpr*>(astNode), env);
        }
        case AST::NodeType::ObjectLiteral: {
            return evaluate_object_expr(dynamic_cast<AST::ObjectLiteral*>(astNode), env);
        }
        case AST::NodeType::CallExpr: {
            return evaluate_call_expr(dynamic_cast<AST::CallExpr*>(astNode), env);
        }
        case AST::NodeType::FunctionDeclaration: {
            return evaluate_fun_declaration(dynamic_cast<AST::FunDeclare*>(astNode), env);
        }
        case AST::NodeType::If: {
            return evaluate_if_statement(dynamic_cast<AST::IfStmt*>(astNode), env);
        }
        case AST::NodeType::CompExpr: {
            return evaluate_comparison_expr(dynamic_cast<AST::CompEx*>(astNode), env);
        }
        default: {
            std::cout << "Interpreter: This AST has not been yet setup for interpretation." << std::endl; // message mainly for things that i havent implemented in the interpreter yet.
            exit(1);
        }
    }
}