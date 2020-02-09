#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include "player_exception.h"
#include "playable.h"
#include "playlist.h"
#include "file.h"
#include "media.h"

/*
 * Wyjątek UnsupportedType informujący o niewspieranym typie pliku.
 */
class UnsupportedType : public PlayerException {
    const char* what() const noexcept override {
        return "unsupported type";
    }
};

/*
 * Klasa reprezentująca odtwarzacz.
 */
class Player {
public:
    /* Domyślny konstruktor dla klasy Player. */
    Player() = default;

    /* Tworzy i zwraca utwór na podstawie pliku, utwór może być odtworzony
     * samodzielnie albo dodany do listy odtwarzania. */
    std::shared_ptr<Playable> openFile(File f) const;

    /* Tworzy i zwraca listę odtwarzania o podanej nazwie wraz z ustawioną domyślną (sekwencyjną) kolejnością
     * odtwarzania, lista może zostać odtworzona samodzielnie albo dodana do innej listy odtwarzania. */
    std::shared_ptr<PlayList> createPlaylist(const char *desc) const;
};

#endif // PLAYER_H
