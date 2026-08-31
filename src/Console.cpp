#include "../include/Console.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

Console::Console() : masterManagement(session) {}

void Console::run() {
    ifstream file("commands.txt");
    if (!file.is_open()) {
        cout << "ERROR: No se pudo abrir el archivo commands.txt" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        // Limpiar espacios en blanco al inicio o final de la linea
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\n'), line.end());

        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos) {
            continue;
        }
        size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);

        if (line.empty()) {
            continue;
        }

        queue<string> commands;
        string currentToken = "";
        bool inQuotes = false;

        // Tokenizacion respetando textos entre comillas dobles
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];

            if (c == '"') {
                inQuotes = !inQuotes;
                currentToken += c;
            } else if (c == ' ' && !inQuotes) {
                if (!currentToken.empty()) {
                    commands.push(currentToken);
                    currentToken = "";
                }
            } else {
                currentToken += c;
            }
        }

        if (!currentToken.empty()) {
            commands.push(currentToken);
        }

        // Mostrar por pantalla el comando que se esta procesando
        cout << session.user << " : " << line << endl << flush;

        if (!commands.empty()) {
            string action = commands.front();
            commands.pop();

            LogsCode result = LogsCode::SUCCESS;

            if (action == "create") {
                result = commandCreate(commands);
            } else if (action == "select") {
                result = commandSelect(commands);
            } else if (action == "insert") {
                result = commandInsert(commands);
            } else if (action == "delete") {
                result = commandDelete(commands);
            } else if (action == "drop") {
                result = commandDrop(commands);
            } else if (action == "update") {
                result = commandUpdate(commands);
            } else if (action == "alter") {
                result = commandAlter(commands);
            } else if (action == "show") {
                result = commandShow(commands);
            } else {
                result = LogsCode::INVALID_COMMAND;
            }

            this->printLogMessage(result);
        }
    }

    file.close();
}

LogsCode Console::commandShow(queue<string> &commands) {
    if (commands.empty()) {
        return LogsCode::INVALID_COMMAND;
    }

    string type = commands.front();
    commands.pop();

    if (type == "databases") {
        return masterManagement.executeShowDatabases();
    } else if (type == "tables") {
        return masterManagement.executeShowTables();
    }

    return LogsCode::INVALID_COMMAND;
}

LogsCode Console::commandCreate(queue<string> &commands) {
    if (commands.empty()) {
        return LogsCode::INVALID_COMMAND;
    }

    string type = commands.front();
    commands.pop();

    if (type == "database") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameDB = commands.front();
        commands.pop();
        return masterManagement.executeCreateDatabase(nameDB);
    } else if (type == "table") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameTable = commands.front();
        commands.pop();
        return masterManagement.executeCreateTable(nameTable);
    }

    return LogsCode::INVALID_COMMAND;
}

LogsCode Console::commandSelect(queue<string> &commands) {
    if (commands.empty()) {
        return LogsCode::INVALID_COMMAND;
    }

    string type = commands.front();
    commands.pop();

    if (type == "database") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameDB = commands.front();
        commands.pop();
        return masterManagement.executeSelectDatabase(nameDB);
    } else if (type == "table") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameTable = commands.front();
        commands.pop();
        return masterManagement.executeSelectTable(nameTable);
    } else if (type == "*") {
        if (!commands.empty() && commands.front() == "order") {
            commands.pop();
            if (!commands.empty() && commands.front() == "by") {
                commands.pop();
                if (!commands.empty()) {
                    string colName = commands.front();
                    commands.pop();
                    bool asc = true;
                    if (!commands.empty()) {
                        string orderType = commands.front();
                        commands.pop();
                        if (orderType == "desc" || orderType == "DESC") {
                            asc = false;
                        }
                    }
                    return masterManagement.executeSelectOrdered(colName, asc);
                }
            }
        }
        return masterManagement.executeSelectAll();
    } else if (type == "where") {
        if (commands.size() >= 3) {
            string col = commands.front(); commands.pop();
            string eq = commands.front(); commands.pop();
            string val = commands.front(); commands.pop();
            val.erase(remove(val.begin(), val.end(), '"'), val.end());
            return masterManagement.executeSelectFilter(col, val);
        }
    }

    return LogsCode::INVALID_COMMAND;
}

LogsCode Console::commandInsert(queue<string> &commands) {
    if (commands.empty()) {
        return LogsCode::INVALID_COMMAND;
    }

    string type = commands.front();
    commands.pop();

    if (type == "column") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameColumn = commands.front();
        commands.pop();
        return masterManagement.executeInsertColumn(nameColumn);
    }

    if (type == "into") {
        vector<string> listColumn;
        vector<string> listValues;

        if (!commands.empty() && commands.front() == "(") {
            commands.pop();
        }

        while (!commands.empty()) {
            string currentToken = commands.front();
            commands.pop();

            if (currentToken == ")") {
                break;
            }

            string cleanName = "";
            for (char c : currentToken) {
                if (c == ',') break;
                cleanName += c;
            }

            if (!cleanName.empty()) {
                listColumn.push_back(cleanName);
            }
        }

        if (!commands.empty() && commands.front() == "values") {
            commands.pop();
        }

        if (!commands.empty() && commands.front() == "(") {
            commands.pop();
        }

        while (!commands.empty()) {
            string currentToken = commands.front();
            commands.pop();

            if (currentToken == ")") {
                break;
            }

            string cleanValue = "";
            for (char c : currentToken) {
                if (c == ',' || c == '"') continue;
                cleanValue += c;
            }

            if (!cleanValue.empty()) {
                listValues.push_back(cleanValue);
            }
        }

        return masterManagement.executeInsertValues(listColumn, listValues);
    }

    return LogsCode::INVALID_COMMAND;
}

