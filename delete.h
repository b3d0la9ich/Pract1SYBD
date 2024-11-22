#pragma once
#include <iostream>
#include <fstream>
#include "insert.h"
#include "struct.h"

using namespace std;

bool ExistColonk(const string& TableName, const string& ColumnName, Node* TableHead);
bool parseWhereClause(istringstream& iss2, string& table, string& column, string& value, const string& TableName, const Table_json& json_table);
bool deleteRowsFromTable(const string& TableName, const string& column, const string& value, const Table_json& json_table);
void del(const string& command, const Table_json& json_table);
