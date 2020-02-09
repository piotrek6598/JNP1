#ifndef FILE_H
#define FILE_H

#include <string>
#include <unordered_map>
#include "player_exception.h"

/*
 * Wyjątek CorruptFile informujący o nieprawidłowej treści argumentu do konstruktora File.
 */
class CorruptFile : public PlayerException {
public:
    const char* what() const noexcept override {
        return "corrupt file";
    }
};

/*
 * Klasa File reprezentująca pliki.
 */
class File {
public:
    /* Konstruktory File. */
    explicit File(std::string desc);
    explicit File(const char *desc) : File(std::string(desc)) {};
private:
    /* Typ pliku. */
    std::string type;

    /* Treść pliku. */
    std::string content;

    /* Słownik (mapa) metadanych pliku. */
    std::unordered_map<std::string, std::string> metadata;

    friend class Player;
};

#endif // FILE_H
