#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>


auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<long long> parsedData;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            parsedData.push_back(std::stoll(line));
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

template< typename T>
void printContent(const std::vector<T>& vec)
{
    std::cout << "=============\n";
    for(const auto& e: vec)
    {
        std::cout << e << std::endl;
    }
    std::cout << "=============\n";
}


template< typename T>
bool isEntryValid(const std::vector<T>& vec, int idx, int preambleLength)
{
    auto number = vec[idx];
    auto minmax = std::minmax_element(vec.begin() + (idx - preambleLength), vec.begin() + idx);
    if(2 * *minmax.first > number) return false;
    if(2 * *minmax.second < number) return false;
    for(int i = idx-preambleLength; i < idx; ++i)
    {
        for(int j = i+1; j < idx; ++j)
        {
            if(vec[i] + vec[j] == number) return true;
        }
    }
    return false;
}

template< typename T>
T findFirstInvalidEntry(const std::vector<T>& vec, int preambleLength)
{
    for(int idx = preambleLength; idx < vec.size(); ++idx)
    {
        if(!isEntryValid(vec, idx, preambleLength)) return vec[idx];
    }
    return -1;
}

template< typename T>
std::pair<int,int> findContiguousRange(const std::vector<T>& vec, T invalidNumber)
{
    for(int idxBegin = 0; idxBegin < vec.size(); ++idxBegin)
    {
        auto contiguousSum = vec[idxBegin];
        for(int idxEnd = idxBegin + 1; idxEnd < vec.size(); ++idxEnd)
        {
            contiguousSum += vec[idxEnd];
            if(contiguousSum == invalidNumber ) return {idxBegin, idxEnd};
            if(contiguousSum > invalidNumber ) break;
            
        }
    }
    return {-1,-1};
}

template< typename T>
T getWeakness(const std::vector<T>& vec, std::pair<int,int> contiguousRange)
{
    auto minmax = std::minmax_element(vec.begin() + contiguousRange.first, vec.begin() + contiguousRange.second + 1);
    return *minmax.first + *minmax.second;
}

int main()
{
    auto parsedFileContent = readFromFile("input.txt");
    printContent(parsedFileContent);

    int premambleLength = 25;
    auto firstInvalidNumber = findFirstInvalidEntry(parsedFileContent, premambleLength);
    std::cout << "Result 1: " << firstInvalidNumber << std::endl;

    auto contiguousRange = findContiguousRange(parsedFileContent, firstInvalidNumber);
    std::cout << "Result 2: [" << contiguousRange.first << ", " <<  contiguousRange.second << "]";
    std::cout << " -> " << getWeakness(parsedFileContent,contiguousRange) << std::endl;
}