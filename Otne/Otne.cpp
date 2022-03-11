#include <iostream>
#include <fstream>
#include <codecvt>
#include <windows.h>

#include "OtneFile.h"
#include "Otne_utf8.h"
#include "lexer/lexer.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "");  // MinGW gcc / MSVC
    std::ios_base::sync_with_stdio(false);  // Linux gcc
    std::ios::sync_with_stdio(false);    // Linux gcc

    // wcout << "你好，世界" << endl;
    
    i18nString source = OtneReadFile(L"code.txt");
    
    wcout << source << endl;
    wcout << L"----------------------------------" << endl;
    Htoken(source);
}