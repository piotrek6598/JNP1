#include "song.h"

Song::Song(std::unordered_map <std::string, std::string> &md, std::string &c) : Media(md, c), CheckContent(",.!?':;-.") {
    checkContent(c);
    if (md.find(ARTIST) == md.end() || md.find(TITLE) == md.end()) {
        throw CorruptMetadata();
    }
}

void Song::play() {
    std::cout << "Song [" << metadata.at(ARTIST) << ", " << metadata.at(TITLE) << "]: " << content << '\n';
}