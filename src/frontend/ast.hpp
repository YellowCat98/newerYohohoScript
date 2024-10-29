#pragma once
#include <optional>

namespace frontend {
    class AST {
        public:
        enum class NodeType {
            Program, // 0
            NumericLiteral, // 1
            Identifier, // 2
            BinaryExpr, // 3
            VarDeclare, // 4
            AssignmentExpr, // 5
            Property, // 6
            ObjectLiteral, // 7
            MemberExpr, // 8
            CallExpr, // 9
            FunctionDeclaration, // 10
            If,
            Else
        };

        struct Stmt {
            Stmt() {}
            virtual ~Stmt() = default;
            NodeType kind;
        };

        struct Program : public Stmt {
            Program() {
                this->kind = NodeType::Program;
            }
            std::deque<Stmt*> body;
        };

        struct Expr : public Stmt {Expr(){}};

        struct VarDeclare : public Stmt {
            VarDeclare() {
                this->kind = NodeType::VarDeclare;
            }

            bool constant;
            std::string identifier;
            std::optional<Expr*> value;
        };

        
        struct BinEx : public Expr {
            BinEx() {
                this->kind = NodeType::BinaryExpr;
            }
            Expr* left;
            Expr* right;
            std::string op;
        };

        struct Identifier : public Expr {
            Identifier() {
                this->kind = NodeType::Identifier;
            }

            std::string symbol;
        };

        struct NumericLiteral : public Expr {
            NumericLiteral() {
                this->kind = NodeType::NumericLiteral;
            }

            int value;
        };

        struct AssignExpr : public Expr {
            AssignExpr() {
                kind = NodeType::AssignmentExpr;
            }
            Expr* assigne;
            Expr* value;
        };

        struct Property : public Expr {
            Property() {
                this->kind = NodeType::Property;
            }

            std::string key;
            std::optional<Expr*> value;
        };
    
        struct ObjectLiteral : public Expr {
            ObjectLiteral() {
                this->kind = NodeType::ObjectLiteral;
            }

            std::deque<Property*> properties;
        };

        struct CallExpr : public Expr {
            CallExpr() {
                this->kind = NodeType::CallExpr;
            }
            std::deque<Expr*> args;
            Expr* caller;
            std::string op;
        };

        struct MemberExpr : public Expr {
            MemberExpr() {
                this->kind = NodeType::MemberExpr;
            }
            Expr* object;
            Expr* property;
            bool computed;
        };

        struct FunDeclare : public Stmt {
            FunDeclare() {
                this->kind = NodeType::FunctionDeclaration;
            }

            std::deque<std::string> parameters;
            std::string name;
            std::deque<Stmt*> body;
        };

        struct ElseStmt : public Stmt {
            ElseStmt() {
                this->kind = NodeType::Else;
            }

            bool multiline;
            std::deque<AST::Stmt*> body;
        };

        struct IfStmt : public Stmt {
            IfStmt() {
                this->kind = NodeType::If;
            }

            AST::Expr* condition;
            bool multiline;
            std::deque<AST::Stmt*> body;
            std::optional<AST::ElseStmt*> elseStmt;
        };
    };
}