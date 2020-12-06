#include <iostream>
#include <vector>
#include <string>
#include <set>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

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
    return std::accumulate(answers.begin(), answers.end(), 0, [](int a, std::string s){return a + s.size();});
}

std::vector<std::string> getUnionOfGroupAnswers(std::vector<std::vector<std::string>> & answers)
{
    std::vector<std::string> combinedAnswerOfGroups;
    for( auto& group: answers)
    {
        /*
        // version 1
        std::string groupAnswer = "";
        for(auto answer: group)
        {
            std::string unionOfTwoAnswers = "";
            std::set_union(answer.begin(), answer.end(), groupAnswer.begin(), groupAnswer.end(), 
                            std::inserter(unionOfTwoAnswers,unionOfTwoAnswers.begin()));
            groupAnswer = unionOfTwoAnswers;
        }
        combinedAnswerOfGroups.push_back(groupAnswer);
        */
        // version2
        std::string allAnswers = std::accumulate(group.begin(), group.end(), std::string("")); // append all strings
        std::sort(allAnswers.begin(), allAnswers.end());
        auto last = std::unique(allAnswers.begin(), allAnswers.end()); //extract all duplicate char sequences..
        allAnswers.erase(last, allAnswers.end()); // .. and remove them
        combinedAnswerOfGroups.push_back(allAnswers);
    }
    return combinedAnswerOfGroups;
}

std::vector<std::string> getIntersectionOfGroupAnswers(std::vector<std::vector<std::string>> & answers)
{
    /*
    std::vector<std::string> combinedAnswerOfGroups;
    for( auto& group: answers)
    {
        // version 1
        // std::string groupAnswer = group[0];
        // for(auto answer: group)
        // {
        //     std::string intersectionOfTwoAnswers = "";
        //     std::set_intersection(answer.begin(), answer.end(), groupAnswer.begin(), groupAnswer.end(), 
        //                           std::inserter(intersectionOfTwoAnswers,intersectionOfTwoAnswers.begin()));
        //     groupAnswer = intersectionOfTwoAnswers;
        // }
        // combinedAnswerOfGroups.push_back(groupAnswer);

        // version 2
        std::string intersectionOfAllAnswers = std::accumulate(std::next(group.begin()), group.end(), group[0],
        [](std::string curS, std::string newS){
            std::string intersection = "";
            std::set_intersection(curS.begin(), curS.end(), newS.begin(), newS.end(),
                                  std::inserter(intersection, intersection.begin()));
            return intersection;
            });
        combinedAnswerOfGroups.push_back(intersectionOfAllAnswers);

    }
    return combinedAnswerOfGroups;
    */

    // version 3
    std::vector<std::string> combinedAnswerOfGroups;
    std::transform(answers.begin(), answers.end(), std::back_inserter(combinedAnswerOfGroups),
    [](std::vector<std::string>& group) -> std::string {
        return std::accumulate(std::next(group.begin()), group.end(), group[0],
        [](std::string curS, std::string newS){
            std::string intersection = "";
            std::set_intersection(curS.begin(), curS.end(), newS.begin(), newS.end(),
                                  std::inserter(intersection, intersection.begin()));
            return intersection;
            });
    } );
    return combinedAnswerOfGroups;
}

template<typename T>
void printContent(const std::vector<T>& vec)
{
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(std::cout, " "));
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