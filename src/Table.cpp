#include "../include/Table.h"
#include <iostream>
#include <algorithm>

using json = nlohmann::json;
using namespace std;

Table::Table(string name, DataBase& db) {
    this->name = name;
    this->path = db.getPath() + "/" + name + ".table";
}

string Table::getName() const {
    return this->name;
}

string Table::getPath() const {
    return this->path;
}

LogsCode Table::create() {
    ifstream test(this->path);

    if (!test.good()) {
        json table;
        table["columns"] = json::array();
        table["records"] = json::array();

        ofstream file(this->path);
        if (!file.good()) {
            return LogsCode::FILE_ERROR;
        }

        file << table.dump(4);
        file.close();
    }

    test.close();
    return LogsCode::SUCCESS;
}

LogsCode Table::addColumn(const string& columnName) {
    ifstream file(this->path);
    if (!file.good()) {
        return LogsCode::FILE_ERROR;
    }

    json table;
    file >> table;
    file.close();

    if (table.contains("columns")) {
        for (const auto& column : table["columns"]) {
            if (column == columnName) {
                return LogsCode::COLUMN_ALREADY_EXISTS;
            }
        }
    }

    table["columns"].push_back(columnName);

    ofstream output(this->path);
    if (!output.good()) {
        return LogsCode::FILE_ERROR;
    }

    output << table.dump(4);
    output.close();

    return LogsCode::SUCCESS;
}

