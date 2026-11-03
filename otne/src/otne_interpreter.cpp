#include "otne_interpreter.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <utility>

namespace otne {

namespace {

// 控制流信号：用异常实现 return / break / continue
struct ReturnSignal {
    Value value;
};
struct BreakSignal {};
struct ContinueSignal {};

[[noreturn]] void Fail(int line, int column, const i18nString& message)
{
    throw RuntimeError{Diagnostic{line, column, message}};
}

double ToDouble(const Value& value)
{
    if (const auto* integer = std::get_if<std::int64_t>(&value)) {
        return static_cast<double>(*integer);
    }
    return std::get<double>(value);
}

bool ValuesEqual(const Value& left, const Value& right)
{
    if (IsNumberValue(left) && IsNumberValue(right)) {
        return ToDouble(left) == ToDouble(right);
    }
    if (IsStringValue(left) && IsStringValue(right)) {
        return std::get<i18nString>(left) == std::get<i18nString>(right);
    }
    if (IsBoolValue(left) && IsBoolValue(right)) {
        return std::get<bool>(left) == std::get<bool>(right);
    }
    if (IsNullValue(left) && IsNullValue(right)) {
        return true;
    }
    if (const auto* left_function = std::get_if<std::shared_ptr<Function>>(&left)) {
        if (const auto* right_function = std::get_if<std::shared_ptr<Function>>(&right)) {
            return left_function->get() == right_function->get();
        }
        return false;
    }
    if (const auto* left_native = std::get_if<std::shared_ptr<NativeFunction>>(&left)) {
        if (const auto* right_native = std::get_if<std::shared_ptr<NativeFunction>>(&right)) {
            return left_native->get() == right_native->get();
        }
        return false;
    }
    return false;
}

int CompareValues(const Value& left, const Value& right, int line, int column)
{
    if (IsNumberValue(left) && IsNumberValue(right)) {
        const double a = ToDouble(left);
        const double b = ToDouble(right);
        if (a < b) {
            return -1;
        }
        return a > b ? 1 : 0;
    }
    if (IsStringValue(left) && IsStringValue(right)) {
        const i18nString& a = std::get<i18nString>(left);
        const i18nString& b = std::get<i18nString>(right);
        if (a < b) {
            return -1;
        }
        return a > b ? 1 : 0;
    }
    Fail(line, column, L"无法比较 " + TypeName(left) + L" 与 " + TypeName(right));
}

Value ApplyBinary(TokenType op, const Value& left, const Value& right, int line, int column)
{
    switch (op) {
    case TokenType::Plus:
        // 任意一侧是字符串时做拼接
        if (IsStringValue(left) || IsStringValue(right)) {
            return Value{ToDisplay(left) + ToDisplay(right)};
        }
        break;
    case TokenType::EqualEqual: return Value{ValuesEqual(left, right)};
    case TokenType::BangEqual: return Value{!ValuesEqual(left, right)};
    case TokenType::Less: return Value{CompareValues(left, right, line, column) < 0};
    case TokenType::LessEqual: return Value{CompareValues(left, right, line, column) <= 0};
    case TokenType::Greater: return Value{CompareValues(left, right, line, column) > 0};
    case TokenType::GreaterEqual: return Value{CompareValues(left, right, line, column) >= 0};
    default: break;
    }

    if (!IsNumberValue(left) || !IsNumberValue(right)) {
        Fail(line, column,
             L"运算符两侧需要数字，实际为 " + TypeName(left) + L" 与 " + TypeName(right));
    }

    const bool use_float =
        std::holds_alternative<double>(left) || std::holds_alternative<double>(right);

    if (!use_float) {
        const std::int64_t a = std::get<std::int64_t>(left);
        const std::int64_t b = std::get<std::int64_t>(right);
        switch (op) {
        case TokenType::Plus: return Value{a + b};
        case TokenType::Minus: return Value{a - b};
        case TokenType::Star: return Value{a * b};
        case TokenType::Slash:
            if (b == 0) {
                Fail(line, column, L"除数不能为 0");
            }
            return Value{a / b};
        case TokenType::Percent:
            if (b == 0) {
                Fail(line, column, L"除数不能为 0");
            }
            return Value{a % b};
        default: break;
        }
    } else {
        const double a = ToDouble(left);
        const double b = ToDouble(right);
        switch (op) {
        case TokenType::Plus: return Value{a + b};
        case TokenType::Minus: return Value{a - b};
        case TokenType::Star: return Value{a * b};
        case TokenType::Slash:
            if (b == 0.0) {
                Fail(line, column, L"除数不能为 0");
            }
            return Value{a / b};
        case TokenType::Percent:
            if (b == 0.0) {
                Fail(line, column, L"除数不能为 0");
            }
            return Value{std::fmod(a, b)};
        default: break;
        }
    }

    Fail(line, column, L"不支持的运算符");
}

i18nString TagName(TypeTag tag)
{
    switch (tag) {
    case TypeTag::Int: return L"int";
    case TypeTag::Float: return L"float";
    case TypeTag::String: return L"string";
    case TypeTag::Bool: return L"bool";
    case TypeTag::Any: break;
    }
    return L"any";
}

void CheckTypeAgainstTag(const std::optional<TypeTag>& type, const Value& value, int line,
                         int column, const i18nString& target)
{
    if (!type.has_value()) {
        return;
    }

    bool matched = true;
    switch (*type) {
    case TypeTag::Any: return;
    case TypeTag::Int: matched = std::holds_alternative<std::int64_t>(value); break;
    case TypeTag::Float: matched = IsNumberValue(value); break;
    case TypeTag::String: matched = IsStringValue(value); break;
    case TypeTag::Bool: matched = IsBoolValue(value); break;
    }

    if (!matched) {
        Fail(line, column,
             target + L" 需要 " + TagName(*type) + L"，实际为 " + TypeName(value));
    }
}

TokenType CompoundToBinary(TokenType op)
{
    switch (op) {
    case TokenType::PlusAssign: return TokenType::Plus;
    case TokenType::MinusAssign: return TokenType::Minus;
    case TokenType::StarAssign: return TokenType::Star;
    case TokenType::SlashAssign: return TokenType::Slash;
    case TokenType::PercentAssign: return TokenType::Percent;
    default: return op;
    }
}

std::size_t CheckArity(std::vector<Value>& arguments, std::size_t expected, const i18nString& name,
                       int line, int column)
{
    if (arguments.size() != expected) {
        Fail(line, column,
             name + L" 需要 " + std::to_wstring(expected) + L" 个参数，实际为 " +
                 std::to_wstring(arguments.size()));
    }
    return expected;
}

}  // namespace

Interpreter::Interpreter() : globals_(std::make_shared<Environment>())
{
    DefineBuiltins();
}

void Interpreter::DefineBuiltins()
{
    const auto make_native = [](const i18nString& name,
                                std::function<Value(std::vector<Value>&, int, int)> call) {
        auto native = std::make_shared<NativeFunction>();
        native->name = name;
        native->call = std::move(call);
        return Value{native};
    };

    const auto write = [](const std::vector<Value>& arguments) {
        for (const Value& argument : arguments) {
            std::cout << ToUtf8(ToDisplay(argument));
        }
    };

    // 输出（无换行）
    const Value print_value =
        make_native(L"print", [write](std::vector<Value>& arguments, int, int) {
            write(arguments);
            return Value{};
        });

    // 输出（自动换行）
    const Value println_value =
        make_native(L"println", [write](std::vector<Value>& arguments, int, int) {
            write(arguments);
            std::cout << '\n';
            return Value{};
        });

    globals_->Define(L"print", print_value, true);
    globals_->Define(L"printf", print_value, true);

    globals_->Define(L"println", println_value, true);

    // 类型转换与查询
    globals_->Define(L"str",
                     make_native(L"str",
                                 [](std::vector<Value>& arguments, int line, int column) {
                                     CheckArity(arguments, 1, L"str", line, column);
                                     return Value{ToDisplay(arguments[0])};
                                 }),
                     true);

    globals_->Define(L"int",
                     make_native(L"int",
                                 [](std::vector<Value>& arguments, int line, int column) {
                                     CheckArity(arguments, 1, L"int", line, column);
                                     const Value& value = arguments[0];
                                     if (const auto* integer = std::get_if<std::int64_t>(&value)) {
                                         return Value{*integer};
                                     }
                                     if (const auto* number = std::get_if<double>(&value)) {
                                         return Value{static_cast<std::int64_t>(*number)};
                                     }
                                     if (const auto* flag = std::get_if<bool>(&value)) {
                                         return Value{static_cast<std::int64_t>(*flag ? 1 : 0)};
                                     }
                                     if (const auto* text = std::get_if<i18nString>(&value)) {
                                         try {
                                             return Value{static_cast<std::int64_t>(
                                                 std::stoll(ToUtf8(*text)))};
                                         } catch (const std::exception&) {
                                             Fail(line, column,
                                                  L"无法把 '" + *text + L"' 转换为 int");
                                         }
                                     }
                                     Fail(line, column, L"无法把 " + TypeName(value) + L" 转换为 int");
                                 }),
                     true);

    globals_->Define(L"float",
                     make_native(L"float",
                                 [](std::vector<Value>& arguments, int line, int column) {
                                     CheckArity(arguments, 1, L"float", line, column);
                                     const Value& value = arguments[0];
                                     if (IsNumberValue(value)) {
                                         return Value{ToDouble(value)};
                                     }
                                     if (const auto* text = std::get_if<i18nString>(&value)) {
                                         try {
                                             return Value{std::stod(ToUtf8(*text))};
                                         } catch (const std::exception&) {
                                             Fail(line, column,
                                                  L"无法把 '" + *text + L"' 转换为 float");
                                         }
                                     }
                                     Fail(line, column,
                                          L"无法把 " + TypeName(value) + L" 转换为 float");
                                 }),
                     true);

    const Value len_value =
        make_native(L"len", [](std::vector<Value>& arguments, int line, int column) {
            CheckArity(arguments, 1, L"len", line, column);
            const Value& value = arguments[0];
            if (const auto* text = std::get_if<i18nString>(&value)) {
                return Value{static_cast<std::int64_t>(text->size())};
            }
            Fail(line, column, L"len 需要字符串，实际为 " + TypeName(value));
        });
    globals_->Define(L"len", len_value, true);

    const Value type_value =
        make_native(L"type", [](std::vector<Value>& arguments, int line, int column) {
            CheckArity(arguments, 1, L"type", line, column);
            return Value{TypeName(arguments[0])};
        });
    globals_->Define(L"type", type_value, true);
}

void Interpreter::Run(const Program& program)
{
    for (const auto& declaration : program.functions) {
        if (globals_->Find(declaration->name) != nullptr) {
            Fail(declaration->line, declaration->column,
                 L"重复定义的函数 '" + declaration->name + L"'");
        }

        auto function = std::make_shared<Function>();
        function->name = declaration->name;
        function->params = declaration->params;
        function->return_type = declaration->return_type;
        function->body = declaration->body.get();
        function->closure = globals_;

        globals_->Define(declaration->name, Value{function}, true);
    }

    Value* entry = globals_->Find(L"main");
    if (entry == nullptr) {
        Fail(0, 0, L"未找到入口函数 main");
    }

    std::vector<Value> arguments;
    CallValue(*entry, arguments, 0, 0);
}

void Interpreter::ExecuteBlock(const std::vector<std::unique_ptr<Stmt>>& statements,
                               const std::shared_ptr<Environment>& environment)
{
    auto scope = std::make_shared<Environment>(environment);
    for (const auto& statement : statements) {
        Execute(*statement, scope);
    }
}

void Interpreter::Execute(const Stmt& statement, const std::shared_ptr<Environment>& environment)
{
    if (const auto* block = dynamic_cast<const BlockStmt*>(&statement)) {
        ExecuteBlock(block->statements, environment);
        return;
    }

    if (const auto* decl = dynamic_cast<const VarDeclStmt*>(&statement)) {
        Value value = Evaluate(*decl->initializer, environment);
        CheckTypeAgainstTag(decl->type, value, decl->line, decl->column,
                            L"变量 '" + decl->name + L"'");
        environment->Define(decl->name, value, decl->is_const);
        return;
    }

    if (const auto* expr_stmt = dynamic_cast<const ExprStmt*>(&statement)) {
        Evaluate(*expr_stmt->expr, environment);
        return;
    }

    if (const auto* if_stmt = dynamic_cast<const IfStmt*>(&statement)) {
        for (const auto& branch : if_stmt->branches) {
            if (IsTruthy(Evaluate(*branch.first, environment))) {
                ExecuteBlock(branch.second->statements, environment);
                return;
            }
        }
        if (if_stmt->else_branch) {
            ExecuteBlock(if_stmt->else_branch->statements, environment);
        }
        return;
    }

    if (const auto* while_stmt = dynamic_cast<const WhileStmt*>(&statement)) {
        while (IsTruthy(Evaluate(*while_stmt->condition, environment))) {
            try {
                ExecuteBlock(while_stmt->body->statements, environment);
            } catch (const BreakSignal&) {
                break;
            } catch (const ContinueSignal&) {
                continue;
            }
        }
        return;
    }

    if (const auto* for_stmt = dynamic_cast<const ForStmt*>(&statement)) {
        auto scope = std::make_shared<Environment>(environment);
        if (for_stmt->initializer) {
            Execute(*for_stmt->initializer, scope);
        }
        while (!for_stmt->condition || IsTruthy(Evaluate(*for_stmt->condition, scope))) {
            try {
                ExecuteBlock(for_stmt->body->statements, scope);
            } catch (const BreakSignal&) {
                break;
            } catch (const ContinueSignal&) {
                // continue 仍需执行步进表达式
            }
            if (for_stmt->step) {
                Evaluate(*for_stmt->step, scope);
            }
        }
        return;
    }

    if (dynamic_cast<const BreakStmt*>(&statement) != nullptr) {
        throw BreakSignal{};
    }

    if (dynamic_cast<const ContinueStmt*>(&statement) != nullptr) {
        throw ContinueSignal{};
    }

    if (const auto* return_stmt = dynamic_cast<const ReturnStmt*>(&statement)) {
        Value value;
        if (return_stmt->value) {
            value = Evaluate(*return_stmt->value, environment);
        }
        throw ReturnSignal{value};
    }

    Fail(statement.line, statement.column, L"无法执行的语句");
}

Value Interpreter::Evaluate(const Expr& expression, const std::shared_ptr<Environment>& environment)
{
    if (const auto* literal = dynamic_cast<const LiteralExpr*>(&expression)) {
        switch (literal->kind) {
        case LiteralExpr::Kind::Null: return Value{};
        case LiteralExpr::Kind::Bool: return Value{literal->bool_value};
        case LiteralExpr::Kind::Int: return Value{literal->int_value};
        case LiteralExpr::Kind::Double: return Value{literal->double_value};
        case LiteralExpr::Kind::String: return Value{literal->string_value};
        }
        return Value{};
    }

    if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        Value* slot = environment->Find(variable->name);
        if (slot == nullptr) {
            Fail(variable->line, variable->column, L"未定义的变量 '" + variable->name + L"'");
        }
        return *slot;
    }

    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        const Value operand = Evaluate(*unary->operand, environment);
        if (unary->op == TokenType::Bang) {
            return Value{!IsTruthy(operand)};
        }
        if (!IsNumberValue(operand)) {
            Fail(unary->line, unary->column,
                 L"一元 '-' 需要数字，实际为 " + TypeName(operand));
        }
        if (const auto* integer = std::get_if<std::int64_t>(&operand)) {
            return Value{-*integer};
        }
        return Value{-std::get<double>(operand)};
    }

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        // 短路求值
        if (binary->op == TokenType::AndAnd) {
            if (!IsTruthy(Evaluate(*binary->left, environment))) {
                return Value{false};
            }
            return Value{IsTruthy(Evaluate(*binary->right, environment))};
        }
        if (binary->op == TokenType::OrOr) {
            if (IsTruthy(Evaluate(*binary->left, environment))) {
                return Value{true};
            }
            return Value{IsTruthy(Evaluate(*binary->right, environment))};
        }

        const Value left = Evaluate(*binary->left, environment);
        const Value right = Evaluate(*binary->right, environment);
        return ApplyBinary(binary->op, left, right, binary->line, binary->column);
    }

    if (const auto* assign = dynamic_cast<const AssignExpr*>(&expression)) {
        Value* slot = environment->Find(assign->name);
        if (slot == nullptr) {
            Fail(assign->line, assign->column, L"未定义的变量 '" + assign->name + L"'");
        }
        if (environment->IsConst(assign->name)) {
            Fail(assign->line, assign->column, L"常量 '" + assign->name + L"' 不能被修改");
        }

        Value value = Evaluate(*assign->value, environment);
        if (assign->op != TokenType::Assign) {
            value = ApplyBinary(CompoundToBinary(assign->op), *slot, value, assign->line,
                                assign->column);
        }
        *slot = value;
        return value;
    }

    if (const auto* call = dynamic_cast<const CallExpr*>(&expression)) {
        const Value callee = Evaluate(*call->callee, environment);
        std::vector<Value> arguments;
        arguments.reserve(call->arguments.size());
        for (const auto& argument : call->arguments) {
            arguments.push_back(Evaluate(*argument, environment));
        }
        return CallValue(callee, arguments, call->line, call->column);
    }

    Fail(expression.line, expression.column, L"无法求值的表达式");
}

