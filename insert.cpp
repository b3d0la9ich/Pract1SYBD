#include "insert.hpp"

// Функция для проверки существования таблицы
bool TableExist(const string& TableName, Node* TableHead){
    Node* current = TableHead;
    while (current){
        if (current->table == TableName) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Функция для проверки на доступность таблицы
bool isloker(const string& TableName, const string& SchemaName){
    string baseDir = "/home/b3d0la9a/don/Pract1SYBD/" + SchemaName + "/" + TableName;
    string lockFile = baseDir + "/" + (TableName + "_lock.txt");

    if (!fs::exists(lockFile)) {
        cerr << "Error: no lock file: " << lockFile << endl;
        return false;
    }

    ifstream file(lockFile);
    if (!file.is_open()){
        cerr << "Error: can not open the lock file: " << lockFile << endl;
        return false;
    }

    string current;
    file >> current;
    file.close();
    return current == "locked";
}

// Функция для копирования названий колонок из одного файла в другой
void copyNameColonk(const string& from_file, const string& to_file){
    string columns;
    ifstream file_from(from_file);
    if (!file_from.is_open()){
        cerr << "Error: cant open the file4: " << from_file << endl;
        return;
    }
    file_from >> columns;
    file_from.close();

    ofstream file_to(to_file);
    if (!file_to.is_open()){
        cerr << "Error: cant open the file5: " << to_file << endl;
        return;
    }
    file_to << columns << endl;
    file_to.close();
}

// Функция для переключения состояния блокировки таблицы
void loker(const string& TableName, const string& SchemaName){
    string baseDir = "/home/b3d0la9a/don/Pract1SYBD/" + SchemaName + "/" + TableName;
    string lockFile = baseDir + "/" + (TableName + "_lock.txt");

    if (!fs::exists(lockFile)) {
        cerr << "Error: no lock file: " << lockFile << "\n";
        return;
    }

    ifstream fileIn(lockFile);
    if (!fileIn.is_open()) {
        cerr << "Error: cant open the lock file: " << lockFile << "\n";
        return;
    }

    string current;
    fileIn >> current;
    fileIn.close();

    ofstream fileOut(lockFile);
    if (!fileOut.is_open()) {
        cerr << "Error: cant open file for lock: " << lockFile << "\n";
        return;
    }

    fileOut << (current == "locked" ? "unlocked" : "locked"); // Записывает противоположное значение в файл
    fileOut.close();
}

// Функция для подсчета существующих csv файлов
int findCsvFileCount(const Table_json& json_table, const string& TableName){
    int CountCSV = 0;
    int NumberCSV = 1;

    while (true) {
        string FileCSV = "/home/b3d0la9a/don/Pract1SYBD/" + json_table.Name + "/" + TableName + "/" + (to_string(NumberCSV) + ".csv");
        
        ifstream fileIn(FileCSV);
        if (!fileIn.is_open()){

            break;
        }
        fileIn.close();

        CountCSV++; // Счетчик найденных файлов
        NumberCSV++; // Переход к следующему файлу
    }

    return CountCSV; // Общее количество существующих файлов
}

// Функция для создания нового csv файла, если текущий достиг максимального числа строк
void createNewCsvFile(const std::string& baseDir, const std::string& TableName, int& NumberCSV, const Table_json& tableJson){
    int MaxRows = tableJson.table_size;

    string FileCSV = baseDir + "/" + TableName + "/" + to_string(NumberCSV) + ".csv";

    rapidcsv::Document doc(FileCSV);
    if (doc.GetRowCount() >= MaxRows) {
        NumberCSV++;
        FileCSV = baseDir + "/" + TableName + "/" + to_string(NumberCSV) + ".csv";
    }

    if (!fs::exists(FileCSV)){
        string FirstCSV = baseDir + "/" + TableName + "/1.csv";
        copyNameColonk(FirstCSV, FileCSV);
    }
}

void insert(const string& command, Table_json json_table){
    istringstream iss(command);
    string word;
    iss >> word >> word;

    if (word != "INTO"){
        cerr << "Error: wrong command" << endl;
        return;
    }

    string TableName;
    iss >> TableName;
    if (!TableExist(TableName, json_table.table_head)){
        cerr << "Error: no such table" << endl;
        return;
    }

    iss >> word;
    if (word != "VALUES") {
        cerr << "Incorrect command" << endl;
        return;
    }

    string values;
    while (iss >> word){
        values += word;
    }

    if (values.front() != '(' || values.back() != ')') {
        cerr << "Incorrect command" << endl;
        return;
    }

    if (isloker(TableName, json_table.Name)){
        cerr << "Table is locked" << endl;
        return;
    }

    loker(TableName, json_table.Name);

    int currentPK;
    string PKFile = "/home/b3d0la9a/don/Pract1SYBD/" + json_table.Name + "/" + TableName + "/" + (TableName + "_pk_sequence.txt");
    ifstream fileIn(PKFile);
    if (!fileIn.is_open()) {
        cerr << "Cant open the file" << endl;
        return;
    }

    fileIn >> currentPK;
    fileIn.close();

    ofstream fileOUT(PKFile);
    if (!fileOUT.is_open()){
        cerr << "Error: cant open the file2" << endl;
        return;
    }
    currentPK++;
    fileOUT << currentPK;
    fileOUT.close();

    int NumberCSV = findCsvFileCount(json_table, TableName);

    string baseDir = "/home/b3d0la9a/don/Pract1SYBD/" + json_table.Name;

    createNewCsvFile(baseDir, TableName, NumberCSV, json_table);

    string SecondCSV = baseDir + "/" + TableName + "/" + to_string(NumberCSV) + ".csv";

    ofstream csv(SecondCSV, ios::app);
    if (!csv.is_open()){
        cerr << "Error: cant open the file3" << endl;
        return;
    }

    csv << currentPK << ",";
    for (int i = 0; i < values.size(); i++){
        if (values[i] == '\'') {
            i++;
            while (i < values.size() && values[i] != '\''){
                csv << values[i++];
            }
            if (i + 1 < values.size() && values[i + 1] != ')'){
                csv << ",";
            }
            else {
                csv << endl;
            }
        }
    }

    csv.close();
    loker(TableName, json_table.Name);
}