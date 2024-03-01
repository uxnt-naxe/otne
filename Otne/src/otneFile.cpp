#include <iostream>
#include <fstream>
#include <codecvt>
#include "otne_utf8.h"
#include "otneFile.h"

using namespace std;

i18nString OtneReadFile(string filename)
{
    i18nChar text[4096];
    i18nString content;
    FILE *file;
    file = fopen(filename.c_str(), "rt+,ccs=UTF-8");
    if (file == NULL)
    {
        return L"File Open Error!";
    }
    while (fgetws(text, 4096, file))
    {
        content += wstring(text);
    }
    fclose(file);
    return content;
}