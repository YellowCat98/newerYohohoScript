#include "lexer.hpp"

using namespace frontend;

std::deque<Lexer::Token*> Lexer::tokenize(const std::string& sourceCode) {
    std::deque<Token*> tokens;
    std::deque<std::string> src;

    std::transform(sourceCode.begin(), sourceCode.end(), std::back_inserter(src),
                    [](char ch) { return std::string(1, ch); });

    auto skipComments = [&src]() {
        while (!src.empty()) {
            if (src.size() >= 2 && src[0] == "/" && src[1] == "/") {
                while (!src.empty() && src[0] != "\n") {
                    src.pop_front();
                }
                if (!src.empty()) {
                    src.pop_front();
                }
            } else if (src.size() >= 2 && src[0] == "/" && src[1] == "*") {
                src.pop_front();
                src.pop_front();
                while (src.size() >= 2 && !(src[0] == "*" && src[1] == "/")) {
                    src.pop_front();
                }
                if (src.size() >= 2) {
                    src.pop_front();
                    src.pop_front();
                }
            } else {
                break;
            }
        }
    };

    while (src.size() > 0) {
        skipComments();

        if (src.empty()) break;
        
        if (src[0] == "(") {
            tokens.push_back(token(src.front(), TokenType::OpenParen));
            src.pop_front();
        } else if (src[0] == ")") {
            tokens.push_back(token(src.front(), TokenType::CloseParen));
            src.pop_front();
        }
        
        else if (src[0] == "{") {
            tokens.push_back(token(src.front(), TokenType::OpenBrace));
            src.pop_front();
        } else if (src[0] == "}") {
            tokens.push_back(token(src.front(), TokenType::CloseBrace));
            src.pop_front();
        }

        else if (src[0] == "[") {
            tokens.push_back(token(src.front(), TokenType::OpenBrack));
            src.pop_front();
        } else if (src[0] == "]") {
            tokens.push_back(token(src.front(), TokenType::OpenBrack));
            src.pop_front();
        }

        else if (src[0] == "+" || src[0] == "-" || src[0] == "*" || src[0] == "/" || src[0] == "%") {
            tokens.push_back(token(src.front(), TokenType::BinOp));
            src.pop_front();
        } else if (src[0] == "=") {
            if (src[1] == "=") {
                tokens.push_back(token(src[0] + src[1], TokenType::ComparisonOp));
                src.pop_front();
                src.pop_front();
            } else {
                tokens.push_back(token(src.front(), TokenType::Equals));
                src.pop_front();
            }
        } else if (src[0] == ";") {
            tokens.push_back(token(src.front(), TokenType::Semicolon));
            src.pop_front();
        } else if (src[0] == ",") {
            tokens.push_back(token(src.front(), TokenType::Comma));
            src.pop_front();
        } else if (src[0] == ":") {
            tokens.push_back(token(src.front(), TokenType::Colon));
            src.pop_front();
        } else if (src[0] == ".") {
            tokens.push_back(token(src.front(), TokenType::Dot));
            src.pop_front();
        } else if (src[0] == ">" || src[0] == "<") {
            tokens.push_back(token(src.front(), TokenType::ComparisonOp));
            src.pop_front();
        } else {
            if (utils::isInt(src[0])) {
                std::string num;
                while (src.size() > 0 && utils::isInt(src[0])) {
                    num += src.front();
                    src.pop_front();
                }

                tokens.push_back(token(num, TokenType::Int));
            } else if (utils::isAlpha(src[0])) {
                std::string ident;
                while (src.size() > 0 && utils::isAlpha(src[0])) {
                    ident += src.front();
                    src.pop_front();
                }
                if (RESERVED.find(ident) == RESERVED.end()) {
                    tokens.push_back(token(ident, TokenType::Identifier));
                } else {
                    tokens.push_back(token(ident, RESERVED[ident]));
                }
            } else if (utils::isSkippable(src[0])) {
                src.pop_front();
            } else {
                std::cout << "Lexer: unrecognized token found: " << src[0];
                exit(1);
            }
        }
    }

    tokens.push_back(token("EOF", Lexer::TokenType::EOF_));
    
    return tokens;
}