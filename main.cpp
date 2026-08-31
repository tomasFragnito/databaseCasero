#include <iostream>
#include "./include/Console.h"

using namespace std;

int main() {
    Console console;

    cout << "========================================" << endl;
    cout << "   Iniciando motor de Base de Datos DB  " << endl;
    cout << "========================================" << endl;

    // Iniciar la consola de ejecucion por lotes de comandos
    console.run();

    return 0;
}
