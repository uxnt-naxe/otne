#include "otne_parser.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace otne {

namespace {

bool IsAssignmentOperator(TokenType type)
{
    switch (type) {
    case TokenType::Assign:
    case TokenType::PlusAssign:
    case TokenType::MinusAssign:
    case TokenType::StarAssign:
    case TokenType::SlashAssign:
    case TokenType::PercentAssign: return true;
    default: return false;
    }
}

}  // namespace

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

const Token& Parser::Current() const
{
    return tokens_[position_];
}

const Token& Parser::Previous() const
{
    return tokens_[position_ - 1];
}

bool Parser::IsAtEnd() const
{
    return Current().type == TokenType::EndOfFile;
}

bool Parser::Check(TokenType type) const
{
    return Current().type == type;
}

bool Parser::Match(TokenType type)
{
    if (!Check(type)) {
        return false;
    }
    Advance();
    return true;
}

const Token& Parser::Advance()
{
    if (!IsAtEnd()) {
        ++position_;
    }
    return Previous();
}

const Token& Parser::Consume(TokenType type, const i18nString& message)
{
    if (Check(type)) {
        return Advance();
    }

    const Token& token = Current();
    throw ParseError{Diagnostic{token.line, token.column, message}};
}

Program Parser::Parse()
{
    Program program;

    while (!IsAtEnd()) {
        if (Check(TokenType::Import)) {
            ParseImport(program);
            continue;
        }
        if (Check(TokenType::Func)) {
            program.functions.push_back(ParseFunction());
            continue;
        }

        const Token& token = Current();
        throw ParseError{Diagnostic{token.line, token.column,
                                    L"顶层只能出现 import 声明或 func 函数定义"}};
    }

    return program;
}

void Parser::ParseImport(Program& program)
{
    Advance();  // import
    const Token& name = Consume(TokenType::Identifier, L"import 后需要模块名");
    program.imports.push_back(name.lexeme);
    Consume(TokenType::Semicolon, L"import 语句缺少 ';'");
}

std::unique_ptr<FunctionDecl> Parser::ParseFunction()
{
    auto function = std::make_unique<FunctionDecl>();

    const Token& func_token = Advance();  // func
    function->line = func_token.line;
    function->column = func_token.column;

    const Token& name = Consume(TokenType::Identifier, L"func 后需要函数名");
    function->name = name.lexeme;

    Consume(TokenType::LeftParen, L"函数名后需要 '('");
    if (!Check(TokenType::RightParen)) {
        do {
            Parameter parameter;
            const Token& parameter_name = Consume(TokenType::Identifier, L"参数名不能为空");
            parameter.name = parameter_name.lexeme;
            parameter.line = parameter_name.line;
            parameter.column = parameter_name.column;
            if (Match(TokenType::Colon)) {
                parameter.type = ParseType();
            }
            function->params.push_back(parameter);
        } while (Match(TokenType::Comma));
    }
    Consume(TokenType::RightParen, L"参数列表缺少 ')'");

    if (Match(TokenType::Colon)) {
        function->return_type = ParseType();
    }

    function->body = ParseBlock();
    return function;
}

TypeTag Parser::ParseType()
{
    const Token& token = Current();
    switch (token.type) {
    case TokenType::IntType: Advance(); return TypeTag::Int;
    case TokenType::FloatType: Advance(); return TypeTag::Float;
    case TokenType::StringType: Advance(); return TypeTag::String;
    case TokenType::BoolType: Advance(); return TypeTag::Bool;
    case TokenType::AnyType: Advance(); return TypeTag::Any;
    default:
        throw ParseError{Diagnostic{token.line, token.column,
                                    L"这里需要类型：int / float / string / bool / any"}};
    }
}

std::unique_ptr<BlockStmt> Parser::ParseBlock()
{
    auto block = std::make_unique<BlockStmt>();
    block->line = Current().line;
    block->column = Current().column;

    Consume(TokenType::LeftBrace, L"这里需要 '{'");
    while (!Check(TokenType::RightBrace) && !IsAtEnd()) {
        block->statements.push_back(ParseStatement());
    }
    Consume(TokenType::RightBrace, L"代码块缺少 '}'");

    return block;
}

std::unique_ptr<Stmt> Parser::ParseStatement()
{
    const Token& token = Current();

    if (Check(TokenType::LeftBrace)) {
        return ParseBlock();
    }
    if (Check(TokenType::Let) || Check(TokenType::Var) || Check(TokenType::Const) ||
        IsTypeKeyword(token.type)) {
        return ParseVarDecl(true);
    }
    if (Check(TokenType::If)) {
        return ParseIf();
    }
    if (Check(TokenType::While)) {
        return ParseWhile();
    }
    if (Check(TokenType::For)) {
        return ParseFor();
    }
    if (Check(TokenType::Break)) {
        auto stmt = std::make_unique<BreakStmt>();
        stmt->line = token.line;
        stmt->column = token.column;
        Advance();
        Consume(TokenType::Semicolon, L"break 语句缺少 ';'");
        return stmt;
    }
    if (Check(TokenType::Continue)) {
        auto stmt = std::make_unique<ContinueStmt>();
        stmt->line = token.line;
        stmt->column = token.column;
        Advance();
        Consume(TokenType::Semicolon, L"continue 语句缺少 ';'");
        return stmt;
    }
    if (Check(TokenType::Return)) {
        return ParseReturn();
    }

    return ParseExpressionStatement();
}