Value Interpreter::CallValue(const Value& callee, std::vector<Value>& arguments, int line,
                             int column)
{
    if (const auto* native = std::get_if<std::shared_ptr<NativeFunction>>(&callee)) {
        if (!*native) {
            Fail(line, column, L"调用了空的内建函数");
        }
        return (*native)->call(arguments, line, column);
    }

    if (const auto* function = std::get_if<std::shared_ptr<Function>>(&callee)) {
        if (!*function) {
            Fail(line, column, L"调用了空的函数");
        }
        const Function& target = **function;

        if (arguments.size() != target.params.size()) {
            Fail(line, column,
                 L"函数 '" + target.name + L"' 需要 " + std::to_wstring(target.params.size()) +
                     L" 个参数，实际为 " + std::to_wstring(arguments.size()));
        }

        auto scope = std::make_shared<Environment>(target.closure);
        for (std::size_t i = 0; i < target.params.size(); ++i) {
            CheckTypeAgainstTag(target.params[i].type, arguments[i], line, column,
                                L"参数 '" + target.params[i].name + L"'");
            scope->Define(target.params[i].name, arguments[i], true);
        }

        try {
            ExecuteBlock(target.body->statements, scope);
        } catch (const ReturnSignal& signal) {
            return signal.value;
        }

        return Value{};
    }

    Fail(line, column, L"无法调用 " + TypeName(callee) + L" 类型的值");
}

}  // namespace otne
