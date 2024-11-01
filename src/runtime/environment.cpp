#include "environment.hpp"
#include "../utils.hpp"
#include <iostream>

using namespace runtime;

Environment* Environment::setupEnv() {
    auto env = new Environment(nullptr);
    env->declareVar("null", utils::MK_NULL(), true);
    env->declareVar("true", utils::MK_BOOL(true), true);
    env->declareVar("false", utils::MK_BOOL(false), true);

    env->declareVar("print", utils::MK_NATIVE_FN([](std::deque<values::RuntimeVal*> args, Environment* scope) -> values::RuntimeVal* {
        for (auto& arg : args) {
            if (arg->type == values::ValueType::Number) {
                std::cout << dynamic_cast<values::NumVal*>(arg)->value;
            } else if (arg->type == values::ValueType::Boolean) {
                auto boolthing = dynamic_cast<values::BoolVal*>(arg)->value;
                if (boolthing) {
                    std::cout << "true";
                } else {
                    std::cout << "false";
                }
            } else if (arg->type == values::ValueType::String) {
                std::cout << dynamic_cast<values::StringVal*>(arg)->value;
            }
        }

        return new values::RuntimeVal();
    }), true);

    env->declareVar("free", utils::MK_NATIVE_FN([](std::deque<values::RuntimeVal*> args, Environment* scope) -> values::RuntimeVal* {
        for (auto& arg : args) {
            delete arg;
        }
        return new values::RuntimeVal();
    }), true);

    env->declareVar("throw", utils::MK_NATIVE_FN([](std::deque<values::RuntimeVal*> args, Environment* scope) -> values::RuntimeVal* {
        throw std::invalid_argument(std::to_string(dynamic_cast<values::NumVal*>(args[0])->value));
        return new values::RuntimeVal();
    }), true);

    env->declareVar("input", utils::MK_NATIVE_FN([](std::deque<values::RuntimeVal*> args, Environment* scope) -> values::RuntimeVal* {
        std::string input;
        for (auto& arg : args) {
            if (arg->type == values::ValueType::Number) {
                std::cout << dynamic_cast<values::NumVal*>(arg)->value;
            } else if (arg->type == values::ValueType::Boolean) {
                auto boolthing = dynamic_cast<values::BoolVal*>(arg)->value;
                if (boolthing) {
                    std::cout << "true";
                } else {
                    std::cout << "false";
                }
            } else if (arg->type == values::ValueType::String) {
                std::cout << dynamic_cast<values::StringVal*>(arg)->value;
            }
        }
        std::getline(std::cin, input);
        return utils::MK_STRING(input);

    }), true);

    return env;
}

values::RuntimeVal* Environment::declareVar(const std::string& name, values::RuntimeVal* value, bool constant) {
    if (variables.find(name) != variables.end()) {
        throw std::invalid_argument(fmt::format("Variable {} is already declared.", name));
    }

    variables.insert({name, value});

    if (constant) {
        constants.insert(name);
    }
    return value;
}

values::RuntimeVal* Environment::assignVar(const std::string& name, values::RuntimeVal* value) {
    auto env = this->resolve(name);
    if (env->constants.find(name) != env->constants.end()) {
        throw std::runtime_error(fmt::format("Cannot reassign to {} as it is constant.", name));
    }
    env->variables[name] = value;
    return value;
}

values::RuntimeVal* Environment::lookupVar(const std::string& name) {
    auto env = this->resolve(name);
    return env->variables[name];
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