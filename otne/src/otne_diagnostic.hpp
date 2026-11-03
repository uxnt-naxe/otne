#pragma once

#include <string>

#include "otne_utf8.hpp"

namespace otne {

// 编译期诊断（词法 / 语法错误）
struct Diagnostic {
    int line = 0;
    int column = 0;
    i18nString message;
};

// 格式化为 UTF-8 文本：error: 文件:行:列: 消息
inline std::string FormatDiagnostic(const Diagnostic& diagnostic, const std::string& source_name)
{
    std::string text = "error: ";
    if (!source_name.empty()) {
        text += source_name;
        text += ':';
        text += std::to_string(diagnostic.line);
        text += ':';
        text += std::to_string(diagnostic.column);
        text += ": ";
    }
    text += ToUtf8(diagnostic.message);
    return text;
}

}  // namespace otne