std::unique_ptr<VarDeclStmt> Parser::ParseVarDecl(bool expect_semicolon)
{
    auto decl = std::make_unique<VarDeclStmt>();
    const Token& start = Current();
    decl->line = start.line;
    decl->column = start.column;

    if (Match(TokenType::Const)) {
        decl->is_const = true;
    } else if (Match(TokenType::Let) || Match(TokenType::Var)) {
        decl->is_const = false;
    } else {
        // 以类型开头：int count = 0;
        decl->type = ParseType();
    }

    const Token& name = Consume(TokenType::Identifier, L"这里需要变量名");
    decl->name = name.lexeme;

    // 变量名后可附加类型标注：let name: string = "";
    if (Match(TokenType::Colon)) {
        decl->type = ParseType();
    }

    Consume(TokenType::Assign, L"变量声明需要使用 '=' 初始化");
    decl->initializer = ParseExpression();

    if (expect_semicolon) {
        Consume(TokenType::Semicolon, L"变量声明缺少 ';'");
    }

    return decl;
}

std::unique_ptr<Stmt> Parser::ParseIf()
{
    auto stmt = std::make_unique<IfStmt>();
    const Token& start = Advance();  // if
    stmt->line = start.line;
    stmt->column = start.column;

    do {
        auto condition = ParseExpression();
        auto body = ParseBlock();
        stmt->branches.emplace_back(std::move(condition), std::move(body));
    } while (Match(TokenType::Elif));

    if (Match(TokenType::Else)) {
        stmt->else_branch = ParseBlock();
    }

    return stmt;
}

std::unique_ptr<Stmt> Parser::ParseWhile()
{
    auto stmt = std::make_unique<WhileStmt>();
    const Token& start = Advance();  // while
    stmt->line = start.line;
    stmt->column = start.column;

    stmt->condition = ParseExpression();
    stmt->body = ParseBlock();

    return stmt;
}

std::unique_ptr<Stmt> Parser::ParseFor()
{
    auto stmt = std::make_unique<ForStmt>();
    const Token& start = Advance();  // for
    stmt->line = start.line;
    stmt->column = start.column;

    Consume(TokenType::LeftParen, L"for 后需要 '('");

    if (Match(TokenType::Semicolon)) {
        // 无初始化语句
    } else if (Check(TokenType::Let) || Check(TokenType::Var) || Check(TokenType::Const) ||
               IsTypeKeyword(Current().type)) {
        stmt->initializer = ParseVarDecl(true);
    } else {
        auto expr_stmt = std::make_unique<ExprStmt>();
        expr_stmt->line = Current().line;
        expr_stmt->column = Current().column;
        expr_stmt->expr = ParseExpression();
        Consume(TokenType::Semicolon, L"for 初始化语句缺少 ';'");
        stmt->initializer = std::move(expr_stmt);
    }

    if (!Check(TokenType::Semicolon)) {
        stmt->condition = ParseExpression();
    }
    Consume(TokenType::Semicolon, L"for 条件后需要 ';'");

    if (!Check(TokenType::RightParen)) {
        stmt->step = ParseExpression();
    }
    Consume(TokenType::RightParen, L"for 缺少 ')'");

    stmt->body = ParseBlock();
    return stmt;
}

std::unique_ptr<Stmt> Parser::ParseReturn()
{
    auto stmt = std::make_unique<ReturnStmt>();
    const Token& start = Advance();  // return
    stmt->line = start.line;
    stmt->column = start.column;

    if (!Check(TokenType::Semicolon)) {
        stmt->value = ParseExpression();
    }
    Consume(TokenType::Semicolon, L"return 语句缺少 ';'");

    return stmt;
}

std::unique_ptr<Stmt> Parser::ParseExpressionStatement()
{
    auto stmt = std::make_unique<ExprStmt>();
    stmt->line = Current().line;
    stmt->column = Current().column;
    stmt->expr = ParseExpression();
    Consume(TokenType::Semicolon, L"表达式语句缺少 ';'");
    return stmt;
}

std::unique_ptr<Expr> Parser::ParseExpression()
{
    return ParseAssignment();
}

std::unique_ptr<Expr> Parser::ParseAssignment()
{
    auto expr = ParseOr();

    if (IsAssignmentOperator(Current().type)) {
        const Token& op = Advance();
        auto* variable = dynamic_cast<VariableExpr*>(expr.get());
        if (variable == nullptr) {
            throw ParseError{Diagnostic{op.line, op.column, L"赋值号左边必须是变量"}};
        }

        auto assign = std::make_unique<AssignExpr>();
        assign->line = variable->line;
        assign->column = variable->column;
        assign->name = variable->name;
        assign->op = op.type;
        assign->value = ParseAssignment();
        return assign;
    }

    return expr;
}

