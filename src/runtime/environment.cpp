#include "environment.hpp"
#include "../utils.hpp"

using namespace runtime;

Environment* Environment::setupEnv() {
    auto env = new Environment(nullptr);
    env->declareVar("null", utils::MK_NULL(), true);
    env->declareVar("true", utils::MK_BOOL(true), true);
    env->declareVar("false", utils::MK_BOOL(false), true);

    env->declareVar("print", utils::MK_NATIVE_FN([](std::deque<values::RuntimeVal*> args, Environment* scope) -> values::RuntimeVal* {
        return utils::MK_NULL();
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
        throw std::invalid_argument(fmt::format("Cannot reassign to {} as it is constant.", name));
    }
    env->variables.insert({name, value});
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