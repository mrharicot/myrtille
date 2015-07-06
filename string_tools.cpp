#include <sstream>

#include "string_tools.h"

std::vector<std::string> split(const std::string &s, const char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, delim)) {
        //if (!item.empty())
        tokens.push_back(item);
    }
    return tokens;
}

std::vector<std::string> split_whitespaces(std::string str)
{
    std::stringstream ss(str);
    std::string       buf;

    std::vector<std::string> tokens;

    while (ss >> buf)
        tokens.push_back(buf);

    return tokens;
}
