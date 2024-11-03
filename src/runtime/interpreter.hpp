#pragma once
#include <memory> // Include for std::unique_ptr
#include "values.hpp"
#include "../frontend/ast.hpp"
#include "environment.hpp"

namespace runtime {
    class interpreter {
    private:
        std::unique_ptr<values::RuntimeVal> evaluate_binary_expr(frontend::AST::BinEx* binop, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_program(frontend::AST::Program* program, Environment* env);
        std::unique_ptr<values::NumVal> evaluate_numeric_binary_expr(std::unique_ptr<values::NumVal> lhs, std::unique_ptr<values::NumVal> rhs, const std::string& op);
        std::unique_ptr<values::RuntimeVal> evaluate_var_declaration(frontend::AST::VarDeclare* declaration, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_assignment(frontend::AST::AssignExpr* node, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_identifier(frontend::AST::Identifier* ident, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_object_expr(frontend::AST::ObjectLiteral* obj, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_call_expr(frontend::AST::CallExpr* expr, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_fun_declaration(frontend::AST::FunDeclare* declaration, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_if_statement(frontend::AST::IfStmt* ifstmt, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_comparison_expr(frontend::AST::CompEx* compEx, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_member_expr(frontend::AST::MemberExpr* member, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_string(frontend::AST::StringLiteral* string, Environment* env);
        std::unique_ptr<values::RuntimeVal> evaluate_while_statement(frontend::AST::WhileStmt* whilestmt, Environment* env);
    public:
        interpreter() {}
        std::unique_ptr<values::RuntimeVal> evaluate(frontend::AST::Stmt* astNode, Environment* env);
    };
}
