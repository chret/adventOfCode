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

int counterNumberOfIncreases(const std::vector<int>& vec)
{
    int num = 0;
    for (int i = 0; i < vec.size() - 1; ++i)
    {
        if(vec[i]<vec[i+1]) ++num;
    }
    return num;
}

int counterNumberOfThreeSumIncreases(const std::vector<int>& vec)
{
    int num = 0;
    for (int i = 0; i < vec.size() - 3; ++i)
    {
        if(vec[i]<vec[i+3]) ++num;
    }
    return num;
}

int main()
{
    auto fileContent = readFromFile("input.txt");
    //printContent(fileContent);

    // part 1
    int result1 = counterNumberOfIncreases(fileContent);
    std::cout << "Part 1 result: " << result1 << std::endl;

    // part 2
    int result2 = counterNumberOfThreeSumIncreases(fileContent);
    std::cout << "Part 2 result: " << result2 << std::endl;
}