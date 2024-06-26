#include <iostream>
#include <fstream>
#include <codecvt>
#include "otne_file.hpp"
#include "otne_utf8.hpp"
#include "otne_lexer.hpp"
using namespace std;




int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");          // Linux MinGW GCC MSVC
    ios::sync_with_stdio(false);    // Linux gcc     

    // i18nString source = OtneReadFile("main.otne");

    i18nString source = OtneReadFile("resources/code/HelloWorld.otne");
    // i18nString source = OtneReadFile("resources/code/main.otne");
    wcout << source << endl;
    wcout << L"----------------------------------" << endl;
    lexer(source);
    return 0;
}



/*
for (int i = 0; i < argc ; i++) {
    if (i != 0) { command_func_line(argv[i]); }
}
*/

/*
func main () {
}

#include <stdio.h>

int main()
{
    // 我的第一个 C 程序
printf("Hello, World! \n");

return 0;
}
*/





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
