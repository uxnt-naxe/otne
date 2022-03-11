#include <iostream>
#include <string>
#include <fstream>
#include <codecvt>
using namespace std;

/*
typedef wstring Hstring;
typedef wchar_t Hchar;

// 文件是utf8编码
void HWriteTxtFile(Hstring text, Hstring file) {
    wstring_convert<codecvt_utf8<Hchar>> conv;
    ofstream outfile;
    outfile.open(file);
    outfile << conv.to_bytes(text);
    outfile.close();
}

Hstring HReadTxtFile(Hstring file)
{
    wstring_convert<codecvt_utf8<Hchar>> conv;
    string line;
    Hstring str;

    ifstream ifs(file);
    if (!ifs) { return L"File open error!"; }

    while (!ifs.eof())
    {
        getline(ifs, line);
        str += conv.from_bytes(line) + L"\n";
    }
    ifs.close();
    return str;
}
*/



int main () {


    printf("Hello");


    

   // Hstring Hello = L"Hello";

    //HWriteTxtFile(Hello, L"hello.otne");
    return 0;
}




