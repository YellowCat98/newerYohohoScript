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

    // we are parsing strings within the lexer, but then later storing it in the AST.

    auto handleEscapeSequence = [&src]() -> std::string {
        src.pop_front();
        if (src.empty()) return "";

        char escapedChar = src.front()[0];
        src.pop_front();

        switch (escapedChar) {
            case 'n': return "\n";
            case 't': return "\t";
            case '\"': return "\"";
            case '\\': return "\\";
            case '\'': return "\'";
            case 'b': return "\b";
            case 'f': return "\f";
            case 'r': return "\r";
            case 'v': return "\v";
            case 'a': return "\a";
            case '0': return "\0";

            default: return std::string(1, escapedChar);
        }
    };

    auto parseStringLiteral = [&src, &handleEscapeSequence]() -> std::string {
        std::string strLiteral;
        src.pop_front();

        while (!src.empty() && src[0] != "\"") {
            if (src[0] == "\\") {
                strLiteral += handleEscapeSequence();
            } else {
                strLiteral += src[0];
                src.pop_front();
            }
        }

        if (!src.empty() && src[0] == "\"") {
            src.pop_front();
        } else {
            throw std::invalid_argument("Lexer: Unterminated string literal.");
        }

        return strLiteral;
    };

    int line = 1;

    while (src.size() > 0) {
        skipComments();

        if (src.empty()) break;

        if (src[0] == "\"") {
            tokens.push_back(token(parseStringLiteral(), line, TokenType::String));
        }
        
        if (src[0] == "(") {
            tokens.push_back(token(src.front(), line, TokenType::OpenParen));
            src.pop_front();
        } else if (src[0] == ")") {
            tokens.push_back(token(src.front(), line, TokenType::CloseParen));
            src.pop_front();
        }
        
        else if (src[0] == "{") {
            tokens.push_back(token(src.front(), line, TokenType::OpenBrace));
            src.pop_front();
        } else if (src[0] == "}") {
            tokens.push_back(token(src.front(), line, TokenType::CloseBrace));
            src.pop_front();
        }

        else if (src[0] == "[") {
            tokens.push_back(token(src.front(), line, TokenType::OpenBrack));
            src.pop_front();
        } else if (src[0] == "]") {
            tokens.push_back(token(src.front(), line, TokenType::OpenBrack));
            src.pop_front();
        }

        else if (src[0] == "+" || src[0] == "-" || src[0] == "*" || src[0] == "/" || src[0] == "%") {
            tokens.push_back(token(src.front(), line, TokenType::BinOp));
            src.pop_front();
        } else if (src[0] == "=") {
            if (src[1] == "=") {
                tokens.push_back(token(src[0] + src[1], line, TokenType::ComparisonOp));
                src.pop_front();
                src.pop_front();
            } else {
                tokens.push_back(token(src.front(), line, TokenType::Equals));
                src.pop_front();
            }
        } else if (src[0] == ";") {
            tokens.push_back(token(src.front(), line, TokenType::Semicolon));
            src.pop_front();
        } else if (src[0] == ",") {
            tokens.push_back(token(src.front(), line, TokenType::Comma));
            src.pop_front();
        } else if (src[0] == ":") {
            tokens.push_back(token(src.front(), line, TokenType::Colon));
            src.pop_front();
        } else if (src[0] == ".") {
            tokens.push_back(token(src.front(), line, TokenType::Dot));
            src.pop_front();
        } else if (src[0] == ">" || src[0] == "<") {
            if (src[1] == "=") {
                tokens.push_back(token(src[0] + src[1], line, TokenType::ComparisonOp));
                src.pop_front();
                src.pop_front();
            } else {
                tokens.push_back(token(src[0], line, TokenType::ComparisonOp));
                src.pop_front();
            }
        } else {
            if (utils::isInt(src[0])) {
                std::string num;
                while (src.size() > 0 && utils::isInt(src[0])) {
                    num += src.front();
                    src.pop_front();
                }

                tokens.push_back(token(num, line, TokenType::Int));
            } else if (utils::isAlpha(src[0])) {
                std::string ident;
                while (src.size() > 0 && utils::isAlpha(src[0])) {
                    ident += src.front();
                    src.pop_front();
                }
                if (RESERVED.find(ident) == RESERVED.end()) {
                    tokens.push_back(token(ident, line, TokenType::Identifier));
                } else {
                    tokens.push_back(token(ident, line, RESERVED[ident]));
                }
            } else if (src[0] == "\n") {
                line += 1;
                src.pop_front();
            } else if (utils::isSkippable(src[0])) {
                src.pop_front();
            } else {
                std::cout << "Lexer: unrecognized token found: " << src[0];
                exit(1);
            }
        }
    }

    tokens.push_back(token("EOF", line, Lexer::TokenType::EOF_));
    
    return tokens;
}