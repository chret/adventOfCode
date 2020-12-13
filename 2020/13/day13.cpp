#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>

using INT = long long;

void readFromFile(std::string fileName, INT & departTime, std::vector<INT> & busIds, std::vector<INT> & departingMinutes)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        std::getline(inputFile,line);
        departTime = std::stoi(line);
        std::getline(inputFile,line);
        std::string busId;
        std::stringstream ss(line);
        INT offsetTime = 0;
        while(getline(ss, busId, ','))
        {
            if(busId != "x"){
                busIds.push_back(std::stoi(busId));
                departingMinutes.push_back(offsetTime);
            }
            offsetTime++;
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
        std::cout << e << ",";
    }
    std::cout << "\n=============\n";
}

INT getWaitingTimeAndId(INT departTime, std::vector<INT>& busIds)
{
    INT nextBus = 0;
    INT minimalWaitingTime = departTime;
    for(const auto & id: busIds)
    {
        INT cycles = departTime / id;
        if(departTime % id != 0) ++cycles;
        INT waitingTime = cycles*id - departTime;
        if(waitingTime < minimalWaitingTime)
        {
            nextBus = id;
            minimalWaitingTime = waitingTime;
        }
    }
    return minimalWaitingTime * nextBus;

}




bool checkCondition(long long t, std::vector<INT>& busIds, std::vector<INT>& offsets )
{
    for(int i = 0; i < busIds.size(); ++i)
    {
        if( (t + static_cast<long long>(offsets[i])) % static_cast<long long>(busIds[i]) != 0 ) return false;
    }
    return true;
}

long long solveTask2BruteForce(std::vector<INT>& busIds, std::vector<INT>& offsets)
{
    long long t = 0;
    long long cycleA = static_cast<long long>(busIds[0]);
    while(!checkCondition(t, busIds, offsets))
    {
        t += cycleA;
    }
    return t;
}

std::pair<INT,INT> getRepeatingCycleA(INT incA, INT incB, INT offsetA, INT offsetAB)
{
    INT offsetCycleA = 0;
    while( (offsetCycleA * incA + offsetA + offsetAB) % incB != 0)
    {
        offsetCycleA++;
    }

    INT cycleA = 1;
    while( ( cycleA * incA * offsetAB ) % incB != 0)
    {
        cycleA++;
    }

    auto newIncA = cycleA * incA;
    auto newOffsetA = offsetCycleA * incA + offsetA;
    return {newOffsetA, newIncA};
}

long long solveTask2(std::vector<INT>& busIds, std::vector<INT>& offsets)
{
    std::pair<INT,INT> curRes = {offsets[0],busIds[0]};
    for(int i= 1; i < busIds.size(); ++i)
    {
        curRes = getRepeatingCycleA(curRes.second,busIds[i],curRes.first,offsets[i]);
        //std::cout << curRes.first << " " << curRes.second << std::endl;
    }
    return curRes.first;
}

int main()
{
    INT departTime;
    std::vector<INT> busIds;
    std::vector<INT> departingMinutes;
    readFromFile("input.txt", departTime, busIds, departingMinutes);
    printContent(busIds);
    printContent(departingMinutes);

    INT result1  = getWaitingTimeAndId(departTime, busIds);
    std::cout << "Result 1: " << result1 << std::endl;

    //std::vector<INT> busIdsTest = {3,5,7};
    //std::vector<INT> departingMinutesTest = {0,1,2};
    //std::cout <<  solveTask2(busIdsTest, departingMinutesTest) << std::endl;
    
    auto result2  = solveTask2(busIds, departingMinutes);
    std::cout << "Result 2: " << result2 << std::endl;
}