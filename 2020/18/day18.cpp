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

using INT = long long;

std::vector<std::string> tokenizeBracketExpr(std::string expr)
{
    std::vector<std::string> elements;
    while(expr.find("(") != std::string::npos)
    {
        elements.push_back("(");
        expr = expr.substr(1);
    }
    auto findEndBracket = expr.find(")");
    if(findEndBracket != std::string::npos)
    {
        elements.push_back(expr.substr(0,findEndBracket));
        expr = expr.substr(findEndBracket);
    } else 
    {
        elements.push_back(expr);
        expr = "";
    }
    while(expr.find(")") != std::string::npos)
    {
        elements.push_back(")");
        expr = expr.substr(1);
    }
    return elements;
}

auto readFromFile(std::string fileName)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    std::vector<std::vector<std::string>> parsedData;

    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            std::vector<std::string> expression;
            std::stringstream ss(line);
            std::string token;
            while(std::getline(ss,token,' '))
            {
                if(token.find_first_of("()") != std::string::npos)
                {
                    auto tokenizedExpr = tokenizeBracketExpr(token);
                    for(int i = 0; i < tokenizedExpr.size(); ++i)
                    {
                        expression.push_back(tokenizedExpr[i]);
                    }
                } else 
                {
                    expression.push_back(token);
                }
            }
            parsedData.push_back(expression);
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

template<typename T>
void printContent(const std::vector<T>& vec)
{
    std::cout << "=============\n";
    for(const auto & e : vec)
    {
        for(const auto & i : e) std::cout << i << " ";
        std::cout << std::endl;
    }
    std::cout << "=============\n";
}

bool is_digit(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

INT doOperation(INT a, INT b, std::string op)
{
    if(op.empty()) return b;
    if(op == "*") return a * b;
    if(op == "+") return a + b;
}

int findMatchingEndBracket(std::vector<std::string> & expr, int start)
{
    int numOpeningBrackets = 1;
    for(int i = start + 1; i < expr.size(); ++i)
    {
        if(expr[i] == ")") --numOpeningBrackets;
        else if(expr[i] == "(") ++numOpeningBrackets;

        if(numOpeningBrackets == 0) return i;
    }
    return -1;
}

INT evaluateExpression(std::vector<std::string> & expr, int start, int end)
{
    INT result = 0;
    std::string op = "";
    for(int i = start; i < end; ++i)
    {
        auto e = expr[i];
        if(is_digit(e)) result = doOperation(result, std::stoll(e), op);
        else if(e == "*" || e == "+") op = e;
        else if(e == "(")
        {
            auto j = findMatchingEndBracket(expr, i);
            result = doOperation(result, evaluateExpression(expr, i+1, j), op);
            i = j;
        }
    }
    return result;

}


int main()
{
    auto parsedFileContent = readFromFile("input.txt");
    printContent(parsedFileContent);

    std::vector<INT> result(parsedFileContent.size(),0);
    for(int i = 0; i < result.size(); ++i)
    {
        auto expr = parsedFileContent[i];
        result[i] = evaluateExpression(expr,0,expr.size());
        //std::cout << result[i] << std::endl;
    }

    auto result1 = std::accumulate(result.begin(), result.end(),INT(0));
    std::cout << "Result 1: " << result1 << std::endl;



}