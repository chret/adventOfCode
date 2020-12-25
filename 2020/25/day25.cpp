#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>

using INT = long long;

INT transformSingleStep(INT subjectNumber, INT value, INT divisor = 20201227)
{
    return (value * subjectNumber) % divisor;
}

INT transformSubjectNumber(INT subjectNumber, INT loopSize, INT divisor = 20201227)
{
    INT value = 1;
    for(INT i = 0; i < loopSize; ++i)
    {
        value = transformSingleStep(subjectNumber, value, divisor);
    }
    return value;
}

INT findLoopSize(INT key, INT subjectNumber)
{
    INT loopSize = 0;
    INT value = 1;

    for(INT value = 1; value != key; value = transformSingleStep(subjectNumber, value))
    {
        ++loopSize;
    }
    return loopSize;
}


int main()
{
    //INT cardKey = 5764801;
    //INT doorKey = 17807724;
    INT cardKey = 14788856;
    INT doorKey = 19316454;

    INT subjectNumber = 7;

    INT cardLoopSize = findLoopSize(cardKey, subjectNumber);
    //INT doorLoopSize = findLoopSize(doorKey, subjectNumber);
    std::cout << cardLoopSize << std::endl;

    INT encryptionKey = transformSubjectNumber(doorKey, cardLoopSize);
    
    auto result1 = encryptionKey; 
    std::cout << "Result 1: " << result1 << std::endl;
}