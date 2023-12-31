#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include <list>

using namespace std;


using json = nlohmann::json;

std::unordered_map<
        std::string, std::unordered_map<
                std::string, std::pair<std::string, int>
        >
> listOfAllScopes;
std::unordered_map<std::string, std::pair<std::string, int>> scopes;

// Function to parse the C++ code and build the symbol table
void
parseCode(const std::string &fileToParse, std::unordered_map<std::string, std::pair<std::string, int>> &symbolTable) {
    std::ifstream file(fileToParse);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileToParse << std::endl;
        return;
    }

    string line;
    int lineNumber = 1;
    while (getline(file, line)) {
        // Skip empty lines and comments
        if ((line.empty() || line.find("//") || line.find("{") || line.find("}")) == 0) {
            lineNumber++;
            continue;
        }

        // Normal declaration or assignement
        size_t pos;
        pos = line.find(";");
        if (pos != std::string::npos) {
            std::string declaration = line.substr(0, pos);
            // Binary op
            if (declaration.find("+") != std::string::npos || declaration.find("=") != std::string::npos ||
                declaration.find("*") != std::string::npos || declaration.find("/") != std::string::npos ||
                declaration.find("%") != std::string::npos || declaration.find("-") != std::string::npos ||
                declaration.find("return") != std::string::npos)
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
                    // Remove the whitespaces from type
                    type.erase(std::remove_if(type.begin(), type.end(), ::isspace),
                               type.end());

                    symbolTable[variable] = std::make_pair(type, lineNumber);
                    //scopes["global"] = std::make_pair(type, lineNumber);
                }
            }
        }

        // Function
        pos = line.find("(");
        if (pos != std::string::npos) {
            std::string declaration = line.substr(0, pos);
            std::size_t lastSpacePos = declaration.rfind(" ");
            if (lastSpacePos != std::string::npos) {
                std::string variable = declaration.substr(lastSpacePos + 1);
                std::string type = declaration.substr(0, lastSpacePos);
                symbolTable[variable] = std::make_pair(type, lineNumber);
                //listOfAllScopes["private"] = std::make_pair(type, lineNumber);

            }
        }
        lineNumber++;
    }
    file.close();
}

// Function to print the symbol table
void printSymbolTable(const std::unordered_map<std::string, std::pair<std::string, int>> &symbolTable) {
    for (const auto &entry: symbolTable) {
        std::cout << "Variable: " << entry.first << ", Type: " << entry.second.first
                  << ", Line: " << entry.second.second << std::endl;
    }
}

json convertMapToJson(const std::unordered_map<std::string, std::pair<std::string, int>> &map) {
    json jsonObj;
    for (const auto &entry: map) {
        const std::string &key = entry.first;
        const std::pair<std::string, int> &value = entry.second;

        json valueJson;
        valueJson["type"] = value.first;
        valueJson["line"] = value.second;

        jsonObj[key] = valueJson;
    }
    return jsonObj;
}

// Print the listOfAllScopes
void testPrintAll() {
    for (const auto &outerPair: listOfAllScopes) {
        std::cout << "Scope: " << outerPair.first << std::endl;
        std::cout << "Symbol Table:" << std::endl;
        for (const auto &innerPair: outerPair.second) {
            std::cout << "    Variable: " << innerPair.first << std::endl;
            std::cout << "    Value: " << innerPair.second.first << std::endl;
            std::cout << "    Integer: " << innerPair.second.second << std::endl;
        }
        std::cout << std::endl;
    }
}

int main() {
    std::string fileToParse = "input.t"; // Script to parse
    std::unordered_map<std::string, std::pair<std::string, int>> symbolTable;
    std::list<
            std::pair<
                    std::string, std::unordered_map<
                            std::string, std::pair<std::string, int>
                    >
            >
    > listOfScopes;

    parseCode(fileToParse, symbolTable);
    printSymbolTable(symbolTable);


    // Json convertor
    json jsonObject = convertMapToJson(symbolTable);

    // Print the JSON object
    std::cout << jsonObject.dump(4) << std::endl;

    std::ofstream outputFile("output.json");

    // Check if the file was opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open the output file." << std::endl;
    }
    outputFile << jsonObject.dump(4);
    //testPrintAll();
}
