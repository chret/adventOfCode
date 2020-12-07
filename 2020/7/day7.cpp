#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

using Rules_T = std::map<std::string, std::map<std::string,int> >;

bool is_alnum(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

auto extractRuleFromLine(std::string line)
{
    std::string key;
    std::map<std::string, int> content;

    std::string delim = " bags contain ";
    auto found = line.find(delim);
    if(found != std::string::npos) key = line.substr(0,found);
    else std::cout << "Delim \"" << delim << "\" not found in line " << line;

    //std::cout << key << ": ";

    std::string containRuleStr = line.substr(found + delim.size());
    std::stringstream ss(containRuleStr);
    std::string token;
    int amount = -1;
    std::string bagType;
    while(getline(ss,token,' '))
    {
        if(is_alnum(token)) amount = std::stoi(token);
        else{
            size_t foundBag = token.find("bag");
            if(foundBag != std::string::npos)
            {
                // rule complete
                if(amount>0)
                {
                    //std::cout << bagType << " (" << amount << "), ";
                    content.insert({bagType, amount});
                    amount = -1;
                    bagType = "";
                } // else: no other bag
            } else
            {
                if(bagType.empty()) bagType += token;
                else bagType += " " + token;
            } 
        }
    }
    //std::cout << std::endl;
    return std::make_pair(key, content);
}

auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    Rules_T parsedData;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            parsedData.insert(extractRuleFromLine(line));
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

void printContent(const Rules_T& rules)
{
    std::cout << "=============\n";
    for(const auto& rule: rules)
    {
        std::cout << rule.first << ": ";
        for(const auto& elem: rule.second)
        {
            std::cout << elem.first << " (" << elem.second << "), ";
        }
        std::cout << std::endl;
    }
    std::cout << "=============\n";
}

long getNumberOfBags(std::string bag, const Rules_T & rules)
{
    auto mapOfContainedBags = rules.find(bag);
    if(mapOfContainedBags->second.empty()) return 1;

    long numberOfBags = 1;
    for(const auto& bagsInside: mapOfContainedBags->second)
    {
        //std::cout << bag << " -> " << bagsInside.first << " " << bagsInside.second << std::endl;
        numberOfBags += long(bagsInside.second) * getNumberOfBags(bagsInside.first, rules);
    }
    return numberOfBags;
}


int main()
{
    auto parsedFileContent = readFromFile("input.txt");

    std::string myBag = "shiny gold";
    std::set<std::string> containingBags;
    std::set<std::string> bagsToCheck = {myBag};
    std::set<std::string> newBagsToCheck;
    while(!bagsToCheck.empty())
    {
        for(const auto& bagToCheck: bagsToCheck)
        {
            for(const auto& rule: parsedFileContent)
            {
                std::string outerBag = rule.first;
                auto innerBags = rule.second;
                for(const auto& bag: innerBags)
                {
                    if( bag.first == bagToCheck)
                    {
                        if(containingBags.find(outerBag) == containingBags.end())
                        {
                            containingBags.insert(outerBag);
                            newBagsToCheck.insert(outerBag);
                        }
                    }
                }
            }
        }
        bagsToCheck = newBagsToCheck;
        newBagsToCheck.clear();
    }

    std::cout << "Result 1: " << containingBags.size() << std::endl;

    //printContent(parsedFileContent);
    int numberOfBags = getNumberOfBags(myBag, parsedFileContent) - 1; // -1: golden bag already there

    std::cout << "Result 2: " << numberOfBags << std::endl;

}