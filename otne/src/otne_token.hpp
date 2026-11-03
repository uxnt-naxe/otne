#pragma once

#include <string>

#include "otne_utf8.hpp"

namespace otne {

// 类型标注：用于变量、参数的可选类型检查
enum class TypeTag { Any, Int, Float, String, Bool };

// Token 类型
enum class TokenType {
    // 字面量与标识符
    Number,
    String,
    Identifier,

    // 关键字
    Import,
    Func,
    Let,
    Var,
    Const,
    If,
    Elif,
    Else,
    While,
    For,
    Break,
    Continue,
    Return,
    True,
    False,
    Null,

    // 类型关键字
    IntType,
    FloatType,
    StringType,
    BoolType,
    AnyType,

    // 符号
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Semicolon,
    Colon,

    // 运算符
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Assign,
    EqualEqual,
    Bang,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    AndAnd,
    OrOr,
    PlusAssign,
    MinusAssign,
    StarAssign,
    SlashAssign,
    PercentAssign,

    EndOfFile,
    Invalid
};

// 词法单元
struct Token {
    TokenType type = TokenType::Invalid;
    i18nString lexeme;   // 源码中的原始文本
    i18nString literal;  // 字符串字面量解码后的内容
    int line = 1;
    int column = 1;
};

// 供 --tokens 输出使用的可读名称
const char* TokenTypeName(TokenType type);

// 类型关键字 -> TypeTag
bool IsTypeKeyword(TokenType type);

// TokenType -> TokenType 对应的源码写法（用于错误提示）
const char* TokenTypeSpelling(TokenType type);

}  // namespace otne
