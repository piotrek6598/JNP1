#include <algorithm>
#include <vector>
#include "shufflemode.h"

void ShuffleMode::getOrder(size_t size, std::vector<size_t> &vec) {
    for (size_t i = 0; i < size; i++)
        vec.push_back(i);
    std::shuffle(vec.begin(), vec.end(), random_engine);
}
