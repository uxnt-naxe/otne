#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "otne_ast.hpp"
#include "otne_token.hpp"
#include "otne_utf8.hpp"

namespace otne {

struct Function;
struct NativeFunction;
class Environment;

// 运行时值：null / bool / int / float / string / 函数 / 内建函数
using Value = std::variant<std::monostate, bool, std::int64_t, double, i18nString,
                           std::shared_ptr<Function>, std::shared_ptr<NativeFunction>>;

// 用户函数
struct Function {
    i18nString name;
    std::vector<Parameter> params;
    std::optional<TypeTag> return_type;
    const BlockStmt* body = nullptr;
    std::shared_ptr<Environment> closure;
};

// 内建函数：参数列表 + 调用点位置，便于报告运行时错误
struct NativeFunction {
    i18nString name;
    std::function<Value(std::vector<Value>&, int, int)> call;
};

// 词法作用域
class Environment
{
public:
    explicit Environment(std::shared_ptr<Environment> parent = nullptr);

    void Define(const i18nString& name, Value value, bool is_const);
    Value* Find(const i18nString& name);
    bool IsConst(const i18nString& name) const;

    const std::shared_ptr<Environment>& parent() const { return parent_; }

private:
    std::shared_ptr<Environment> parent_;
    std::unordered_map<i18nString, Value> values_;
    std::unordered_set<i18nString> constants_;
};

// 值的类型名（用于错误信息）
i18nString TypeName(const Value& value);

// 值的可读文本（用于 print / 字符串拼接）
i18nString ToDisplay(const Value& value);

bool IsNullValue(const Value& value);
bool IsNumberValue(const Value& value);
bool IsStringValue(const Value& value);
bool IsBoolValue(const Value& value);
bool IsFunctionValue(const Value& value);

// 真值判断：null / false / 0 / 0.0 / "" 为假
bool IsTruthy(const Value& value);

}  // namespace otne
