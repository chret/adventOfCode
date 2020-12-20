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


/* -> = x, v = y
 *   0
 * 3   1
 *   2
 */
class Tile
{
public: 
    Tile() : size_(0) {}
    Tile(int size) : size_(size){ content_ = std::vector<char>(size_ * size_,' ');}

    //void set(int x, int y, char c) {content_[x + y*size_] = c;}
    char operator()(int x, int y) const {return content_[x + y*size_];}
    char& operator()(int x, int y) {return content_[x + y*size_];}

    void rotate()
    {
        std::vector<char> newContent(size_*size_,' ');
        for(int j = 0; j < size_; ++j)
            for(int i = 0; i < size_; ++i)
                newContent[size_ - j - 1 + i * size_] = content_[i + j* size_];
        content_ = newContent;
    }

    void flipY()
    {
        std::vector<char> newContent(size_*size_,' ');
        for(int j = 0; j < size_; ++j)
            for(int i = 0; i < size_; ++i)
                newContent[i + (size_ - 1 - j) * size_] = content_[i + j* size_];
        content_ = newContent;
    }

    void flipX()
    {
        std::vector<char> newContent(size_*size_,' ');
        for(int j = 0; j < size_; ++j)
            for(int i = 0; i < size_; ++i)
                newContent[(size_ - 1 - i) + j * size_] = content_[i + j* size_];
        content_ = newContent;
    }

    auto getBorder(int dir) const
    {
        std::vector<char> border(size_);
        if(dir == 0)
        {
            for(int i = 0; i < size_; ++i) border[i] = content_[i + 0 * size_];
        } 
        else if(dir == 1)
        {
            for(int i = 0; i < size_; ++i) border[i] = content_[(size_ - 1)  + i * size_];
        }
        else if(dir == 2){
            for(int i = 0; i < size_; ++i) border[i] = content_[i  + (size_ - 1) * size_];
        }
        else if(dir == 3)
        {
            for(int i = 0; i < size_; ++i) border[i] = content_[0  + i * size_];
        }
        return border;
    }

    int size() const { return size_; }

    int numberOfChar(char c) const{
        return std::count_if(content_.begin(), content_.end(), [c](char s){return s == c;} );
    }

private:
    int size_;
    std::vector<char> content_;
};

std::ostream &operator<<(std::ostream &os, Tile const &m) { 
    for(int j = 0; j < m.size(); ++j)
    {
        for(int i = 0; i < m.size(); ++i)
        {
            os << m(i,j);
        }
        os << std::endl;
    }
    return os;
}

class Monster
{
public:
    Monster(std::string s)
    {
        int x = 0;
        int y = 0;
        for(const auto & c : s)
        {
            if(c == '#')
            {
                xOffset_.push_back(x);
                yOffset_.push_back(y);
            }
            x++; 
            if (c == '\n')
            {
                xSize_ = x - 1;
                x = 0;
                y++;
            }
        }
        ySize_ = y + 1;
    }

    int num() const {return xOffset_.size(); }
    int xSize() const {return xSize_; }
    int ySize() const {return ySize_; }
    std::pair<int,int> get(int i) const { return {xOffset_[i],yOffset_[i]};}

private:
    int xSize_ = 0;
    int ySize_ = 0;
    std::vector<int> xOffset_;
    std::vector<int> yOffset_;
};



auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        std::map<int,Tile> tiles;
        int x = 0;
        int y = 0;
        int tileID;
        int tileSize = 10;
        Tile curTile;
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
                curTile = Tile(tileSize);
                x = 0;
                y = 0;
            } else 
            {
                for(const auto & c : line)
                {
                    curTile(x++,y) = c;
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

bool checkAllRotations(const Tile & t1, Tile & t2, int dir)
{
    auto border1 = t1.getBorder(dir);
    int invDir = getInverseDirection(dir);
    for(int i = 0; i < 4; ++i)
    {
        auto border2 = t2.getBorder(invDir);
        if(checkBorderMatch(border1,border2)) return true;
        t2.rotate();
    }
    return false;
}

bool checkAllCombinations(const Tile & t1, Tile & t2, int dir)
{
    if(checkAllRotations(t1,t2,dir)) return true;

    t2.flipX();
    if(checkAllRotations(t1,t2,dir)) return true;

    return false;
}

void findAndinsertTopLeftCornerPiece(std::map<int,Tile> & tiles, std::vector<int> & tileArrangement, int tilesPerDirection)
{
    // find a corner tile
    std::vector<int> matchDirs;
    for(auto & t1 : tiles)
    {
        for(auto & t2 : tiles)
        {
            if(t1.first == t2.first) continue;
            for(int i = 0; i < 4; ++i)
            {
                if(checkAllCombinations(t1.second,t2.second,i))
                {
                    matchDirs.push_back(i);
                    break;
                }
            }        
        }
        if(matchDirs.size() == 2)
        {
            // rotate to have it as upper left corner piece
            int minMatchDir = std::min(matchDirs[0],matchDirs[1]);
            int maxMatchDir = std::max(matchDirs[0],matchDirs[1]);
            if(minMatchDir == 0)
            {
                if(maxMatchDir == 1)
                {
                    t1.second.rotate();
                }
                else{
                    t1.second.rotate();
                    t1.second.rotate();
                }
            } else if (minMatchDir == 1)
            {
                // already fine
            } else if (minMatchDir == 2)
            {
                t1.second.rotate();
                t1.second.rotate();
                t1.second.rotate();
            }

            tileArrangement[0 + 0 * tilesPerDirection] = t1.first;
            break;
        }
        matchDirs.clear();
    }
}


void fillTopRow(std::map<int,Tile> & tiles, std::vector<int> & tileArrangement, int tilesPerDirection)
{
    // fill top row from left to tight
    int searchDir = 1;
    for(int x = 0; x < tilesPerDirection-1; ++x)
    {
        int curTileID = tileArrangement[x + 0 * tilesPerDirection];
        auto t1 = tiles[curTileID];
        for(auto& t2 : tiles)
        {
            int nextTileID = t2.first;
            if(std::find(tileArrangement.begin(), tileArrangement.end(), nextTileID) != tileArrangement.end()) continue;
            if(checkAllCombinations(t1, t2.second, searchDir))
            {
                tileArrangement[x + 1 + 0 * tilesPerDirection] = nextTileID;
                break;
            }
        }
    }
}

void fillColumn(std::map<int,Tile> & tiles, std::vector<int> & tileArrangement, int tilesPerDirection, int column)
{
    // fill column from top to bottom
    int searchDir = 2;
    for(int y = 0; y < tilesPerDirection-1; ++y)
    {
        int curTileID = tileArrangement[column + y * tilesPerDirection];
        auto t1 = tiles[curTileID];
        for(auto& t2 : tiles)
        {
            int nextTileID = t2.first;
            if(std::find(tileArrangement.begin(), tileArrangement.end(), nextTileID) != tileArrangement.end()) continue;
            if(checkAllCombinations(t1, t2.second, searchDir))
            {
                tileArrangement[column + (y+1) * tilesPerDirection] = nextTileID;
                break;
            }
        }
    }
}

auto buildPuzzle(std::map<int,Tile> & tiles, int tilesPerDirection)
{
    std::vector<int> tileArrangement(tilesPerDirection*tilesPerDirection,-1);
    findAndinsertTopLeftCornerPiece(tiles, tileArrangement, tilesPerDirection);

    fillTopRow(tiles, tileArrangement, tilesPerDirection);

    for(int x = 0; x < tilesPerDirection; ++x)
    {
        fillColumn(tiles, tileArrangement, tilesPerDirection,x);
    }
    std::copy (tileArrangement.begin(), tileArrangement.end(), std::ostream_iterator<int>(std::cout, ", "));

    return tileArrangement;
}

auto multiplicationOfCornerIDs(std::vector<int> & tileArrangement, int tilesPerDirection)
{
    return static_cast<long long>(tileArrangement[0 + 0 * tilesPerDirection]) * static_cast<long long>(tileArrangement[tilesPerDirection-1 + 0 * tilesPerDirection ]) *  
           static_cast<long long>(tileArrangement[0 + (tilesPerDirection-1) * tilesPerDirection]) * static_cast<long long>(tileArrangement[(tilesPerDirection-1) + (tilesPerDirection-1)*tilesPerDirection]);
}

auto assembleTotalPuzzle(std::map<int,Tile> & tiles, std::vector<int> & tileArrangement, int tilesPerDirection, int patchSize)
{
    int puzzleSize = patchSize * tilesPerDirection;
    Tile puzzle(puzzleSize);
    for(int y = 0; y < tilesPerDirection; ++y)
    {
        for(int x = 0; x < tilesPerDirection; ++x)
        {
            int tileID = tileArrangement[x + y * tilesPerDirection];
            Tile & curTile = tiles[tileID];
            int beginIdx = x * patchSize + y * patchSize * tilesPerDirection;
            for(int j = 0; j < patchSize; ++j)
            {
                for(int i = 0; i < patchSize; ++i)
                {
                    puzzle(x * patchSize + i, y * patchSize + j) = curTile(i+1,j+1);
                }
            }
        }
    }
    return puzzle;
}

bool isMonsterAtPosition(Tile & p, Monster & m, int x, int y)
{
    int monsterNum= m.num();
    for(int i = 0; i < monsterNum; ++i)
    {
        auto offsets = m.get(i);
        if(p(x+offsets.first,y+offsets.second) != '#') return false;
    }
    std::cout << "Monster at: " << x << " " << y << std::endl;
    return true;
}

int getNumberOfSeaMonsters(Tile & p, Monster & m)
{
    int numberOfMonsters = 0;
    int monsterSizeX = m.xSize();
    int monsterSizeY = m.ySize();
    for(int y = 0; y < p.size() - monsterSizeY; ++y)
    {
        for(int x = 0; x < p.size() - monsterSizeX; ++x)
        {
            if(isMonsterAtPosition(p,m,x,y)) ++numberOfMonsters;
        }
    }
    return numberOfMonsters;
}

int findMonsters(Tile & p, Monster & m)
{
    int numberOfMonsters = 0;
    for(int i = 0; i < 4; ++i)
    {
        numberOfMonsters = getNumberOfSeaMonsters(p,m);
        if(numberOfMonsters > 0) return numberOfMonsters;
        p.rotate();
    }
    p.flipX();
    for(int i = 0; i < 4; ++i)
    {
        numberOfMonsters = getNumberOfSeaMonsters(p,m);
        if(numberOfMonsters > 0) return numberOfMonsters;
        p.rotate();
    }
    return 0;
}


int main()
{
    auto tiles = readFromFile("input.txt");
    std::cout << "Number of tiles: " << tiles.size() << std::endl;
    
    int tilesPerDirection = static_cast<int>(std::sqrt(tiles.size()));
    auto finishedPuzzleIDs = buildPuzzle(tiles, tilesPerDirection);
    long long result1 = multiplicationOfCornerIDs(finishedPuzzleIDs,tilesPerDirection); 
    std::cout << "Result 1: " << result1 << std::endl;

    int patchSize = 8;
    auto puzzle = assembleTotalPuzzle(tiles, finishedPuzzleIDs, tilesPerDirection, patchSize);
    //std::cout << puzzle.size() << std::endl;

    std::string monsterString("                  # \n#    ##    ##    ###\n #  #  #  #  #  #   ");
    Monster seaMonster(monsterString);
    int monsterSize = seaMonster.num();
    std::cout << seaMonster.xSize() << " " << seaMonster.ySize() << std::endl;

    int numberOfMonsters = findMonsters(puzzle, seaMonster);
    std::cout << puzzle << std::endl;
    std::cout << "Number of monsters: " << numberOfMonsters << std::endl;

    std::cout << "Result 2: " << puzzle.numberOfChar('#') - numberOfMonsters * monsterSize << std::endl;
}