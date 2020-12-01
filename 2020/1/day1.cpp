#include <iostream>
#include <vector>
#include <string>

#include <fstream>

std::vector<int> readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<int> content;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            content.push_back(std::stoi(line));
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

template<typename T>
void printContent(const std::vector<T>& vec)
{
    for (const auto& entry : vec)
    {
        std::cout << entry << " ";
    }
    std::cout << std::endl;
}

int computeProductOfSoughtEntries(const std::vector<int>& vec, int expectedSum)
{
    for (int i = 0; i < vec.size() - 1; ++i)
    {
        for (int j = i+1; j < vec.size(); ++j)
        {
            if(vec[i] + vec[j] == expectedSum)
            {
                return vec[i] * vec[j];
            }
        }
    }
}

int main()
{
    auto fileContent = readFromFile("input.txt");
    //printContent(fileContent);
    int expectedSum = 2020;
    int result = computeProductOfSoughtEntries(fileContent, expectedSum);
    std::cout << "Result: " << result << std::endl;
}