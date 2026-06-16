#pragma once
#include <windows.h>
#include <string>

class Process {
private:
    std::wstring name;
    DWORD pid;

public:
    Process(std::wstring processName, DWORD processPid)
        : name(processName), pid(processPid) {
    }

    std::wstring getName() const {
        return name;
        }

    DWORD getPid() const {
            return pid;
    }

};
