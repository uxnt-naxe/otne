#include <iostream>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <codecvt>

#include "../Otne_utf8.h"
#include "lexer.h"

using namespace std;

i18nString key[] =
{
    L"import", L"func", L"main", L"num", L"text", L"bool", L"return",
    L"导入", L"函数", L"主码", L"数存", L"文本", L"布尔", L"返回"
};






int isKey(i18nString token)
{
    for (int i = 0; i < 14; i++)
    {
        if (token == key[i])
        {
            return true;
        }
    }
    return false;
}

bool isChar(i18nChar ch)
{
    if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) { return true; }
    else { return false; }
}

bool isEChar(i18nChar ch)
{
    if ((ch >= L'а' && ch <= L'я') || (ch >= L'А' && ch <= L'Я')) { return true; }
    else { return false; }
}



bool isZHchar(i18nChar ch)
{
    if ((ch >= L'一' && ch <= L'龥') || isChar(ch)) { return true; }
    else { return false; }
}

bool isNum(i18nChar ch)
{
    if (ch >= L'0' && ch <= L'9') { return true; }
    else { return false; }
}




// 自制词法分析器
void Htoken(i18nString Text) {
    i18nChar ch;
    int stlPos = 0;
    i18nString token = L"";

    while (stlPos < Text.length()) {
        ch = Text[stlPos];

        if (isZHchar(ch)) {
            while (isZHchar(ch)) {
                token += ch;
                stlPos++;
                ch = Text[stlPos];
            }
            if (isKey(token)) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                wcout << token;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                token = L"";
            }
            else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
                wcout << token;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                token = L"";
            }
        }


        else if (isNum(ch)) {
            while (isNum(ch) || (ch == L'.')) {
                token += ch;
                stlPos++;
                ch = Text[stlPos];
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
        }

        else if (ch == L'(') {

            token += ch;
            stlPos++;
            ch = Text[stlPos];

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
        }
        else if (ch == L')') {

            token += ch;
            stlPos++;
            ch = Text[stlPos];

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
        }

        else if (ch == L'{') {

            token += ch;
            stlPos++;
            ch = Text[stlPos];

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
        }
        else if (ch == L'}') {

            token += ch;
            stlPos++;
            ch = Text[stlPos];

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
        }



        else if (ch == L'\"') {
            bool s = 1;
            token += ch;
            while (s)
            {
                stlPos++;
                ch = Text[stlPos];
                token += ch;
                if (ch == L'\"') { s = 0; }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
            stlPos++;
        }

        else {
            wcout << ch;
            stlPos++;
        }






        /*
        if (isZHchar(ch)) {
            token += ch;
            while (isZHchar(ch)) {
                ch = Text[stlPos++];
                //stlPos++;
                token += ch;
            }
            if (isKey(token)) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                wcout << token;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                token = L"";
            }
        }
        else if (ch == L'\"') {
            bool ol = 1;
            token += ch;
            while(ol)
            {
                ch = Text[stlPos++];
                token += ch;
                if (ch == L'\"') { ol = 0; }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
        }
        else {

            wcout << ch;

            ch = Text[stlPos++];
        }
        */



    }
}






/*
// 中引
导入 终端屏幕;
函数 主码()
{
    终端屏幕("你好，宇宙");
}
*/







/*
        else if (ch == L'(') {
            bool s = 1;
            token += ch;
            while (s)
            {
                stlPos++;
                ch = Text[stlPos];
                token += ch;
                if (ch == L')') { s = 0; }
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            wcout << token;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            token = L"";
            stlPos++;
        }
        */







 