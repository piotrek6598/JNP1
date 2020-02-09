#include "movie.h"

Movie::Movie(std::unordered_map<std::string, std::string> &md, std::string &c) : Media(md, c),
                                                                                 CheckContent(",.!?':;-.") {
    checkContent(c);
    if (metadata.find(TITLE) == metadata.end() || metadata.find(YEAR) == metadata.end()) {
        throw CorruptMetadata();
    }
    for (char &ch: content) {
        ch = rot13(ch);
    }
}


void Movie::play() {
    std::cout << "Movie [" << metadata.at(TITLE) << ", " << metadata.at(YEAR) << "]: " << content << '\n';
}


inline char Movie::rot13(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) {
        return c + 13;
    } else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) {
        return c - 13;
    }
    return c;
}