#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

// 跨平台的国际化字符串类型
// 源码统一按 UTF-8 读取，内部以宽字符处理
using i18nString = std::wstring;
using i18nChar = wchar_t;

// 宽字符串 -> UTF-8 字节串
// 输出统一使用 UTF-8，避免依赖运行环境的 locale 设置
inline std::string ToUtf8(const i18nString& text)
{
    std::string result;
    result.reserve(text.size());

    // 追加一个 Unicode 码点
    const auto append_code_point = [&result](std::uint32_t code_point) {
        if (code_point < 0x80) {
            result.push_back(static_cast<char>(code_point));
        } else if (code_point < 0x800) {
            result.push_back(static_cast<char>(0xC0 | (code_point >> 6)));
            result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
        } else if (code_point < 0x10000) {
            result.push_back(static_cast<char>(0xE0 | (code_point >> 12)));
            result.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
        } else {
            result.push_back(static_cast<char>(0xF0 | (code_point >> 18)));
            result.push_back(static_cast<char>(0x80 | ((code_point >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((code_point >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (code_point & 0x3F)));
        }
    };

    for (std::size_t i = 0; i < text.size(); ++i) {
        std::uint32_t code_point = static_cast<std::uint32_t>(text[i]);

        // Windows 下 wchar_t 为 UTF-16，高位代理需与低位代理合并
        if (sizeof(wchar_t) == 2 && code_point >= 0xD800 && code_point <= 0xDBFF &&
            i + 1 < text.size()) {
            const std::uint32_t low = static_cast<std::uint32_t>(text[i + 1]);
            if (low >= 0xDC00 && low <= 0xDFFF) {
                code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low - 0xDC00);
                ++i;
            }
        }

        append_code_point(code_point);
    }

    return result;
}
