#pragma once

#include <windows.h>
#include <string>

namespace ConsoleUtils {
    // Cursor control
    void setCursorVisibility(bool visible);
    bool isCursorVisible();
    
    // Console positioning
    void gotoxy(int x, int y);
    void clearArea(int x, int y, int width, int height);
    
    // Console setup
    void setConsoleTitle(const std::string& title);
    void setConsoleSize(int width, int height);
    void moveConsoleWindow(int x, int y, int width, int height);
}
