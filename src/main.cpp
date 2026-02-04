#include "ProcessManager.h"

int main() {
    // Main entry point for Windows Process Monitor
    // Initializes and runs the process management system
    // Author: Flexsz

    ProcessManager manager;
    
    manager.initializeConsole();
    
    // set theme
    manager.setWindowsTaskManagerTheme();
    
    // run main loop
    manager.run();
    
    return 0;
}
