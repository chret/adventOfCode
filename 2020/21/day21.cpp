#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <functional>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <array>


auto readFromFile(std::string fileName, std::vector<std::set<std::string>> & ingredients, 
                  std::vector<std::set<std::string>> & allergenes)
{
    std::ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile,line))
        {
            auto bracketPos = line.find("(");
            auto curIngredients = line.substr(0,bracketPos);
            std::stringstream ss(curIngredients);
            std::string ingredient;
            std::set<std::string> ingredientSet;
            while(getline(ss,ingredient,' '))
            {
                ingredientSet.insert(ingredient);
            }
            ingredients.push_back(ingredientSet);

            auto endingBracketPos = line.find(")");
            auto beginPos = bracketPos+10;
            auto curAllergenes = line.substr(beginPos,endingBracketPos-beginPos);
            auto removeIt = std::remove(curAllergenes.begin(), curAllergenes.end(),',');
            curAllergenes.erase(removeIt, curAllergenes.end());
            std::stringstream ssA(curAllergenes);
            std::string allergene;
            std::set<std::string> allergeneSet;
            while(getline(ssA,allergene,' '))
            {
                allergeneSet.insert(allergene);
            }
            allergenes.push_back(allergeneSet);
           
        }

        inputFile.close();
    }
    else
    {
        std::cout << "Could not open file "  <<  fileName << std::endl;
        std::abort();
    }
}

template< typename T>
void print(const std::set<T>& set)
{
    for(const auto & e : set)
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}

template< typename T>
void print(const std::map<std::string, std::set<T>>& map)
{
    for(const auto & e : map)
    {
        std::cout << e.first << ": ";
        print(e.second);
    }
    std::cout << std::endl;
}

auto assembleMappingAToB( std::vector<std::set<std::string>> & a, std::vector<std::set<std::string>> & b)
{
    std::map<std::string, std::set<std::string> > aToBMap;
    for(int f = 0; f < a.size(); ++f)
    {
        for(const auto & i : a[f])
        {
            if(aToBMap.find(i) != aToBMap.end())
            {
                aToBMap[i].insert(b[f].begin(), b[f].end());
            } else {
                aToBMap[i] = b[f];
            }
        }
    }
    return aToBMap;
}

std::set<std::string> getAllIngredients(std::vector<std::set<std::string>> & ingredients)
{
    std::set<std::string> result;
    for(const auto & i: ingredients)
    {
        result.insert(i.begin(), i.end());
    }
    return result;
}

std::set<std::string> intersectSets(const std::set<std::string>& s1, const std::set<std::string>& s2)
{
    std::set<std::string> result;
    std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(result,result.end()));
    return result;
}



auto assembleAllergeneMap(std::vector<std::set<std::string>> & ingredients, std::vector<std::set<std::string>> & allergenes)
{
    std::map<std::string,std::set<std::string>> allergeneMap;
    for(int i = 0; i < ingredients.size()-1; ++i)
    {
        for(int j = i+1; j < ingredients.size(); ++j)
        {
            auto allergeneIntersect = intersectSets(allergenes[i], allergenes[j]);
            if(allergeneIntersect.size() > 0)
            {
                auto ingredientIntersect = intersectSets(ingredients[i], ingredients[j]);
                for(const auto & al : allergeneIntersect)
                {
                    if(allergeneMap.find(al) != allergeneMap.end())
                    {
                        allergeneMap[al] = intersectSets(allergeneMap[al], ingredientIntersect);

                    }else
                    {
                        allergeneMap[al] = ingredientIntersect;
                    }
                    
                }

            }
        }
        for(const auto & al: allergenes[i])
        {
            if(allergeneMap.find(al) == allergeneMap.end())
            {
                // no intersect found before, insert all
                allergeneMap[al] = ingredients[i];
            }
        }
    }
    return allergeneMap;

}

bool isContainedInMap(std::map<std::string,std::set<std::string>> & allergeneMap, std::string ingredient)
{
    for(const auto & alPair : allergeneMap)
    {
        if(alPair.second.find(ingredient) != alPair.second.end()) return true;
    }
    return false;
}

std::set<std::string> findMissingIngredients(std::map<std::string,std::set<std::string>> & allergeneMap, std::set<std::string> & allIngredients)
{
    std::set<std::string> missingIngredients;
    for(const auto & i : allIngredients)
    {
        if(!isContainedInMap(allergeneMap,i)) missingIngredients.insert(i);
    }
    return missingIngredients;
}

int countIngredientsOccuranceInFoods(std::vector<std::set<std::string>> & foods, std::set<std::string> & ingredients)
{
    int num = 0;
    for(const auto & i: ingredients)
    {
        for(const auto & f: foods)
        {
            if(f.find(i) != f.end()) ++num;
        }
    }
    return num;
}

bool removeIngredientFromAllergeneMap(std::map<std::string,std::set<std::string>> & allergeneMap, std::string ingredient)
{
    bool hasChanged = false;
    for(auto & al : allergeneMap)
    {
        auto & ingredientSet = al.second;
        auto it = ingredientSet.find(ingredient);
        if(it != ingredientSet.end())
        {
            if(ingredientSet.size() > 1)
            {
                ingredientSet.erase(it);
                hasChanged = true;
            }
        }
    }
    return hasChanged;
}

void pruneAllergeneMap(std::map<std::string,std::set<std::string>> & allergeneMap)
{
    bool hasChanged = true;
    while(hasChanged)
    {
        hasChanged = false;
        for(const auto & al : allergeneMap)
        {
            if(al.second.size() == 1)
            {
                hasChanged |= removeIngredientFromAllergeneMap(allergeneMap,*al.second.begin());
            }
        }  
    }
}

std::string buildResult2String(std::map<std::string,std::set<std::string>> & allergeneMap)
{
    std::string result = "";
    for(const auto & alPair : allergeneMap)
    {
        result += *alPair.second.begin() + ",";
    }
    return result.substr(0,result.size()-1);
}

int main()
{
    std::vector<std::set<std::string>> ingredients;
    std::vector<std::set<std::string>> allergenes;
    readFromFile("input.txt", ingredients, allergenes);

    auto allIngredients = getAllIngredients(ingredients);
    //print(allIngredients);
    auto allergeneMap = assembleAllergeneMap(ingredients, allergenes);
    auto ingredientsWithoutAllergenes = findMissingIngredients(allergeneMap,allIngredients);
    //print(ingredientsWithoutAllergenes);
    int numOccurancesMissingIngredients = countIngredientsOccuranceInFoods(ingredients, ingredientsWithoutAllergenes);

    int result1 = numOccurancesMissingIngredients; 
    std::cout << "Result 1: " << result1 << std::endl;

    pruneAllergeneMap(allergeneMap);
    auto result2 = buildResult2String(allergeneMap);
    std::cout << "Result 2: " << result2 << std::endl;
}