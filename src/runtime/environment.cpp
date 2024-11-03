#include "environment.hpp"
#include "../utils.hpp"
#include <iostream>

using namespace runtime;

Environment* Environment::setupEnv() {
    auto env = new Environment(nullptr);
    env->declareVar("null", utils::MK_NULL(), true);
    env->declareVar("true", utils::MK_BOOL(true), true);
    env->declareVar("false", utils::MK_BOOL(false), true);

    env->declareVar("print", utils::MK_NATIVE_FN([](std::deque<std::unique_ptr<values::RuntimeVal>> args, Environment* scope) -> std::unique_ptr<values::RuntimeVal> {
        for (auto& arg : args) {
            if (arg->type == values::ValueType::Number) {
                std::cout << dynamic_cast<values::NumVal*>(arg.get())->value;
            } else if (arg->type == values::ValueType::Boolean) {
                auto boolthing = dynamic_cast<values::BoolVal*>(arg.get())->value;
                if (boolthing) {
                    std::cout << "true";
                } else {
                    std::cout << "false";
                }
            } else if (arg->type == values::ValueType::String) {
                std::cout << dynamic_cast<values::StringVal*>(arg.get())->value;
            }
        }

        return std::make_unique<values::RuntimeVal>();
    }), true);

    env->declareVar("throw", utils::MK_NATIVE_FN([](std::deque<std::unique_ptr<values::RuntimeVal>> args, Environment* scope) -> std::unique_ptr<values::RuntimeVal> {
        throw std::invalid_argument(std::to_string(dynamic_cast<values::NumVal*>(args[0].get())->value));
    }), true);

    env->declareVar("input", utils::MK_NATIVE_FN([](std::deque<std::unique_ptr<values::RuntimeVal>> args, Environment* scope) -> std::unique_ptr<values::RuntimeVal> {
        std::string input;
        for (auto& arg : args) {
            if (arg->type == values::ValueType::Number) {
                std::cout << dynamic_cast<values::NumVal*>(arg.get())->value;
            } else if (arg->type == values::ValueType::Boolean) {
                auto boolthing = dynamic_cast<values::BoolVal*>(arg.get())->value;
                if (boolthing) {
                    std::cout << "true";
                } else {
                    std::cout << "false";
                }
            } else if (arg->type == values::ValueType::String) {
                std::cout << dynamic_cast<values::StringVal*>(arg.get())->value;
            }
        }

        std::getline(std::cin, input);
        return utils::MK_STRING(input);

    }), true);

    return env;
}

std::unique_ptr<values::RuntimeVal> Environment::declareVar(const std::string& name, std::unique_ptr<values::RuntimeVal> value, bool constant) {
    if (variables.find(name) != variables.end()) {
        throw std::invalid_argument(fmt::format("Variable {} is already declared.", name));
    }

    variables.insert({name, std::move(value)});

    if (constant) {
        constants.insert(name);
    }
    return value;
}

std::unique_ptr<values::RuntimeVal> Environment::assignVar(const std::string& name, std::unique_ptr<values::RuntimeVal> value) {
    auto env = this->resolve(name);
    if (env->constants.find(name) != env->constants.end()) {
        throw std::runtime_error(fmt::format("Cannot reassign to {} as it is constant.", name));
    }
    env->variables[name] = std::move(value);
    return value;
}

std::unique_ptr<values::RuntimeVal> Environment::lookupVar(const std::string& name) {
    auto env = this->resolve(name);
    return std::move(env->variables[name]);
}

Environment* Environment::resolve(const std::string& name) {
    if (variables.find(name) != variables.end()) {
        return this;
    }

    if (parent == nullptr) {
        throw std::invalid_argument(fmt::format("Cannot resolve {} as it doesn't exist.", name));
    }

    return parent->resolve(name);
}