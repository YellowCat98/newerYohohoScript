#pragma once
#include "runtime/values.hpp"

namespace utils {
    bool isAlpha(const std::string& str);

    bool isInt(const std::string& str);

    bool isSkippable(const std::string& str);

    std::string readFile(const std::string& filePath);

    runtime::values::NumVal* MK_NUM(int value);

    runtime::values::NullVal* MK_NULL();
    
    runtime::values::NativeFnValue* MK_NATIVE_FN(runtime::values::FunctionCall call);

    runtime::values::BoolVal* MK_BOOL(bool value);
}