LogsCode Table::addValues(const vector<string>& columns, const vector<string>& values, const json& dbMetadata) {
    ifstream file(this->path);
    if (!file.good()) {
        return LogsCode::FILE_ERROR;
    }

    json table;
    file >> table;
    file.close();

    json newRecord = json::object();

    // mapear columnas e insertar valores
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i < values.size()) {
            newRecord[columns[i]] = values[i];
        }
    }

    // valida restricciones basadas en Metadata (PK, FK, NOT_NULL)
    if (dbMetadata.contains("tables")) {
        for (const auto& tbl : dbMetadata["tables"]) {
            if (tbl["name"] == this->name) {
                if (tbl.contains("columns")) {
                    for (const auto& col : tbl["columns"]) {
                        string colName = col["name"];

                        // Validar NOT_NULL
                        if (col.contains("constraints")) {
                            for (const auto& c : col["constraints"]) {
                                string type = c["type"];

                                if (type == "NOT_NULL" || type == "PRIMARY_KEY") {
                                    if (!newRecord.contains(colName) || newRecord[colName].get<string>().empty()) {
                                        return LogsCode::NOT_NULL_VIOLATION;
                                    }
                                }

                                // Validar PRIMARY_KEY (Unicidad)
                                if (type == "PRIMARY_KEY") {
                                    string valToCheck = newRecord[colName];
                                    for (const auto& rec : table["records"]) {
                                        if (rec.contains(colName) && rec[colName] == valToCheck) {
                                            return LogsCode::PRIMARY_KEY_VIOLATION;
                                        }
                                    }
                                }

                                // Validar FOREIGN_KEY (Existencia en tabla referenciada)
                                if (type == "FOREIGN_KEY") {
                                    string refTable = c["refTable"];
                                    string refCol = c["refColumn"];
                                    string valToCheck = newRecord.contains(colName) ? newRecord[colName].get<string>() : "";

                                    string refPath = "databases/" + dbMetadata["name"].get<string>() + "/" + refTable + ".table";
                                    ifstream refFile(refPath);
                                    if (!refFile.good()) {
                                        return LogsCode::FOREIGN_KEY_VIOLATION;
                                    }

                                    json refData;
                                    refFile >> refData;
                                    refFile.close();

                                    bool foundRef = false;
                                    if (refData.contains("records")) {
                                        for (const auto& r : refData["records"]) {
                                            if (r.contains(refCol) && r[refCol] == valToCheck) {
                                                foundRef = true;
                                                break;
                                            }
                                        }
                                    }

                                    if (!foundRef) {
                                        return LogsCode::FOREIGN_KEY_VIOLATION;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    table["records"].push_back(newRecord);

    ofstream output(this->path);
    if (!output.good()) {
        return LogsCode::FILE_ERROR;
    }

    output << table.dump(4);
    output.close();

    return LogsCode::SUCCESS;
}

void Table::displayContent() const {
    ifstream file(this->path);
    if (!file.good()) return;

    json table;
    file >> table;
    file.close();

    cout << "\n=== TABLA: " << this->name << " ===" << endl;
    if (table.contains("records")) {
        for (const auto& rec : table["records"]) {
            cout << "| ";
            for (auto it = rec.begin(); it != rec.end(); ++it) {
                cout << it.key() << ": " << it.value() << " | ";
            }
            cout << endl;
        }
    }
    cout << "==========================\n" << endl;
}

void Table::displayContentOrdered(const string& columnName, bool ascending) const {
    ifstream file(this->path);
    if (!file.good()) return;

    json table;
    file >> table;
    file.close();

    if (!table.contains("records")) return;

    vector<json> records = table["records"].get<vector<json>>();

    sort(records.begin(), records.end(), [&columnName, ascending](const json& a, const json& b) {
        string valA = a.contains(columnName) ? a[columnName].get<string>() : "";
        string valB = b.contains(columnName) ? b[columnName].get<string>() : "";

        if (ascending) {
            return valA < valB;
        } else {
            return valA > valB;
        }
    });

    cout << "\n=== TABLA: " << this->name << " (ORDENADO POR " << columnName << ") ===" << endl;
    for (const auto& rec : records) {
        cout << "| ";
        for (auto it = rec.begin(); it != rec.end(); ++it) {
            cout << it.key() << ": " << it.value() << " | ";
        }
        cout << endl;
    }
    cout << "==========================\n" << endl;
}

void Table::displayContentFilter(const string& columnName, const string& value) const {
    ifstream file(this->path);
    if (!file.good()) return;

    json table;
    file >> table;
    file.close();

    cout << "\n=== TABLA: " << this->name << " (FILTRADO " << columnName << " = " << value << ") ===" << endl;
    if (table.contains("records")) {
        for (const auto& rec : table["records"]) {
            if (rec.contains(columnName) && rec[columnName] == value) {
                cout << "| ";
                for (auto it = rec.begin(); it != rec.end(); ++it) {
                    cout << it.key() << ": " << it.value() << " | ";
                }
                cout << endl;
            }
        }
    }
    cout << "==========================\n" << endl;
}

LogsCode Table::deleteRecord(const string& columnName, const string& value) {
    ifstream file(this->path);
    if (!file.good()) return LogsCode::FILE_ERROR;

    json table;
    file >> table;
    file.close();

    if (!table.contains("records")) return LogsCode::SUCCESS;

    json updatedRecords = json::array();
    for (const auto& rec : table["records"]) {
        if (!rec.contains(columnName) || rec[columnName] != value) {
            updatedRecords.push_back(rec);
        }
    }

    table["records"] = updatedRecords;

    ofstream output(this->path);
    if (!output.good()) return LogsCode::FILE_ERROR;

    output << table.dump(4);
    output.close();

    return LogsCode::SUCCESS;
}

LogsCode Table::updateRecord(const string& filterCol, const string& filterVal, const string& targetCol, const string& newVal) {
    ifstream file(this->path);
    if (!file.good()) return LogsCode::FILE_ERROR;

    json table;
    file >> table;
    file.close();

    if (!table.contains("records")) return LogsCode::SUCCESS;

    for (auto& rec : table["records"]) {
        if (rec.contains(filterCol) && rec[filterCol] == filterVal) {
            rec[targetCol] = newVal;
        }
    }

    ofstream output(this->path);
    if (!output.good()) return LogsCode::FILE_ERROR;

    output << table.dump(4);
    output.close();

    return LogsCode::SUCCESS;
}
