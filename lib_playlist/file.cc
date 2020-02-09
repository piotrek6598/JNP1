#include <regex>
#include <vector>
#include <string>
#include "file.h"

// Kopiowanie `desc`, by móc delegować konstruktor.
File::File(std::string desc) {
    static std::regex pattern("[^\\|]+");
    std::string s_desc("xx");
    s_desc.insert(1, desc);
    std::vector <std::string> data;
    auto regex_iter = std::sregex_iterator(s_desc.begin(), s_desc.end(), pattern);
    auto regex_iter_end = std::sregex_iterator();
    for (auto it = regex_iter; it != regex_iter_end; ++it) {
        data.push_back(it->str());
    }
    if (data.size() < 2) {
        throw CorruptFile();
    }
    type = data[0].substr(1);
    for (size_t i = 1; i < data.size() - 1; i++) {
        size_t delimeter = data[i].find(':');
        if (delimeter == std::string::npos) {
            throw CorruptFile();
        }
        std::string name = data[i].substr(0, delimeter);
        std::string value = data[i].substr(delimeter + 1);
        metadata.insert_or_assign(name, value);
    }
    content = data.back().substr(0, data.back().length() - 1);
}