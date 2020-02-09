#ifndef SONG_H
#define SONG_H

#include <iostream>
#include "player_exception.h"
#include "media.h"
#include "checkcontent.h"

/*
 * Klasa Song reprezentująca piosenkę.
 */
class Song : public Media, CheckContent {
public:
    /* Konstruktor piosenki. */
    Song(std::unordered_map <std::string, std::string> &md, std::string &c);

    /* Odtwarzanie piosenki. */
    void play() override;
};

#endif // SONG_H