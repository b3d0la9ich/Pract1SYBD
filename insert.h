#pragma once
#include <iostream>
#include <filesystem>
#include "rapidcsv.h" 
#include "struct.h"

using namespace std;
namespace fs = filesystem;

bool TableExist(const string& TableName, Node* TableHead);
bool isloker(const string& TableName, const string& SchemaName);
void copyNameColonk(const string& from_file, const string& to_file);
void loker(const string& TableName, const string& SchemaName);
int findCsvFileCount(const Table_json& json_table, const string& TableName);
void createNewCsvFile(const string& baseDir, const string& TableName, int& csvNumber, const Table_json& tableJson);
void insert(const string& command, Table_json json_table);