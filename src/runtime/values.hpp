#pragma once
#include <string>
#include <unordered_map>
#include <functional>

namespace runtime {
    class values {
    public:
        values() = delete;
        enum class ValueType {
            Null, // 0
            Number, // 1
            Boolean, // 2
            Object, // 3
            NativeFn // 4
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

            std::string value = "";
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

            std::unordered_map<std::string, RuntimeVal*> properties;
        };

        using FunctionCall = std::function<values::RuntimeVal*(std::deque<values::RuntimeVal*>, runtime::Environment*)>;

        struct NativeFnValue : public RuntimeVal {
            NativeFnValue() {
                type = ValueType::NativeFn;
            }

            FunctionCall call;
        };
    };
}