// NOTE: Console GUI Implementation
// --------------------------------
// This code manages the real-time display of process information.
// It uses careful cursor positioning to avoid screen flickering.
// 
// Important: Modifying coordinate calculations may break the layout.
// Test any changes with various screen sizes.
// 
// Author: Flexsz
// Last Modified: Last Modified: 2026 February 5

#include "ProcessManager.h"
#include "ConsoleUtils.h"
#include <iostream>
#include <tlhelp32.h>
#include <iomanip>
#include <sstream>
#include <psapi.h>

using namespace std;

const int ProcessManager::BOX_X = 5;
const int ProcessManager::BOX_Y = 3;
const int ProcessManager::BOX_WIDTH = 80;
const int ProcessManager::MAX_PROCESSES_DISPLAY = 35;

ProcessManager::ProcessManager() : currentScroll(0)
{
}

void ProcessManager::initializeConsole()
{
    ConsoleUtils::setConsoleTitle("FProc Manager by Flexsz (v3.7)");
    ConsoleUtils::moveConsoleWindow(100, 100, 900, 950);
    ConsoleUtils::setConsoleSize(BOX_WIDTH + 10, MAX_PROCESSES_DISPLAY + 20);

    HWND consoleWindow = GetConsoleWindow();
    SetWindowPos(consoleWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    system("cls");
}

void ProcessManager::run()
{
    refreshCompleteScreen(BOX_X, BOX_Y, BOX_WIDTH);

    while (true)
    {
        handleInput();
        Sleep(50); // set whatever you want, idk
    }
}

string ProcessManager::getProcessMemoryUsage(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess)
    {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
        {
            CloseHandle(hProcess);
            DWORD mb = pmc.WorkingSetSize / (1024 * 1024);
            return to_string(mb) + " MB";
        }
        CloseHandle(hProcess);
    }
    return "N/A";
}

vector<ProcessInfo> ProcessManager::getAllProcesses()
{
    vector<ProcessInfo> processes;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap == INVALID_HANDLE_VALUE)
    {
        return processes;
    }

    PROCESSENTRY32 pe = {sizeof(pe)};

    if (Process32First(hSnap, &pe))
    {
        do
        {
            ProcessInfo info;
            info.pid = pe.th32ProcessID;
            info.name = pe.szExeFile;
            info.memory = getProcessMemoryUsage(pe.th32ProcessID);
            processes.push_back(info);
        } while (Process32Next(hSnap, &pe));
    }

    CloseHandle(hSnap);
    return processes;
}


void ProcessManager::writeFullLine(int x, int y, int width, const string &text, bool isBorder)
{
    ConsoleUtils::gotoxy(x, y);

    if (isBorder)
    {
        cout << string(width, '#');
        return;
    }

    cout << "#";

    int innerWidth = width - 2;
    string displayText = text;

    if (displayText.length() > innerWidth)
    {
        displayText = displayText.substr(0, innerWidth - 3) + "...";
    }

    cout << displayText;

    int remaining = innerWidth - displayText.length();
    if (remaining > 0)
    {
        cout << string(remaining, ' ');
    }

    cout << "#";
}

void ProcessManager::drawProcessList(int boxX, int boxY, int width, int totalProcesses)
{
    int startY = boxY + 2;

    for (int i = 0; i < MAX_PROCESSES_DISPLAY; i++)
    {
        int processIndex = currentScroll + i;
        ConsoleUtils::gotoxy(boxX, startY + i);

        if (processIndex < totalProcesses)
        {
            char buffer[120];
            snprintf(buffer, sizeof(buffer), "%3d. %-30s [PID:%-8d] RAM: %-10s",
                     processIndex + 1,
                     allProcesses[processIndex].name.c_str(),
                     allProcesses[processIndex].pid,
                     allProcesses[processIndex].memory.c_str());

            cout << "# " << buffer;

            int textLen = strlen(buffer) + 2;
            int remaining = width - 2 - textLen;
            if (remaining > 0)
            {
                cout << string(remaining, ' ');
            }
            cout << "#";
        }
        else
        {
            cout << "#" << string(width - 2, ' ') << "#";
        }
    }
}

void ProcessManager::updateRAMValues(int totalProcesses)
{
    int startY = BOX_Y + 2;

    for (int i = 0; i < MAX_PROCESSES_DISPLAY; i++)
    {
        int processIndex = currentScroll + i;
        if (processIndex < totalProcesses)
        {
            int ramStartX = BOX_X + 58;

            ConsoleUtils::gotoxy(ramStartX, startY + i);
            cout << "          ";
            ConsoleUtils::gotoxy(ramStartX, startY + i);

            string newRAM = getProcessMemoryUsage(allProcesses[processIndex].pid);
            allProcesses[processIndex].memory = newRAM;
            cout << newRAM;
        }
    }
}

