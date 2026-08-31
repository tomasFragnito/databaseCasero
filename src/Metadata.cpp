#include "../include/Metadata.h"
#include <iostream>

using namespace std;

Metadata::Metadata() {
    this->createMetadataFile();
}

json Metadata::loadMetadata() {
    ifstream file("metadata.json");
    json metadata;

    if (file.is_open()) {
        file >> metadata;
        file.close();
    }

    return metadata;
}

void Metadata::saveMetadata(json& metadata) {
    ofstream file("metadata.json");
    if (file.is_open()) {
        file << metadata.dump(4);
        file.close();
    }
}

bool Metadata::existsDataBase(const string& name) {
    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        for (auto& database : metadata["databases"]) {
            if (database["name"] == name) {
                return true;
            }
        }
    }

    return false;
}

bool Metadata::existsTable(const string& nameTable, DataBase* database) {
    if (database == nullptr) {
        return false;
    }

    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        for (auto& db : metadata["databases"]) {
            if (db["name"] == database->getName()) {
                if (db.contains("tables")) {
                    for (auto& table : db["tables"]) {
                        if (table["name"] == nameTable) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

LogsCode Metadata::createMetadataFile() {
    if (!filesystem::exists("metadata.json")) {
        json metadata;
        metadata["databases"] = json::array();
        this->saveMetadata(metadata);
    }
    return LogsCode::SUCCESS;
}

LogsCode Metadata::addDataBase(DataBase& db) {
    if (this->existsDataBase(db.getName())) {
        return LogsCode::DATABASE_ALREADY_EXISTS;
    }

    json metadata = this->loadMetadata();
    metadata["databases"].push_back({
        {"name", db.getName()},
        {"tables", json::array()}
    });

    this->saveMetadata(metadata);
    return LogsCode::SUCCESS;
}

LogsCode Metadata::addTable(const string& dbName, Table& table) {
    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        for (auto& database : metadata["databases"]) {
            if (database["name"] == dbName) {
                json newTableObj = json::object();
                newTableObj["name"] = table.getName();
                newTableObj["columns"] = json::array();
                
                database["tables"].push_back(newTableObj);
                this->saveMetadata(metadata);
                return LogsCode::SUCCESS;
            }
        }
    }

    return LogsCode::DATABASE_NOT_FOUND;
}

LogsCode Metadata::addColumn(const string& columnName, const string& dbName, Table& table) {
    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        for (auto& database : metadata["databases"]) {
            if (database["name"] == dbName) {
                if (database.contains("tables")) {
                    for (auto& tbl : database["tables"]) {
                        if (tbl["name"] == table.getName()) {
                            json colObj = json::object();
                            colObj["name"] = columnName;
                            colObj["constraints"] = json::array();

                            tbl["columns"].push_back(colObj);
                            this->saveMetadata(metadata);
                            return LogsCode::SUCCESS;
                        }
                    }
                }
            }
        }
    }

    return LogsCode::DATABASE_NOT_FOUND;
}

LogsCode Metadata::removeDataBase(const string& dbName) {
    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        auto& dbs = metadata["databases"];
        for (auto it = dbs.begin(); it != dbs.end(); ++it) {
            if ((*it)["name"] == dbName) {
                dbs.erase(it);
                this->saveMetadata(metadata);
                return LogsCode::SUCCESS;
            }
        }
    }

    return LogsCode::DATABASE_NOT_FOUND;
}

LogsCode Metadata::removeTable(const string& dbName, const string& tableName) {
    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        for (auto& database : metadata["databases"]) {
            if (database["name"] == dbName) {
                if (database.contains("tables")) {
                    auto& tbls = database["tables"];
                    for (auto it = tbls.begin(); it != tbls.end(); ++it) {
                        if ((*it)["name"] == tableName) {
                            tbls.erase(it);
                            this->saveMetadata(metadata);
                            return LogsCode::SUCCESS;
                        }
                    }
                }
            }
        }
    }

    return LogsCode::TABLE_NOT_FOUND;
}

LogsCode Metadata::addConstraintToMetadata(const string& dbName, const string& tableName, const string& columnName, const string& constraintType, const string& refTable, const string& refCol) {
    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        for (auto& database : metadata["databases"]) {
            if (database["name"] == dbName) {
                if (database.contains("tables")) {
                    for (auto& tbl : database["tables"]) {
                        if (tbl["name"] == tableName) {
                            for (auto& col : tbl["columns"]) {
                                if (col["name"] == columnName) {
                                    json cObj = json::object();
                                    cObj["type"] = constraintType;
                                    if (!refTable.empty()) {
                                        cObj["refTable"] = refTable;
                                        cObj["refColumn"] = refCol;
                                    }
                                    col["constraints"].push_back(cObj);
                                    this->saveMetadata(metadata);
                                    return LogsCode::SUCCESS;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return LogsCode::TABLE_NOT_FOUND;
}

LogsCode Metadata::showDatabases() {
    json metadata = this->loadMetadata();

    if (!metadata.contains("databases")) {
        return LogsCode::DATABASE_NOT_FOUND;
    } else {
        cout << "\n=== BASES DE DATOS ===" << endl;
        if (!metadata["databases"].empty()) {
            for (const auto& db : metadata["databases"]) {
                cout << " - " << db["name"].get<string>() << endl;
            }
        } else {
            cout << "(Sin bases de datos)" << endl;
        }
        return LogsCode::SUCCESS;
    }
}

LogsCode Metadata::showTables(const string& dbName) {
    json metadata = this->loadMetadata();

    if (metadata.contains("databases")) {
        for (auto& db : metadata["databases"]) {
            if (db["name"] == dbName) {
                cout << "\n=== TABLAS DE: " << dbName << " ===" << endl;
                if (db.contains("tables") && !db["tables"].empty()) {
                    for (const auto& tbl : db["tables"]) {
                        cout << " - " << tbl["name"].get<string>() << endl;
                    }
                } else {
                    cout << "(Sin tablas)" << endl;
                }
                return LogsCode::SUCCESS;
            }
        }
        return LogsCode::DATABASE_NOT_FOUND;
    } else {
        return LogsCode::DATABASE_NOT_FOUND;
    }
}