#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <fstream>


std::vector<std::string> readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<std::string> content;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            content.push_back(line);
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

int getNumberOfOnesAtPos(std::vector<std::string>& p, int pos)
{
    return std::count_if(p.begin(), p.end(), [pos](const std::string& s){s[pos] == '1';});
}

std::pair<int,int> evaluateProtocol(std::vector<std::string>& protocol)
{
    auto length = protocol[0].size();

    std::string gammaRateStr(""), epsilonRateStr("");
    for(int i = 0; i < length;++i)
    {
        int count = getNumberOfOnesAtPos(protocol, i);
        if(2*count > protocol.size())
        {
            gammaRateStr += "1";
            epsilonRateStr += "0";
        }else
        {
            gammaRateStr += "0";
            epsilonRateStr += "1";
        }
    }
    int gammaRate = std::stoi(gammaRateStr, nullptr, 2);
    int epsilonRate = std::stoi(epsilonRateStr, nullptr, 2);
    int powerConsumption = gammaRate * epsilonRate;

    // part 2
    std::vector<std::string> protOxy = protocol;
    for(int i = 0; i < length; ++i)
    {
        int numElems = protOxy.size();
        if(numElems == 1) break;
        int countOne = getNumberOfOnesAtPos(protOxy,i);
        auto num = (2*countOne >= numElems) ? '1' : '0';
        protOxy.erase(std::remove_if(protOxy.begin(), protOxy.end(), [i,num](std::string& s){return s[i] != num;}), protOxy.end());
        //std::cout << protOxy.size() << std::endl;
    }

    std::vector<std::string> protCO2 = protocol;
    for(int i = 0; i < length; ++i)
    {
        int numElems = protCO2.size();
        if(numElems == 1) break;
        int countOne = getNumberOfOnesAtPos(protCO2,i);
        auto num = (2*countOne >= numElems) ? '0' : '1';
        protCO2.erase(std::remove_if(protCO2.begin(), protCO2.end(), [i,num](std::string& s){return s[i] != num;}), protCO2.end());
        //std::cout << protCO2.size() << std::endl;
    }

    int oxyGenRate = std::stoi(protOxy[0], nullptr, 2);
    int co2Rate = std::stoi(protCO2[0], nullptr, 2);
    int lifeSupportRating = oxyGenRate * co2Rate;
    return std::make_pair(powerConsumption, lifeSupportRating);
}


int main()
{
    auto fileContent = readFromFile("input.txt");
    //printContent(fileContent);

    // part 1
    auto result = evaluateProtocol(fileContent);
    std::cout << "Part 1 result: " << result.first << std::endl;
    std::cout << "Part 2 result: " << result.second << std::endl;

}