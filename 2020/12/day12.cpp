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

class Coordinate
{
public:
    Coordinate(int x_, int y_) : x(x_), y(y_) {}

    Coordinate operator+(const Coordinate & b) const
    {
        return Coordinate(this->x + b.x, this->y + b.y);
    }

    Coordinate operator-(const Coordinate & b) const
    {
        return Coordinate(this->x - b.x, this->y - b.y);
    }

    Coordinate operator*(int s) const
    {
        return Coordinate(this->x * s, this->y * s);
    }

    void operator+=(const Coordinate & b)
    {
        this->x += b.x;
        this->y += b.y;
    }

    bool operator==(const Coordinate & b) const
    {
        return (this->x == b.x) && (this->y == b.y);
    }

    int norm()
    {
        return std::abs(x) + std::abs(y);
    }

    void rotate(Coordinate center, char direction, int degrees)
    {
        Coordinate offset = *this - center;
        if(degrees == 180)
        {
            this->x = center.x - offset.x;
            this->y = center.y - offset.y;
        }else if(degrees == 90)
        {
            if(direction == 'R')
            {
                this->x = center.x + offset.y;
                this->y = center.y - offset.x;
            }
            if(direction == 'L')
            {
                this->x = center.x - offset.y;
                this->y = center.y + offset.x;
            }
        } else  if (degrees == 270)
        {
            if(direction == 'R') rotate(center, 'L', 90);
            if(direction == 'L') rotate(center, 'R', 90);
        } else {
            std::cout << "Rotation not supported!\n";
        }
    }

    int x, y;
};

std::ostream& operator<< (std::ostream &out, Coordinate const& co) {
    out << "(" << co.x << "," << co.y << ")";
    return out;
}

using Inst_T = std::pair<char,int>;

auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<Inst_T> parsedData;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            parsedData.push_back({line[0], std::stoi(line.substr(1))});
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

template< typename T>
void printContent(const std::vector<T>& vec)
{
    std::cout << "=============\n";
    for(const auto & e : vec)
    {
        std::cout << e.first << " " << e.second << std::endl;
    }
    std::cout << "=============\n";
}

class Direction
{
public:
    Direction(char dir) {
        auto it = std::find(directions_.begin(), directions_.end(), dir);
        dirIdx_ = std::distance(directions_.begin(), it);
    }

    void rotate(char direction, int degrees)
    {
        int deltaIdx = degrees / 90;
        if(direction == 'L')
        {
            dirIdx_ -= deltaIdx;
        } else if(direction == 'R')
        {
            dirIdx_ += deltaIdx;
        } else 
        {
            std::cout<< "Wrong direction provided!\n";
        }
        renormalizeDirIdx();
    }

    Coordinate getCoordinate()
    {
        if(dirIdx_ == 0) return Coordinate(0,1);
        else if(dirIdx_ == 1) return Coordinate(1,0);
        else if(dirIdx_ == 2) return Coordinate(0,-1);
        else return Coordinate(-1,0);
        
    }

private:

    void renormalizeDirIdx()
    {
        while(dirIdx_ < 0)
        {
            dirIdx_ += 4;
        }
        while(dirIdx_ > 3)
        {
            dirIdx_ -= 4;
        }
    }

    int dirIdx_;
    std::array<char,4> directions_ = {'N','E','S','W'};
};

Coordinate navigate(Coordinate curPos, Direction curDir, std::vector<Inst_T> & instructions)
{
    for(const auto & inst : instructions)
    {
        char newDir = inst.first;
        int val = inst.second;
        if(newDir == 'L' || newDir == 'R')
        {
            curDir.rotate(newDir, val);
        } else if(newDir == 'F')
        {
            Coordinate movementDir = curDir.getCoordinate();
            curPos += (movementDir * val);
        } else 
        {
            Direction instructionDir(newDir);
            Coordinate movementDir = instructionDir.getCoordinate();
            curPos += (movementDir * val);
        }
    }
    return curPos;
}

Coordinate navigateCorrectly(Coordinate curPos, Coordinate curWaypointPos, std::vector<Inst_T> & instructions)
{
    for(const auto & inst : instructions)
    {
        char newDir = inst.first;
        int val = inst.second;
        if(newDir == 'L' || newDir == 'R')
        {
            curWaypointPos.rotate(curPos, newDir, val);
        } else if(newDir == 'F')
        {
            Coordinate movementDir = curWaypointPos - curPos;
            curPos += (movementDir * val);
            curWaypointPos = curPos + movementDir;
        } else 
        {
            Direction instructionDir(newDir);
            Coordinate movementDir = instructionDir.getCoordinate();
            curWaypointPos += (movementDir * val);
        }
        //std::cout << "curPos: " << curPos << ", waypont: " << curWaypointPos << std::endl;
    }
    return curPos;

}

int main()
{
    auto parsedFileContent = readFromFile("input.txt");
    //printContent(parsedFileContent);

    Coordinate curPos(0,0);
    Direction curDir('E');
    auto finalPos = navigate(curPos, curDir, parsedFileContent);
    int result1 = finalPos.norm();
    std::cout << "Result 1: " << result1 << std::endl;

    Coordinate curWaypointPos(10,1);
    auto finalPos2 = navigateCorrectly(curPos, curWaypointPos, parsedFileContent);
    int result2 = finalPos2.norm();
    std::cout << "Result 2: " << result2 << std::endl;
}