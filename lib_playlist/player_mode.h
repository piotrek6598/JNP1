#ifndef PLAYER_MODE_H
#define PLAYER_MODE_H

#include <cstddef>
#include <memory>
#include <vector>

/*
 * Klasa PlayerMode reprezentujący tryb odtwarzania playlisty.
 */
class PlayerMode {
public:
    virtual void getOrder (size_t size, std::vector<size_t> &vec) = 0;
};

/*
 * Zwraca sekwencyjny sposĂłb odtwarzania.
 */
std::shared_ptr<PlayerMode> createSequenceMode();

/*
 * Zwraca losowy sposób odtwarzania z utworzonym obiektem
 * std::default_random_engine z zadanym ziarnem.
 */
std::shared_ptr<PlayerMode> createOddEvenMode();

/*
 * Zwraca sposób odtwarzania nieparzyste/parzyste.
 */
std::shared_ptr<PlayerMode> createShuffleMode(size_t seed);

#endif // PLAYER_MODE_H
