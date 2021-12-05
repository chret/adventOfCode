#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>

#include <fstream>
#include <sstream>

constexpr int cols = 5;
constexpr int rows = 5;

using Board_T = std::array<std::pair<int,bool>, cols*rows>;

void readFromFile(std::string fileName, std::vector<int>& numbers, std::vector<Board_T>& boards)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;

        std::getline(inputFile,line);
        // extract number list
        std::istringstream ss(line);
        std::string numStr;
        while(std::getline(ss,numStr,','))
        {
            numbers.push_back(std::stoi(numStr));
        }

        int cntY = 0;
        Board_T board;
        while(std::getline(inputFile,line))
        {
            if(line.empty()) continue;
            if(cntY == rows)
            {
                boards.push_back(board);
                cntY = 0;
            }
            
            std::istringstream ss(line);
            for(int cntX = 0; cntX < cols; ++cntX)
            {
                int num;
                ss >> num;
                board[cntY*cols+cntX] = std::make_pair(num, false);
            }
            ++cntY;
        }
        boards.push_back(board);
        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
    }
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

template<typename T>
void printBoard(const T& board)
{
    for(int y = 0; y < rows; ++y)
    {
        for(int x = 0; x < cols; ++x)
        {
            if(board[y*cols+x].second) std::cout << "X";
            std::cout << board[y*cols+x].first << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void playRound(Board_T& board, int number)
{
    auto res = std::find_if(board.begin(), board.end(), [number](std::pair<int,bool> e){return e.first == number;});
    if(res != std::end(board))
    {
        res->second=true;
    }
}

bool isBoardWinning(Board_T& board)
{
    // check rows
    for(int j = 0; j < rows; ++j)
    {
        bool winning = true;
        for(int i = 0; i < cols; ++i)
        {
            winning &= board[j*cols + i].second;
        }
        if(winning) return true;
    }

    for(int i = 0; i < cols; ++i)
    {
        bool winning = true;
        for(int j = 0; j < rows; ++j)
        {
            winning &= board[j*cols + i].second;
        }
        if(winning) return true;
    }
    return false;
}

int evaluateScore(Board_T& board, int number)
{
    int sumUnmarked = 0;
    for(const auto& e : board)
    {
        if(!e.second) sumUnmarked += e.first;
    }
    return sumUnmarked * number;
}

int playGame(std::vector<Board_T> boards, std::vector<int>& numbers, bool playToWin)
{
    std::cout << "Playing for " << numbers.size() << " rounds with " << boards.size() << " boards" << std::endl;
    std::vector<int> boardScore(boards.size(), -1);
    int numBoardsStillPlaying = boards.size();
    for(int round = 0; round < numbers.size(); ++round)
    {
        std::cout << "Round " << round << " with num boards " << numBoardsStillPlaying << std::endl;
        for(int b = 0; b < boards.size(); ++b)
        {
            if(boardScore[b] != -1) continue;
            playRound(boards[b], numbers[round]);
            if(isBoardWinning(boards[b]))
            {
                boardScore[b] = evaluateScore(boards[b], numbers[round]);
                std::cout << "Board " << b << " winning in round " << round << " with score " << boardScore[b] << std::endl;
                
                --numBoardsStillPlaying;
                if(playToWin) return boardScore[b];
                else if(numBoardsStillPlaying == 0)
                {
                    printContent(boardScore);
                    return boardScore[b];
                }
            } 
        }
    }
    return -1;
}


int main()
{
    std::vector<int> numbers; 
    std::vector<Board_T> boards;
    readFromFile("input.txt", numbers, boards);
    printContent(numbers);

    // part 1
    auto result1 = playGame(boards, numbers, true);
    std::cout << "Part 1 result: " << result1 << std::endl;

    // part 2
    auto result2 = playGame(boards, numbers, false);
    std::cout << "Part 2 result: " << result2 << std::endl;
}