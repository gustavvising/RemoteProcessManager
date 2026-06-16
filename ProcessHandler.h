#pragma once

#include <vector>
#include <memory>
#include <string>
#include <windows.h>
#include "Process.h"

class ProcessHandler {
private:
    std::vector<std::unique_ptr<Process>> processList;

public:
    bool updateProcessList();
    bool killProcessByPid(DWORD pid);
    bool suspendProcess(DWORD pid);
    bool resumeProcess(DWORD pid);
    void printProcessList();
};