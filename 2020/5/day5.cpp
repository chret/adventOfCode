#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <fstream>
#include <sstream>
#include <algorithm>

auto readFromFile(std::string fileName)
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
        std::abort();
    }
    return content;
}

template<typename T>
void printContent(const std::vector<T>& vec)
{
    for (const auto& entry : vec)
    {
        std::cout << entry << std::endl;
    }
    std::cout << std::endl;
}

using Position = std::pair<int,int>;
Position findSeatPosition(const std::string& code, int rowL, int rowU, int columnL, int columnR)
{
    if(code.empty()) return Position(rowL,columnL);
    auto head = code[0];
    auto tail = code.substr(1);
    int numRows = rowU - rowL + 1;
    int numColumns = columnR - columnL + 1;
    //std::cout << code << " " << rowL << " " << rowU << "  " << columnL << " " << columnR << std::endl;
    switch (head)
    {
    case 'F':
        return findSeatPosition(tail, rowL, rowU-numRows/2, columnL, columnR);
    case 'B':
        return findSeatPosition(tail, rowL+numRows/2, rowU, columnL, columnR);
    case 'L':
        return findSeatPosition(tail, rowL, rowU, columnL, columnR-numColumns/2);
    case 'R':
        return findSeatPosition(tail, rowL, rowU, columnL+numColumns/2, columnR);
    default:
        std::cout << "Invalid string found " << head << std::endl;
        abort();
    }
}

int getSeatID(const Position& position)
{
    return position.first * 8 + position.second;
}

int main()
{
    auto fileContent = readFromFile("input.txt");
    //printContent(fileContent);

    //auto position = findSeatPosition("BBFFBBFRLL",0,127,0,7);
    //std::cout << position.first << " " << position.second << " " << position.first * 8 + position.second << std::endl;

    int highestNumber= 0;
    std::vector<int> occupiedSeats;
    for(const auto& code: fileContent)
    {
        auto position = findSeatPosition(code,0,127,0,7);
        auto seatID = getSeatID(position);
        highestNumber = std::max(highestNumber, seatID);
        occupiedSeats.push_back(seatID);
    }
    std::cout << "Result 1: " << highestNumber << " ( = highest seat number)" << std::endl;

    std::cout << "Number of occupied seats " << occupiedSeats.size() << std::endl;
    std::sort(occupiedSeats.begin(), occupiedSeats.end());
    int startSeat = 0;
    int endSeat = occupiedSeats.size()-1;
    for(int i = startSeat; i < endSeat; ++i)
    {
        int diff = occupiedSeats[i+1] - occupiedSeats[i];
        if(diff == 2){
            std::cout << "Result 2: " << occupiedSeats[i] + 1 << std::endl;
            break;
        }
    }
}