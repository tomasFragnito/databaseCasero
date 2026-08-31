#include "../include/DataBase.h"
#include <filesystem>

using namespace std;

DataBase::DataBase(string name) {
    this->name = name;
    this->path = "databases/" + name;
}

string DataBase::getName() const {
    return this->name;
}

string DataBase::getPath() const {
    return this->path;
}

void DataBase::create() {
    // Si la carpeta de la base de datos no existe, se crea en el sistema de archivos
    if (!filesystem::exists(this->path)) {
        filesystem::create_directories(this->path);
    }
}
