#include "parser.hpp"

using namespace frontend;

bool Parser::notEOF() {
    return this->tokens[0]->type != Lexer::TokenType::EOF_;
}

Lexer::Token* Parser::at() {
    return this->tokens[0];
}

Lexer::Token* Parser::eat() {
    auto prev = tokens.front();
    tokens.pop_front();
    return prev;
}

Lexer::Token* Parser::expect(Lexer::TokenType type,  std::string err) {
    auto prev = tokens.front();
    tokens.pop_front();
    if (prev->type != type) {
        std::cout << "Parser Error: " << err << std::endl;
        exit(1);
    }

    return prev;
}

AST::Expr* Parser::parse_primary_expr() {
    auto tk = at()->type;

    switch (tk) {
        case Lexer::TokenType::Identifier: {
            auto ident = new AST::Identifier();
            ident->symbol = eat()->value;
            return ident;
        }
        case Lexer::TokenType::Int: {
            auto num = new AST::NumericLiteral();
            num->kind = AST::NodeType::NumericLiteral;
            num->value = std::stoi(eat()->value);
            return num;
        }
        case Lexer::TokenType::OpenParen: {
            eat();
            auto value = this->parse_expr();
            expect(Lexer::TokenType::CloseParen, "Unexpected Token found, expected: ");
            return value;
        }
        default: {
            std::cout << "Parser: unexpected token found: " << at()->value << std::endl;
            exit(1);
        }
    }
}

AST::Expr* Parser::parse_additive_expr() {
    auto left = this->parse_multiplicative_expr();

    while (at()->value == "+" || at()->value == "-") {
        auto op = eat()->value;
        auto right = this->parse_multiplicative_expr();
        auto binop = new AST::BinEx();
        binop->left = left;
        binop->right = right;
        binop->op = op;
        left = binop;
    }

    return left;
}

AST::Expr* Parser::parse_call_member_expr() {
    auto member = this->parse_member_expr();

    if (at()->type == Lexer::TokenType::OpenParen) {
        return this->parse_call_expr(member);
    }

    return member;
}

AST::Expr* Parser::parse_call_expr(AST::Expr* caller) {
    auto call_expr = new AST::Expr();
    call_expr = new AST::CallExpr();
    dynamic_cast<AST::CallExpr*>(call_expr)->caller = caller;
    dynamic_cast<AST::CallExpr*>(call_expr)->args = this->parse_args();

    if (at()->type == Lexer::TokenType::OpenParen) {
        call_expr = this->parse_call_expr(call_expr);
    }

    return call_expr;
}

std::deque<AST::Expr*> Parser::parse_args() {
    expect(Lexer::TokenType::OpenParen, "Expected open parenthesis.");
    std::deque<AST::Expr*> args = parse_arguments_list();

    expect(Lexer::TokenType::CloseParen, "Missing closing parenthisis.");

    return args;
}

std::deque<AST::Expr*> Parser::parse_arguments_list() {
    std::deque<AST::Expr*> args;

    while (notEOF() && at()->type == Lexer::TokenType::Comma && eat()) {
        args.push_back(parse_assignment_expr());
    }

    return args;
}

AST::Expr* Parser::parse_member_expr() {
    auto object = parse_primary_expr();

    while (at()->type == Lexer::TokenType::Dot || at()->type == Lexer::TokenType::OpenBrack) {
        auto op = eat();

        AST::Expr* property;
        bool computed;

        if (op->type == Lexer::TokenType::Dot) {
            computed = false;
            property = parse_primary_expr();

            if (property->kind != AST::NodeType::Identifier) {
                throw std::invalid_argument("RHS is not an identifier.");
            }
        } else {
            computed = true;
            property = parse_expr();
            expect(Lexer::TokenType::CloseBrack, "Missing closing bracket.");
        }

        object = new AST::MemberExpr();
        dynamic_cast<AST::MemberExpr*>(object)->object = object;
        dynamic_cast<AST::MemberExpr*>(object)->property = property;
        dynamic_cast<AST::MemberExpr*>(object)->computed = computed;
    }

    return object;
}

AST::Expr* Parser::parse_multiplicative_expr() {
    auto left = this->parse_call_member_expr();

    while (at()->value == "/" || at()->value == "*" || at()->value == "%") {
        auto op = eat()->value;
        auto right = this->parse_call_member_expr();
        auto binop = new AST::BinEx();
        binop->left = left;
        binop->right = right;
        binop->op = op;
        left = binop;

        
    }

    return left;
}

AST::Expr* Parser::parse_assignment_expr() {
    auto left = this->parse_object_expr();

    if (at()->type == Lexer::TokenType::Equals) {
        eat();
        auto value = this->parse_assignment_expr();
        auto return_val = new AST::AssignExpr();
        return_val->value = value;
        return_val->assigne = left;
        return return_val;
    }

    return left;
}

AST::Expr* Parser::parse_object_expr() {
    if (at()->type != Lexer::TokenType::OpenBrace) {
        return this->parse_additive_expr();
    }

    eat();
    std::deque<AST::Property*> properties;

    while (notEOF() && at()->type != Lexer::TokenType::CloseBrace) {
        auto key = this->expect(Lexer::TokenType::Identifier, "Expected identifier for object literal.")->value;
        if (at()->type == Lexer::TokenType::Comma) {
            eat();
            auto property = new AST::Property();
            property->key = key;
            properties.push_back(property);
            continue;
        } else if (at()->type == Lexer::TokenType::CloseBrace) {
            auto property = new AST::Property();
            property->key = key;
            properties.push_back(property);
            continue;
        }

        expect(Lexer::TokenType::Colon, "Missing colon following identifier in ObjectExpr");
        auto value = this->parse_expr();

        auto property = new AST::Property();

        property->value = value;
        property->key = key;
        properties.push_back(property);
        if (at()->type != Lexer::TokenType::CloseBrace) {
            expect(Lexer::TokenType::Comma, "Expected comma following property.");
        }
    }

    expect(Lexer::TokenType::CloseBrace, "Object literal missing closing Braces.");
    auto return_val = new AST::ObjectLiteral();
    return_val->properties = properties;
    return return_val;
}

AST::Expr* Parser::parse_expr() {
    return this->parse_assignment_expr();
}

AST::Stmt* Parser::parse_var_declaration() {
    auto isConstant = eat()->type == Lexer::TokenType::Const;
    auto identifier = expect(Lexer::TokenType::Identifier, "Expected identifier following var | const")->value;

    if (at()->type == Lexer::TokenType::Semicolon) {
        eat();
        if (isConstant)
            throw std::invalid_argument("No value given for const expression.");
        
        return new AST::VarDeclare();
    }

    expect(Lexer::TokenType::Equals, "Expected Equals following identifier.");
    auto declaration = new AST::VarDeclare();
    declaration->value = this->parse_expr();
    declaration->identifier = identifier;
    declaration->constant = isConstant;

    expect(Lexer::TokenType::Semicolon, "Expected ';' for variable declaration.");

    return declaration;
}

AST::Stmt* Parser::parse_stmt() {
    switch (at()->type) {
        case Lexer::TokenType::Var: {
            return this->parse_var_declaration();
        }
        case Lexer::TokenType::Const: {
            return this->parse_var_declaration();
        }
        default: {
            return this->parse_expr();
        }
    }
}

AST::Program* Parser::produceAST(std::string const& sourceCode) {
    this->tokens = lexer->tokenize(sourceCode);
    auto program = new AST::Program();

    while (notEOF()) {
        program->body.push_back(this->parse_stmt());
    }

    return program;
}