#pragma once
#include <string>
#include <deque>
#include <algorithm>
#include <iostream>
#include "../utils.hpp"
#include <unordered_map>


namespace frontend {
    class Lexer {
    public:
        Lexer() {}

        enum class TokenType {
            Int, // 0
            Identifier, // 1
            Equals, // 2
            OpenParen, // 3
            CloseParen, // 4
            BinOp, // 5
            Var, // 6
            Const, // 7
            Semicolon, // 8
            Comma, // 9
            CloseBrace, // 10
            OpenBrace, // 11
            OpenBrack, // 12
            CloseBrack, // 13
            Colon, // 14
            Dot, // 15
            Null, // 16
            EOF_, // 17
        };
        struct Token {
            std::string value;
            TokenType type;
            Token(std::string value, TokenType type) : value(value), type(type) {}

        };

        std::deque<Token*> tokenize(const std::string& sourceCode);
    private:
        Token* token(std::string value, TokenType type) {
            return new Token(value, type);
        }

        std::unordered_map<std::string, Lexer::TokenType> RESERVED = {
            {"var", Lexer::TokenType::Var},
            {"const", Lexer::TokenType::Const}
        };
    };
}