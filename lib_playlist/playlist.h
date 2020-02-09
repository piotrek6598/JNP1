#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>
#include "media.h"
#include "playable.h"
#include "player_mode.h"
#include "player_exception.h"

/*
 * Wyjątek OutOfRangeException sygnalizuje wyjście poza zakres.
 */
class OutOfRangeException : public PlayerException {
public:
    const char* what() const noexcept override {
        return "index out of range";
    }
};

/*
 * Wyjątek CycleDetectedException sygnalizuje wystąpienie cyklu w liście odtwarzania.
 */
class CycleDetectedException : public PlayerException {
public:
    const char* what() const noexcept override {
        return "playlist cycle detected";
    }
};

/*
 * Klasa PlayList reprezentująca listę odtwarzania.
 */
class PlayList : public Playable {
private:
    std::string desc;
    std::shared_ptr<PlayerMode> mode;
    std::vector<std::shared_ptr<Playable>> pieces;
public:
    /* Konstruktory listy odtwarzania. */
    explicit PlayList(std::string &desc) : desc(desc), mode(createSequenceMode()), pieces() {};
    explicit PlayList(const char *desc) : desc(desc), mode(createSequenceMode()), pieces() {};

    /* Uruchomienie listy odtwarzania. */
    void play() override;

    /* Dodanie utworu na końcu listy odtwarzania. */
    void add(const std::shared_ptr<Playable> &element);

    /* Dodanie elementu na określonej pozycji w liście odtwarzania
     * (pozycje są numerowane od 0). */
    void add(const std::shared_ptr<Playable> &element, size_t position);

    /* Usunięcie ostatniego elementu z listy odtwarzania. */
    void remove();

    /* Usunięcie element z okreĹlonej pozycji listy odtwarzania. */
    void remove(size_t posiotion);

    /* Ustawia sposób (kolejność) odtwarzania utworów, sposób odtwarzania może
     * zostać utworzony za pomocą obiektu typu std::shared_ptr<PlayerMode>. */
    void setMode(const std::shared_ptr<PlayerMode> &mode);

    /* Sprawdzenie czy lista odtwarzania zawiera dany obiekt */
    bool contains(Playable *ptr) override;

};

#endif // PLAYLIST_H
