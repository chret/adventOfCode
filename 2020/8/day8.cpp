#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

using Op_T = std::pair<std::string,int>;

auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<Op_T> parsedData;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            std::string op;
            int amount;
            std::stringstream ss(line);
            ss >> op >> amount;
            parsedData.push_back({op,amount});
        }
        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
    return parsedData;
}

void printContent(const std::vector<Op_T>& vec)
{
    std::cout << "=============\n";
    for(const auto& e: vec)
    {
        std::cout << e.first << " " << e.second << std::endl;
    }
    std::cout << "=============\n";
}

int acc;

int decodeOp(const Op_T &op, int idx)
{
    if(op.first == "nop")
    {
        return ++idx;
    } else if(op.first == "acc")
    {
        acc += op.second;
        return ++idx;
    } else if(op.first == "jmp")
    {
        return idx + op.second;
    }
}

bool checkForTermination(const std::vector<Op_T>& vec)
{
    acc = 0;
    int idx = 0;
    std::vector<bool> visitedPositions(vec.size(), false);
    while(true)
    {
        if(idx == vec.size()) return true; // terminate
        if(visitedPositions[idx]) return false; // loop

        visitedPositions[idx] = true;
        idx = decodeOp(vec[idx], idx);
    }
}

int main()
{
    auto parsedFileContent = readFromFile("input.txt");
    //printContent(parsedFileContent);

    checkForTermination(parsedFileContent);
    std::cout << "Result 1: " << acc << std::endl;

    for(int i = 0; i < parsedFileContent.size(); ++i)
    {
        auto modifiedVec = parsedFileContent;
        // check for possible modification
        if(modifiedVec[i].first == "jmp") modifiedVec[i].first = "nop";
        else if(modifiedVec[i].first == "nop") modifiedVec[i].first = "jmp";
        else continue;

        if(checkForTermination(modifiedVec)) break;
    }
    std::cout << "Result 2: " << acc << std::endl;

}