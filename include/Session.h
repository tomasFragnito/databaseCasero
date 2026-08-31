#pragma once

#include <string>
#include "DataBase.h"
#include "Table.h"

using namespace std;

// Struct que mantiene la sesion actual activa del cliente/consola
struct Session {
    string user = "user";

    DataBase* selectDB = nullptr;
    Table* selectTable = nullptr;

    ~Session() {
        if (selectDB != nullptr) {
            delete selectDB;
            selectDB = nullptr;
        }
        if (selectTable != nullptr) {
            delete selectTable;
            selectTable = nullptr;
        }
    }
};
