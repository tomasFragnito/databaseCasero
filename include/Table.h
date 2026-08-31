#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>
#include <fstream>
#include "DataBase.h"
#include "LogsCode.h"
#include "Constraints.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class Table {
private:
    string name;
    string path;
    Constraints constraints;

public:
    Table(string name, DataBase& db);

    string getName() const;
    string getPath() const;

    LogsCode create();
    LogsCode addColumn(const string& columnName);
    LogsCode addValues(const vector<string>& columns, const vector<string>& values, const json& dbMetadata);
    
    // Muestreo de datos
    void displayContent() const;
    void displayContentOrdered(const string& columnName, bool ascending) const;
    void displayContentFilter(const string& columnName, const string& value) const;

    // Eliminación y actualización de registros
    LogsCode deleteRecord(const string& columnName, const string& value);
    LogsCode updateRecord(const string& filterCol, const string& filterVal, const string& targetCol, const string& newVal);
};

#endif
