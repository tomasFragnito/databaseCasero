#pragma once

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum class ColumnConstraint {
    PRIMARY_KEY,
    FOREIGN_KEY,
    AUTO_INCREMENT,
    NOT_NULL,
    UNIQUE,
    MIN_LENGTH,
    MAX_LENGTH,
    DEFAULT_VALUE
};

class Constraints {
private:
    unordered_map<string, vector<ColumnConstraint>> columnConstraints;
    unordered_map<string, string> foreignKeyReferences; // Columna -> "TablaReferenciada.ColumnaReferenciada"

public:
    Constraints() {}

    // Agregar una restriccion a una columna
    void addConstraint(const string& columnName, ColumnConstraint constraint);

    // Agregar FK especificando la referencia
    void addForeignKey(const string& columnName, const string& targetTable, const string& targetColumn);

    // Verificar si una columna tiene una restriccion
    bool hasConstraint(const string& columnName, ColumnConstraint constraint) const;

    // Obtener la referencia de FK
    string getForeignKeyReference(const string& columnName) const;

    vector<ColumnConstraint> getConstraints(const string& columnName) const;
};