LogsCode Console::commandDelete(queue<string> &commands) {
    if (commands.empty()) {
        return LogsCode::INVALID_COMMAND;
    }

    string type = commands.front();
    commands.pop();

    if (type == "column") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameColumn = commands.front();
        commands.pop();
        return masterManagement.executeDeleteColumn(nameColumn);
    } else if (type == "from") {
        if (commands.size() >= 3) {
            string col = commands.front(); commands.pop();
            string eq = commands.front(); commands.pop();
            string val = commands.front(); commands.pop();
            val.erase(remove(val.begin(), val.end(), '"'), val.end());
            return masterManagement.executeDeleteRecord(col, val);
        }
    }

    return LogsCode::INVALID_COMMAND;
}

LogsCode Console::commandDrop(queue<string> &commands) {
    if (commands.empty()) {
        return LogsCode::INVALID_COMMAND;
    }

    string type = commands.front();
    commands.pop();

    if (type == "database") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameDB = commands.front();
        commands.pop();
        return masterManagement.executeDropDatabase(nameDB);
    } else if (type == "table") {
        if (commands.empty()) return LogsCode::INVALID_COMMAND;
        string nameTable = commands.front();
        commands.pop();
        return masterManagement.executeDropTable(nameTable);
    }

    return LogsCode::INVALID_COMMAND;
}

LogsCode Console::commandUpdate(queue<string> &commands) {
    if (commands.size() >= 7) {
        string tokenSet = commands.front(); commands.pop();
        string targetCol = commands.front(); commands.pop();
        string eq1 = commands.front(); commands.pop();
        string newVal = commands.front(); commands.pop();
        string tokenWhere = commands.front(); commands.pop();
        string filterCol = commands.front(); commands.pop();
        string eq2 = commands.front(); commands.pop();
        string filterVal = commands.front(); commands.pop();

        newVal.erase(remove(newVal.begin(), newVal.end(), '"'), newVal.end());
        filterVal.erase(remove(filterVal.begin(), filterVal.end(), '"'), filterVal.end());

        return masterManagement.executeUpdateRecord(filterCol, filterVal, targetCol, newVal);
    }
    return LogsCode::INVALID_COMMAND;
}

LogsCode Console::commandAlter(queue<string> &commands) {
    if (commands.size() >= 5) {
        string tokenTable = commands.front(); commands.pop();
        string tokenAdd = commands.front(); commands.pop();
        string tokenConstraint = commands.front(); commands.pop();
        string colName = commands.front(); commands.pop();
        string constraintType = commands.front(); commands.pop();

        string refTable = "";
        string refCol = "";

        if (constraintType == "FOREIGN_KEY" && commands.size() >= 2) {
            refTable = commands.front(); commands.pop();
            refCol = commands.front(); commands.pop();
        }

        return masterManagement.executeAddConstraint(colName, constraintType, refTable, refCol);
    }
    return LogsCode::INVALID_COMMAND;
}

void Console::printLogMessage(LogsCode code) {
    switch (code) {
        case LogsCode::SUCCESS:
            cout << " -> SUCCESS: Operacion realizada correctamente." << endl;
            break;
        case LogsCode::DATABASE_NOT_SELECTED:
            cout << " -> ERROR: No se ha seleccionado ninguna base de datos." << endl;
            break;
        case LogsCode::TABLE_NOT_SELECTED:
            cout << " -> ERROR: No se ha seleccionado ninguna tabla." << endl;
            break;
        case LogsCode::DATABASE_NOT_FOUND:
            cout << " -> ERROR: Base de datos no encontrada." << endl;
            break;
        case LogsCode::TABLE_NOT_FOUND:
            cout << " -> ERROR: Tabla no encontrada." << endl;
            break;
        case LogsCode::DATABASE_ALREADY_EXISTS:
            cout << " -> ERROR: La base de datos ya existe." << endl;
            break;
        case LogsCode::TABLE_ALREADY_EXISTS:
            cout << " -> ERROR: La tabla ya existe." << endl;
            break;
        case LogsCode::COLUMN_ALREADY_EXISTS:
            cout << " -> ERROR: La columna ya existe." << endl;
            break;
        case LogsCode::PRIMARY_KEY_VIOLATION:
            cout << " -> ERROR: Violacion de Restriccion Primary Key (Valor duplicado)." << endl;
            break;
        case LogsCode::FOREIGN_KEY_VIOLATION:
            cout << " -> ERROR: Violacion de Restriccion Foreign Key (Clave foranea no existe en la tabla referenciada)." << endl;
            break;
        case LogsCode::NOT_NULL_VIOLATION:
            cout << " -> ERROR: Violacion de Restriccion Not Null (Valor obligatorio faltante)." << endl;
            break;
        case LogsCode::FILE_ERROR:
            cout << " -> ERROR: Ocurrio un error al acceder al archivo." << endl;
            break;
        case LogsCode::INVALID_COMMAND:
            cout << " -> ERROR: Comando invalido o sintaxis incorrecta." << endl;
            break;
        default:
            cout << " -> ERROR: Estado desconocido." << endl;
            break;
    }
}