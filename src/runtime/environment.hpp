#pragma once
#include "values.hpp"
#include "../utils.hpp"
#include <unordered_map>
#include <stdexcept>
#include <set>
#include <fmt/core.h>
#include <deque>

namespace runtime {
    class Environment {
    private:
        Environment* parent;
        std::unordered_map<std::string, values::RuntimeVal*> variables;
        std::set<std::string> constants;
    public:
        Environment(Environment* parent) : parent(parent) {
            bool global = this->parent ? true : false;
        }
        values::RuntimeVal* declareVar(const std::string& name, values::RuntimeVal* value, bool constant);
        values::RuntimeVal* assignVar(const std::string& name, values::RuntimeVal* value);
        values::RuntimeVal* lookupVar(const std::string& name);
        Environment* resolve(const std::string& name);

        static Environment* setupEnv();
    };
}