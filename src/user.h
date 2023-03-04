#include <Arduino.h>

// test table of allowed cards
const uint8_t allowed_cards[][7] = {
    // 0x67 0x38 0x1A 0x2D
    //{0x67, 0x38, 0x1A, 0x2D, 0x00, 0x00, 0x00}, // 4 bytes
    // 0x04 0x30 0x8A 0x3A 0xFF 0x42 0x80
    {0x04, 0x30, 0x8A, 0x3A, 0xFF, 0x42, 0x80}, // 7 bytes
};
const uint8_t allowed_cards_length[] = {
    // 4,
    7};

// function that takes a pointer to a UID from an NFC card and UID length, and returns true if the card is allowed to open the door, false otherwise
bool check_card(const uint8_t *uid, const uint8_t uidLength)
{
    // check if the card is in the allowed cards table
    for (uint8_t i = 0; i < sizeof(allowed_cards) / sizeof(allowed_cards[0]); i++)
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
            }
        }
    }
    return false;
}