#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <fstream>
#include <sstream>
#include <algorithm>

using Passport_T = std::map<std::string, std::string> ;


auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<Passport_T> content;
    if(inputFile.is_open())
    {
        std::string line;
        Passport_T curPassport;
        while(std::getline(inputFile,line))
        {
            if(line.size() == 0)
            {
                content.push_back(curPassport);
                curPassport.clear();
            }

            // parse line
            std::stringstream ss(line);
            while(getline(ss, line, ' ')){
                auto delimPos = line.find(":");
                std::string key = line.substr(0,delimPos);
                std::string value = line.substr(delimPos+1);
                curPassport[key] = value;

            }
        }
        content.push_back(curPassport);
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
        for (const auto& elem : entry)
        {
            std::cout << elem.first << ": " << elem.second << std::endl;

        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool checkPassport(Passport_T& passport)
{
    if(passport.size() == 8) return true;
    if(passport.size() == 7)
    {
        if(passport.find("cid") == passport.end()) return true;
    }
    return false;
}

bool is_digits(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool is_alnum(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isalnum);
}

bool checkDigit(const std::string &value, int lower, int upper)
{
    if(is_digits(value))
    {
        int digit = std::stoi(value);
        if(digit >= lower && digit <= upper)
        {
            return true;
        }
    }
    return false;
}

bool checkHeight(const std::string &value)
{
    int cmPos = value.find("cm");
    if(cmPos !=std::string::npos)
    {
        int heightInCm = std::stoi(value.substr(0,cmPos));
        if(heightInCm >= 150 && heightInCm <= 193) return true;
        return false;
    }
    int inPos = value.find("in");
    if(inPos !=std::string::npos)
    {
        int heightInIn = std::stoi(value.substr(0,inPos));
        if(heightInIn >= 59 && heightInIn <= 76) return true;
        return false;
    }
    return false;
}

bool checkHairColor(const std::string & value)
{
    if(value[0] == '#')
    {
        std::string colorVal = value.substr(1);
        return colorVal.find_first_not_of("0123456789abcdef") == std::string::npos;
    }
    return false;
}

bool checkEyeColor(const std::string & value)
{
    std::vector<std::string> validColors = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
    for (const auto& validColor: validColors)
    {
        if(value == validColor) return true;
    }
    return false;
}

bool checkPassportID(const std::string & value)
{
    if(value.size() == 9)
    {
        if(is_digits(value)) return true;
    }
    return false;
}

bool checkEntry(const std::string& key, const std::string value)
{
    if(key == "byr")
    {
        return checkDigit(value, 1920, 2002);
    }
    if(key == "iyr")
    {
        return checkDigit(value, 2010, 2020);
    }
    if(key == "eyr")
    {
        return checkDigit(value, 2020, 2030);
    }
    if(key == "hgt")
    {
        return checkHeight(value);
    }
    if(key == "hcl")
    {
        return checkHairColor(value);
    }
    if(key == "ecl")
    {
        return checkEyeColor(value);
    }
    if(key == "pid")
    {
        return checkPassportID(value);
    }
    if(key == "cid")
    {
        return true;
    }
    return false;
}

bool checkPassportPart2(Passport_T& passport)
{
    if(checkPassport(passport))
    {
        bool isValid = true;
        for (const auto& entry : passport)
        {
            std::string key= entry.first;
            std::string value = entry.second;

            isValid = isValid & checkEntry(key, value);
        }
        return isValid;
    }

    return false;
}



int main()
{
    auto fileContent = readFromFile("input.txt");
    //printContent(fileContent);

    int numValidPassports = std::count_if(fileContent.begin(), fileContent.end(), checkPassport);
    std::cout << "Result 1: " << numValidPassports << std::endl;

/*
    std::string testKey = "pid";
    std::string testVal = "0123456789";
    std::cout << checkEntry(testKey, testVal) << std::endl;
    */

    int numValidPassportsPart2 = std::count_if(fileContent.begin(), fileContent.end(), checkPassportPart2);
    std::cout << "Result 2: " << numValidPassportsPart2 << std::endl;

    return 0;

}