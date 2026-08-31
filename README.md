# Motor de Base de Datos Relacional en C++

Proyecto de motor de Base de Datos relacional desarrollado en **C++17**. Diseñado de forma modular y con vision de ser manentible a largo plazo, el motor lee e interpreta comandos secuenciales mediante un archivo de texto llamado "commands" (commands.txt), gestionando la persistencia de datos mediante archivos JSON representando las tablas, cuales estan formadas por filas y columnas. Las bases de datos se almacenan dentro de directorios cuales almacenan los JSON.

La arquitectura del sistema esta basada mediante una capa de orquestación centralizada de comandos (`MasterManagement`), estado de sesión del usuario cual almacena las bases de datos y tablas seleccionadas (`Session`) y un flujo del manejo de Logs y errores (`LogsCode`).

---

## Características

* **Arquitectura**: Desacoplamiento estricto entre la interfaz de procesamiento de comandos (`Console`), el orquestador del dominio (`MasterManagement`), los esquemas globales (`Metadata`) y la gestión física en disco (`Table` / `DataBase`). Main unicamente se encarga de inicializar el codigo.
  El motor se encarga de reflejar los cambios dentro de un archivo JSON llamado "Metadata", donde estan las reglas (constraits) de las tablas junto a sus columnas como forma de garantizar la atomicidad del motor y la posiblidad de su modificacion manual sin utilizar comandos (esto puede probocar fallos en el motor si estos no coinciden con la respectiva tabla o sintaxis utilizada, manipular con cuidado si no se comprende su funcionamiento).

* **Motor de Persistencia JSON**: Modelado estructurado sobre almacenamiento local usando `nlohmann::json`.
* **Motor de Reglas e Integridad Referencial**:
* `PRIMARY_KEY`: Evaluación de unicidad sobre registros existentes y restricción de valores nulos.
* `FOREIGN_KEY`: Validación cruzada en tiempo de inserción contra la existencia previa del registro en la tabla y columna referenciada.
* `NOT_NULL`: Verificación en tiempo de ejecución para evitar registros incompletos.

* **Sintaxis**: Tokenizador tolerante a cadenas con espacios agrupadas por comillas dobles, con limpieza explícita de caracteres de control Unix/Windows (`\r`, `\n`).

---

## 📁 Estructura del Proyecto

```text
.
├── include/
│   ├── Console.h          # Parser e intérprete del DSL de comandos por lote
│   ├── Constraints.h      # Modelado de reglas de integridad (PK, FK, NOT_NULL)
│   ├── DataBase.h         # Entidad de dominio 'Database' y abstracción de directorio
│   ├── json.hpp           # Motor de serialización nlohmann::json
│   ├── LogsCode.h         # Enumerador global de estados de ejecución y errores
│   ├── MasterManagement.h # Fachada/Orquestador central y validaciones de sesión
│   ├── Metadata.h         # Mapeo y persistencia del esquema global (metadata.json)
│   ├── Session.h          # Contexto de ejecución activo (DB y Tabla seleccionadas)
│   └── Table.h            # Operaciones de bajo nivel sobre archivos .table (DML/DDL)
├── src/
│   ├── Console.cpp
│   ├── Constraints.cpp
│   ├── DataBase.cpp
│   ├── MasterManagement.cpp
│   ├── Metadata.cpp
│   └── Table.cpp
├── commands.txt           # Script de comandos en lote para prueba del motor
└── main.cpp               # Punto de entrada de la aplicación

```

---

## Compilación y Pipeline de Ejecución

### Requisitos Técnicos

* Compilador con soporte nativo para **C++17** (`g++`, `clang++` o MSVC).
* Soporte para `std::filesystem`.

### Compilación Básica (GCC/Terminal)

```bash
g++ -std=c++17 main.cpp src/*.cpp -Iinclude -o db_engine

```

### Ejecución

Asegúrate de que el archivo `commands.txt` se encuentre en la misma ruta relativa de ejecución del ejecutable:
( misma ubicaciondel archivo main.cpp )
---

## Especificación de Sintaxis y Comandos (`commands.txt`)

### 1. Inspección de Esquemas y Metadatos

```sql
show databases
select database empresa
    show tables

```

### 2. Definición de Datos y Restricciones

```sql
create database empresa
select database empresa

create table roles
select table roles
insert column id_rol
insert column nombre_rol
alter table add constraint id_rol PRIMARY_KEY

create table usuarios
select table usuarios
insert column id
insert column nombre
insert column id_rol
alter table add constraint id PRIMARY_KEY
alter table add constraint id_rol FOREIGN_KEY roles id_rol

```

### 3. Manipulación de Datos

```sql
select table roles
insert into ( id_rol, nombre_rol ) values ( 1, "Admin" )

select table usuarios
insert into ( id, nombre, id_rol ) values ( 100, "Tomas", 1 )

```

### 4. Consultas, Filtrado y Ordenamiento

```sql
select *
select where nombre = "Tomas"
select * order by id desc

```

### 5. Actualización, Borrado de Registros y Destrucción de Entidades

```sql
update set nombre = "Tomas Fragnito" where id = 100
delete from id = 100
drop table usuarios
drop database empresa

```
