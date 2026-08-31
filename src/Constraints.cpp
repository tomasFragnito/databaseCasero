#include "../include/Constraints.h"

using namespace std;

void Constraints::addConstraint(const string& columnName, ColumnConstraint constraint) {
    this->columnConstraints[columnName].push_back(constraint);
}

void Constraints::addForeignKey(const string& columnName, const string& targetTable, const string& targetColumn) {
    this->columnConstraints[columnName].push_back(ColumnConstraint::FOREIGN_KEY);
    this->foreignKeyReferences[columnName] = targetTable + "." + targetColumn;
}

bool Constraints::hasConstraint(const string& columnName, ColumnConstraint constraint) const {
    auto it = this->columnConstraints.find(columnName);
    if (it != this->columnConstraints.end()) {
        for (const auto& c : it->second) {
            if (c == constraint) {
                return true;
            }
        }
    }
    return false;
}

string Constraints::getForeignKeyReference(const string& columnName) const {
    auto it = this->foreignKeyReferences.find(columnName);
    if (it != this->foreignKeyReferences.end()) {
        return it->second;
    }
    return "";
}

vector<ColumnConstraint> Constraints::getConstraints(const string& columnName) const {
    auto it = this->columnConstraints.find(columnName);
    if (it != this->columnConstraints.end()) {
        return it->second;
    }
    return vector<ColumnConstraint>();
}
