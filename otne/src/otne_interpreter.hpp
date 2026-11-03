#pragma once

#include <memory>
#include <vector>

#include "otne_ast.hpp"
#include "otne_diagnostic.hpp"
#include "otne_value.hpp"

namespace otne {

// 运行时错误
struct RuntimeError {
    Diagnostic diagnostic;
};

// 树遍历解释器
class Interpreter
{
public:
    Interpreter();

    // 执行程序：先登记全部函数，再调用入口函数 main
    void Run(const Program& program);

private:
    void DefineBuiltins();

    void ExecuteBlock(const std::vector<std::unique_ptr<Stmt>>& statements,
                      const std::shared_ptr<Environment>& environment);
    void Execute(const Stmt& statement, const std::shared_ptr<Environment>& environment);
    Value Evaluate(const Expr& expression, const std::shared_ptr<Environment>& environment);

    Value CallValue(const Value& callee, std::vector<Value>& arguments, int line, int column);

    std::shared_ptr<Environment> globals_;
};

}  // namespace otne
