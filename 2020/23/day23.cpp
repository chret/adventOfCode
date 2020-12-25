#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <functional>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <chrono>

using Cups_T = std::list<int>;

void print(Cups_T & cups)
{
    for(const auto & c : cups)
    {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}

Cups_T createCupList(std::string initStr, int& minLabel, int& maxLabel, int& size)
{
    Cups_T cups;
    minLabel = initStr.size();
    maxLabel = 0;
    for(int i = 0; i < initStr.size(); ++i)
    {   
        int label = initStr[i] - 48;
        minLabel = std::min(minLabel, label);
        maxLabel = std::max(maxLabel, label); 
        cups.push_back(label);    
    }
    size = initStr.size();
    return cups;
}

void extendToSize(Cups_T & cups, int size, int& maxLabel)
{
    for(int i = maxLabel+1; i <= size; ++i)
    {   
        cups.push_back(i);    
    }
    maxLabel = size;
}

int normalizeLabel(int label, int minLabel, int maxLabel) 
{
    if(label < minLabel) return maxLabel;
    if(label > maxLabel) return minLabel;
    return label;
}

using Clock = std::chrono::steady_clock;

void playGame(Cups_T & cups, int size, int rounds, int minLabel, int maxLabel)
{
    double d1(0), d2(0), d3(0);

    // additional data structure for speeding up finding position of a label
    std::vector<Cups_T::iterator> iteratorStorage(size);
    for(auto it = cups.begin(); it != cups.end(); ++it)
    {
        iteratorStorage[*it-1] = it;
    }

    for(int i = 0; i < rounds; ++i)
    {
        auto t1 = Clock::now();

        int curLabel = cups.front();
        cups.pop_front();
        cups.push_back(curLabel);
        iteratorStorage[curLabel-1] = std::next(cups.end(),-1);
        int destinationLabel = normalizeLabel(curLabel - 1, minLabel, maxLabel);

        auto moveCups = Cups_T(cups.begin(), std::next(cups.begin(),3));
        cups.erase(cups.begin(),std::next(cups.begin(),3));
        for(auto it = moveCups.begin(); it != moveCups.end(); ++it)
        {
            iteratorStorage[*it-1] = it;
        }

        while(std::find(moveCups.begin(), moveCups.end(), destinationLabel) != moveCups.end())
        {
            destinationLabel = normalizeLabel(--destinationLabel, minLabel, maxLabel);
        }

        auto t2 = Clock::now();

        //auto findIt = std::find(cups.begin(), cups.end(), destinationLabel);
        auto findIt = iteratorStorage[destinationLabel-1]; 

        auto t3 = Clock::now();

        cups.splice(std::next(findIt,1),moveCups);

        auto t4 = Clock::now();

        std::chrono::duration<double> dur1 = t2-t1;
        std::chrono::duration<double> dur2 = t3-t2;
        std::chrono::duration<double> dur3 = t4-t3;

        d1 += dur1.count();
        d2 += dur2.count();
        d3 += dur3.count();
    }
    //std::cout << d1 << " " << d2 << " " << d3 << std::endl;
}

int getResult1(Cups_T & cups)
{
    auto findIt1 = std::find(cups.begin(), cups.end(), 1);
    std::stringstream ss;
    for(auto it = std::next(findIt1,1); it != cups.end(); ++it) ss << *it;
    for(auto it = cups.begin(); it != findIt1; ++it) ss << *it;
    return std::stoi(ss.str());
}

long long getResult2(Cups_T & cups)
{
    auto findIt1 = std::find(cups.begin(), cups.end(), 1);
    int numbers = 2;
    int num = 0;
    long long result = 1;
    for(auto it = std::next(findIt1,1); it != cups.end(); ++it)
    {
        if(num == numbers) break;
        result *= *it;
        ++num;
    }
    for(auto it = cups.begin(); it != findIt1; ++it)
    {
        if(num == numbers) break;
        result *= *it;
        ++num;
    }
    return result;
}

int main()
{
    //std::string inputStr = "389125467"; // test
    std::string inputStr = "916438275";

    int rounds = 100;

    int minLabel, maxLabel, size;
    auto cups = createCupList(inputStr, minLabel, maxLabel, size);
    playGame(cups, size, rounds, minLabel, maxLabel);
    //print(cups);
    int result1 = getResult1(cups); 
    std::cout << "Result 1: " << result1 << std::endl;

    int rounds2 = 10000000;
    int targetSize = 1000000;

    int minLabel2, maxLabel2, size2;
    auto cups2 = createCupList(inputStr, minLabel2, maxLabel2, size2);
    extendToSize(cups2, targetSize, maxLabel2);
    playGame(cups2, targetSize, rounds2, minLabel2, maxLabel2);
    auto result2 = getResult2(cups2); 
    std::cout << "Result 2: " << result2 << std::endl;
    
}