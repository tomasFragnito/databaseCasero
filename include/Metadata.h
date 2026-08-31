#ifndef METADATA_H
#define METADATA_H

#include <string>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "DataBase.h"
#include "Table.h"
#include "LogsCode.h"

using json = nlohmann::json;
using namespace std;

class Metadata {
public:
    Metadata();
    
    json loadMetadata();
    void saveMetadata(json& metadata);

    bool existsDataBase(const string& name);
    bool existsTable(const string& nameTable, DataBase* database);
    
    LogsCode createMetadataFile();
    LogsCode addDataBase(DataBase& db);
    LogsCode addTable(const string& dbName, Table& table);
    LogsCode addColumn(const string& columnName, const string& dbName, Table& table);
    LogsCode removeDataBase(const string& dbName);
    LogsCode removeTable(const string& dbName, const string& tableName);
    LogsCode addConstraintToMetadata(const string& dbName, const string& tableName, const string& columnName, const string& constraintType, const string& refTable = "", const string& refCol = "");
    LogsCode showDatabases();
    LogsCode showTables(const string& dbName);
};

#endif
