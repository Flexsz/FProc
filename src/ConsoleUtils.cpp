#include "ConsoleUtils.h"
#include <iostream>
#include <string>

using namespace std;

namespace ConsoleUtils
{
    static bool s_cursorVisible = true;

    void setCursorVisibility(bool visible)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = visible;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        s_cursorVisible = visible;
    }

    bool isCursorVisible()
    {
        return s_cursorVisible;
    }

    void gotoxy(int x, int y)
    {
        COORD coords = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coords);
    }

    void clearArea(int x, int y, int width, int height)
    {
        setCursorVisibility(false);
        for (int i = 0; i < height; i++)
        {
            gotoxy(x, y + i);
            cout << string(width, ' ');
        }
        setCursorVisibility(true);
    }

    void setConsoleTitle(const string &title)
    {
        SetConsoleTitleA(title.c_str());
    }

    void setConsoleSize(int width, int height)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD size = {static_cast<SHORT>(width), static_cast<SHORT>(height)};
        SetConsoleScreenBufferSize(hConsole, size);
    }

    void moveConsoleWindow(int x, int y, int width, int height)
    {
        HWND consoleWindow = GetConsoleWindow();
        MoveWindow(consoleWindow, x, y, width, height, TRUE);
    }
}
