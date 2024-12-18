#include "bitmap.h"

bool Bitmap_Get(Bitmap* bitmap, uint64_t index)
{
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8; // The remainder
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    if ((bitmap->Buffer[byteIndex] & bitIndexer) > 0)
    {
        return true;
    }

    return false;
}

void Bitmap_Set(Bitmap* bitmap, uint64_t index, bool value)
{
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8; // The remainder
    uint8_t bitIndexer = 0b10000000 >> bitIndex;

    // Set that bit the false
    bitmap->Buffer[byteIndex] &= ~(bitIndexer);

    // Check if we need to set that bit
    if (value)
    {
        bitmap->Buffer[byteIndex] |= bitIndexer;
    }
}


