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

int countAndRemoveSubstring(std::string& str, const std::string& sub)
{
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos; offset = str.find(sub, offset))
    {
        ++count;
        str.erase(offset,sub.length());
    }
    return count;
}

class Coordinate
{
public:
    explicit Coordinate(int a_) : x(a_), y(a_){}
    Coordinate(int x_, int y_) : x(x_), y(y_) {}

    Coordinate operator+(const Coordinate & b) const
    {
        return Coordinate(this->x + b.x, this->y + b.y);
    }

    void operator+=(const Coordinate & b)
    {
        this->x += b.x;
        this->y += b.y;
    }

    Coordinate operator*(int a)
    {
        return Coordinate(a * this->x, a* this->y);
    }

    bool operator==(const Coordinate & b) const
    {
        return (this->x == b.x) && (this->y == b.y);
    }

    int x, y;
};

std::ostream& operator<< (std::ostream &out, Coordinate const& co) {
    out << "(" << co.x << "," << co.y << ")";
    return out;
}

// https://stackoverflow.com/questions/9729390/how-to-use-unordered-set-with-custom-types/9729747#9729747
template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct coordinate_hash
{
    inline std::size_t operator()(const Coordinate & v) const
    {
         std::size_t seed = 0;
         hash_combine(seed, v.x);
         hash_combine(seed, v.y);
         return seed;
    }
};

using Grid_T = std::unordered_set<Coordinate, coordinate_hash>;

enum Dir {e, se, sw, w, nw, ne};
std::array<std::string,6> directionStrings = {"e", "se", "sw", "w", "nw", "ne"};
std::array<Coordinate,6> directions = {Coordinate( 1,0), Coordinate( 1,-1), Coordinate(0,-1),
                                       Coordinate(-1,0), Coordinate(-1, 1), Coordinate(0, 1)};



auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<std::vector<int> > parsedData;
    std::array<int,6> searchingOrder = {1,2,4,5,0,3};
    int x(0),y(0),z(0),w(0);
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            std::vector<int> movementsPerDirection(6,0);
            for(const auto & o : searchingOrder){
                movementsPerDirection[o] = countAndRemoveSubstring(line, directionStrings[o]);
            }
            parsedData.push_back(movementsPerDirection);

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

auto executeTiling(std::vector<std::vector<int> > instructions)
{
    Grid_T flippedTiles;
    for(const auto & i: instructions)
    {
        Coordinate coord(0);
        for(int o = 0; o < directions.size(); ++o)
        {
            auto dir = directions[o];
            coord += dir * i[o];
        }
        if(flippedTiles.find(coord) != flippedTiles.end())
        {
            // was aöready there -> remove
            flippedTiles.erase(coord);
        } else 
        {
            flippedTiles.insert(coord);
        }
    }
    return flippedTiles;
}

auto determineIterationSpace(Grid_T & flippedTiles)
{
    int xMin(0), xMax(0), yMin(0), yMax(0);
    for(const auto & t : flippedTiles)
    {
        xMin = std::min(xMin, t.x);
        xMax = std::max(xMax, t.x);
        yMin = std::min(yMin, t.y);
        yMax = std::max(yMax, t.y);
    }
    //std::cout << xMin-1 << " " << xMax+1 << " " << yMin-1 << " " << yMax+1 << std::endl;
    return std::make_tuple(xMin-1, xMax+1, yMin-1,yMax+1);
}

int countAdjacentFlippedTiles(Coordinate & coord, Grid_T & flippedTiles)
{
    int num = 0;
    for(int i = 0; i < directions.size(); ++i)
    {
        Coordinate adjacentCoord = coord + directions[i];
        if(flippedTiles.find(adjacentCoord) != flippedTiles.end()) ++num;
    }
    return num;
}

void updateTiling(Grid_T & flippedTiles)
{
    Grid_T newFlippedTiles = flippedTiles;
    auto iterationSpace = determineIterationSpace(flippedTiles);
    
    for(int y = std::get<2>(iterationSpace); y <= std::get<3>(iterationSpace);++y)
    {
        for(int x = std::get<0>(iterationSpace); x <= std::get<1>(iterationSpace);++x)
        {
            Coordinate coord(x,y);
            int numAdjacentFlippedTiles = countAdjacentFlippedTiles(coord, flippedTiles);
            if(flippedTiles.find(coord) != flippedTiles.end())
            {
                if(numAdjacentFlippedTiles == 0 || numAdjacentFlippedTiles > 2)
                {
                    newFlippedTiles.erase(coord);
                }
            } else 
            {
                if(numAdjacentFlippedTiles == 2)
                {
                    newFlippedTiles.insert(coord);
                }
            }
        }
    }
    std::swap(flippedTiles, newFlippedTiles);
}


int main()
{
    auto parsedData = readFromFile("input.txt");

    auto flippedTiles = executeTiling(parsedData);
    int result1 = flippedTiles.size(); 
    std::cout << "Result 1: " << result1 << std::endl;

    int numberOfUpdates = 100;
    for(int t = 1; t <= numberOfUpdates; ++t)
    {
        updateTiling(flippedTiles);
        std::cout << "Day " << t << ": " << flippedTiles.size() << std::endl;
    }

    int result2 = flippedTiles.size(); 
    std::cout << "Result 2: " << result2 << std::endl;
}