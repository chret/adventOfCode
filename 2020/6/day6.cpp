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
    std::vector<std::vector<std::string>> content;
    if(inputFile.is_open())
    {
        std::vector<std::string> yesAnswersOfGroup;
        std::string line;
        while(std::getline(inputFile,line))
        {
            if(line.empty())
            {
                content.push_back(yesAnswersOfGroup);
                yesAnswersOfGroup.clear();
                continue;
            }

            std::sort(line.begin(), line.end()); // sorting later required in union/intersection algorithms
            yesAnswersOfGroup.push_back(line);
        }
        if(!yesAnswersOfGroup.empty()) content.push_back(yesAnswersOfGroup);
        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
    return content;
}

int getSumOfAnwers(const std::vector<std::string>& answers)
{
    int sumOfAnswers = 0;
    for(const auto& group: answers)
    {
        int numGroupAnswers = group.size();
        sumOfAnswers += numGroupAnswers;
    }
    return sumOfAnswers;
}

std::vector<std::string> getUnionOfGroupAnswers(std::vector<std::vector<std::string>> & answers)
{
    std::vector<std::string> combinedAnswerOfGroups;
    for( auto& group: answers)
    {
        std::string groupAnswer = "";
        for(auto answer: group)
        {
            std::string unionOfTwoAnswers = "";
            std::set_union(answer.begin(), answer.end(), groupAnswer.begin(), groupAnswer.end(), 
                           std::inserter(unionOfTwoAnswers,unionOfTwoAnswers.begin()));
            groupAnswer = unionOfTwoAnswers;
        }
        combinedAnswerOfGroups.push_back(groupAnswer);
    }
    return combinedAnswerOfGroups;
}

std::vector<std::string> getIntersectionOfGroupAnswers(std::vector<std::vector<std::string>> & answers)
{
    std::vector<std::string> combinedAnswerOfGroups;
    for( auto& group: answers)
    {
        std::string groupAnswer = group[0];
        for(auto answer: group)
        {
            std::string intersectionOfTwoAnswers = "";
            std::set_intersection(answer.begin(), answer.end(), groupAnswer.begin(), groupAnswer.end(), 
                                  std::inserter(intersectionOfTwoAnswers,intersectionOfTwoAnswers.begin()));
            groupAnswer = intersectionOfTwoAnswers;
        }
        combinedAnswerOfGroups.push_back(groupAnswer);
    }
    return combinedAnswerOfGroups;
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
    auto parsedFileContent = readFromFile("input.txt");

    auto result1 = getUnionOfGroupAnswers(parsedFileContent);
    //printContent(result1);
    std::cout << "Result 1: " << getSumOfAnwers(result1) << std::endl;

    auto result2 = getIntersectionOfGroupAnswers(parsedFileContent);
    //printContent(result2);
    std::cout << "Result 2: " << getSumOfAnwers(result2) << std::endl;

}