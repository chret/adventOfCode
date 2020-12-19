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

using Rule = std::vector< std::vector<int> >;
using Rules = std::map<int, Rule >;
using FinalRule = std::vector< std::string >;
using FinalRules = std::map<int, FinalRule >;


void readFromFile(std::string fileName, Rules& rules, FinalRules& finalRules, 
                  std::vector<std::string>& messages)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            //std::cout << line << std::endl;
            if(line.empty()) break;

            auto sepPos = line.find(":");
            int ruleNumber = std::stoi(line.substr(0,sepPos));
            line = line.substr(sepPos+2);

            std::vector<int> subRule;
            Rule currentRule;
            std::stringstream ss(line);
            std::string elem;
            while(std::getline(ss,elem,' '))
            {
                //std::cout << elem << std::endl;
                if(elem.find("\"")!=std::string::npos)
                {
                    FinalRule finalRule(1,elem.substr(1,1));
                    finalRules[ruleNumber] = finalRule;
                    break;
                } else if (elem.find("|")!=std::string::npos)
                {
                    currentRule.push_back(subRule);
                    subRule.clear();
                } else 
                {
                    subRule.push_back(std::stoi(elem));
                }
            }
            if(!subRule.empty())
            {
                currentRule.push_back(subRule);
                rules[ruleNumber] = currentRule;
            }

        }

        while(std::getline(inputFile,line))
        {
            messages.push_back(line);
        }

        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
}

bool canRuleBeEvaluated(Rule & rule, FinalRules& finalRules)
{
    for(const auto & r: rule)
    {
        for(const auto & ruleNumber: r)
        {
            if(finalRules.find(ruleNumber) == finalRules.end()) return false;
        }
    }
    return true;
}

FinalRule combineWithNextRule(const FinalRule & curRule, const FinalRule & nextRule)
{
    FinalRule newRule;
    for(int i = 0; i < curRule.size(); ++i)
    {
        for(int j = 0; j < nextRule.size(); ++j)
        {
            newRule.push_back(curRule[i] + nextRule[j]);
        }
    }
    return newRule;
}

FinalRule evaluateRule(Rule & rule, FinalRules& finalRules)
{
    FinalRule finalRule;
    for(const auto & r: rule)
    {
        FinalRule curFinalRule(1,"");
        for(const auto & ruleNumber: r)
        {
            auto nextRuleIt = finalRules.find(ruleNumber);
            if(nextRuleIt != finalRules.end())
            {
                curFinalRule = combineWithNextRule(curFinalRule, nextRuleIt->second);
            }
        }
        finalRule.insert(finalRule.end(), curFinalRule.begin(), curFinalRule.end());
    }
    return finalRule;
}

void evaluateRules(Rules& rules, FinalRules& finalRules, int terminationSize = 0)
{
    while(rules.size() > terminationSize)
    {
        for(auto ruleIt = rules.begin(); ruleIt != rules.end(); ++ruleIt)
        {
            int ruleNumber = ruleIt->first;
            auto curRule = ruleIt->second;
            if(canRuleBeEvaluated(curRule, finalRules))
            {
                auto newFinalRule = evaluateRule(curRule, finalRules);
                finalRules[ruleNumber] = newFinalRule;
                ruleIt = rules.erase(ruleIt);
            }
        }
    }
}

bool isMatchedByRule(std::string msg, FinalRule & rule)
{
    return std::find(rule.begin(), rule.end(), msg) != rule.end();
}

int numberOfMatchesFromFront(std::string msg, FinalRule & rule)
{
    int numMatches = 0;
    for(const auto & r : rule)
    {
        int rLength = r.size();
        std::string frontSubStr = msg.substr(0,rLength);
        if(frontSubStr == r) return 1 + numberOfMatchesFromFront(msg.substr(rLength), rule);
    }
    return numMatches;
}

int numberOfMatchesFromBack(std::string msg, FinalRule & rule)
{
    for(const auto & r : rule)
    {
        int rLength = r.size();
        int beginR = msg.size()-rLength;
        if(beginR > 0)
        {
            std::string backSubStr = msg.substr(beginR);
            if(backSubStr == r) return 1 + numberOfMatchesFromBack(msg.substr(0,beginR), rule);
        }
    }
    return 0;
}

bool isMatchedByLoopedRules(std::string msg, FinalRule & rule42, FinalRule & rule31)
{
    // pattern: *42 *31, but number of matches of 31 has to be < number of matches of 42
    // also, "fully matched" -> no unmatched remainder in message

    int sizeRule42 = rule42[0].size();
    int sizeRule31 = rule31[0].size();
    int numMatches42 = numberOfMatchesFromFront(msg, rule42);
    int numMatches31 = numberOfMatchesFromBack(msg, rule31);
    int remainderMsgSize = msg.size() - numMatches31 * sizeRule31 - numMatches42 * sizeRule42;
    
    //std::cout << msg << " " << numMatches42 << " " << numMatches31 << " " << remainderMsgSize << std::endl;
    return (remainderMsgSize == 0) && (numMatches42 > numMatches31)  && (numMatches31 > 0);
}

int main()
{
    Rules rules;
    FinalRules finalRules;
    std::vector<std::string> messages;
    readFromFile("input.txt", rules, finalRules, messages);

    Rules rulesPart1 = rules;
    FinalRules finalRulesPart1 = finalRules;
    evaluateRules(rulesPart1, finalRulesPart1);
    FinalRule zeroRule = finalRulesPart1[0];
    std::cout << "Final number of rules: " << zeroRule.size() << std::endl;
    int result1 = std::count_if(messages.begin(), messages.end(), [&zeroRule](std::string s){return isMatchedByRule(s,zeroRule);});
    std::cout << "Result 1: " << result1 << std::endl;

    Rules rulesPart2 = rules;
    rulesPart2[8] = {{42},{42,8}};
    rulesPart2[11] = {{42,31},{42,11,31}};
    FinalRules finalRulesPart2 = finalRules;
    evaluateRules(rulesPart2, finalRulesPart2, 3);
    FinalRule rule42 = finalRulesPart1[42];
    FinalRule rule31 = finalRulesPart1[31];
    
    std::cout << "Number of rules: " << rule42.size() << " " << rule31.size() << std::endl;
    int result2 = std::count_if(messages.begin(), messages.end(), [&rule42,&rule31](std::string s){return isMatchedByLoopedRules(s,rule42,rule31);});
    std::cout << "Result 2: " << result2 << std::endl;
}