#include <string>
#include <cctype>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <fmt/core.h>
#include "utils.hpp"

using namespace runtime;

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

    std::unique_ptr<values::NumVal> MK_NUM(int value) {
        auto return_val = std::make_unique<values::NumVal>();
        return_val->value = value;
        return return_val;
    }

    std::unique_ptr<values::NullVal> MK_NULL() {
        return std::make_unique<values::NullVal>();
    }

    std::unique_ptr<values::NativeFnValue> MK_NATIVE_FN(values::FunctionCall call) {
        auto return_val = std::make_unique<values::NativeFnValue>();
        return_val->call = call;
        return return_val;
    }

    std::unique_ptr<values::BoolVal> MK_BOOL(bool value) {
        auto return_val = std::make_unique<values::BoolVal>();
        return_val->value = value;
        return return_val;
    }

    std::unique_ptr<values::StringVal> MK_STRING(const std::string& value) {
        auto return_val = std::make_unique<values::StringVal>();
        return_val->value = value;
        return return_val;
    }
}