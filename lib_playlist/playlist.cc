#include <iostream>
#include "playable.h"
#include "player_mode.h"
#include "playlist.h"


void PlayList::add(const std::shared_ptr<Playable> &element) {
    if (element->contains(this)) {
        throw CycleDetectedException();
    }
    pieces.push_back(element);
}

void PlayList::add(const std::shared_ptr<Playable> &element, size_t position) {
    if (position > pieces.size()) {
        throw OutOfRangeException();
    }
    if (element->contains(this)) {
        throw CycleDetectedException();
    }
    pieces.insert(pieces.begin() + position, element);
}

void PlayList::remove() {
    pieces.pop_back();
}

void PlayList::remove(size_t position) {
    if (position >= pieces.size()) {
        throw OutOfRangeException();
    }
    pieces.erase(pieces.begin() + position);
}

void PlayList::setMode(const std::shared_ptr<PlayerMode> &mode) {
    this->mode = mode;
}

void PlayList::play() {
    std::cout << "Playlist [" << desc << "]" << std::endl;
    std::vector<size_t> order;
    mode->getOrder(pieces.size(), order);
    for (auto i : order) {
        pieces[i]->play();
    }
}

bool PlayList::contains(Playable *ptr) {
    if (this == ptr) {
        return true;
    }
    for (size_t i = 0; i < pieces.size(); i++) {
        if (pieces[i]->contains(ptr)) {
            return true;
        }
    }
    return false;
}
