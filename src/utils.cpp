#include <string>
#include <cctype>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fmt/core.h>
#include "utils.hpp"

namespace utils {
    bool isAlpha(const std::string& str) {
        for (char ch : str) {
            if (!std::isalpha(static_cast<unsigned char>(ch))) {
                return false;
            }
        }
        return true;
    }

    bool isInt(const std::string& str) {
        try {
            auto num = std::stoi(str); // useless value!
            return true;
        } catch (...) {
            return false;
        }
    }

    bool isSkippable(const std::string& str) {
        return str == "" || str == " " || str == "\n" || str == "\r" || str == "\t";
    }

    File* readFile(const std::string& filePath) {
        std::string fileName = std::filesystem::path(filePath).filename().string();
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return new File("", fileName);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return new File(buffer.str(), fileName);
    }

    runtime::values::NumVal* MK_NUM(int value) {
        auto return_val = new runtime::values::NumVal();
        return_val->value = value;
        return return_val;
    }

    runtime::values::NullVal* MK_NULL() {
        return new runtime::values::NullVal();
    }
    
    runtime::values::NativeFnValue* MK_NATIVE_FN(runtime::values::FunctionCall call) {
        auto return_val = new runtime::values::NativeFnValue();
        return_val->call = call;
        return return_val;
    }

    runtime::values::BoolVal* MK_BOOL(bool value) {
        auto return_val = new runtime::values::BoolVal();
        return_val->value = value;
        return return_val;
    }
}