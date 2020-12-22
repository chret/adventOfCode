#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <functional>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>

using Deck_T = std::deque<int>;
using History_T = std::unordered_set<size_t>;
using WinHistory_T = std::unordered_map<size_t,bool>;
std::hash<std::string> hasher;

//#define VERBOSE_MODE


auto readFromFile(std::string fileName, Deck_T & deck1, Deck_T & deck2)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        bool playerTwo = false;
        while(std::getline(inputFile,line))
        {
            if(line.empty()) continue;
            else if(line.find("Player 1:") != std::string::npos) continue;
            else if(line.find("Player 2:") != std::string::npos) {
                playerTwo = true;
                continue;
            } else {
                int number = std::stoi(line);
                if(playerTwo) deck2.push_back(number);
                else deck1.push_back(number);
            }
        }
        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
}

void print(const Deck_T& set)
{
    for(const auto & e : set)
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}

void playRound(Deck_T & deck1, Deck_T & deck2)
{
    int val1 = deck1[0];
    int val2 = deck2[0];
    if(val1<val2)
    {
        deck2.push_back(val2);
        deck2.push_back(val1);
    } else 
    {
        deck1.push_back(val1);
        deck1.push_back(val2);
    }
    deck1.pop_front();
    deck2.pop_front();
}

bool playGame(Deck_T & deck1, Deck_T & deck2)
{
    int t = 1;
    while(!deck1.empty() && !deck2.empty())
    {
        //std::cout << "Round " << t++ << std::endl;
        playRound(deck1, deck2);
        //print(deck1);
        //print(deck2);
    }

    // return if deck 2 won
    if(deck1.empty()) return true;
    else return false;
}

int getScore(Deck_T & deck)
{
    int score = 0;
    int num = deck.size();
    for(const auto & d : deck)
    {
        score += (num--) * d;
    }
    return score;
}

size_t getDeckHash(Deck_T & deck1, Deck_T & deck2)
{
    std::string str = "";
    for(const auto & d : deck1) str += std::to_string(d) + " ";
    str += "| ";
    for(const auto & d : deck2) str += std::to_string(d) + " ";
    return hasher(str);
}

bool checkAndUpdateHistory(Deck_T & deck1, Deck_T & deck2, History_T & history)
{
    auto curHash = getDeckHash(deck1,deck2);
    if(history.find(curHash) != history.end()) return true;
    else{
        history.insert(curHash);
        return false;
    }
}

Deck_T makeDeckCopy(Deck_T & deck, int n)
{
    Deck_T copy;
    std::copy_n(deck.begin(), n, std::back_inserter(copy));
    return copy;
}

// returns true if player 2 wins
bool playRecursiveGame(Deck_T & deck1, Deck_T & deck2, History_T & history, WinHistory_T & totalHistory)
{

    while(!deck1.empty() && !deck2.empty())
    {
        #ifdef VERBOSE_MODE
        print(deck1);
        print(deck2);
        #endif
        
        if(checkAndUpdateHistory(deck1, deck2, history)) return false;

        int val1 = deck1[0];
        int val2 = deck2[0];

        deck1.pop_front();
        deck2.pop_front();
        
        int size1 = deck1.size();
        int size2 = deck2.size();

        bool playerTwoWins = false;

        if(size1 >= val1 && size2 >= val2) 
        {
            auto copyDeck1 = makeDeckCopy(deck1, val1);
            auto copyDeck2 = makeDeckCopy(deck2, val2);
            auto gameHash = getDeckHash(copyDeck1, copyDeck2);
            if(totalHistory.find(gameHash) != totalHistory.end())
            {
                #ifdef VERBOSE_MODE   
                std::cout << "Playing subgame to determine winner.. but game has been played before!" << std::endl;
                #endif
                playerTwoWins = totalHistory[gameHash];
            }
            else
            {
                History_T newHistory;
                #ifdef VERBOSE_MODE   
                std::cout << "Playing subgame to determine winner.." << std::endl;
                #endif
                playerTwoWins = playRecursiveGame(copyDeck1, copyDeck2, newHistory, totalHistory);
                totalHistory[gameHash] = playerTwoWins;
            }
            
        }
        else {
            if(val1<val2)
            {
                playerTwoWins = true;
            } else 
            {
                playerTwoWins = false;
            }
        }

        if(playerTwoWins)
        {   
            #ifdef VERBOSE_MODE   
            std::cout << "Player 2 wins round!" << std::endl;
            #endif
            deck2.push_back(val2);
            deck2.push_back(val1);
        } else 
        {
            #ifdef VERBOSE_MODE   
            std::cout << "Player 1 wins round!" << std::endl;
            #endif
            deck1.push_back(val1);
            deck1.push_back(val2);
        }

    }

    if(deck1.empty())
    {
        #ifdef VERBOSE_MODE        
        std::cout << "Player 2 wins the game" << std::endl;
        #endif
        return true;
    } else {
        #ifdef VERBOSE_MODE         
        std::cout << "Player 1 wins the game" << std::endl;
        #endif
        return false;
    }

}

int main()
{
    Deck_T deck1;
    Deck_T deck2;
    readFromFile("input.txt", deck1, deck2);
    #ifdef VERBOSE_MODE
    print(deck1);
    print(deck2);
    #endif

    auto deck1R1 = deck1;
    auto deck2R1 = deck2;
    bool isDeck2WinnerR1 = playGame(deck1R1, deck2R1);
    int result1 = getScore((isDeck2WinnerR1) ? deck2R1 : deck1R1); 
    std::cout << "Result 1: " << result1 << std::endl;

    History_T history;
    WinHistory_T totalHistory;
    bool isDeck2WinnerR2 = playRecursiveGame(deck1, deck2, history, totalHistory);
    int result2 = getScore((isDeck2WinnerR2) ? deck2 : deck1); 
    std::cout << "Result 2: " << result2 << std::endl;
}