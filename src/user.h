#pragma once

#include <Arduino.h>
#include "keyblocks_all.h"



// will be replaced by :
#define User_max_length 16

class User
{
private:
    bool allowed_keys[nKeyblocks];
    bool open_door = false;
    bool check_card();
    bool load_allowed_keys();
public:
    uint8_t uid[User_max_length];
    uint8_t uidLength;
    bool logged_in = false;
    User();
    ~User();
    void logIn(uint8_t *uid, uint8_t *uidLength);
    void logOut();
    bool canOpenDoor();
    bool canReleaseKey(uint8_t n_keyblock);
};

User user;