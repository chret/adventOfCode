#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <functional>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>

class Cups
{
public:
    Cups(std::string initStr)
    {
        cups_ = std::vector<int>(initStr.size());
        for(int i = 0; i < initStr.size(); ++i) cups_[i] = initStr[i] - 48; //WTF c++..
        auto minMax = std::minmax_element(cups_.begin(), cups_.end());
        minLabel_ = *minMax.first;
        maxLabel_ = *minMax.second;
    }

    Cups(std::string initStr, int size)
    {
        cups_ = std::vector<int>(size);
        for(int i = 0; i < initStr.size(); ++i) cups_[i] = initStr[i] - 48;
        minLabel_ = *std::min_element(cups_.begin(), std::next(cups_.begin(), initStr.size()));
        for(int i = initStr.size(); i < size; ++i) cups_[i] = i+1;
        maxLabel_ = size-1;
    }

    int operator()(int i) const {return cups_[this->normalizeCup(i)];}
    int& operator()(int i) {return cups_[this->normalizeCup(i)];}
    auto size() const {return cups_.size();}
    bool isLabelContainedInRange(int label, int begin, int end)
    {
        for(int i = begin; i < end; ++i)
        {
            if(this->operator()(i) == label) return true;
        }
        return false;
    }
    int normalizeCup(int i) const 
    {
        return i%cups_.size();
    }
    int normalizeLabel(int l) const
    {
        if(l < minLabel_) return maxLabel_;
        if(l > maxLabel_) return minLabel_;
        return l;
    }
    int getCupFromLabel(int l) const
    {
        for(int i = 0; i < cups_.size(); ++i)
        {
            if(cups_[i] == l) return i;
        }
        return -1;
    }
    int getDistance(int i, int j) const
    {
        if(j >= i) return j-i;
        else return j-i+cups_.size();
    }
    void moveCups(int srcBegin, int srcEnd, int dstBegin)
    {
        int numCupsToBeMoved = this->getDistance(srcBegin, srcEnd);
        std::vector<int> cupsToBeMoved(numCupsToBeMoved);
        for(int i = 0; i < numCupsToBeMoved; ++i) cupsToBeMoved[i] = this->operator()(i+srcBegin);
        
        int rangeDistance = this->getDistance(srcEnd, dstBegin);
        for(int i = 0; i < rangeDistance; ++i) this->operator()(i+srcBegin) = this->operator()(i+srcEnd);

        for(int i = 0; i < numCupsToBeMoved; ++i) this->operator()(i+srcBegin+rangeDistance) = cupsToBeMoved[i];
    }

    int toNumber() const{
        int cup1 = this->getCupFromLabel(1);
        std::stringstream ss;
        for(int i= 0; i < cups_.size()-1;++i) ss << this->operator()(i+cup1+1);
        return std::stoi(ss.str());
    }

    long long toResult2() const{
        int cup1 = this->getCupFromLabel(1);
        return static_cast<long long>(this->operator()(cup1+1)) * static_cast<long long>(this->operator()(cup1+2));
    }

private:
    std::vector<int> cups_;
    int minLabel_;
    int maxLabel_;
};

std::ostream &operator<<(std::ostream &os, Cups const &c) { 
    for(int i = 0; i < c.size(); ++i) os << c(i) << " ";
    return os;
}

void playRound(Cups & cups, int curCup)
{
    int curLabel = cups(curCup);
    int destinationLabel = cups.normalizeLabel(curLabel - 1);
    int numMoveCups = 3;
    while(cups.isLabelContainedInRange(destinationLabel,curCup+1,curCup+1+numMoveCups))
    {
        destinationLabel = cups.normalizeLabel(--destinationLabel);
    }
    int destinationCup = cups.getCupFromLabel(destinationLabel);
    //std::cout << destinationCup << " " << destinationLabel << std::endl;
    cups.moveCups(curCup+1,curCup+1+numMoveCups,destinationCup+1);
}

int playGameOne(std::string str, int rounds)
{
    Cups cups(str);
    for(int i = 0; i < rounds; ++i)
    {
        //std::cout << "Move " << i+1 << ": " << cups << std::endl;
        playRound(cups, cups.normalizeCup(i));
    }
    return cups.toNumber();
}

auto playGameTwo(std::string str, int rounds, int size)
{
    Cups cups(str,size);
    for(int i = 0; i < rounds; ++i)
    {
        if(i%1000 == 0) std::cout << i+1 << std::endl;
        //int cup1 = cups.getCupFromLabel(1);
        //std::cout << i+1 << ": " << cups << "| " << cup1 << " | "  << cups(cup1+1) << ", " << cups(cup1+2) << std::endl;
        playRound(cups, cups.normalizeCup(i));
    }
    return cups.toResult2();
}

int main()
{
    std::string testStr = "389125467";
    std::string inputStr = "39564287";

    int result1 = playGameOne(inputStr, 100); 
    std::cout << "Result 1: " << result1 << std::endl;

    //auto result2 = playGameTwo(testStr, 200, 40);
    auto result2 = playGameTwo(inputStr, 10000000, 1000000);
    std::cout << "Result 2: " << result2 << std::endl;
}