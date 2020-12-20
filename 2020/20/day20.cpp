#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <functional>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>

int SIZE = 10;

/* -> = x, v = y
 *   0
 * 3   1
 *   2
 */
class Tile
{
public: 
    Tile() { content_ = std::vector<char>(SIZE * SIZE,' ');}

    void set(int x, int y, char c) {content_[x + y*SIZE] = c;}
    char operator()(int x, int y) const {return content_[x + y*SIZE];}
    char& operator()(int x, int y) {return content_[x + y*SIZE];}

    void rotate()
    {
        std::vector<char> newContent(SIZE*SIZE,' ');
        for(int j = 0; j < SIZE; ++j)
            for(int i = 0; i < SIZE; ++i)
                newContent[SIZE - j - 1 + i * SIZE] = content_[i + j* SIZE];
        content_ = newContent;
    }

    void flipY()
    {
        std::vector<char> newContent(SIZE*SIZE,' ');
        for(int j = 0; j < SIZE; ++j)
            for(int i = 0; i < SIZE; ++i)
                newContent[i + (SIZE - 1 - j) * SIZE] = content_[i + j* SIZE];
        content_ = newContent;
    }

    void flipX()
    {
        std::vector<char> newContent(SIZE*SIZE,' ');
        for(int j = 0; j < SIZE; ++j)
            for(int i = 0; i < SIZE; ++i)
                newContent[(SIZE - 1 - i) + j * SIZE] = content_[i + j* SIZE];
        content_ = newContent;
    }

    auto getBorder(int dir) const
    {
        std::vector<char> border(SIZE);
        if(dir == 0)
        {
            for(int i = 0; i < SIZE; ++i) border[i] = content_[i + 0 * SIZE];
        } 
        else if(dir == 1)
        {
            for(int i = 0; i < SIZE; ++i) border[i] = content_[(SIZE - 1)  + i * SIZE];
        }
        else if(dir == 2){
            for(int i = 0; i < SIZE; ++i) border[i] = content_[i  + (SIZE - 1) * SIZE];
        }
        else if(dir == 3)
        {
            for(int i = 0; i < SIZE; ++i) border[i] = content_[0  + i * SIZE];
        }
        return border;
    }

private:
    std::vector<char> content_;
};

std::ostream &operator<<(std::ostream &os, Tile const &m) { 
    for(int j = 0; j < SIZE; ++j)
    {
        for(int i = 0; i < SIZE; ++i)
        {
            os << m(i,j);
        }
        os << std::endl;
    }
    return os;
}


auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        std::map<int,Tile> tiles;
        Tile curTile;
        int x = 0;
        int y = 0;
        int tileID;
        while(std::getline(inputFile,line))
        {
            //std::cout << line << std::endl;
            if(line.empty())
            {
                tiles[tileID] = curTile;
                continue;
            }else if(line.find("Tile") != std::string::npos)
            {
                tileID = std::stoi(line.substr(5,4));
                curTile = Tile();
                x = 0;
                y = 0;
            } else 
            {
                for(const auto & c : line)
                {
                    curTile.set(x++,y,c);
                }
                ++y;
                x = 0;
            }
        }

        tiles[tileID] = curTile;
        inputFile.close();
        return tiles;
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
}

void print(const std::vector<char>& vec)
{
    for(const auto & e : vec)
    {
        std::cout << e;
    }
    std::cout << std::endl;
}

bool checkBorderMatch(std::vector<char> & border1, std::vector<char> & border2)
{
    for(int i = 0; i < border1.size(); ++i)
    {
        if(border1[i] != border2[i]) return false;
    }
    return true;
}

int getInverseDirection(int dir)
{
    if(dir == 0) return 2;
    if(dir == 1) return 3;
    if(dir == 2) return 0;
    if(dir == 3) return 1;
    return -1;
}

bool checkAllRotations(const Tile & t1, Tile t2, int dir)
{
    //std::cout << t2 << std::endl;
    auto border1 = t1.getBorder(dir);
    int invDir = getInverseDirection(dir);
    //print(border1);
    std::cout << std::endl;
    for(int i = 0; i < 4; ++i)
    {
        auto border2 = t2.getBorder(invDir);
        //print(border2);
        if(checkBorderMatch(border1,border2)) return true;
        t2.rotate();
    }
    return false;
}

bool checkAllCombinations(const Tile & t1, Tile t2, int dir)
{
    if(checkAllRotations(t1,t2,dir)) return true;

    t2.flipX();
    if(checkAllRotations(t1,t2,dir)) return true;

    return false;
}

std::vector<int> findCornerTiles(std::map<int,Tile> & tiles)
{
    // = tiles with two non-matching directions
    std::vector<int> cornerTileIDs;
    int matches = 0;
    for(const auto & t1 : tiles)
    {
        matches = 0;
        for(const auto & t2 : tiles)
        {
            if(t1.first == t2.first) continue;
            for(int i = 0; i < 4; ++i)
            {
                if(checkAllCombinations(t1.second,t2.second,i))
                {
                    ++matches;
                    break;
                }
            }        
        }
        //std::cout << t1.first << " " << matches << std::endl;
        if(matches == 2) cornerTileIDs.push_back(t1.first);
        //else if(matches < 2) std::cout << "Error! Too few matches!" << std::endl;

    }
    return cornerTileIDs;

}

int main()
{
    
    auto tiles = readFromFile("input.txt");
    std::cout << "Number of tiles: " << tiles.size() << std::endl;
    int tilesPerDirection = static_cast<int>(std::sqrt(tiles.size()));
/*
    Tile t1 = tiles[2311];
    std::cout << t1 << std::endl << std::endl;
    print(t1.getBorder(0));
    print(t1.getBorder(1));
    print(t1.getBorder(2));
    print(t1.getBorder(3));
    std::cout << std::endl;
    Tile t2 = tiles[1951];
    std::cout << checkAllCombinations(t1, t2, 3)  << std::endl;
*/
    std::vector<int> tileArrangement(tilesPerDirection*tilesPerDirection,-1);

    auto cornerTileIDs = findCornerTiles(tiles);

    long long result1 = std::accumulate(cornerTileIDs.begin(), cornerTileIDs.end(), long long(1), std::multiplies<long long>());
    std::cout << "Result 1: " << result1 << std::endl;
}