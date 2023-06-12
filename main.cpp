#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Function to parse the C++ code and build the symbol table
void parseCode(const std::string& filename, std::unordered_map<std::string, std::pair<std::string, int>>& symbolTable) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    int lineNumber = 1;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if ((line.empty() || line.find("//") || line.find("{") || line.find("}")) == 0) {
            lineNumber++;
            continue;
        }

        // Normal declaration or assignement
        std::size_t pos;
        pos = line.find(";");
        if (pos != std::string::npos) {
            std::string declaration = line.substr(0, pos);
            // Binary op
            if (declaration.find("+") != std::string::npos || declaration.find("=") != std::string::npos ||
                declaration.find("*") != std::string::npos || declaration.find("/") != std::string::npos ||
                declaration.find("%") != std::string::npos || declaration.find("-") != std::string::npos )
                // Assignment
            {
                // we have to lookup in symbol table
                lineNumber++;
                continue;
            }

            // Declaration
            std::size_t lastSpacePos = declaration.rfind(" ");
            {
                if (lastSpacePos != std::string::npos) {
                    std::string variable = declaration.substr(lastSpacePos + 1);
                    std::string type = declaration.substr(0, lastSpacePos);
                    symbolTable[variable] = std::make_pair(type, lineNumber);
                }
            }
        }

        // Function
        pos = line.find("()");
        if (pos != std::string::npos) {
            std::string declaration = line.substr(0, pos);
            std::size_t lastSpacePos = declaration.rfind(" ");
            if (lastSpacePos != std::string::npos) {
                std::string variable = declaration.substr(lastSpacePos + 1);
                std::string type = declaration.substr(0, lastSpacePos);
                symbolTable[variable] = std::make_pair(type, lineNumber);
            }
        }
        lineNumber++;
    }
    file.close();
}

// Function to print the symbol table
void printSymbolTable(const std::unordered_map<std::string, std::pair<std::string, int>>& symbolTable) {
    for (const auto& entry : symbolTable) {
        std::cout << "Variable: " << entry.first << ", Type: " << entry.second.first
                  << ", Line: " << entry.second.second << std::endl;
    }
}

json convertMapToJson(const std::unordered_map<std::string, std::pair<std::string, int>>& map) {
    json jsonObj;
    for (const auto& entry : map) {
        const std::string& key = entry.first;
        const std::pair<std::string, int>& value = entry.second;

        json valueJson;
        valueJson["type"] = value.first;
        valueJson["line"] = value.second;

        jsonObj[key] = valueJson;
    }
    return jsonObj;
}

int main() {
    std::string filename = "input.t"; // Replace with the name of your C++ file
    std::unordered_map<std::string, std::pair<std::string, int>> symbolTable;

    parseCode(filename, symbolTable);
    printSymbolTable(symbolTable);


    // Json convertor
    json jsonObject = convertMapToJson(symbolTable);

    // Print the JSON object
    std::cout << jsonObject.dump(4) << std::endl;
    return 0;
}