std::unique_ptr<BinaryExpr> Parser::MakeBinary(std::unique_ptr<Expr> left, TokenType op,
                                               std::unique_ptr<Expr> right)
{
    auto binary = std::make_unique<BinaryExpr>();
    binary->line = left->line;
    binary->column = left->column;
    binary->op = op;
    binary->left = std::move(left);
    binary->right = std::move(right);
    return binary;
}

std::unique_ptr<Expr> Parser::ParseOr()
{
    auto expr = ParseAnd();
    while (Match(TokenType::OrOr)) {
        const TokenType op = Previous().type;
        expr = MakeBinary(std::move(expr), op, ParseAnd());
    }
    return expr;
}

std::unique_ptr<Expr> Parser::ParseAnd()
{
    auto expr = ParseEquality();
    while (Match(TokenType::AndAnd)) {
        const TokenType op = Previous().type;
        expr = MakeBinary(std::move(expr), op, ParseEquality());
    }
    return expr;
}

std::unique_ptr<Expr> Parser::ParseEquality()
{
    auto expr = ParseComparison();
    while (Check(TokenType::EqualEqual) || Check(TokenType::BangEqual)) {
        const TokenType op = Advance().type;
        expr = MakeBinary(std::move(expr), op, ParseComparison());
    }
    return expr;
}

std::unique_ptr<Expr> Parser::ParseComparison()
{
    auto expr = ParseTerm();
    while (Check(TokenType::Less) || Check(TokenType::LessEqual) ||
           Check(TokenType::Greater) || Check(TokenType::GreaterEqual)) {
        const TokenType op = Advance().type;
        expr = MakeBinary(std::move(expr), op, ParseTerm());
    }
    return expr;
}

std::unique_ptr<Expr> Parser::ParseTerm()
{
    auto expr = ParseFactor();
    while (Check(TokenType::Plus) || Check(TokenType::Minus)) {
        const TokenType op = Advance().type;
        expr = MakeBinary(std::move(expr), op, ParseFactor());
    }
    return expr;
}

std::unique_ptr<Expr> Parser::ParseFactor()
{
    auto expr = ParseUnary();
    while (Check(TokenType::Star) || Check(TokenType::Slash) || Check(TokenType::Percent)) {
        const TokenType op = Advance().type;
        expr = MakeBinary(std::move(expr), op, ParseUnary());
    }
    return expr;
}

std::unique_ptr<Expr> Parser::ParseUnary()
{
    if (Check(TokenType::Bang) || Check(TokenType::Minus)) {
        const Token& op = Advance();
        auto unary = std::make_unique<UnaryExpr>();
        unary->line = op.line;
        unary->column = op.column;
        unary->op = op.type;
        unary->operand = ParseUnary();
        return unary;
    }

    return ParseCall();
}

std::unique_ptr<Expr> Parser::ParseCall()
{
    auto expr = ParsePrimary();

    while (Match(TokenType::LeftParen)) {
        auto call = std::make_unique<CallExpr>();
        call->line = expr->line;
        call->column = expr->column;
        call->callee = std::move(expr);

        if (!Check(TokenType::RightParen)) {
            do {
                call->arguments.push_back(ParseExpression());
            } while (Match(TokenType::Comma));
        }
        Consume(TokenType::RightParen, L"函数调用缺少 ')'");

        expr = std::move(call);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::ParsePrimary()
{
    const Token& token = Current();

    if (Match(TokenType::Number)) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->line = token.line;
        literal->column = token.column;

        const std::string text(token.lexeme.begin(), token.lexeme.end());
        try {
            if (text.find('.') == std::string::npos) {
                literal->kind = LiteralExpr::Kind::Int;
                literal->int_value = std::stoll(text);
            } else {
                literal->kind = LiteralExpr::Kind::Double;
                literal->double_value = std::stod(text);
            }
        } catch (const std::exception&) {
            throw ParseError{Diagnostic{token.line, token.column, L"数字字面量超出范围"}};
        }
        return literal;
    }

    if (Match(TokenType::String)) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->line = token.line;
        literal->column = token.column;
        literal->kind = LiteralExpr::Kind::String;
        literal->string_value = token.literal;
        return literal;
    }

    if (Match(TokenType::True) || Match(TokenType::False)) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->line = token.line;
        literal->column = token.column;
        literal->kind = LiteralExpr::Kind::Bool;
        literal->bool_value = (token.type == TokenType::True);
        return literal;
    }

    if (Match(TokenType::Null)) {
        auto literal = std::make_unique<LiteralExpr>();
        literal->line = token.line;
        literal->column = token.column;
        literal->kind = LiteralExpr::Kind::Null;
        return literal;
    }

    if (Match(TokenType::Identifier)) {
        auto variable = std::make_unique<VariableExpr>();
        variable->line = token.line;
        variable->column = token.column;
        variable->name = token.lexeme;
        return variable;
    }

    if (Match(TokenType::LeftParen)) {
        auto expr = ParseExpression();
        Consume(TokenType::RightParen, L"缺少 ')'");
        return expr;
    }

    throw ParseError{Diagnostic{token.line, token.column, L"无法解析的表达式"}};
}

}  // namespace otne
