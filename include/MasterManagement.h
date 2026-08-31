#ifndef MASTERMANAGEMENT_H
#define MASTERMANAGEMENT_H

#include <string>
#include <queue>
#include <vector>
#include "Session.h"
#include "DataBase.h"
#include "Table.h"
#include "Metadata.h"
#include "LogsCode.h"

using namespace std;

class MasterManagement {
private:
    Session& session;
    Metadata metadata;

public:
    MasterManagement(Session& session);

    Session& getSession();

    bool existsDataBase(string name);
    bool existsTable(string nameTable, DataBase* database);
    bool hasSelectDB();
    bool hasSelectTable();

    LogsCode addDataBase(DataBase& db);
    LogsCode addTable(string dbName, Table& table);
    LogsCode selectDataBase(string name);
    LogsCode selectTable(DataBase& db, string tableName);
    LogsCode addColumn(string columnName);

    // Metodos de orquestacion con validacion pesada
    LogsCode executeCreateDatabase(string nameDB);
    LogsCode executeCreateTable(string nameTable);
    LogsCode executeSelectDatabase(string nameDB);
    LogsCode executeSelectTable(string nameTable);
    LogsCode executeInsertColumn(string columnName);
    LogsCode executeInsertValues(const vector<string>& columns, const vector<string>& values);
    LogsCode executeDeleteColumn(string nameColumn);
    LogsCode executeDeleteRecord(string columnName, string value);
    LogsCode executeUpdateRecord(string filterCol, string filterVal, string targetCol, string newVal);
    LogsCode executeDropDatabase(string nameDB);
    LogsCode executeDropTable(string nameTable);
    
    // Muestreo de datos
    LogsCode executeSelectAll();
    LogsCode executeSelectOrdered(string columnName, bool ascending);
    LogsCode executeSelectFilter(string columnName, string value);
    
    // Asignación de Constraints
    LogsCode executeAddConstraint(string columnName, string constraintType, string refTable = "", string refCol = "");

    LogsCode executeShowDatabases();
    LogsCode executeShowTables();
};

#endif
