#include <iostream>
#include <vector>
#include <string>

#include <fstream>
#include <sstream>
#include <algorithm>

struct LineEntry
{
    LineEntry(std::string line)
    {
        line.replace(line.find("-"), 1, " ");
        line.replace(line.find(":"), 1, " ");
        std::stringstream ss;
        ss << line;
        ss >> num1_ >> num2_ >> letter_ >> pwd_;

    }

    int num1_;
    int num2_;
    std::string letter_;
    std::string pwd_;
};

bool checkPwd(LineEntry& entry)
{
    int numberOccurances = std::count(entry.pwd_.begin(), entry.pwd_.end(), entry.letter_[0]);
    if(numberOccurances >= entry.num1_ && numberOccurances <= entry.num2_) return true;
    else return false;
}

bool checkPwdPart2(LineEntry& entry)
{
    if(entry.pwd_[entry.num1_-1] == entry.letter_[0] && entry.pwd_[entry.num2_-1] != entry.letter_[0]) return true;
    if(entry.pwd_[entry.num1_-1] != entry.letter_[0] && entry.pwd_[entry.num2_-1] == entry.letter_[0]) return true;
    return false;
}

std::ostream& operator<< (std::ostream &out, LineEntry const& data) {
    out << data.num1_ << '-' << data.num2_ << " " << data.letter_ << ": " << data.pwd_;
    return out;
}


std::vector<LineEntry> readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<LineEntry> content;
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            content.push_back(LineEntry(line));
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

int main()
{
    auto fileContent = readFromFile("input.txt");
    int numberOfValidPasswords = 0;
    for(auto& entry: fileContent )
    {
        if(checkPwd(entry)) ++numberOfValidPasswords;
    }
    std::cout << "Result 1: " << numberOfValidPasswords << std::endl;

    int numberOfValidPasswordsPart2 = 0;
    for(auto& entry: fileContent )
    {
        if(checkPwdPart2(entry)) ++numberOfValidPasswordsPart2;
    }
    std::cout << "Result 2: " << numberOfValidPasswordsPart2 << std::endl;
}