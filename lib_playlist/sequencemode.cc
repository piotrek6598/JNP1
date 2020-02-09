#include "sequencemode.h"
#include <vector>

void SequenceMode::getOrder(size_t size, std::vector<size_t> &vec) {
    for (size_t i = 0; i < size; i++) {
        vec.push_back(i);
    }
}
