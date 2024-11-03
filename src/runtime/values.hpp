#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <deque>
#include "../frontend/ast.hpp"

namespace runtime {
    class Environment;
    class values {
    public:
        values() = delete;
        enum class ValueType {
            Null, // 0
            Number, // 1
            Boolean, // 2
            Object, // 3
            NativeFn, // 4
            Function, // 5
            String, // 6
        };

        struct RuntimeVal {
            RuntimeVal() {}
            virtual ~RuntimeVal() = default;

            ValueType type;
        };

        struct NullVal : public RuntimeVal {
            NullVal() {
                type = ValueType::Null;
            }

            std::nullptr_t value = nullptr;
        };

        struct NumVal : public RuntimeVal {
            NumVal() {
                type = ValueType::Number;
            }

            int value;
        };

        struct BoolVal : public RuntimeVal {
            BoolVal() {
                type = ValueType::Boolean;
            }

            bool value;
        };
    
        struct ObjectVal : public RuntimeVal {
            ObjectVal() {
                type = ValueType::Object;
            }

            std::unordered_map<std::string, std::unique_ptr<RuntimeVal>> properties;
        };

        using FunctionCall = std::function<std::unique_ptr<values::RuntimeVal>(std::deque<std::unique_ptr<values::RuntimeVal>>, runtime::Environment*)>;

        struct NativeFnValue : public RuntimeVal {
            NativeFnValue() {
                type = ValueType::NativeFn;
            }

            FunctionCall call;
        };

        struct FunValue : public RuntimeVal { // undertale reference???
            FunValue() {
                type = ValueType::Function;
            }

            std::string name;
            std::deque<std::string> params;
            Environment* decEnv;
            std::deque<frontend::AST::Stmt*> body;
        };

        struct StringVal : public RuntimeVal {
            StringVal() {type = ValueType::String;}

            std::string value;
        };
    };
}