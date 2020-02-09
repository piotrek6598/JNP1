#include "oddevenmode.h"
#include <vector>

void OddEvenMode::getOrder(size_t size, std::vector<size_t> &vec) {
    for (size_t i = 1; i < size; i += 2)
        vec.push_back(i);
    for (size_t i = 0; i < size; i += 2)
        vec.push_back(i);
}
