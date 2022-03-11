#include <iostream>
#include <fstream>
#include <codecvt>
#include <windows.h>

#include "OtneFile.h"
#include "Otne_utf8.h"
#include "lexer/lexer.h"

using namespace std;

void command_func_line(string str) {

    wstring_convert<codecvt_utf8<i18nChar>> conv;
    if (str == "-help" || str == "-?") {
        wcout << OtneReadFile(L"help.txt");
    }
    else if(str == "-v" || str == "-version" || str == "-version")
    {
        wcout << L"Otne version 0.0.1" << endl;
    }
    else {
        i18nString source = OtneReadFile(conv.from_bytes(str));
        //wcout << source << endl;
        //wcout << L"----------------------------------" << endl;
        Htoken(source);
    }
}

int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "");  // MinGW gcc / MSVC
    std::ios_base::sync_with_stdio(false);  // Linux gcc
    std::ios::sync_with_stdio(false);    // Linux gcc
    for (int i = 0; i < argc ; i++) {
        if (i != 0) { command_func_line(argv[i]); }
    }
    return 0;
}