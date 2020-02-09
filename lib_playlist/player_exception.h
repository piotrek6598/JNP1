#ifndef PLAYER_EXCEPTION_H
#define PLAYER_EXCEPTION_H

#include <exception>

/*
 * Wyjątek PlayerException będący nadrzędnym nad wszystkimi wyjątaki z biblioteki `lib_playlist`.
 */
class PlayerException : public std::exception {
public:
    virtual const char* what() const noexcept {
        return "player exception";
    }
};

#endif // PLAYER_EXCEPTION_H