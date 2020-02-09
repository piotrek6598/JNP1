#ifndef MEDIA_H
#define MEDIA_H

#include <unordered_map>
#include <memory>
#include "playable.h"
#include "player_exception.h"

/*
 * Stałe tekstowe.
 */
const std::string TITLE = "title";
const std::string YEAR = "year";
const std::string ARTIST = "artist";

/*
 * Wyjątek CorruptMetadata informujący o uszkodzonych metadanych w pliku.
 */
class CorruptMetadata : public PlayerException {
    const char* what() const noexcept override {
        return "corrupt metadata";
    }
};

/*
 * Klasa Media reprezentująca utwór dowolnego typu.
 */
class Media : public Playable {
public:
    /* Odtworzenie utworu. */
    void play() override = 0;
protected:
    /* Konstruktor. */
    Media(std::unordered_map<std::string, std::string> &md, std::string &c) : content(c), metadata(md) {};

    /* Treść. */
    std::string content;

    /* Słownik (mapa) metadanych. */
    std::unordered_map <std::string, std::string> metadata;
};

#endif // MEDIA_H