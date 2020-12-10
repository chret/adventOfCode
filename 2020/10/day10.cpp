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
    std::vector<int> parsedData;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            parsedData.push_back(std::stoi(line));
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

int findMutlipliedJoltDifferences(std::vector<int> & vec)
{
    std::sort(vec.begin(), vec.end());
    //printContent(vec);
    std::vector<int> diffs(vec.size(),0);
    std::adjacent_difference(vec.begin(), vec.end(), diffs.begin());
    int num1JoltDiffs = std::count_if(diffs.begin(), diffs.end(), [](int i){return i == 1;});
    int num3JoltDiffs = std::count_if(diffs.begin(), diffs.end(), [](int i){return i == 3;}) + 1;
    //std::cout << num1JoltDiffs << " " <<  num3JoltDiffs << std::endl;
    return num1JoltDiffs * num3JoltDiffs;
}

auto findAllValidPermutations(std::vector<int> & vec)
{
    std::sort(vec.begin(), vec.end());
    printContent(vec);
    std::vector<int> diffs(vec.size()+2,0);
    std::adjacent_difference(vec.begin(), vec.end(), diffs.begin()+1);
    printContent(diffs);

    diffs.front() = 3;
    diffs.back() = 3;

    auto itNext3 = std::find(diffs.begin()+1, diffs.end(),3);
    auto itCur3 = diffs.begin();
    long long permutations = 1;
    while(itNext3 != diffs.end())
    {
        auto distance = std::distance(itCur3, itNext3) - 1;
        std::cout << distance << std::endl;
        if(distance == 2) permutations *= 2;
        else if(distance == 3) permutations *= 4;
        else if(distance == 4) permutations *= 7;
        else if(distance == 5) permutations *= 13;


        itCur3 = itNext3;
        itNext3 = std::find(itCur3+1, diffs.end(),3);

    }

    return permutations;
}



int main()
{
    auto parsedFileContent = readFromFile("input.txt");
    //printContent(parsedFileContent);

    auto result1 = findMutlipliedJoltDifferences(parsedFileContent);
    std::cout << "Result 1: " << result1 << std::endl;

    auto result2 = findAllValidPermutations(parsedFileContent);
    std::cout << "Result 2: " << result2 << std::endl;

}