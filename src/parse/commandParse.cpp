#include "../../inc/libs.hpp"

list<string> commandParse(string str)
{
    list<string> result;
    if (str.find(','))
    {
        std::stringstream ss(str);
        string item;
        while (std::getline(ss, item, ','))
            result.push_back(item);
    }
    return result;
}
