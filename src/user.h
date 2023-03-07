#pragma once

#include <Arduino.h>
#include "keyblock_csts.h"



// will be replaced by :
#define User_max_length 16

class User
{
private:
    bool allowed_keys[nKeyblocks];
    bool open_door = false;
    bool check_card();
    bool load_allowed_keys();
    uint8_t userindex = -1; // to be removed when database is implemented
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
