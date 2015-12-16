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

std::vector<std::string> split_whitespaces(const std::string &str)
{
    std::stringstream ss(str);
    std::string       buf;

    std::vector<std::string> tokens;

    while (ss >> buf)
        tokens.push_back(buf);

    return tokens;
}


std::string strip_filename(const std::string &full_path)
{
    size_t position = full_path.find_last_of("/\\");
    return full_path.substr(0, position);
}
