#include <iostream>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <codecvt>

#include "../Otne_utf8.h"
#include "lexer.h"

using namespace std;



template<class T>

int getLength(T& arr)
{
    return sizeof(arr) / sizeof(arr[0]);
}


i18nString KeyWord[] = {
    L"import", 
    L"static", 
    L"func", L"num", L"text", L"string", 
    L"return", 
    L"bool", L"true", L"false", L"null", 
    L"if", L"elif", L"else", L"for", L"while", L"break" 
};

int isKeyWord(i18nString token) {
    for (int i = 0; i < getLength(KeyWord); i++) {
        if (token == KeyWord[i]) { 
            return true; 
        }
    }
    return false;
}

bool isEnglish(i18nChar ch) {
    if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) { return true; }
    else { return false; }
}

bool isRussian(i18nChar ch) {
    if ((ch >= L'а' && ch <= L'я') || (ch >= L'А' && ch <= L'Я')) { return true; }
    else { return false; }
}

bool isChinese(i18nChar ch) {
    if ((ch >= L'一' && ch <= L'龥')) { return true; }
    else { return false; }
}

bool isText(i18nChar ch) {
    if (isEnglish(ch) || isChinese(ch)) { return true; }
    else { return false; }
}

bool isNum(i18nChar ch)
{
    if (ch >= L'0' && ch <= L'9') { return true; }
    else { return false; }
}




// 词法分析器 lexer  Token
void lexer(i18nString Text) {
    i18nChar ch;
    int stlPos = 0;
    i18nString token = L"";

    while (stlPos < Text.length()) {
        ch = Text[stlPos];

        if (isText(ch)) {
            while (isText(ch)) {
                token += ch;
                stlPos++;
                ch = Text[stlPos];
            }
            if (isKeyWord(token)) {
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







 