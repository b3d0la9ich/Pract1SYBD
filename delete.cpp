#include "delete.h"

// Функция для проверки на существование колонки
bool ExistColonk(const string& TableName, const string& ColumnName, Node* TableHead){
    if (!TableHead) {
        cerr << "Error: list of table is empty" << endl;
        return false;
    }

    Node* currentTable = TableHead;

    while (currentTable) {
        if (currentTable->table == TableName) {
            ListNode* currentColumn = currentTable->column;

            while (currentColumn) {
                if (currentColumn->column_name == ColumnName){
                    return true;
                }
                currentColumn = currentColumn->next;
            }

            cerr << "Column" << ColumnName << " didnt exist in table " << TableName << endl;
            return false;
        }

        currentTable = currentTable->next;
    }

    cerr << "Table " << TableName << "didnt exist" << endl;
    return false;
}

// Функция для парсинга WHERE 
bool parseWhereClause(istringstream& iss2, string& table, string& column, string& value, const string& TableName, const Table_json& json_table){
    string indication;
    iss2 >> indication;

    if (indication.find('.') == string::npos){ 
        cerr << "Error: incorrect command1" << endl;
        return false;
    }

    size_t dotPos = indication.find('.');
    table = indication.substr(0, dotPos);
    column = indication.substr(dotPos + 1);

    if (table != TableName) {
        cerr << "Error: incorrect command2" << endl;
        return false;
    }

    if (!ExistColonk(TableName, column, json_table.table_head)) {
        cerr << "No such column" << endl;
        return false;
    }

    iss2 >> indication;
    if (indication != "=") {
        cerr << "Error: incorrect command3" << endl;
        return false;
    }

    iss2 >> value;
    if (value.front() != '\'' || value.back() != '\'') {
        cerr << "Error: incorrect command4" << endl;
        return false;
    }

    value = value.substr(1, value.size() - 2);
    return true;
}

// Функция для удаление строк из всех csv файлов, связанных с таблицей, где значение совпадает с заданным
bool deleteRowsFromTable(const string& TableName, const string& column, const string& value, const Table_json& json_table){
    int amountCSV = 1;
    bool deletedSTR = false;

    while (true) {
        fs::path filePath = fs::path("/home/b3d0la9a/don/Pract1SYBD/") / json_table.Name / TableName / (to_string(amountCSV) + ".csv");
        ifstream file(filePath);
        if (!file.is_open()) {
            break;
        }
        file.close();
        amountCSV++;
    }

    for (size_t icsv = 1; icsv < amountCSV; icsv++){
        string filePath = "/home/b3d0la9a/don/Pract1SYBD/" + json_table.Name + "/" + TableName + "/" + (to_string(icsv) + ".csv");
        rapidcsv::Document doc(filePath);

        int columnIndex = doc.GetColumnIdx(column);
        size_t amountRow = doc.GetRowCount();

        if (columnIndex == -1) {
            cerr << "Error: column didnt exist in CSV: " << column << endl;
            return false;
        }

        for (size_t i = 0; i < amountRow;) {
            if (doc.GetCell<string>(columnIndex, i) == value) {
                doc.RemoveRow(i);
                deletedSTR = true;
                amountRow--;
            } else {
                i++;
            }
        }
        doc.Save(filePath);
    }

    return deletedSTR;   
}

// Функция для удаления строк из таблицы
void del(const string& command, const Table_json& json_table){
    istringstream iss(command);
    string indication;

    // Проверка начало команды
    if (!(iss >> indication && indication == "DELETE" && iss >> indication && indication == "FROM")) {
        cerr << "Error: incorrect table" << endl;
        return; 
    }

    // Чтение имени таблицы
    string TableName;
    iss >> TableName;
    if (!TableExist(TableName, json_table.table_head)) {
        cerr << "Error: incorrect table" << endl;
        return;
    }

    // Проверка на наличие "WHERE"
    string whereCMD;
    if (!(iss >> whereCMD && whereCMD == "WHERE")) {
        cerr << "Error: incorrect command5" << endl;
        return;
    }

    
    string table, column, value;
    if (!parseWhereClause(iss, table, column, value, TableName, json_table)) {
        return;
    }

    if (isloker(TableName, json_table.Name)) {
        cerr << "Table is locked" << endl;
        return;
    }
    loker(TableName, json_table.Name);

    bool deletedSTR = deleteRowsFromTable(TableName, column, value, json_table);

    if (!deletedSTR) {
        cout << "This value didnt finded" << endl;
    }

    loker(TableName, json_table.Name);
}
