#pragma once
#include "values.hpp"
#include <unordered_map>
#include <stdexcept>
#include <set>
#include <fmt/core.h>
#include <deque>

namespace runtime {
    class Environment {
    private:
        Environment* parent;
        std::unordered_map<std::string, std::unique_ptr<values::RuntimeVal>> variables;
        std::set<std::string> constants;
    public:
        Environment(Environment* parent) : parent(parent) {
            bool global = this->parent ? true : false;
        }
        std::unique_ptr<values::RuntimeVal> declareVar(const std::string& name, std::unique_ptr<values::RuntimeVal> value, bool constant);
        std::unique_ptr<values::RuntimeVal> assignVar(const std::string& name, std::unique_ptr<values::RuntimeVal> value);
        std::unique_ptr<values::RuntimeVal> lookupVar(const std::string& name);
        Environment* resolve(const std::string& name);

        static Environment* setupEnv();
    };
}