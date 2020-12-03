#include <iostream>
#include <vector>
#include <string>

#include <fstream>
#include <sstream>
#include <algorithm>


class Coordinate
{
public:
    Coordinate(int x_, int y_) : x(x_), y(y_) {}

    Coordinate operator+(const Coordinate & b)
    {
        return Coordinate(this->x + b.x, this->y + b.y);
    }

    Coordinate operator-(const Coordinate & b)
    {
        return Coordinate(this->x - b.x, this->y - b.y);
    }

    void operator+=(const Coordinate & b)
    {
        this->x += b.x;
        this->y += b.y;
    }

    void mapXPeriodic(int xLength)
    {
        if(x >= xLength) x -= xLength;
    }

    void mapYPeriodic(int yLength)
    {
        if(y >= yLength) y -= yLength;
    }

    int x, y;
};

std::ostream& operator<< (std::ostream &out, Coordinate const& co) {
    out << "(" << co.x << "," << co.y << ")";
    return out;
}

class ForestGrid
{
public:
    ForestGrid(const std::vector<std::string> & fileContent)
    {
        forestMap_ = fileContent;
        horiSize_ = forestMap_[0].length();
        vertSize_ = forestMap_.size();
    }

    bool isTree(const Coordinate& coord) const
    {
        if(forestMap_[coord.y][coord.x] == '#') return true;
        return false;
    }

    int getHorizontalSize() const {return horiSize_;}
    int getVerticalSize() const {return vertSize_;}

private:
    std::vector<std::string> forestMap_;
    int horiSize_;
    int vertSize_;
};

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
        std::abort();
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

int getNumberOfTreesOnWay(ForestGrid& grid, Coordinate coord, Coordinate slope)
{
    int numTrees = 0;
    while(coord.y < grid.getVerticalSize())
    {
        if(grid.isTree(coord)) numTrees++;
        coord += slope;
        coord.mapXPeriodic(grid.getHorizontalSize());
    }
    return numTrees;
}

int main()
{
    auto fileContent = readFromFile("input.txt");
    ForestGrid grid(fileContent);
    Coordinate startCoord(0,0);
    // part 1
    int numTrees = getNumberOfTreesOnWay(grid, startCoord, Coordinate(3,1));
    std::cout << "Result 1: " << numTrees << std::endl;

    // part 2
    std::vector<Coordinate> slopes = {Coordinate(1,1), Coordinate(3,1), Coordinate(5,1), Coordinate(7,1), Coordinate(1,2)};

    long long productOfNumTrees = 1;
    for(auto& slope : slopes)
    {
        int numTreesOfSlope = getNumberOfTreesOnWay(grid, startCoord, slope);
        std::cout << numTreesOfSlope << std::endl;
        productOfNumTrees *= static_cast<long long>(numTreesOfSlope);
    } 
    std::cout << "Result 2: " << productOfNumTrees << std::endl;


}