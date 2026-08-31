#pragma once

#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include "Session.h"
#include "MasterManagement.h"
#include "LogsCode.h"

class Console {
private:
    Session session;
    MasterManagement masterManagement;

public:
    Console();
    
    void run();

    LogsCode commandCreate(std::queue<std::string> &commands);
    LogsCode commandSelect(std::queue<std::string> &commands);
    LogsCode commandInsert(std::queue<std::string> &commands);
    LogsCode commandDelete(std::queue<std::string> &commands);
    LogsCode commandDrop(std::queue<std::string> &commands);
    LogsCode commandUpdate(std::queue<std::string> &commands);
    LogsCode commandAlter(std::queue<std::string> &commands);
    LogsCode commandShow(std::queue<std::string> &commands);

    void printLogMessage(LogsCode code);
};
