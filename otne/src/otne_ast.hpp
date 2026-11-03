#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "otne_token.hpp"
#include "otne_utf8.hpp"

namespace otne {

// ---------------- 表达式 ----------------
struct Expr {
    virtual ~Expr() = default;
    int line = 1;
    int column = 1;
};

struct LiteralExpr : Expr {
    enum class Kind { Null, Bool, Int, Double, String };

    Kind kind = Kind::Null;
    bool bool_value = false;
    std::int64_t int_value = 0;
    double double_value = 0.0;
    i18nString string_value;
};

struct VariableExpr : Expr {
    i18nString name;
};

struct UnaryExpr : Expr {
    TokenType op = TokenType::Invalid;
    std::unique_ptr<Expr> operand;
};

struct BinaryExpr : Expr {
    TokenType op = TokenType::Invalid;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct AssignExpr : Expr {
    i18nString name;
    TokenType op = TokenType::Assign;  // = += -= *= /= %=
    std::unique_ptr<Expr> value;
};

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> arguments;
};

// ---------------- 语句 ----------------
struct Stmt {
    virtual ~Stmt() = default;
    int line = 1;
    int column = 1;
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
};

struct VarDeclStmt : Stmt {
    i18nString name;
    std::optional<TypeTag> type;
    bool is_const = false;
    std::unique_ptr<Expr> initializer;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
};

struct IfStmt : Stmt {
    // if / elif 分支，按顺序求值
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<BlockStmt>>> branches;
    std::unique_ptr<BlockStmt> else_branch;
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
};

struct ForStmt : Stmt {
    std::unique_ptr<Stmt> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> step;
    std::unique_ptr<BlockStmt> body;
};

struct BreakStmt : Stmt {};

struct ContinueStmt : Stmt {};

struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value;
};

struct Parameter {
    i18nString name;
    std::optional<TypeTag> type;
    int line = 1;
    int column = 1;
};

struct FunctionDecl {
    i18nString name;
    std::vector<Parameter> params;
    std::optional<TypeTag> return_type;
    std::unique_ptr<BlockStmt> body;
    int line = 1;
    int column = 1;
};

// 一个完整源文件：import 声明 + 函数定义
struct Program {
    std::vector<i18nString> imports;
    std::vector<std::unique_ptr<FunctionDecl>> functions;
};

}  // namespace otne
