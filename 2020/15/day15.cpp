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

using INT = long long;

template< typename T>
void printContent(const std::vector<T>& vec)
{
    std::cout << "=============\n";
    for(const auto & e : vec)
    {
        std::cout << e.first << " " << e.second << "\n";
    }
    std::cout << "=============\n";
}

INT playGame(std::vector<INT> numbers, INT endRound)
{
    INT round = numbers.size();
    while(round < endRound)
    {
        INT lastNumber = numbers[numbers.size()-1];
        INT distanceToLastPos = -1;
        for(INT i = 1; i < numbers.size();++i)
        {
            if(numbers[numbers.size()-1-i] == lastNumber) 
            {
                distanceToLastPos = i;
                break;
            }
        }
        if(distanceToLastPos != -1)
        {
            numbers.push_back(distanceToLastPos);

        } else
        {
            numbers.push_back(0);
        }
        //std::cout << lastNumber << " -> " << numbers[numbers.size()-1] << std::endl;
        ++round;
    }
    return numbers[numbers.size()-1];
}

INT playGameImproved(std::vector<INT> numbers, INT endRound)
{
    std::map<INT,INT> numbersMap; // pair<numbers, round>
    for(int i = 0; i < numbers.size()-1; ++i)
    {
        numbersMap[numbers[i]] = i+1;
    }
    INT round = numbers.size();
    INT lastNumber = numbers[numbers.size()-1];

    // last element is not yet in the map to avoid finding it

    INT newLastNumber = lastNumber;
    while(round < endRound)
    {
        auto found = numbersMap.find(lastNumber);
        if(found != numbersMap.end())
        {
            newLastNumber = round - found->second; 

        } else 
        {
            newLastNumber = 0;
        }
        numbersMap[lastNumber] = round;
        lastNumber = newLastNumber;
        ++round;
    }
    return lastNumber;
}


int main()
{
    //std::vector<INT> startingNumbers = {0,3,6};
    std::vector<INT> startingNumbers = {19,20,14,0,9,1};
    INT endRound1 = 2020;
    INT result1 = playGame(startingNumbers, endRound1);
    std::cout << "Result 1: " << result1 << std::endl;

    INT endRound2 = 30000000;
    INT result2 = playGameImproved(startingNumbers, endRound2);
    std::cout << "Result 2: " << result2 << std::endl;
}