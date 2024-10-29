#pragma once
#include "lexer.hpp"
#include "ast.hpp"
#include "../utils.hpp"
#include <deque>
#include "fmt/core.h"

namespace frontend {
    class Parser {
    private:
        Lexer* lexer;
        std::deque<Lexer::Token*> tokens;

        bool notEOF();
        AST::Stmt* parse_stmt();
        AST::Expr* parse_expr();
        AST::Expr* parse_primary_expr();
        AST::Expr* parse_additive_expr();
        AST::Expr* parse_multiplicative_expr();
        AST::Stmt* parse_var_declaration();
        AST::Expr* parse_assignment_expr();
        AST::Expr* parse_object_expr();
        AST::Expr* parse_call_member_expr();
        AST::Expr* parse_call_expr(AST::Expr* caller);
        std::deque<AST::Expr*> parse_args();
        std::deque<AST::Expr*> parse_arguments_list();
        AST::Expr* parse_member_expr();
        AST::Stmt* parse_fun_declaration();
        AST::Stmt* parse_if_condition();
        AST::Expr* parse_comparison_expr();
        Lexer::Token* eat();
        Lexer::Token* at();
        Lexer::Token* expect(Lexer::TokenType type, std::string err);
    public:
    Parser() {
        lexer = new Lexer();
    }
    AST::Program* produceAST(std::string const& source);

    };
}