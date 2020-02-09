#ifndef SHUFFLEMODE_H
#define SHUFFLEMODE_H

#include <vector>
#include <random>
#include "player_mode.h"

/*
 * Klasa ShuffleMode reprezentująca losowy tryb odtwarzania.
 */
class ShuffleMode : public PlayerMode {
private:
    std::default_random_engine random_engine;
public:
    ShuffleMode(size_t seed) : random_engine(seed) {};
    void getOrder(size_t size, std::vector<size_t> &vec) override;
};

#endif // SHUFFLEMODE_H
