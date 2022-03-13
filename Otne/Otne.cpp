#include <iostream>
#include <fstream>
#include <codecvt>
#include <windows.h>

#include "OtneFile.h"
#include "Otne_utf8.h"
#include "lexer/lexer.h"

using namespace std;

/*
void command_func_line(string str)
{
    if (str == "-help" || str == "-?")
    {
        wcout << OtneReadFile("help.txt");
    }
    else if (str == "-v" || str == "-version" || str == "-version")
    {
        wcout << L"Otne version 0.0.1" << endl;
    }
    else
    {
        i18nString source = OtneReadFile(str);
        lexer(source);
    }
}
*/
int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");            // Linux MinGW GCC MSVC
    ios::sync_with_stdio(false);      // Linux gcc

    i18nString source = OtneReadFile("code.otne");
    wcout << source << endl;
    wcout << L"----------------------------------" << endl;
    lexer(source);


    /*
    for (int i = 0; i < argc ; i++) {
        if (i != 0) { command_func_line(argv[i]); }
    }
    */

    return 0;
}
