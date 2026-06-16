#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include "ProcessHandler.h"
#include "Process.h"

bool ProcessHandler::updateProcessList() {
    DWORD dwFlags = TH32CS_SNAPPROCESS;
    DWORD th32ProcessID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(dwFlags, th32ProcessID);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cerr << "CreateToolhelp32Snapshot failed. Error: " << error << std::endl;
        return false;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            processList.push_back(std::make_unique<Process>(pe.szExeFile, pe.th32ProcessID));
        } while (Process32Next(hSnapshot, &pe));
    }
    else {
        DWORD error = GetLastError();
        std::cerr << "Process32First failed. Error: " << error << std::endl;
        CloseHandle(hSnapshot);
        return false;
    }

    CloseHandle(hSnapshot);

    return true;
}

void ProcessHandler::printProcessList() {
    for (const auto& process : processList) {
        std::wcout << "Name: " << process->getName()
                   << " Pid: " << process->getPid() << std::endl;
    }
}

bool ProcessHandler::killProcessByPid(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

    if (hProcess == NULL) {
        std::cerr << "Error: Could not open process. Error code: " << GetLastError() << std::endl;
        return false;
    }

    if (!TerminateProcess(hProcess, 1)) {
        std::cerr << "Error: TerminateProcess failed. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    CloseHandle(hProcess);
    return true;
}

bool setProcessStateByPid(DWORD pid, bool resume) {
    HANDLE hSnapThreads = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapThreads == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Could not create snapshot. Error code: " << GetLastError() << std::endl;
        return false;
    }

    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hSnapThreads, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread != NULL) {
                    if (resume) {
                        ResumeThread(hThread);
                    }
                    else {
                        SuspendThread(hThread);
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapThreads, &te));
    }
    else {
        std::cerr << "Error: Could not traverse threads. Error code: " << GetLastError() << std::endl;
        CloseHandle(hSnapThreads);
        return false;
    }

    CloseHandle(hSnapThreads);
    return true;
}

bool ProcessHandler::suspendProcess(DWORD pid) {
    return setProcessStateByPid(pid, false);
}

bool ProcessHandler::resumeProcess(DWORD pid) {
    return setProcessStateByPid(pid, true);
}