#include <sstream>

#include "string_tools.h"

std::vector<std::string> split_whitespaces(std::string str)
{
    std::stringstream ss(str);
    std::string       buf;

    std::vector<std::string> tokens;

    while (ss >> buf)
        tokens.push_back(buf);

    return tokens;
}
