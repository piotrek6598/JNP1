#include <memory>
#include "player.h"
#include "song.h"
#include "movie.h"
#include "playlist.h"
#include "playable.h"

std::shared_ptr<Playable> Player::openFile(File file) const {
    if (file.type == "audio") {
        return std::make_shared<Song>(file.metadata, file.content);
    } else if (file.type == "video") {
        return std::make_shared<Movie>(file.metadata, file.content);
    } else {
        throw UnsupportedType();
    }
}

std::shared_ptr<PlayList> Player::createPlaylist(const char *desc) const {
    return std::make_shared<PlayList>(desc);
}
