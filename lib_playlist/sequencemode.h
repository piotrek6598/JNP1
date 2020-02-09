#ifndef SEQUENCEMODE_H
#define SEQUENCEMODE_H

#include "player_mode.h"

/*
 * Klasa SequenceMode reprezentująca sekwencyjny tryb odtwarzania (domyślny).
 */
class SequenceMode : public PlayerMode {
public:
    SequenceMode() = default;
    void getOrder(size_t size, std::vector<size_t> &vec) override;
};

#endif // SEQUENCEMODE_H