void ProcessManager::setWindowsTaskManagerTheme()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND console = GetConsoleWindow();

    SetWindowLong(console, GWL_EXSTYLE, GetWindowLong(console, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(console, 0, 240, LWA_ALPHA);

    CONSOLE_SCREEN_BUFFER_INFOEX csb;
    csb.cbSize = sizeof(csb);
    GetConsoleScreenBufferInfoEx(hConsole, &csb);

    csb.ColorTable[0] = RGB(30, 30, 35);
    csb.ColorTable[1] = RGB(0, 90, 158);
    csb.ColorTable[7] = RGB(240, 240, 240);
    csb.ColorTable[8] = RGB(180, 180, 180);
    csb.ColorTable[9] = RGB(255, 80, 80);
    csb.ColorTable[10] = RGB(80, 200, 120);
    csb.ColorTable[11] = RGB(255, 200, 50);
    csb.ColorTable[12] = RGB(0, 120, 212);
    csb.ColorTable[13] = RGB(200, 80, 200);

    SetConsoleScreenBufferInfoEx(hConsole, &csb);

    CONSOLE_FONT_INFOEX font;
    font.cbSize = sizeof(font);
    GetCurrentConsoleFontEx(hConsole, FALSE, &font);
    wcscpy_s(font.FaceName, L"Consolas");
    font.dwFontSize.Y = 16;
    SetCurrentConsoleFontEx(hConsole, FALSE, &font);

    system("cls");
    SetConsoleTextAttribute(hConsole, 0x07);
}

void ProcessManager::refreshCompleteScreen(int boxX, int boxY, int width)
{
    ConsoleUtils::setCursorVisibility(false);

    allProcesses = getAllProcesses();
    int totalProcesses = static_cast<int>(allProcesses.size());

    ConsoleUtils::clearArea(boxX, boxY, width, MAX_PROCESSES_DISPLAY + 6);

    writeFullLine(boxX, boxY, width, "", true);

    string title = "  PROCESS MANAGER - TOTAL: " + to_string(totalProcesses) +
                   " | Showing: " + to_string(min(MAX_PROCESSES_DISPLAY, totalProcesses)) +
                   " | Scroll: " + to_string(currentScroll) + "/" + to_string(totalProcesses) + " ";
    writeFullLine(boxX, boxY + 1, width, title);

    drawProcessList(boxX, boxY, width, totalProcesses);
    writeFullLine(boxX, boxY + MAX_PROCESSES_DISPLAY + 3, width, "", true);

    writeFullLine(boxX, boxY + MAX_PROCESSES_DISPLAY + 4, width,
                  " R=Refresh RAM | F=Full Refresh | Q=Quit | UP/DOWN=Scroll ");

    ConsoleUtils::gotoxy(boxX, boxY + MAX_PROCESSES_DISPLAY + 6);

    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm local_tm;
    localtime_s(&local_tm, &now_time);

    stringstream ss;
    ss << put_time(&local_tm, "%H:%M:%S");
    cout << "Last update: " << ss.str() << " | Total processes: " << totalProcesses;

    ConsoleUtils::setCursorVisibility(true);
}

void ProcessManager::handleInput()
{
    if (GetAsyncKeyState('R') & 0x8000)
    {
        updateRAMValues(static_cast<int>(allProcesses.size()));

        ConsoleUtils::gotoxy(BOX_X, BOX_Y + MAX_PROCESSES_DISPLAY + 6);
        auto now = chrono::system_clock::now();
        time_t now_time = chrono::system_clock::to_time_t(now);
        tm local_tm;
        localtime_s(&local_tm, &now_time);

        stringstream ss;
        ss << put_time(&local_tm, "%H:%M:%S");
        cout << "RAM updated at " << ss.str() << " | Total: " << allProcesses.size();

        Sleep(200);
    }

    if (GetAsyncKeyState('F') & 0x8000)
    {
        refreshCompleteScreen(BOX_X, BOX_Y, BOX_WIDTH);
        Sleep(200);
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        if (currentScroll > 0)
        {
            currentScroll--;
            refreshCompleteScreen(BOX_X, BOX_Y, BOX_WIDTH);
        }
        Sleep(100);
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        if (currentScroll + MAX_PROCESSES_DISPLAY < allProcesses.size())
        {
            currentScroll++;
            refreshCompleteScreen(BOX_X, BOX_Y, BOX_WIDTH);
        }
        Sleep(100);
    }

    if (GetAsyncKeyState(VK_PRIOR) & 0x8000)
    {
        currentScroll = max(0, currentScroll - MAX_PROCESSES_DISPLAY);
        refreshCompleteScreen(BOX_X, BOX_Y, BOX_WIDTH);
        Sleep(100);
    }

    if (GetAsyncKeyState(VK_NEXT) & 0x8000)
    {
        currentScroll = min(static_cast<int>(allProcesses.size()) - MAX_PROCESSES_DISPLAY,
                            currentScroll + MAX_PROCESSES_DISPLAY);
        refreshCompleteScreen(BOX_X, BOX_Y, BOX_WIDTH);
        Sleep(100);
    }

    if (GetAsyncKeyState('Q') & 0x8000)
    {
        system("cls");
        exit(0);
    }

    static DWORD lastAutoRefresh = GetTickCount();
    if (GetTickCount() - lastAutoRefresh > 1000)
    {
        updateRAMValues(static_cast<int>(allProcesses.size()));
        lastAutoRefresh = GetTickCount();
    }
}
