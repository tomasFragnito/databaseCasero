#include "../include/MasterManagement.h"
#include <iostream>

using namespace std;

MasterManagement::MasterManagement(Session& session) : session(session) {}

Session& MasterManagement::getSession() {
    return this->session;
}

bool MasterManagement::existsDataBase(string name) {
    return this->metadata.existsDataBase(name);
}

bool MasterManagement::existsTable(string nameTable, DataBase* database) {
    return this->metadata.existsTable(nameTable, database);
}

bool MasterManagement::hasSelectDB() {
    if (this->session.selectDB != nullptr) {
        return true;
    } else {
        return false;
    }
}

bool MasterManagement::hasSelectTable() {
    if (this->session.selectTable != nullptr) {
        return true;
    } else {
        return false;
    }
}

LogsCode MasterManagement::addDataBase(DataBase& db) {
    return this->metadata.addDataBase(db);
}

LogsCode MasterManagement::addTable(string dbName, Table& table) {
    return this->metadata.addTable(dbName, table);
}

LogsCode MasterManagement::selectDataBase(string name) {
    if (!this->existsDataBase(name)) {
        return LogsCode::DATABASE_NOT_FOUND;
    }

    if (this->session.selectDB != nullptr) {
        delete this->session.selectDB;
    }

    this->session.selectDB = new DataBase(name);
    
    // Al cambiar de DB, reseteamos la tabla seleccionada
    if (this->session.selectTable != nullptr) {
        delete this->session.selectTable;
        this->session.selectTable = nullptr;
    }

    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::selectTable(DataBase& db, string tableName) {
    if (!this->existsTable(tableName, &db)) {
        return LogsCode::TABLE_NOT_FOUND;
    }

    if (this->session.selectTable != nullptr) {
        delete this->session.selectTable;
    }

    this->session.selectTable = new Table(tableName, db);

    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::addColumn(string columnName) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }

    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::executeCreateDatabase(string nameDB) {
    if (this->metadata.existsDataBase(nameDB)) {
        return LogsCode::DATABASE_ALREADY_EXISTS;
    }

    DataBase db(nameDB);
    db.create();
    return this->metadata.addDataBase(db);
}

LogsCode MasterManagement::executeCreateTable(string nameTable) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }

    if (this->metadata.existsTable(nameTable, this->session.selectDB)) {
        return LogsCode::TABLE_ALREADY_EXISTS;
    }

    Table table(nameTable, *this->session.selectDB);
    LogsCode log = table.create();
    if (log != LogsCode::SUCCESS) {
        return log;
    }

    return this->metadata.addTable(this->session.selectDB->getName(), table);
}

LogsCode MasterManagement::executeSelectDatabase(string nameDB) {
    return this->selectDataBase(nameDB);
}

LogsCode MasterManagement::executeSelectTable(string nameTable) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }

    return this->selectTable(*this->session.selectDB, nameTable);
}

LogsCode MasterManagement::executeInsertColumn(string columnName) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    LogsCode log = this->session.selectTable->addColumn(columnName);
    if (log != LogsCode::SUCCESS) {
        return log;
    }

    return this->metadata.addColumn(columnName, this->session.selectDB->getName(), *this->session.selectTable);
}

LogsCode MasterManagement::executeInsertValues(const vector<string>& columns, const vector<string>& values) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    json dbMeta = json::object();
    json metadataAll = this->metadata.loadMetadata();
    if (metadataAll.contains("databases")) {
        for (const auto& db : metadataAll["databases"]) {
            if (db["name"] == this->session.selectDB->getName()) {
                dbMeta = db;
                break;
            }
        }
    }

    return this->session.selectTable->addValues(columns, values, dbMeta);
}

LogsCode MasterManagement::executeDeleteColumn(string nameColumn) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::executeDeleteRecord(string columnName, string value) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    return this->session.selectTable->deleteRecord(columnName, value);
}

LogsCode MasterManagement::executeUpdateRecord(string filterCol, string filterVal, string targetCol, string newVal) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    return this->session.selectTable->updateRecord(filterCol, filterVal, targetCol, newVal);
}

LogsCode MasterManagement::executeDropDatabase(string nameDB) {
    if (!this->metadata.existsDataBase(nameDB)) {
        return LogsCode::DATABASE_NOT_FOUND;
    }

    string path = "databases/" + nameDB;
    if (filesystem::exists(path)) {
        filesystem::remove_all(path);
    }

    if (this->session.selectDB != nullptr && this->session.selectDB->getName() == nameDB) {
        delete this->session.selectDB;
        this->session.selectDB = nullptr;
        
        if (this->session.selectTable != nullptr) {
            delete this->session.selectTable;
            this->session.selectTable = nullptr;
        }
    }

    return this->metadata.removeDataBase(nameDB);
}

LogsCode MasterManagement::executeDropTable(string nameTable) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }

    if (!this->metadata.existsTable(nameTable, this->session.selectDB)) {
        return LogsCode::TABLE_NOT_FOUND;
    }

    string path = this->session.selectDB->getPath() + "/" + nameTable + ".table";
    if (filesystem::exists(path)) {
        filesystem::remove(path);
    }

    if (this->session.selectTable != nullptr && this->session.selectTable->getName() == nameTable) {
        delete this->session.selectTable;
        this->session.selectTable = nullptr;
    }

    return this->metadata.removeTable(this->session.selectDB->getName(), nameTable);
}

LogsCode MasterManagement::executeSelectAll() {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    this->session.selectTable->displayContent();
    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::executeSelectOrdered(string columnName, bool ascending) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    this->session.selectTable->displayContentOrdered(columnName, ascending);
    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::executeSelectFilter(string columnName, string value) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    this->session.selectTable->displayContentFilter(columnName, value);
    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::executeAddConstraint(string columnName, string constraintType, string refTable, string refCol) {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }
    if (!this->hasSelectTable()) {
        return LogsCode::TABLE_NOT_SELECTED;
    }

    return this->metadata.addConstraintToMetadata(this->session.selectDB->getName(), this->session.selectTable->getName(), columnName, constraintType, refTable, refCol);
}

LogsCode MasterManagement::executeShowDatabases() {
    this->metadata.showDatabases();
    return LogsCode::SUCCESS;
}

LogsCode MasterManagement::executeShowTables() {
    if (!this->hasSelectDB()) {
        return LogsCode::DATABASE_NOT_SELECTED;
    }

    this->metadata.showTables(this->session.selectDB->getName());
    return LogsCode::SUCCESS;
}