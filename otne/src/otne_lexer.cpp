#include "otne_lexer.hpp"

#include <unordered_map>
#include <utility>

namespace otne {

namespace {

bool IsAsciiDigit(i18nChar ch) { return ch >= L'0' && ch <= L'9'; }
bool IsAsciiAlpha(i18nChar ch) { return (ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z'); }
// 标识符：ASCII 字母或下划线开头
bool IsIdentifierStart(i18nChar ch) { return IsAsciiAlpha(ch) || ch == L'_'; }
bool IsIdentifierPart(i18nChar ch) { return IsIdentifierStart(ch) || IsAsciiDigit(ch); }

// 关键字表
const std::unordered_map<i18nString, TokenType>& KeyWords()
{
    static const std::unordered_map<i18nString, TokenType> keywords = {
        {L"use", TokenType::Import},     {L"func", TokenType::Func},
        {L"let", TokenType::Let},           {L"var", TokenType::Var},
        {L"const", TokenType::Const},       {L"if", TokenType::If},
        {L"elif", TokenType::Elif},         {L"else", TokenType::Else},
        {L"while", TokenType::While},       {L"for", TokenType::For},
        {L"break", TokenType::Break},       {L"continue", TokenType::Continue},
        {L"ret", TokenType::Return},        {L"true", TokenType::True},
        {L"false", TokenType::False},       {L"null", TokenType::Null},
        {L"int", TokenType::IntType},       {L"float", TokenType::FloatType},
        {L"string", TokenType::StringType}, {L"bool", TokenType::BoolType},
        {L"any", TokenType::AnyType},
    };
    return keywords;
}

}  // namespace

const char* TokenTypeName(TokenType type)
{
    switch (type) {
    case TokenType::Number: return "NUMBER";
    case TokenType::String: return "STRING";
    case TokenType::Identifier: return "IDENTIFIER";
    case TokenType::Import: return "IMPORT";
    case TokenType::Func: return "FUNC";
    case TokenType::Let: return "LET";
    case TokenType::Var: return "VAR";
    case TokenType::Const: return "CONST";
    case TokenType::If: return "IF";
    case TokenType::Elif: return "ELIF";
    case TokenType::Else: return "ELSE";
    case TokenType::While: return "WHILE";
    case TokenType::For: return "FOR";
    case TokenType::Break: return "BREAK";
    case TokenType::Continue: return "CONTINUE";
    case TokenType::Return: return "RETURN";
    case TokenType::True: return "TRUE";
    case TokenType::False: return "FALSE";
    case TokenType::Null: return "NULL";
    case TokenType::IntType: return "TYPE_INT";
    case TokenType::FloatType: return "TYPE_FLOAT";
    case TokenType::StringType: return "TYPE_STRING";
    case TokenType::BoolType: return "TYPE_BOOL";
    case TokenType::AnyType: return "TYPE_ANY";
    case TokenType::LeftParen: return "LEFT_PAREN";
    case TokenType::RightParen: return "RIGHT_PAREN";
    case TokenType::LeftBrace: return "LEFT_BRACE";
    case TokenType::RightBrace: return "RIGHT_BRACE";
    case TokenType::Comma: return "COMMA";
    case TokenType::Semicolon: return "SEMICOLON";
    case TokenType::Colon: return "COLON";
    case TokenType::Plus: return "PLUS";
    case TokenType::Minus: return "MINUS";
    case TokenType::Star: return "STAR";
    case TokenType::Slash: return "SLASH";
    case TokenType::Percent: return "PERCENT";
    case TokenType::Assign: return "ASSIGN";
    case TokenType::EqualEqual: return "EQUAL_EQUAL";
    case TokenType::Bang: return "BANG";
    case TokenType::BangEqual: return "BANG_EQUAL";
    case TokenType::Less: return "LESS";
    case TokenType::LessEqual: return "LESS_EQUAL";
    case TokenType::Greater: return "GREATER";
    case TokenType::GreaterEqual: return "GREATER_EQUAL";
    case TokenType::AndAnd: return "AND_AND";
    case TokenType::OrOr: return "OR_OR";
    case TokenType::PlusAssign: return "PLUS_ASSIGN";
    case TokenType::MinusAssign: return "MINUS_ASSIGN";
    case TokenType::StarAssign: return "STAR_ASSIGN";
    case TokenType::SlashAssign: return "SLASH_ASSIGN";
    case TokenType::PercentAssign: return "PERCENT_ASSIGN";
    case TokenType::EndOfFile: return "EOF";
    case TokenType::Invalid: return "INVALID";
    }
    return "UNKNOWN";
}

bool IsTypeKeyword(TokenType type)
{
    switch (type) {
    case TokenType::IntType:
    case TokenType::FloatType:
    case TokenType::StringType:
    case TokenType::BoolType:
    case TokenType::AnyType: return true;
    default: return false;
    }
}

const char* TokenTypeSpelling(TokenType type)
{
    switch (type) {
    case TokenType::LeftParen: return "'('";
    case TokenType::RightParen: return "')'";
    case TokenType::LeftBrace: return "'{'";
    case TokenType::RightBrace: return "'}'";
    case TokenType::Comma: return "','";
    case TokenType::Semicolon: return "';'";
    case TokenType::Colon: return "':'";
    case TokenType::Identifier: return "标识符";
    default: return TokenTypeName(type);
    }
}

Lexer::Lexer(const i18nString& source) : source_(source) {}

i18nChar Lexer::Peek(std::size_t offset) const
{
    const std::size_t index = position_ + offset;
    return index < source_.size() ? source_[index] : L'\0';
}

i18nChar Lexer::Advance()
{
    const i18nChar ch = source_[position_];
    ++position_;
    if (ch == L'\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return ch;
}

bool Lexer::Match(i18nChar expected)
{
    if (Peek() != expected) {
        return false;
    }
    Advance();
    return true;
}

void Lexer::Emit(std::vector<Token>& tokens, TokenType type, const i18nString& lexeme,
                 int line, int column)
{
    Token token;
    token.type = type;
    token.lexeme = lexeme;
    token.line = line;
    token.column = column;
    tokens.push_back(std::move(token));
}

void Lexer::SkipLineComment()
{
    Advance();
    Advance();
    while (position_ < source_.size() && Peek() != L'\n') {
        Advance();
    }
}

void Lexer::SkipBlockComment(std::vector<Diagnostic>& diagnostics, int line, int column)
{
    Advance();
    Advance();
    while (position_ < source_.size()) {
        if (Peek() == L'*' && Peek(1) == L'/') {
            Advance();
            Advance();
            return;
        }
        Advance();
    }
    diagnostics.push_back(Diagnostic{line, column, L"块注释缺少结束标记 '*/'"});
}

void Lexer::ScanIdentifier(std::vector<Token>& tokens, int line, int column)
{
    const std::size_t start = position_;
    while (IsIdentifierPart(Peek())) {
        Advance();
    }

    const i18nString text = source_.substr(start, position_ - start);
    const auto& keywords = KeyWords();
    const auto it = keywords.find(text);
    if (it != keywords.end()) {
        Emit(tokens, it->second, text, line, column);
    } else {
        Emit(tokens, TokenType::Identifier, text, line, column);
    }
}

void Lexer::ScanNumber(std::vector<Token>& tokens, int line, int column)
{
    const std::size_t start = position_;
    while (IsAsciiDigit(Peek())) {
        Advance();
    }
    // 小数点后必须跟数字，避免把 "1." 当成数字
    if (Peek() == L'.' && IsAsciiDigit(Peek(1))) {
        Advance();
        while (IsAsciiDigit(Peek())) {
            Advance();
        }
    }

    Emit(tokens, TokenType::Number, source_.substr(start, position_ - start), line, column);
}

void Lexer::ScanString(std::vector<Token>& tokens, std::vector<Diagnostic>& diagnostics,
                       int line, int column)
{
    Advance();  // 跳过起始引号

    i18nString value;
    bool terminated = false;

    while (position_ < source_.size()) {
        const i18nChar ch = Peek();

        if (ch == L'"') {
            Advance();
            terminated = true;
            break;
        }

        if (ch == L'\\') {
            Advance();
            if (position_ >= source_.size()) {
                break;
            }
            switch (Peek()) {
            case L'n': value.push_back(L'\n'); break;
            case L't': value.push_back(L'\t'); break;
            case L'r': value.push_back(L'\r'); break;
            case L'\\': value.push_back(L'\\'); break;
            case L'"': value.push_back(L'"'); break;
            case L'\'': value.push_back(L'\''); break;
            default: value.push_back(Peek()); break;
            }
            Advance();
            continue;
        }

        value.push_back(ch);
        Advance();
    }

    if (!terminated) {
        diagnostics.push_back(Diagnostic{line, column, L"字符串缺少结束引号"});
    }

    Token token;
    token.type = TokenType::String;
    token.lexeme = value;
    token.literal = value;
    token.line = line;
    token.column = column;
    tokens.push_back(std::move(token));
}

std::vector<Token> Lexer::Scan(std::vector<Diagnostic>& diagnostics)
{
    std::vector<Token> tokens;

    while (position_ < source_.size()) {
        const int line = line_;
        const int column = column_;
        const i18nChar ch = Peek();

        // 空白
        if (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n') {
            Advance();
            continue;
        }

        // 注释
        if (ch == L'/' && Peek(1) == L'/') {
            SkipLineComment();
            continue;
        }
        if (ch == L'/' && Peek(1) == L'*') {
            SkipBlockComment(diagnostics, line, column);
            continue;
        }

        // 标识符与关键字
        if (IsIdentifierStart(ch)) {
            ScanIdentifier(tokens, line, column);
            continue;
        }

        // 数字
        if (IsAsciiDigit(ch)) {
            ScanNumber(tokens, line, column);
            continue;
        }

        // 字符串
        if (ch == L'"') {
            ScanString(tokens, diagnostics, line, column);
            continue;
        }

        Advance();
        switch (ch) {
        case L'(':
            Emit(tokens, TokenType::LeftParen, L"(", line, column);
            break;
        case L')':
            Emit(tokens, TokenType::RightParen, L")", line, column);
            break;
        case L'{':
            Emit(tokens, TokenType::LeftBrace, L"{", line, column);
            break;
        case L'}':
            Emit(tokens, TokenType::RightBrace, L"}", line, column);
            break;
        case L',':
            Emit(tokens, TokenType::Comma, L",", line, column);
            break;
        case L';':
            Emit(tokens, TokenType::Semicolon, L";", line, column);
            break;
        case L':':
            Emit(tokens, TokenType::Colon, L":", line, column);
            break;
        case L'+':
            Emit(tokens, Match(L'=') ? TokenType::PlusAssign : TokenType::Plus, L"+", line, column);
            break;
        case L'-':
            Emit(tokens, Match(L'=') ? TokenType::MinusAssign : TokenType::Minus, L"-", line, column);
            break;
        case L'*':
            Emit(tokens, Match(L'=') ? TokenType::StarAssign : TokenType::Star, L"*", line, column);
            break;
        case L'/':
            Emit(tokens, Match(L'=') ? TokenType::SlashAssign : TokenType::Slash, L"/", line, column);
            break;
        case L'%':
            Emit(tokens, Match(L'=') ? TokenType::PercentAssign : TokenType::Percent, L"%", line, column);
            break;
        case L'=':
            Emit(tokens, Match(L'=') ? TokenType::EqualEqual : TokenType::Assign, L"=", line, column);
            break;
        case L'!':
            Emit(tokens, Match(L'=') ? TokenType::BangEqual : TokenType::Bang, L"!", line, column);
            break;
        case L'<':
            Emit(tokens, Match(L'=') ? TokenType::LessEqual : TokenType::Less, L"<", line, column);
            break;
        case L'>':
            Emit(tokens, Match(L'=') ? TokenType::GreaterEqual : TokenType::Greater, L">", line, column);
            break;
        case L'&':
            if (Match(L'&')) {
                Emit(tokens, TokenType::AndAnd, L"&&", line, column);
            } else {
                diagnostics.push_back(Diagnostic{line, column, L"逻辑与需要写作 '&&'"});
            }
            break;
        case L'|':
            if (Match(L'|')) {
                Emit(tokens, TokenType::OrOr, L"||", line, column);
            } else {
                diagnostics.push_back(Diagnostic{line, column, L"逻辑或需要写作 '||'"});
            }
            break;
        default:
            diagnostics.push_back(Diagnostic{line, column, i18nString(L"未知字符 '") + ch + L"'"});
            break;
        }
    }

    Emit(tokens, TokenType::EndOfFile, L"", line_, column_);
    return tokens;
}

}  // namespace otne
