#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem> // директория
#include "json.hpp"
#include "struct.h"

using namespace std;
using json = nlohmann::json;
namespace fs = filesystem;

void DelDirectory(const fs::path& PathDirectory);
void Create_Dir_Files(const fs::path& PathSheme, const json& structure, Table_json& json_table);
void Parser(Table_json& json_table);