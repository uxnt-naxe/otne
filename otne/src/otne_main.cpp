#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "otne_diagnostic.hpp"
#include "otne_file.hpp"
#include "otne_interpreter.hpp"
#include "otne_lexer.hpp"
#include "otne_parser.hpp"
#include "otne_utf8.hpp"

namespace {

const char* const kDefaultSource = "resources/code/HelloWorld.otne";
const char* const kVersion = "0.2.0";

void PrintUsage()
{
    std::cout << "otne —— 中引语言解释器\n\n"
              << "用法: otne [选项] [源文件]\n\n"
              << "选项:\n"
              << "  -h, --help      显示帮助\n"
              << "  -v, --version   显示版本\n"
              << "  -t, --tokens    只输出词法单元，不执行\n\n"
              << "未指定源文件时运行 " << kDefaultSource << "\n";
}

void InitConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::ios::sync_with_stdio(false);
}

void ReportDiagnostics(const std::vector<otne::Diagnostic>& diagnostics,
                       const std::string& filename)
{
    for (const auto& diagnostic : diagnostics) {
        std::cerr << otne::FormatDiagnostic(diagnostic, filename) << '\n';
    }
}

void PrintTokens(const std::vector<otne::Token>& tokens)
{
    for (const auto& token : tokens) {
        std::cout << token.line << ':' << token.column << "  " << otne::TokenTypeName(token.type);
        if (!token.lexeme.empty()) {
            std::cout << "  " << ToUtf8(token.lexeme);
        }
        std::cout << '\n';
    }
}

}  // namespace

int main(int argc, char* argv[])
{
    InitConsole();

    std::string filename;
    bool dump_tokens = false;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument == "-h" || argument == "--help") {
            PrintUsage();
            return 0;
        }
        if (argument == "-v" || argument == "--version") {
            std::cout << "otne " << kVersion << '\n';
            return 0;
        }
        if (argument == "-t" || argument == "--tokens") {
            dump_tokens = true;
            continue;
        }
        if (argument.size() > 1 && argument[0] == '-') {
            std::cerr << "error: 未知选项 " << argument << '\n';
            return 2;
        }
        filename = argument;
    }

    if (filename.empty()) {
        filename = kDefaultSource;
    }

    i18nString source;
    if (!OtneReadFile(filename, source)) {
        std::cerr << "error: 无法打开文件 " << filename << '\n';
        return 1;
    }

    otne::Lexer lexer(source);
    std::vector<otne::Diagnostic> diagnostics;
    std::vector<otne::Token> tokens = lexer.Scan(diagnostics);
    if (!diagnostics.empty()) {
        ReportDiagnostics(diagnostics, filename);
        return 1;
    }

    if (dump_tokens) {
        PrintTokens(tokens);
        return 0;
    }

    otne::Program program;
    try {
        otne::Parser parser(tokens);
        program = parser.Parse();
    } catch (const otne::ParseError& error) {
        ReportDiagnostics({error.diagnostic}, filename);
        return 1;
    }

    try {
        otne::Interpreter interpreter;
        interpreter.Run(program);
    } catch (const otne::RuntimeError& error) {
        ReportDiagnostics({error.diagnostic}, filename);
        return 1;
    }

    return 0;
}
