#include <iostream>
#include <vector>
#include <string>
#include <utility>

#include <fstream>

using Entry_T = std::pair<std::string,int>;

std::vector<Entry_T> readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<Entry_T> content;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            auto wsPos = line.find(" ");
            content.push_back(std::make_pair(line.substr(0,wsPos), std::stoi(line.substr(wsPos+1))));
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

std::pair<int,int> steerSubmarine(std::vector<Entry_T> & instructions)
{
    std::pair<int,int> position(0,0); // x,y
    for(const auto& instr: instructions)
    {
        auto keyword = instr.first;
        auto amount = instr.second;
        if(keyword == "forward") position.first += amount;
        else if(keyword == "up") position.second -= amount;
        else if(keyword == "down") position.second += amount;
        else std::cout << "Unknown keyword " << keyword << std::endl;
    }
    return position;
}

std::pair<int,int> steerSubmarineCorrectly(std::vector<Entry_T> & instructions)
{
    std::pair<int,int> position(0,0); // x,y
    int aim = 0;
    for(const auto& instr: instructions)
    {
        auto keyword = instr.first;
        auto amount = instr.second;
        if(keyword == "forward") 
        {
            position.first += amount;
            position.second += aim * amount;
        }
        else if(keyword == "up") aim -= amount;
        else if(keyword == "down") aim += amount;
        else std::cout << "Unknown keyword " << keyword << std::endl;
    }
    return position;
}


int main()
{
    auto fileContent = readFromFile("input.txt");
    //printContent(fileContent);

    // part 1
    auto result1 = steerSubmarine(fileContent);
    std::cout << "Part 1 result: " << result1.first*result1.second << std::endl;

    // part 2
    auto result2 = steerSubmarineCorrectly(fileContent);
    std::cout << "Part 2 result: " << result2.first*result2.second << std::endl;
}