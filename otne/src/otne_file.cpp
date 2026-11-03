#include "otne_file.hpp"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>

namespace {

// 将 UTF-8 字节流解码为宽字符串
// 非法或截断的字节会被跳过，避免影响后续词法分析
void AppendUtf8(std::wstring& out, const std::string& bytes)
{
    std::size_t i = 0;
    while (i < bytes.size()) {
        const unsigned char lead = static_cast<unsigned char>(bytes[i]);
        std::uint32_t code_point = 0;
        std::size_t length = 0;

        if (lead < 0x80) {
            code_point = lead;
            length = 1;
        } else if ((lead & 0xE0) == 0xC0) {
            code_point = lead & 0x1F;
            length = 2;
        } else if ((lead & 0xF0) == 0xE0) {
            code_point = lead & 0x0F;
            length = 3;
        } else if ((lead & 0xF8) == 0xF0) {
            code_point = lead & 0x07;
            length = 4;
        } else {
            ++i;  // 非法起始字节，直接跳过
            continue;
        }

        if (i + length > bytes.size()) {
            break;  // 截断的多字节序列
        }

        for (std::size_t k = 1; k < length; ++k) {
            code_point = (code_point << 6) |
                         (static_cast<unsigned char>(bytes[i + k]) & 0x3F);
        }

        if (sizeof(wchar_t) == 2 && code_point > 0xFFFF) {
            // Windows 下 wchar_t 为 UTF-16，需要拆成代理对
            code_point -= 0x10000;
            out.push_back(static_cast<wchar_t>(0xD800 + (code_point >> 10)));
            out.push_back(static_cast<wchar_t>(0xDC00 + (code_point & 0x3FF)));
        } else {
            out.push_back(static_cast<wchar_t>(code_point));
        }

        i += length;
    }
}

}  // namespace

bool OtneReadFile(const std::string& filename, i18nString& out)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    const std::string bytes((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

    i18nString content;
    content.reserve(bytes.size());
    AppendUtf8(content, bytes);

    out = std::move(content);
    return true;
}
