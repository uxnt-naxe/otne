#pragma once

#include <string>

#include "otne_utf8.hpp"

// 读取 UTF-8 编码的源文件并转换为宽字符串
// 成功返回 true；失败返回 false，且不修改 out
bool OtneReadFile(const std::string& filename, i18nString& out);
