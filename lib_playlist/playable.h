#ifndef PLAYABLE_H
#define PLAYABLE_H

/*
 * Klasa Playable reprezentujące obiekty mogące zostać odtworzone przez odtwarzacz.
 */
class Playable {
public:
    /* Odtworzenie obiektu przez odtwarzacz. */
    virtual void play() = 0;

    /* Sprawdzenie czy obiekt zawiera inny obiekt. */
    virtual bool contains(Playable *ptr) {
        return this == ptr;
    }
};

#endif // PLAYABLE_H
