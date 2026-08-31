#pragma once

#include <string>

using namespace std;

class DataBase {
private:
    string name;
    string path;

public:
    DataBase(string name);

    string getName() const;
    string getPath() const;

    void create();
};
