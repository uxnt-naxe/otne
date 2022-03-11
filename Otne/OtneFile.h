#ifndef _OTNE_FILE_HPP
#define _OTNE_FILE_HPP

#include <iostream>
#include <fstream>
#include <codecvt>
#include "Otne_utf8.h"

using namespace std;

void HWriteTxtFile(i18nString text, i18nString file) {
    wstring_convert<codecvt_utf8<i18nChar>> conv;
    ofstream ofs(file);
    ofs << conv.to_bytes(text);
    ofs.close();
}


i18nString HReadTxtFile(i18nString file)
{
    wstring_convert<codecvt_utf8<i18nChar>> conv;
    string line;
    i18nString str;
    ifstream ifs(file);
    if (!ifs) { return file + L" " + L"File Open Error!"; }
    while (!ifs.eof())
    {
        getline(ifs, line);
        str += conv.from_bytes(line) + L"\n";
    }
    ifs.close();
    return str;
}



i18nString OtneReadFile(i18nString file) {
    wstring_convert<codecvt_utf8<i18nChar>> conv;
    ifstream ifs(file);
    string line;
    i18nString str;
    if (!ifs) { return file + L" " + L"File Open Error!"; }
    while (!ifs.eof()) {
        getline(ifs, line);
        str += conv.from_bytes(line) + L"\n";
    }
    ifs.close();
    return str;
}


void OtneWriteFile(i18nString text, i18nString file) {
    wstring_convert<codecvt_utf8<i18nChar>> conv;
    ofstream ofs(file);
    ofs << conv.to_bytes(text);
    ofs.close();
}


#endif /*_OTNE_FILE_HPP*/