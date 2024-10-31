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
            Fun, // 16
            If, // 17
            Else, // 18
            ComparisonOp, // 19
            String, // 20
            While, // 21
            Break, // 22
            Null, // 23
            EOF_, // 24
        };
        struct Token {
            std::string value;
            TokenType type;
            int position;
            Token(std::string value, TokenType type, int position) : value(value), type(type), position(position) {}

        };

        std::deque<Token*> tokenize(const std::string& sourceCode);
    private:
        Token* token(std::string value, int position, TokenType type) {
            return new Token(value, type, position);
        }

        std::unordered_map<std::string, Lexer::TokenType> RESERVED = {
            {"var", Lexer::TokenType::Var},
            {"const", Lexer::TokenType::Const},
            {"fun", Lexer::TokenType::Fun},
            {"if", Lexer::TokenType::If},
            {"else", Lexer::TokenType::Else},
            {"while", Lexer::TokenType::While},
            {"break", Lexer::TokenType::Break}
        };
    };
}