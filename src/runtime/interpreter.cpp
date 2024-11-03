#include "interpreter.hpp"
#include <iostream>
#include "../utils.hpp"

using namespace runtime;
using namespace frontend;

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_program(AST::Program* program, Environment* env) {
    auto lastEvaluated = std::make_unique<values::RuntimeVal>();

    for (auto& statement : program->body) {
        lastEvaluated = evaluate(statement, env);
    }

    return lastEvaluated;
}

std::unique_ptr<values::NumVal> interpreter::evaluate_numeric_binary_expr(std::unique_ptr<values::NumVal> lhs, std::unique_ptr<values::NumVal> rhs, const std::string& op) {
    int result = 0;

    if (op == "+") result = lhs->value + rhs->value; else
    if (op == "-") result = lhs->value - rhs->value; else
    if (op == "*") result = lhs->value * rhs->value; else
    if (op == "/") result = lhs->value / rhs->value; else
    result = lhs->value % rhs->value;


    auto returnvalue = std::make_unique<values::NumVal>();
    returnvalue->value = result;
    return returnvalue;
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_binary_expr(AST::BinEx* binop, Environment* env) {
    auto lhs = evaluate(binop->left, env);
    auto rhs = evaluate(binop->right, env);

    if (lhs->type == values::ValueType::Number && rhs->type == values::ValueType::Number) {
        return evaluate_numeric_binary_expr(std::make_unique<values::NumVal>(*static_cast<values::NumVal*>(lhs.get())), std::make_unique<values::NumVal>(*static_cast<values::NumVal*>(rhs.get())), binop->op);
    }

    return std::make_unique<values::RuntimeVal>();
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_identifier(AST::Identifier* ident, Environment* env) {
    return env->lookupVar(ident->symbol);
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_object_expr(AST::ObjectLiteral* obj, Environment* env) {
    auto object = std::make_unique<values::ObjectVal>();

    for (auto& prop : obj->properties) {
        auto runtimeVal = (prop->value.value() == nullptr) ? env->lookupVar(prop->key) : evaluate(static_cast<AST::Stmt*>(prop->value.value()), env);

        object->properties.emplace(prop->key, std::move(runtimeVal));
    }

    return object;
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_call_expr(AST::CallExpr* expr, Environment* env) {
    std::deque<std::unique_ptr<values::RuntimeVal>> args;
    for (auto& arg : expr->args) {
        args.push_back(std::move(evaluate(arg, env)));
    }
    auto fn = evaluate(expr->caller, env);

    if (fn->type == values::ValueType::NativeFn) {
        auto result = static_cast<values::NativeFnValue*>(fn.get())->call(std::move(args), env);
        return result;
    }

    if (fn->type == values::ValueType::Function) {
        
        auto func = static_cast<values::FunValue*>(fn.get());
        auto scope = std::unique_ptr<Environment>(func->decEnv);

        for (int i = 0; i < func->params.size(); ++i) {
            auto name = func->params[i];
            scope->declareVar(name, std::move(args[i]), false);
        }

        auto result = std::unique_ptr<values::RuntimeVal>();
        for (auto& stmt : func->body) {
            result = evaluate(stmt, scope.get());
        }

        return result;
    }

    delete expr;

    throw std::runtime_error("Interpreter: Cannot call value that is not a function.");
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_var_declaration(AST::VarDeclare* declaration, Environment* env) {
    auto value = declaration->value ? evaluate(declaration->value.value(), env) : utils::MK_NULL();
    return env->declareVar(declaration->identifier, std::move(value), declaration->constant);
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_assignment(AST::AssignExpr* node, Environment* env) {
    if (node->assigne->kind != AST::NodeType::Identifier) {
        throw std::runtime_error(fmt::format("Invalid LHS in assignment expression."));
    }
    auto name = static_cast<AST::Identifier*>(node->assigne)->symbol;
    return env->assignVar(name, evaluate(node->value, env));
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_fun_declaration(AST::FunDeclare* declaration, Environment* env) {
    auto fn = std::make_unique<values::FunValue>();
    fn->name = declaration->name;
    fn->params = declaration->parameters;
    fn->decEnv = env;
    fn->body = declaration->body;

    return env->declareVar(declaration->name, std::move(fn), true);
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_if_statement(AST::IfStmt* ifstmt, Environment* env) {
    bool condition = static_cast<values::BoolVal*>(evaluate(ifstmt->condition, env).get())->value;

    bool hasElse = ifstmt->elseStmt.has_value();

    auto lastEvaluated = std::make_unique<values::RuntimeVal>();

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

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_comparison_expr(AST::CompEx* compEx, Environment* env) {
    auto left = static_cast<values::NumVal*>(evaluate(compEx->left, env).get());
    auto right = static_cast<values::NumVal*>(evaluate(compEx->right, env).get());

    bool result;

    if (compEx->op == "<") result = left->value < right->value; else
    if (compEx->op == ">") result = left->value > right->value; else
    if (compEx->op == "==") result = left->value == right->value; else
    if (compEx->op == ">=") result = left->value >= right->value; else
    if (compEx->op == "<=") result = left->value <= right->value;


    return utils::MK_BOOL(result);
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_member_expr(AST::MemberExpr* member, Environment* env) {
    auto objectVal = evaluate(member->object, env);

    auto propertyIdent = static_cast<AST::Identifier*>(member->property);
    if (!propertyIdent) {
        throw std::runtime_error("Interpreter: Property in member expression is not an identifier.");
    }
    
    auto propertyName = propertyIdent->symbol;

    if (objectVal->type == values::ValueType::Object) {
        auto object = static_cast<values::ObjectVal*>(objectVal.get());

        auto it = object->properties.find(propertyName);
        if (it != object->properties.end()) {
            delete object;
            return std::move(it->second);
        } else {
            throw std::runtime_error(fmt::format("Property '{}' does not exist on the object.", propertyName));
        }
    }

    throw std::runtime_error("Interpreter: Attempted to access a member on a non-object type.");
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_string(AST::StringLiteral* string, Environment* env) {
    auto stringVal = std::make_unique<values::StringVal>();
    stringVal->value = string->value;
    return stringVal;
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate_while_statement(AST::WhileStmt* whilestmt, Environment* env) {
    auto lastEvaluated =std::make_unique<values::RuntimeVal>();

    while (true) {
        bool condition = static_cast<values::BoolVal*>(evaluate(whilestmt->condition, env).get())->value;

        if (!condition) break;
        try {
            for (auto& stmt : whilestmt->body) {
                lastEvaluated = evaluate(stmt, env);
            }
        } catch (const utils::Break&) {
            break;
        }

    }

    return lastEvaluated;
}

std::unique_ptr<values::RuntimeVal> interpreter::evaluate(AST::Stmt* astNode, Environment* env) {
    switch (astNode->kind) {
        case AST::NodeType::NumericLiteral: {
            auto value = std::make_unique<values::NumVal>();
            value->value = static_cast<AST::NumericLiteral*>(astNode)->value;
            return value;
        }
        case AST::NodeType::Identifier: {
            return evaluate_identifier(static_cast<AST::Identifier*>(astNode), env);
        }
        case AST::NodeType::BinaryExpr: {
            return evaluate_binary_expr(static_cast<AST::BinEx*>(astNode), env);
        }
        case AST::NodeType::Program: {
            return evaluate_program(static_cast<AST::Program*>(astNode), env);
        }
        case AST::NodeType::VarDeclare: {
            return evaluate_var_declaration(static_cast<AST::VarDeclare*>(astNode), env);
        }
        case AST::NodeType::AssignmentExpr: {
            return evaluate_assignment(static_cast<AST::AssignExpr*>(astNode), env);
        }
        case AST::NodeType::ObjectLiteral: {
            return evaluate_object_expr(static_cast<AST::ObjectLiteral*>(astNode), env);
        }
        case AST::NodeType::CallExpr: {
            return evaluate_call_expr(static_cast<AST::CallExpr*>(astNode), env);
        }
        case AST::NodeType::FunctionDeclaration: {
            return evaluate_fun_declaration(static_cast<AST::FunDeclare*>(astNode), env);
        }
        case AST::NodeType::If: {
            return evaluate_if_statement(static_cast<AST::IfStmt*>(astNode), env);
        }
        case AST::NodeType::CompExpr: {
            return evaluate_comparison_expr(static_cast<AST::CompEx*>(astNode), env);
        }
        case AST::NodeType::MemberExpr: {
            return evaluate_member_expr(static_cast<AST::MemberExpr*>(astNode), env);
        }
        case AST::NodeType::StringLiteral: {
            return evaluate_string(static_cast<AST::StringLiteral*>(astNode), env);
        }
        case AST::NodeType::While: {
            return evaluate_while_statement(static_cast<AST::WhileStmt*>(astNode), env);
        }
        case AST::NodeType::BreakStmt: {
            throw utils::Break();
        }
        default: {
            std::cout << "Interpreter: This AST has not been yet setup for interpretation." << std::endl; // message mainly for things that i havent implemented in the interpreter yet.
            exit(1);
        }
    }
}