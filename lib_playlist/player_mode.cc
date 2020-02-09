#include "player_mode.h"
#include "sequencemode.h"
#include "oddevenmode.h"
#include "shufflemode.h"

using std::shared_ptr;
using std::make_shared;

shared_ptr<PlayerMode> createSequenceMode() {
    return make_shared<SequenceMode>();
}

shared_ptr<PlayerMode> createOddEvenMode() {
    return make_shared<OddEvenMode>();
}

shared_ptr<PlayerMode> createShuffleMode(size_t seed) {
    return make_shared<ShuffleMode>(seed);
}
