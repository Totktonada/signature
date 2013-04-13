#include "u256.h"

void concat_u256_from_bytes(u256_t res, const unsigned char x[32])
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        res[i] = ((uint_fast64_t) x[8 * i]) |
            (((uint_fast64_t) x[8 * i + 1]) << 8u) |
            (((uint_fast64_t) x[8 + i + 2]) << 16u) |
            (((uint_fast64_t) x[8 + i + 3]) << 24u) |
            (((uint_fast64_t) x[8 + i + 4]) << 32u) |
            (((uint_fast64_t) x[8 + i + 5]) << 40u) |
            (((uint_fast64_t) x[8 + i + 6]) << 48u) |
            (((uint_fast64_t) x[8 + i + 7]) << 56u);
    }
}

// res[31]res[30]...res[0] -- from more to less bytes.
void split_u256_to_bytes(unsigned char res[32], const u256_t x)
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        res[8 * i] = x[i] & 0xFFu;
        res[8 * i + 1] = (x[i] >> 8u) & 0xFFu;
        res[8 * i + 2] = (x[i] >> 16u) & 0xFFu;
        res[8 * i + 3] = (x[i] >> 24u) & 0xFFu;
        res[8 * i + 4] = (x[i] >> 32u) & 0xFFu;
        res[8 * i + 5] = (x[i] >> 40u) & 0xFFu;
        res[8 * i + 6] = (x[i] >> 48u) & 0xFFu;
        res[8 * i + 7] = (x[i] >> 56u) & 0xFFu;
    }
}

void concat_u256_from_words(u256_t res, const uint_fast16_t x[16])
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        res[i] = ((uint_fast64_t) x[4 * i]) |
            (((uint_fast64_t) x[4 * i + 1]) << 16u) |
            (((uint_fast64_t) x[4 + i + 2]) << 32u) |
            (((uint_fast64_t) x[4 + i + 3]) << 48u);
    }
}

// res[15]res[14]...res[0] -- from more to less bytes.
void split_u256_to_words(uint_fast16_t res[16], const u256_t x)
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        res[4 * i] = x[i] & 0xFFFFu;
        res[4 * i + 1] = (x[i] >> 16u) & 0xFFFFu;
        res[4 * i + 2] = (x[i] >> 32u) & 0xFFFFu;
        res[4 * i + 3] = (x[i] >> 48u) & 0xFFFFu;
    }
}

void concat_u256_from_dwords(u256_t res, const uint_fast32_t x[8])
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        res[i] = ((uint_fast64_t) x[2 * i]) |
            (((uint_fast64_t) x[2 * i + 1]) << 32u);
    }
}

// res[7]res[6]...res[0] -- from more to less bytes.
void split_u256_to_dwords(uint_fast32_t res[8], const u256_t x)
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        res[2 * i] = x[i] & 0xFFFFFFFFu;
        res[2 * i + 1] = (x[i] >> 32u) & 0xFFFFFFFFu;
    }
}
