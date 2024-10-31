#include <iostream>
#include <fstream>
#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"
#include "runtime/interpreter.hpp"
#include "runtime/values.hpp"
#include "runtime/environment.hpp"
#include <rift.hpp>
#include <fmt/core.h>

using namespace frontend;

std::string constructProgramStr(frontend::AST::Program* program) {
    std::string body;
    for (auto& stmt : program->body) {
        switch (stmt->kind) {
            case AST::NodeType::NumericLiteral: {
                auto num = dynamic_cast<AST::NumericLiteral*>(stmt);
                body += rift::format("      [kind: {kind}, value: {value}]\n", {
                    {"kind", rift::Value::from(static_cast<int>(stmt->kind))},
                    {"value", rift::Value::from(num->value)}
                });
                break;
            }
            case AST::NodeType::Identifier: {
                auto num = dynamic_cast<AST::Identifier*>(stmt);
                body += rift::format("      [kind: {kind}, value: {value}]\n", {
                    {"kind", rift::Value::from(static_cast<int>(stmt->kind))},
                    {"value", rift::Value::from(num->symbol)}
                });
                break;
            }
            case AST::NodeType::BinaryExpr: {
                auto num = dynamic_cast<AST::BinEx*>(stmt);
                body += rift::format("      [kind: {kind}, [\n          left: {left}\n          right:  {right}\n           operator: {operator}]\n]\n", {
                    {"kind", rift::Value::from(static_cast<int>(stmt->kind))},
                    {"left", rift::Value::from(dynamic_cast<AST::NumericLiteral*>(num->left)->value)},
                    {"right", rift::Value::from(dynamic_cast<AST::NumericLiteral*>(num->right)->value)},
                    {"operator", rift::Value::from(num->op)}
                });
                break;
            }
        }
    }

    return body;
}

std::string createThing(const std::string& str) {
    std::string thing;
    bool first = true;
    for (char c : str) {
        if (!first) {
            thing += ", ";
        }
        thing += std::to_string(static_cast<int>(c));
        first = false;
    }
    return thing;
}

int main(int argc, const char* argv[]) {
    auto lex = new frontend::Lexer();
    if (argc < 2) {
        std::cout << "Missing argument: <yhs file>" << std::endl;
        return 1;
    }
    auto source = utils::readFile(argv[1]);

    auto parser = new frontend::Parser();


    auto env = runtime::Environment::setupEnv();

    auto interpreter = new runtime::interpreter();
    try {
        ///*
        auto program = parser->produceAST(source);
        auto evaluated = interpreter->evaluate(program, env);
        //*/
        /*
        auto lexer = new Lexer();
        auto tokens = lexer->tokenize(source->contents);
        for (auto& token : tokens) {
            fmt::print("{}", rift::format("type: {type}, value: {value}\ndata values: {data_values}\n\n--------------\n", {
                {"type", rift::Value::from(static_cast<int>(token->type))},
                {"value", rift::Value::from(token->value)},
                {"data_values", rift::Value::from(createThing(token->value))}
            }));
        }
        */
        // since when can you comment multiline comments????

    } catch (std::invalid_argument& e) {
        fmt::print("{}", e.what());
        return 1;
    } catch (std::runtime_error& e) {
        fmt::print("{}", e.what());
        return 1;
    } catch (...) {
        fmt::print("An unknown error has ocurred.");
        return 1;
    }

    return 0;
}