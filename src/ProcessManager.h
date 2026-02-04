#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <chrono>
#include <tlhelp32.h>
#include <psapi.h>
#include <cwchar>
struct ProcessInfo
{
    std::string name;
    DWORD pid;
    std::string memory;
};

class ProcessManager
{
private:
    static const int BOX_X;
    static const int BOX_Y;
    static const int BOX_WIDTH;
    static const int MAX_PROCESSES_DISPLAY;

    int currentScroll;
    std::vector<ProcessInfo> allProcesses;

    // private methods
    std::string getProcessMemoryUsage(DWORD pid);
    std::vector<ProcessInfo> getAllProcesses();
    void drawProcessList(int boxX, int boxY, int width, int totalProcesses);
    void writeFullLine(int x, int y, int width, const std::string &text, bool isBorder = false);

public:
    ProcessManager();
    // core funcs
    void initializeConsole();
    void run();
    void refreshCompleteScreen(int boxX, int boxY, int width);
    void updateRAMValues(int totalProcesses);
    void setWindowsTaskManagerTheme();
    void handleInput();

    // get funcs
    int getProcessCount() const { return static_cast<int>(allProcesses.size()); }
    int getCurrentScroll() const { return currentScroll; }
};

// custom theme function
void setWindowsTaskManagerTheme();
