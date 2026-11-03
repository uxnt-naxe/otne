#include "otne_value.hpp"

#include <sstream>
#include <utility>

namespace otne {

Environment::Environment(std::shared_ptr<Environment> parent) : parent_(std::move(parent)) {}

void Environment::Define(const i18nString& name, Value value, bool is_const)
{
    values_[name] = std::move(value);
    if (is_const) {
        constants_.insert(name);
    } else {
        constants_.erase(name);
    }
}

Value* Environment::Find(const i18nString& name)
{
    const auto it = values_.find(name);
    if (it != values_.end()) {
        return &it->second;
    }
    if (parent_) {
        return parent_->Find(name);
    }
    return nullptr;
}

bool Environment::IsConst(const i18nString& name) const
{
    if (constants_.count(name) != 0) {
        return true;
    }
    if (parent_) {
        return parent_->IsConst(name);
    }
    return false;
}

namespace {

struct DisplayVisitor {
    i18nString operator()(std::monostate) const { return L"null"; }

    i18nString operator()(bool value) const { return value ? L"true" : L"false"; }

    i18nString operator()(std::int64_t value) const { return std::to_wstring(value); }

    i18nString operator()(double value) const
    {
        std::wostringstream stream;
        stream << value;
        return stream.str();
    }

    i18nString operator()(const i18nString& value) const { return value; }

    i18nString operator()(const std::shared_ptr<Function>& value) const
    {
        return L"<fn " + (value ? value->name : i18nString()) + L">";
    }

    i18nString operator()(const std::shared_ptr<NativeFunction>& value) const
    {
        return L"<native " + (value ? value->name : i18nString()) + L">";
    }
};

}  // namespace

i18nString ToDisplay(const Value& value)
{
    return std::visit(DisplayVisitor{}, value);
}

i18nString TypeName(const Value& value)
{
    if (std::holds_alternative<std::monostate>(value)) {
        return L"null";
    }
    if (std::holds_alternative<bool>(value)) {
        return L"bool";
    }
    if (std::holds_alternative<std::int64_t>(value)) {
        return L"int";
    }
    if (std::holds_alternative<double>(value)) {
        return L"float";
    }
    if (std::holds_alternative<i18nString>(value)) {
        return L"string";
    }
    if (std::holds_alternative<std::shared_ptr<Function>>(value)) {
        return L"function";
    }
    return L"native";
}

bool IsNullValue(const Value& value)
{
    return std::holds_alternative<std::monostate>(value);
}

bool IsNumberValue(const Value& value)
{
    return std::holds_alternative<std::int64_t>(value) || std::holds_alternative<double>(value);
}

bool IsStringValue(const Value& value)
{
    return std::holds_alternative<i18nString>(value);
}

bool IsBoolValue(const Value& value)
{
    return std::holds_alternative<bool>(value);
}

bool IsFunctionValue(const Value& value)
{
    return std::holds_alternative<std::shared_ptr<Function>>(value) ||
           std::holds_alternative<std::shared_ptr<NativeFunction>>(value);
}

bool IsTruthy(const Value& value)
{
    if (const auto* flag = std::get_if<bool>(&value)) {
        return *flag;
    }
    if (const auto* number = std::get_if<std::int64_t>(&value)) {
        return *number != 0;
    }
    if (const auto* number = std::get_if<double>(&value)) {
        return *number != 0.0;
    }
    if (const auto* text = std::get_if<i18nString>(&value)) {
        return !text->empty();
    }
    return !IsNullValue(value);
}

}  // namespace otne
