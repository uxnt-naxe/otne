#pragma once

#include <vector>

#include "otne_ast.hpp"
#include "otne_diagnostic.hpp"
#include "otne_token.hpp"

namespace otne {

// 语法错误：携带出错位置
struct ParseError {
    Diagnostic diagnostic;
};

// 递归下降语法分析器
class Parser
{
public:
    explicit Parser(const std::vector<Token>& tokens);

    // 解析整个源文件；失败时抛出 ParseError
    Program Parse();

private:
    const Token& Current() const;
    const Token& Previous() const;
    bool IsAtEnd() const;
    bool Check(TokenType type) const;
    bool Match(TokenType type);
    const Token& Advance();
    const Token& Consume(TokenType type, const i18nString& message);

    void ParseImport(Program& program);
    std::unique_ptr<FunctionDecl> ParseFunction();
    TypeTag ParseType();
    std::unique_ptr<BlockStmt> ParseBlock();

    std::unique_ptr<Stmt> ParseStatement();
    std::unique_ptr<VarDeclStmt> ParseVarDecl(bool expect_semicolon);
    std::unique_ptr<Stmt> ParseIf();
    std::unique_ptr<Stmt> ParseWhile();
    std::unique_ptr<Stmt> ParseFor();
    std::unique_ptr<Stmt> ParseReturn();
    std::unique_ptr<Stmt> ParseExpressionStatement();

    std::unique_ptr<Expr> ParseExpression();
    std::unique_ptr<Expr> ParseAssignment();
    std::unique_ptr<Expr> ParseOr();
    std::unique_ptr<Expr> ParseAnd();
    std::unique_ptr<Expr> ParseEquality();
    std::unique_ptr<Expr> ParseComparison();
    std::unique_ptr<Expr> ParseTerm();
    std::unique_ptr<Expr> ParseFactor();
    std::unique_ptr<Expr> ParseUnary();
    std::unique_ptr<Expr> ParseCall();
    std::unique_ptr<Expr> ParsePrimary();

    std::unique_ptr<BinaryExpr> MakeBinary(std::unique_ptr<Expr> left, TokenType op,
                                           std::unique_ptr<Expr> right);

    const std::vector<Token>& tokens_;
    std::size_t position_ = 0;
};

}  // namespace otne
