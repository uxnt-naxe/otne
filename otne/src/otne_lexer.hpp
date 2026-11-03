#pragma once

#include <vector>

#include "otne_diagnostic.hpp"
#include "otne_token.hpp"

namespace otne {

// 词法分析器：把源码切分为 Token 序列
class Lexer
{
public:
    explicit Lexer(const i18nString& source);

    // 扫描全部 Token（末尾附带 EndOfFile）；词法错误写入 diagnostics
    std::vector<Token> Scan(std::vector<Diagnostic>& diagnostics);

private:
    i18nChar Peek(std::size_t offset = 0) const;
    i18nChar Advance();
    bool Match(i18nChar expected);

    void SkipLineComment();
    void SkipBlockComment(std::vector<Diagnostic>& diagnostics, int line, int column);

    void ScanIdentifier(std::vector<Token>& tokens, int line, int column);
    void ScanNumber(std::vector<Token>& tokens, int line, int column);
    void ScanString(std::vector<Token>& tokens, std::vector<Diagnostic>& diagnostics, int line, int column);

    void Emit(std::vector<Token>& tokens, TokenType type, const i18nString& lexeme, int line, int column);

    const i18nString& source_;
    std::size_t position_ = 0;
    int line_ = 1;
    int column_ = 1;
};

}  // namespace otne
