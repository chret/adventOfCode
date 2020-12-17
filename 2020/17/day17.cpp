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

// https://stackoverflow.com/questions/9729390/how-to-use-unordered-set-with-custom-types/9729747#9729747
template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

class Coordinate
{
public:
    explicit Coordinate(int a_) : x(a_), y(a_), z(a_), w(a_) {}
    Coordinate(int x_, int y_, int z_, int w_) : x(x_), y(y_), z(z_), w(w_) {}

    Coordinate operator+(const Coordinate & b) const
    {
        return Coordinate(this->x + b.x, this->y + b.y, this->z + b.z, this->w + b.w);
    }

    void operator+=(const Coordinate & b)
    {
        this->x += b.x;
        this->y += b.y;
        this->z += b.z;
        this->w += b.w;
    }

    bool operator==(const Coordinate & b) const
    {
        return (this->x == b.x) && (this->y == b.y) && (this->z == b.z) && (this->w == b.w);
    }

    int x, y, z, w;
};

std::ostream& operator<< (std::ostream &out, Coordinate const& co) {
    out << "(" << co.x << "," << co.y << "," << co.z << "," << co.w << ")";
    return out;
}

struct coordinate_hash
{
    inline std::size_t operator()(const Coordinate & v) const
    {
         std::size_t seed = 0;
         hash_combine(seed, v.x);
         hash_combine(seed, v.y);
         hash_combine(seed, v.z);
         hash_combine(seed, v.w);
         return seed;
    }
};

using Grid_T = std::unordered_set<Coordinate, coordinate_hash>;
std::vector<Coordinate> neighborDirs;
Coordinate coordMin(0), coordMax(0);

auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    Grid_T parsedData;
    int x(0),y(0),z(0),w(0);
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            x = 0;
            for(auto lineIt = line.begin(); lineIt != line.end(); ++lineIt)
            {
                if(*lineIt == '#') parsedData.insert(Coordinate(x,y,z,w));
                ++x;
            }
            ++y;
        }
        inputFile.close();
        coordMax.x = x-1;
        coordMax.y = y-1;
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
    return parsedData;
}

int getNumActiveCoordinatesInNeighborhood(const Grid_T& grid, Coordinate pos)
{
    int num = 0;
    for(const auto& dir: neighborDirs)
    {
        auto newPos = pos + dir;
        auto gridElem = grid.find(newPos);
        if(gridElem != grid.end())
        {
            ++num;
        }
    }
    return num;
}

bool updateRule(const Grid_T& grid, Coordinate pos)
{
    auto gridElem = grid.find(pos);

    int numActiveStatesinNeighborhood = getNumActiveCoordinatesInNeighborhood(grid, pos);
    if(gridElem != grid.end())
    {
        // is active
        if(numActiveStatesinNeighborhood == 2 || numActiveStatesinNeighborhood == 3) return true;
        else return false;
    }
    else
    {
        // is inactive
        if(numActiveStatesinNeighborhood == 3) return true;
        else return false;
    }
}

void updateStep(Grid_T& srcGrid, Grid_T& dstGrid, Coordinate pos,
                std::function<char (const Grid_T& grid, Coordinate pos)> updateFct)
{
    bool coordinateState = updateFct(srcGrid, pos);
    if(coordinateState) dstGrid.insert(pos);
}

void singleStep(Grid_T& srcGrid, std::function<char (const Grid_T& grid, Coordinate pos)> updateFct, bool useFourthDimension)
{
    bool hasChanged = false;
    Grid_T dstGrid;
    for(int z = coordMin.z -1; z <= coordMax.z + 1; ++z)
    {
        for(int y = coordMin.y -1; y <= coordMax.y + 1; ++y)
        {
            for(int x = coordMin.x -1; x <= coordMax.x + 1; ++x)
            {
                if(useFourthDimension)
                {
                    for(int w = coordMin.w -1; w <= coordMax.w + 1; ++w)
                    {                                    
                        updateStep(srcGrid, dstGrid, Coordinate(x,y,z,w), updateFct);
                    }
                } else {
                    updateStep(srcGrid, dstGrid, Coordinate(x,y,z,0), updateFct);
                }
            }
        }
    }

    coordMin += Coordinate(-1);
    coordMax += Coordinate(1);
    std::swap(srcGrid, dstGrid);
}

int getNumberOfActiveCoordinates(Grid_T& grid)
{
    return grid.size();
}

void createNeighborDirections(bool useFourthDimension)
{
    neighborDirs.clear();
    for(int z = -1; z <= 1; ++z)
    {
        for(int y = -1; y <= 1; ++y)
        {
            for(int x = -1; x <= 1; ++x)
            {
                if(useFourthDimension)
                {
                    for(int w = -1; w <= 1; ++w)
                    {
                        if(!(x==0 && y==0 && z==0 && w==0)) neighborDirs.push_back(Coordinate(x,y,z,w));
                    }
                } else {
                    if(!(x==0 && y==0 && z==0)) neighborDirs.push_back(Coordinate(x,y,z,0));
                }
            }
        }
    }
}

int main()
{
    auto parsedFileContent = readFromFile("input.txt");

    int numSteps = 6;

    createNeighborDirections(false);
    auto myGrid = parsedFileContent;
    for(int i = 0; i < numSteps; ++i)
    {
        singleStep(myGrid, updateRule, false);
    }  
    auto result1 = getNumberOfActiveCoordinates(myGrid);
    std::cout << "Result 1: " << result1 << std::endl;

    createNeighborDirections(true);
    auto myGrid2 = parsedFileContent;
    for(int i = 0; i < numSteps; ++i)
    {
        singleStep(myGrid2, updateRule, true);
    }  
    auto result2 = getNumberOfActiveCoordinates(myGrid2);
    std::cout << "Result 2: " << result2 << std::endl;

}