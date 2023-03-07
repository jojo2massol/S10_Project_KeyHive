#include "user.h"

// test table of allowed cards
const uint8_t allowed_cards[][User_max_length] = {
    {0x67, 0x38, 0x1A, 0x2D, 0x00, 0x00, 0x00}, // 4 bytes // NFC tag
    {0x04, 0x30, 0x8A, 0x3A, 0xFF, 0x42, 0x80}, // 7 bytes //Georges de Massol
    {0x04, 0x1D, 0x85, 0x52, 0xC4, 0x52, 0x80}, // 7 bytes // Etienne Sommier
};
const uint8_t allowed_cards_length[] = {
    4,
    7,
    7,
};

// fake test TO BE REMOVED
const bool allowed_keys_fake[][nKeyblocks] = {
    // NFC tag
    {false, false},
    // Georges de Massol
    {true, false},
    // Etienne Sommier
    {false, true},
};

// function that takes a pointer to a UID from an NFC card and UID length,
// and returns true if the card is allowed to open the door, false otherwise
// this function should be removed and replaced by a function that checks the user's rights
bool User::check_card()
{
    // check if the card is in the allowed cards table
    for (uint8_t i = 0; i < sizeof(allowed_cards_length); i++)
    {
        // check if the UID length is the same
        if (uidLength == allowed_cards_length[i])
        {
            // check if the UID is the same
            bool same = true;
            for (uint8_t j = 0; j < uidLength; j++)
            {
                if (uid[j] != allowed_cards[i][j])
                {
                    same = false;
                    break;
                }
            }
            if (same)
            {
                return true;

                // to be removed when database is implemented
                userindex = i;
            }
        }
    }
    return false;
}

// return true if the user is allowed to open the door
bool User::load_allowed_keys()
{
    open_door = false;
    // if the user is not in the database, return false
    if (!check_card())
    {
        return false;
    }

    // TODO
    for (uint8_t i = 0; i < nKeyblocks; i++)
    {
        allowed_keys[i] = allowed_keys_fake[userindex][i];

        if (allowed_keys[i] == true)
            open_door = true;
    }
    return open_door;
}

User::User()
{
    logged_in = false;
    open_door = false;
    for (int i = 0; i < User_max_length; i++)
    {
        uid[i] = 0;
    }
    uidLength = 0;
}

User::~User()
{
}

void User::logIn(uint8_t *uid, uint8_t *uidLength)
{
    open_door = false;
    this->uidLength = *uidLength;
    for (int i = 0; i < *uidLength; i++)
    {
        this->uid[i] = uid[i];
    }
    logged_in = true;
    // load allowed keys.
    load_allowed_keys();
}
void User::logOut()
{
    logged_in = false;
    open_door = false;
    for (int i = 0; i < User_max_length; i++)
    {
        uid[i] = 0;
    }
    uidLength = 0;
}
bool User::canOpenDoor()
{
    return open_door;
}
bool User::canReleaseKey(uint8_t n_keyblock)
{
    if (!logged_in)
        return false;

    if (!canOpenDoor())
        return false;

    return allowed_keys[n_keyblock];
}
