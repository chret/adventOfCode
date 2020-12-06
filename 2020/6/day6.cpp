#include <iostream>
#include <vector>
#include <string>
#include <set>

#include <fstream>
#include <sstream>
#include <algorithm>

auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<std::set<char>> content;
    if(inputFile.is_open())
    {
        std::set<char> yesAnswers;
        std::string line;
        while(std::getline(inputFile,line))
        {
            if(line.empty())
            {
                content.push_back(yesAnswers);
                yesAnswers.clear();
                continue;
            }
        
            for(const auto& a: line)
            {
                yesAnswers.insert(a);
            }
        }
        if(!yesAnswers.empty()) content.push_back(yesAnswers);
        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
    return content;
}

auto readFromFile2(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<std::set<char>> content;
    if(inputFile.is_open())
    {
        std::set<char> yesAnswers;
        std::string line;
        bool isFirstEntry = true;
        while(std::getline(inputFile,line))
        {
            if(line.empty())
            {
                content.push_back(yesAnswers);
                yesAnswers.clear();
                isFirstEntry = true;
                continue;
            }
            if(isFirstEntry)
            {
                for(auto& a: line)
                {
                    yesAnswers.insert(a);
                }
                isFirstEntry = false;
            } else 
            {
                std::set<char> yesAnswersPerson;
                for(auto& a: line)
                {
                    yesAnswersPerson.insert(a);
                }
                // find common answers
                std::set<char> commonYesAnswers;
                std::set_intersection(yesAnswers.begin(), yesAnswers.end(),
                                      yesAnswersPerson.begin(), yesAnswersPerson.end(), 
                                      std::inserter(commonYesAnswers, commonYesAnswers.begin()));
                //std::cout << yesAnswers.size() << " $ " << yesAnswersPerson.size() << " -> " << commonYesAnswers.size() << std::endl;
                yesAnswers = commonYesAnswers;

            }

        }
        if(!yesAnswers.empty()) content.push_back(yesAnswers);
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
        std::cout << entry << std::endl;
    }
    std::cout << std::endl;
}


int main()
{
    auto fileContent1 = readFromFile("input.txt");

    int sumOfAnswers = 0;
    for(const auto& group: fileContent1)
    {
        int numGroupAnswers = group.size();
        sumOfAnswers += numGroupAnswers;
    }

    std::cout << "Result 1: " << sumOfAnswers << std::endl;

    auto fileContent2 = readFromFile2("input.txt");
    int sumOfAnswers2 = 0;
    for(const auto& group: fileContent2)
    {
        int numGroupAnswers2 = group.size();
        sumOfAnswers2 += numGroupAnswers2;
    }

    std::cout << "Result 2: " << sumOfAnswers2 << std::endl;

}