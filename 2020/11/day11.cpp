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

int Nx;
int Ny;

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
    Coordinate(int x_, int y_) : x(x_), y(y_) {}

    Coordinate operator+(const Coordinate & b) const
    {
        return Coordinate(this->x + b.x, this->y + b.y);
    }

    Coordinate operator-(const Coordinate & b) const
    {
        return Coordinate(this->x - b.x, this->y - b.y);
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

using Grid_T = std::unordered_map<Coordinate, char, coordinate_hash>;
std::array<Coordinate,8> neighborDirs = {Coordinate(1,0), Coordinate(0,-1), Coordinate(-1,0), Coordinate(0,1),
                                         Coordinate(1,1), Coordinate(1,-1),Coordinate(-1,-1), Coordinate(-1,1)};


auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    Grid_T parsedData;
    int i(0),j(0);
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            i = 0;
            for(auto lineIt = line.begin(); lineIt != line.end(); ++lineIt)
            {
                parsedData[Coordinate(i,j)] = *lineIt;
                ++i;
            }
            ++j;
        }
        Nx = i;
        Ny = j;
        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
    return parsedData;
}

void printContent(const Grid_T& vec)
{
    std::cout << "=============\n";
    for(int y = 0; y < Ny; ++y)
    {
        for(int x = 0; x < Nx; ++x)
        {
            Coordinate pos(x,y);
            auto e = vec.find(pos);
            if(e != vec.end()) std::cout << e->second;
        }
        std::cout << std::endl;

    }
    std::cout << "=============\n";
}

bool isEmpty(char seatType)
{
    return seatType == 'L';
}

bool isOccupied(char seatType)
{
    return seatType == '#';
}

bool isFloor(char seatType)
{
    return seatType == '.';
}

int numSeatsInNeighborhood(const Grid_T& grid, Coordinate pos, char seatType)
{
    int num = 0;
    for(const auto& dir: neighborDirs)
    {
        auto newPos = pos + dir;
        auto gridElem = grid.find(newPos);
        if(gridElem != grid.end())
        {
            if(gridElem->second == seatType) ++num;
        }
    }
    return num;
}


auto getVisibleSeatTypes(const Grid_T& grid, Coordinate pos)
{
    std::vector<char> seats;
    for(const auto& dir: neighborDirs)
    {
        auto newPos = pos + dir;
        auto gridElem = grid.find(newPos);
        while(gridElem != grid.end())
        {
            if(!isFloor(gridElem->second))
            {
                seats.push_back(gridElem->second);
                break;
            }
            newPos += dir;
            gridElem = grid.find(newPos);
        }
    }
    return seats;
}


char getNewSeatTypeAtPositionRules1(const Grid_T& grid, Coordinate pos)
{
    auto gridElem = grid.find(pos);
    if(gridElem != grid.end())
    {
        char seatType = gridElem->second;
        if(isEmpty(seatType))
        {
            int numOccupiedSeatsInNeighborhood = numSeatsInNeighborhood(grid, pos, '#');
            if(numOccupiedSeatsInNeighborhood == 0) return '#';
        } else if(isOccupied(seatType))
        {
            int numOccupiedSeatsInNeighborhood = numSeatsInNeighborhood(grid, pos, '#');
            if(numOccupiedSeatsInNeighborhood >= 4) return 'L';
        }
        // no changes
        return seatType;
    }
    else
    {
        std::cout << "Error!" << std::endl;
    }
    
}

char getNewSeatTypeAtPositionRules2(const Grid_T& grid, Coordinate pos)
{
    auto gridElem = grid.find(pos);
    if(gridElem != grid.end())
    {
        char seatType = gridElem->second;
        if(isEmpty(seatType))
        {
            auto visibleSeats = getVisibleSeatTypes(grid,pos);
            int numOccupiedSeats = std::count_if(visibleSeats.begin(), visibleSeats.end(), [](char c){return isOccupied(c);});
            if(numOccupiedSeats == 0) return '#';
        } else if(isOccupied(seatType))
        {
            auto visibleSeats = getVisibleSeatTypes(grid,pos);
            int numOccupiedSeats = std::count_if(visibleSeats.begin(), visibleSeats.end(), [](char c){return isOccupied(c);});
            if(numOccupiedSeats >= 5) return 'L';
        }
        // no changes
        return seatType;
    }
    else
    {
        std::cout << "Error!" << std::endl;
    }
    
}

bool singleStep(Grid_T& srcGrid, std::function<char (const Grid_T& grid, Coordinate pos)> updateSeatFct)
{
    bool hasChanged = false;
    Grid_T dstGrid;
    for(auto & gridElem: srcGrid)
    {
        Coordinate pos = gridElem.first;
        char seatType = gridElem.second;
        char newSeatType = updateSeatFct(srcGrid, pos);
        if(seatType != newSeatType) hasChanged |= true;
        dstGrid[pos] = newSeatType;
    }
    std::swap(srcGrid, dstGrid);
    return hasChanged;
}

void simulateUntilSteadyState(Grid_T& grid, std::function<char (const Grid_T& grid, Coordinate pos)> updateSeatFct)
{
    int timestep = 0;
    while(singleStep(grid,updateSeatFct))
    {
        ++timestep;
    }
    std::cout << "Finished in " << timestep << " timesteps!" << std::endl;
}

int getNumberOfOccupiedSeats(Grid_T& grid)
{
    return std::count_if(grid.begin(), grid.end(), [](std::pair<const Coordinate,char>& entry){return isOccupied(entry.second);});
}


int main()
{
    auto parsedFileContent = readFromFile("input.txt");
    printContent(parsedFileContent);

    std::cout << "size = " <<  Nx  << " x " << Ny <<  std::endl;

    auto myGrid = parsedFileContent;
    simulateUntilSteadyState(myGrid, getNewSeatTypeAtPositionRules1);    
    auto result1 = getNumberOfOccupiedSeats(myGrid);
    std::cout << "Result 1: " << result1 << std::endl;

    myGrid = parsedFileContent;
    simulateUntilSteadyState(myGrid, getNewSeatTypeAtPositionRules2);    
    auto result2 = getNumberOfOccupiedSeats(myGrid);
    std::cout << "Result 2: " << result2 << std::endl;

}