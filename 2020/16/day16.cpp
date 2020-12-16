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


void readFromFile(std::string fileName, std::map<std::string, std::vector<int>>& rules, std::vector<std::vector<int>> & tickets)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            //std::cout << line << std::endl;
            if(line.empty() ) break;

            std::vector<int> ranges;
            auto sep = line.find(": ");
            std::string key = line.substr(0,sep);
            line = line.substr(sep+2);

            sep = line.find("-");
            ranges.push_back(std::stoi(line.substr(0,sep)));
            line = line.substr(sep+1);

            sep = line.find(" or ");
            ranges.push_back(std::stoi(line.substr(0,sep)));
            line = line.substr(sep+4);

            sep = line.find("-");
            ranges.push_back(std::stoi(line.substr(0,sep)));
            line = line.substr(sep+1);

            ranges.push_back(std::stoi(line));

            rules[key] = ranges;
        }

        while(std::getline(inputFile,line))
        {
            //std::cout << line << std::endl;
            if(!::isdigit(line[0])) continue;
            std::stringstream ss(line);
            std::string number;
            std::vector<int> numbersOfTicket;
            while(getline(ss, number, ',')){
                numbersOfTicket.push_back(std::stoi(number));
            }
            tickets.push_back(numbersOfTicket);
        }

        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
}

template< typename T>
void printContent(const std::vector<T>& vec)
{
    std::cout << "=============\n";
    for(const auto & e : vec)
    {
        std::cout << e << "\n";
    }
    std::cout << "=============\n";
}

bool isNumberInRanges(int number, std::vector<int>& ranges)
{
    return (number >= ranges[0] && number <= ranges[1]) || (number >= ranges[2] && number <= ranges[3]);
}

bool isNumberValid(int number, std::map<std::string, std::vector<int>>& rules)
{
    for(const auto & r: rules)
    {
        auto ranges = r.second;
        bool isInRange = isNumberInRanges(number, ranges);
        if(isInRange) return true;
    }
    return false;
}

int getTicketScanningErrorRate(std::map<std::string, std::vector<int>>& rules, std::vector<std::vector<int>> & tickets)
{
    int rate = 0;
    auto it = std::begin(tickets);
    while(it != std::end(tickets))
    {
        bool remove = false;
        for(const auto& number: *it)
        {
            if(!isNumberValid(number, rules))
            {
                rate += number;
                remove = true;
            }     
        }
        if(remove) it = tickets.erase(it);
        else ++it;
    }
    return rate;
}



auto mapPositionToType(std::map<std::string, std::vector<int>>& rules, std::vector<std::vector<int>> & tickets)
{
    std::vector<std::vector<std::string>> typeAtPosition(rules.size());

    for(int position = 0; position < rules.size(); ++position)
    {
        for(const auto& r: rules)
        {
            std::string ruleName = r.first;
            auto ranges = r.second;
            bool ruleValid = true;
            
            for(const auto & ticket: tickets)
            {
                int number = ticket[position];
                bool isInRange = isNumberInRanges(number, ranges);
                if(!isInRange)
                {
                    ruleValid = false;
                    break;
                }
            }
            if(ruleValid)
            {
                typeAtPosition[position].push_back(ruleName);
            }
        }
    }
    return typeAtPosition;
}

void pruneFromRule(std::string rule, std::vector<std::vector<std::string>>& ruleOrdering)
{
    for(int i= 0; i < ruleOrdering.size(); ++i)
    {
        auto & possibleRules = ruleOrdering[i];
        if(possibleRules.size() > 1)
        {
            auto it = std::find(possibleRules.begin(), possibleRules.end(), rule);
            if(it != possibleRules.end())
            {
                possibleRules.erase(it);
            }
        }
    }
}

auto pruneOrdering(std::vector<std::vector<std::string>>& ruleOrdering)
{
    int position = 0;
    while(true)
    {
        auto possibleRules = ruleOrdering[position];
        if(possibleRules.size() == 1)
        {
            auto uniqueRule = possibleRules[0];
            pruneFromRule(uniqueRule, ruleOrdering);
        }
        ++position;
        if(position == ruleOrdering.size())
        {
            if(std::all_of(ruleOrdering.begin(), ruleOrdering.end(), [](std::vector<std::string>&s){return s.size() == 1;}))
            {
                break;
            }
            else
            {
                position = 0;
            }
        }
    }

    std::vector<std::string> uniqueRules;
    for(int i = 0; i < ruleOrdering.size(); ++i)
    {
        uniqueRules.push_back(ruleOrdering[i][0]);
    }
    return uniqueRules;
}

auto getResult2(std::vector<int> & myTicket, std::vector<std::string>& rules)
{
    long long result = 1;
    for(int i = 0; i < myTicket.size(); ++i)
    {
        if(rules[i].find("departure") == 0)
        {
            result *= myTicket[i];
        }
    }
    return result;
}

int main()
{
    std::map<std::string, std::vector<int>> rules; 
    std::vector<std::vector<int>> tickets;
    readFromFile("input.txt", rules, tickets);

    int result1 = getTicketScanningErrorRate(rules, tickets);
    std::cout << "Result 1: " << result1 << std::endl;

    auto ruleOrdering = mapPositionToType(rules, tickets);
    auto uniqueRules = pruneOrdering(ruleOrdering);
    auto result2 = getResult2(tickets[0], uniqueRules);
    std::cout << "Result 2: " << result2 << std::endl;

}