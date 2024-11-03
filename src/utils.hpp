#pragma once
#include "runtime/values.hpp"

namespace utils {
    bool isAlpha(const std::string& str);

    bool isInt(const std::string& str);

    bool isSkippable(const std::string& str);


    struct File {
        std::string contents;
        std::string name;
        File(const std::string& contents, const std::string& name) : contents(contents), name(name) {}
    };

    File* readFile(const std::string& filePath);

    std::unique_ptr<runtime::values::NumVal> MK_NUM(int value);

    std::unique_ptr<runtime::values::NullVal> MK_NULL();
    
    std::unique_ptr<runtime::values::NativeFnValue> MK_NATIVE_FN(runtime::values::FunctionCall call);

    std::unique_ptr<runtime::values::BoolVal> MK_BOOL(bool value);
    std::unique_ptr<runtime::values::StringVal> MK_STRING(const std::string& value);

    class Break : public std::exception {
    public:
        Break() {};
        const char* what() const noexcept override {
            return "";
        }
    };
}