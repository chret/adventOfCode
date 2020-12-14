#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <functional>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>
#include <bitset>

using INT = unsigned long long;
using Memory_T = std::map<INT, INT>;

const int bitMaskSize = 36;

auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector< std::pair<std::string, std::string> > content;
    if(inputFile.is_open())
    {
        std::string line;
        std::string delim = " = ";
        while(std::getline(inputFile,line))
        {
            auto sep = line.find(delim);
            content.push_back({line.substr(0,sep), line.substr(sep+delim.size())});
        }

        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
    return content;
}

template< typename T>
void printContent(const std::vector<T>& vec)
{
    std::cout << "=============\n";
    for(const auto & e : vec)
    {
        std::cout << e.first << " " << e.second << "\n";
    }
    std::cout << "=============\n";
}

auto extractMemPos(std::string lhs)
{
    auto find1 = lhs.find("[");
    auto find2 = lhs.find("]");
    auto numStr = lhs.substr(find1+1,find2);
    return std::stoll(numStr);
}

void processMemSet(Memory_T & memory, std::string mask, std::string lhs, std::string rhs)
{
    auto memPosition = extractMemPos(lhs);
    auto value = std::stoll(rhs);
    auto bitSet = std::bitset<bitMaskSize>(value);
    if(mask.size() != bitMaskSize) std::cout << "Wrong sized mask " << mask << std::endl;

    //std::cout << " mask = " << mask << std::endl;
    //std::cout << bitSet.to_string() << " = " << value << " -> ";
    for(int i = 0; i < bitSet.size(); ++i)
    {
        if(mask[bitMaskSize-i-1] == 'X') continue;
        if(mask[bitMaskSize-i-1] == '1') bitSet[i] = true;
        if(mask[bitMaskSize-i-1] == '0') bitSet[i] = false;
    }

    auto valueAfterMask = bitSet.to_ullong();
    //std::cout << bitSet.to_string() << " = " << valueAfterMask << std::endl;
    memory[memPosition] = valueAfterMask;
}

void setMem(Memory_T & memory, std::string mask, std::bitset<bitMaskSize> bitSet, INT value, int pos)
{
    for(int i = pos; i < bitSet.size(); ++i)
    {
        if(mask[bitMaskSize-i-1] == 'X') 
        {
            bitSet[i] = false;
            setMem(memory, mask, bitSet, value, i+1);
            bitSet[i] = true;
            continue;
        }
        if(mask[bitMaskSize-i-1] == '1') bitSet[i] = true;
    }
    auto memPosition = bitSet.to_ullong();
    //std::cout << bitSet.to_string() << " (" << memPosition << ") = " << value << std::endl;
    memory[memPosition] = value;
}

void processMemSetVersion2(Memory_T & memory, std::string mask, std::string lhs, std::string rhs)
{
    auto memPosition = extractMemPos(lhs);
    auto value = std::stoll(rhs);
    auto bitSet = std::bitset<bitMaskSize>(memPosition);
    if(mask.size() != bitMaskSize) std::cout << "Wrong sized mask " << mask << std::endl;
    setMem(memory, mask, bitSet, value, 0);
}

void runIntializationRoutine(Memory_T & memory, const std::vector< std::pair<std::string, std::string> > & commands, bool version2)
{
    std::string mask;
    for(const auto & c : commands)
    {
        std::string lhs = c.first;
        std::string rhs = c.second;

        auto foundMask = lhs.find("mask");
        if(foundMask != std::string::npos)
        {
            mask = rhs;
            //std::cout << "New mask = " << mask << std::endl;
        } else {
            if(version2) processMemSetVersion2(memory, mask, lhs, rhs);
            else processMemSet(memory, mask, lhs, rhs);
        }
    }
}

auto sumUpMemoryElements(Memory_T& memory)
{
    return std::accumulate(memory.begin(), memory.end(), static_cast<INT>(0), 
                           [](INT s, const std::pair<INT,INT>& e){return s + e.second;});
}

int main()
{
    auto parsedContent = readFromFile("input.txt");
    printContent(parsedContent);

    Memory_T memory;
    runIntializationRoutine(memory, parsedContent, false);
    auto result1 = sumUpMemoryElements(memory);
    std::cout << "Result 1: " << result1 << std::endl;

    Memory_T memory2;
    runIntializationRoutine(memory2, parsedContent, true);
    auto result2 = sumUpMemoryElements(memory2);
    std::cout << "Result 2: " << result2 << std::endl;
}