#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>

#include <fstream>
#include <sstream>


class Coordinate
{
public:
    Coordinate(std::string pair) {
        auto idxOfComma = pair.find(",");
        if(idxOfComma!=std::string::npos)
        {
            x = std::stoi(pair.substr(0,idxOfComma));
            y = std::stoi(pair.substr(idxOfComma+1,pair.length()-(idxOfComma+1)));
        } else 
        {
            std::cout << "Invalid coordinate string pair given " << pair << std::endl;
        }
    }
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

    bool operator==(const Coordinate & b)
    {
        return (this->x == b.x) && (this->y == b.y);
    }

    bool operator!=(const Coordinate & b){ return !(this->operator==(b));}

    int x, y;
};

std::ostream& operator<< (std::ostream &out, Coordinate const& co) {
    out << "(" << co.x << "," << co.y << ")";
    return out;
}


using CoordinatePair_T = std::pair<Coordinate, Coordinate>;

std::vector<CoordinatePair_T> readFromFile(std::string fileName)
{
    std::vector<CoordinatePair_T> lines;
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            auto idxOfFirstWs = line.find(" ");
            auto idxOfLastWs = line.rfind(" ");
            if(idxOfFirstWs != std::string::npos && idxOfLastWs != std::string::npos)
            {
                Coordinate c1(line.substr(0,idxOfFirstWs));
                Coordinate c2(line.substr(idxOfLastWs+1,line.length()-(idxOfLastWs+1)));
                lines.push_back(std::make_pair(c1,c2));
            } else 
            {
                std::cout << "Invalid line read " << line << std::endl;
            }
        }
        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
    }
    return lines;
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

void printContent(const std::vector<CoordinatePair_T>& vec)
{
    for (const auto& entry : vec)
    {
        std::cout << entry.first << " -> " << entry.second << ", ";
    }
    std::cout << std::endl;
}

CoordinatePair_T getWorldExtent(const std::vector<CoordinatePair_T>& lines)
{
    int xMin = std::numeric_limits<int>::max();
    int yMin = std::numeric_limits<int>::max();
    int xMax = std::numeric_limits<int>::min();
    int yMax = std::numeric_limits<int>::min();

    for(const auto& p: lines)
    {
        auto c1 = p.first;
        xMin = std::min(xMin,c1.x);
        yMin = std::min(yMin,c1.y);
        xMax = std::max(xMax,c1.x);
        yMax = std::max(yMax,c1.y);

        // :(
        auto c2 = p.second;
        xMin = std::min(xMin,c2.x);
        yMin = std::min(yMin,c2.y);
        xMax = std::max(xMax,c2.x);
        yMax = std::max(yMax,c2.y);
    }

    return std::make_pair(Coordinate(xMin,yMin), Coordinate(xMax,yMax));
}


template<typename T>
class World
{
public:
    World(const CoordinatePair_T& extent) : cMin_(extent.first), cMax_(extent.second)
    {
        std::cout << cMin_ << " " << cMax_ << std::endl;
        auto diff = cMax_ - cMin_;
        xLength_ = diff.x + 1;
        yLength_ = diff.y + 1;
        data_ = std::vector<T>(xLength_ * yLength_, T());
    }

    T& operator()(int x, int y) {return data_[(y-cMin_.y) * xLength_ + (x-cMin_.x)];}
    T operator()(int x, int y) const {return data_[(y-cMin_.y) * xLength_ + (x-cMin_.x)];}

    int getXMin() const {return cMin_.x;}
    int getYMin() const {return cMin_.y;}
    int getXMax() const {return cMax_.x;}
    int getYMax() const {return cMax_.y;}
    int getXLength() const {return xLength_;}
    int getYLength() const {return yLength_;}

    int numOfElem(T elem) const {return std::count_if(data_.begin(), data_.end(), [elem](T i){return i == elem;});}
    int numLargerThanElem(T elem) const {return std::count_if(data_.begin(), data_.end(), [elem](T i){return i > elem;});}
    T maxElem()const {return *std::max_element(data_.begin(), data_.end());}

private:
    Coordinate cMin_;
    Coordinate cMax_;
    int xLength_;
    int yLength_;
    std::vector<T> data_;
};

template<typename T>
std::ostream& operator<< (std::ostream &out, World<T> const& w) {

    for(int y = w.getYMin(); y <= w.getYMax(); ++y)
    {
        for(int x = w.getXMin(); x <= w.getXMax(); ++x)
        {
            auto val = w(x,y);
            if(val == T()) out << ".";
            else out << val;
        }
        out << std::endl;
    }
    return out;
}

void processSegments(World<int>& world, const std::vector<CoordinatePair_T>& segments, bool skipDiagonal)
{
    for(const auto& seg : segments)
    {
        auto diff = seg.first - seg.second;
        if(diff.x == 0) 
        {
            // vertical segment
            auto x = seg.first.x;
            auto y1 = seg.first.y;
            auto y2 = seg.second.y;
            if(y1>y2) std::swap(y1,y2);
            for(int y = y1; y <= y2; ++y) world(x,y)++;
        } else if(diff.y == 0)
        {
            // horizontal segment
            auto y = seg.first.y;
            auto x1 = seg.first.x;
            auto x2 = seg.second.x;
            if(x1>x2) std::swap(x1,x2);
            for(int x = x1; x <= x2; ++x) world(x,y)++;
        } else if (!skipDiagonal)
        {
            auto diffInc = Coordinate(-diff.x/std::abs(diff.x), -diff.y/std::abs(diff.y));
            auto coord = seg.first;
            while(coord != seg.second){
                world(coord.x,coord.y)++;
                coord += diffInc;
            }
            world(coord.x,coord.y)++;
        }
    }
}

int main()
{
    auto segments = readFromFile("input.txt");
    //printContent(segments);

    auto worldExtent = getWorldExtent(segments);
    World<int> world(worldExtent);
    //std::cout << world << std::endl;

    // part 1
    auto world1 = world;
    processSegments(world1, segments, true);
    auto result1 = world1.numLargerThanElem(1);
    std::cout << "Part 1 result: " << result1 << std::endl;

    // part 2
    processSegments(world, segments, false);
    auto result2 = world.numLargerThanElem(1);
    std::cout << "Part 2 result: " << result2 << std::endl;
    //std::cout << world << std::endl;
}