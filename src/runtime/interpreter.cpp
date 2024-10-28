#include "interpreter.hpp"

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
    if (op == "/") result = lhs->value / rhs->value; else // TODO: handle division by 0
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

    return new values::NullVal();
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

    if (fn->type != values::ValueType::NativeFn) {
        throw std::invalid_argument("Cannot call value as it is not a function.");
    }

    auto result = dynamic_cast<values::NativeFnValue*>(fn)->call(args, env);

    return result;
}

values::RuntimeVal* interpreter::evaluate_var_declaration(AST::VarDeclare* declaration, Environment* env) {
    auto value = declaration->value ? evaluate(*declaration->value, env) : utils::MK_NULL();
    return env->declareVar(declaration->identifier, value, declaration->constant);
}

values::RuntimeVal* interpreter::evaluate_assignment(AST::AssignExpr* node, Environment* env) {
    if (node->assigne->kind != AST::NodeType::Identifier) {
        throw std::invalid_argument(fmt::format("Invalid LHS in assignment expression."));
    }
    auto name = dynamic_cast<AST::Identifier*>(node->assigne)->symbol;
    return env->assignVar(name, evaluate(node->value, env));
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
        default: {
            std::cout << "Interpreter: This AST has not been yet setup for interpretation." << std::endl; // message mainly for things that i havent implemented in the interpreter yet.
            exit(1);
        }
    }
}