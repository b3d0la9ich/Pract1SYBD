#pragma once
#include <iostream>
#include "delete.h"
#include "insert.h"
#include "struct.h"

void select(const string& query, const Table_json& json_table);
bool processConditionTable(const Table_json& json_table, const string& table1, const string& table2, const string& column1, const string& column2);
bool processConditionString(const Table_json& json_table, const string& table, const string& column, const string& s);
void crossJoinAndFilter(const Table_json& json_table, const string& table1, const string& table2, const string& column1, const string& column2);
bool findDot(const string& indication);
string ignoreQuotes(const string& indication);
void separationDot(const string& word, string& table, string& column, const Table_json& json_table);
