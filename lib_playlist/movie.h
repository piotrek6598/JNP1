#ifndef MOVIE_H
#define MOVIE_H

#include <iostream>
#include "player_exception.h"
#include "media.h"
#include "checkcontent.h"

/*
 * Klasa Movie reprezentująca film.
 */
class Movie : public Media, CheckContent {
public:
    /* Konstruktor przyjmujący mapę metadanych `md` i treść filmu `c` (w ROT13).*/
    Movie(std::unordered_map<std::string, std::string> &md, std::string &c);

    /* Odtworzenie filmu. */
    void play() override;
private:
    /* Konwersja znaku na/z ROT13. */
    static inline char rot13(char c);
};

#endif // MOVIE_H