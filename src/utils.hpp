#pragma once
#include <string>
#include <cctype>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "runtime/values.hpp"

namespace utils {
    inline bool isAlpha(const std::string& str) {
        for (char ch : str) {
            if (!std::isalpha(static_cast<unsigned char>(ch))) {
                return false;
            }
        }
        return true;
    }

    inline bool isInt(const std::string& str) {
        try {
            std::stoi(str);
            return true;
        } catch (...) {
            return false;
        }
    }

    inline bool isSkippable(const std::string& str) {
        return str == "" || str == " " || str == "\n" || str == "\r" || str == "\t";
    }

    inline std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    inline runtime::values::NumVal* MK_NUM(int value = 0) {
        auto return_val = new runtime::values::NumVal();
        return_val->value = value;
        return return_val;
    }

    inline runtime::values::NullVal* MK_NULL() {
        return new runtime::values::NullVal();
    }
    
    inline runtime::values::NativeFnValue* MK_NATIVE_FN(runtime::values::FunctionCall call) {
        auto return_val = new runtime::values::NativeFnValue();
        return_val->call = call;
        return return_val;
    }

    inline runtime::values::BoolVal* MK_BOOL(bool value) {
        auto return_val = new runtime::values::BoolVal();
        return_val->value = value;
        return return_val;
    }
}