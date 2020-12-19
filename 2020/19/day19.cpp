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

void evaluteRules(Rules& rules, FinalRules& finalRules)
{
    while(!rules.empty())
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

int main()
{
    Rules rules;
    FinalRules finalRules;
    std::vector<std::string> messages;
    readFromFile("input.txt", rules, finalRules, messages);
    evaluteRules(rules, finalRules);
    FinalRule zeroRule = finalRules[0];
    std::cout << "Final number of rules: " << zeroRule.size() << std::endl;;
    
    int result1 = std::count_if(messages.begin(), messages.end(), [&zeroRule](std::string s){return isMatchedByRule(s,zeroRule);});
    std::cout << "Result 1: " << result1 << std::endl;
}