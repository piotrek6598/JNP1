#ifndef ODDEVENMODE_H
#define ODDEVENMODE_H

#include <vector>
#include "player_mode.h"

/*
 * Klasa OddEvenMode impementująca odwarzanie utworów z listy w kolejności nieparzystych i parzystych.
 */
class OddEvenMode : public PlayerMode {
public:
    OddEvenMode() = default;
    void getOrder(size_t size, std::vector<size_t> &vec) override;
};

#endif // ODDEVENMODE_H
