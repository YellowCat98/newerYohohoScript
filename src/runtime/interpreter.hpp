#pragma once
#include "values.hpp"
#include "../frontend/ast.hpp"
#include "environment.hpp"


namespace runtime {
    class interpreter {
    private:
        values::RuntimeVal* evaluate_binary_expr(frontend::AST::BinEx* binop, Environment* env);
        values::RuntimeVal* evaluate_program(frontend::AST::Program* program, Environment* env);
        values::NumVal* evaluate_numeric_binary_expr(values::NumVal* lhs, values::NumVal* rhs, const std::string& op);
        values::RuntimeVal* evaluate_var_declaration(frontend::AST::VarDeclare* declaration, Environment* env);
        values::RuntimeVal* evaluate_assignment(frontend::AST::AssignExpr* node, Environment* env);
        values::RuntimeVal* evaluate_identifier(frontend::AST::Identifier* ident, Environment* env);
        values::RuntimeVal* evaluate_object_expr(frontend::AST::ObjectLiteral* obj, Environment* env);
        values::RuntimeVal* evaluate_call_expr(frontend::AST::CallExpr* expr, Environment* env);
        values::RuntimeVal* evaluate_fun_declaration(frontend::AST::FunDeclare* declaration, Environment* env);
        values::RuntimeVal* evaluate_if_statement(frontend::AST::IfStmt* ifstmt, Environment* env);
    public:
        interpreter() {}
        values::RuntimeVal* evaluate(frontend::AST::Stmt* astNode, Environment* env);
    };
}