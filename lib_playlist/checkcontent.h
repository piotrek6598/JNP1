#ifndef CHECK_CONTENT_H
#define CHECK_CONTENT_H

#include <string>
#include "player_exception.h"


class CorruptContent : public PlayerException {
public:
    const char* what() const noexcept override {
        return "corrupt content";
    }
};

class CheckContent {
protected:
    explicit CheckContent(const char *specChar) : specialCharacters(specChar) {};
    void checkContent(std::string &content) const;
private:
    const char *specialCharacters;
};

#endif // CHECK_CONTENT_H