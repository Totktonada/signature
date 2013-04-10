#include <stdint.h>
#include "simple_substitution.h"

//     Defines
// ===================================================================

#define S_BLOCKS 8u
#define S_BLOCK_SIZE 16u
#define KEYS 8u
#define SINT_ROUNDS_1 24
#define SINT_ROUNDS_2 8

//     Static data
// ===================================================================

// Random 32-bits numbers.
static const uint_fast32_t keys[KEYS] =
{
    0x4FAF51BEu, 0x5C3D20EFu, 0x49BA3FCEu, 0x54C6A33Eu,
    0x5B7E3D72u, 0xE95B3FC4u, 0xCE42D79Au, 0xBA591FFEu
};

// Central Bank of Russian Federation S-boxes.
// Used as example in GOST R 34.11-94.
static const uint_fast8_t s[S_BLOCKS][S_BLOCK_SIZE] =
{
    { 4u, 10u,  9u,  2u, 13u,  8u,  0u, 14u,  6u, 11u,  1u, 12u,  7u, 15u,  5u,   3u},
    {14u, 11u,  4u, 12u,  6u, 13u, 15u, 10u,  2u,  3u,  8u,  1u,  0u,  7u,  5u,   9u},
    { 5u,  8u,  1u, 13u, 10u,  3u,  4u,  2u, 14u, 15u, 12u,  7u,  6u,  0u,  9u,  11u},
    { 7u, 13u, 10u,  1u,  0u,  8u,  9u, 15u, 14u,  4u,  6u, 12u, 11u,  2u,  5u,   3u},
    { 6u, 12u,  7u,  1u,  5u, 15u, 13u,  8u,  4u, 10u,  9u, 14u,  0u,  3u, 11u,   2u},
    { 4u, 11u, 10u,  0u,  7u,  2u,  1u, 13u,  3u,  6u,  8u,  5u,  9u, 12u, 15u,  14u},
    {13u, 11u,  4u,  1u,  3u, 15u,  5u,  9u,  0u, 10u, 14u,  7u,  6u,  8u,  2u,  12u},
    { 1u, 15u, 13u,  0u,  5u,  7u, 10u,  4u,  9u,  2u,  3u, 14u,  6u, 11u,  8u,  12u}
};

//     Functions
// ===================================================================

void set32_from64(uint_fast32_t * h, uint_fast32_t * l,
    const uint_fast64_t * from)
{
    uint_fast64_t fromv = (*from) & 0xFFFFFFFFFFFFFFFF;

    *h = fromv >> 32u;
    *l = fromv & 0xFFFFFFFFu;
}

void set64_from32(uint_fast64_t * to, const uint_fast32_t * h,
    const uint_fast32_t * l)
{
    uint_fast64_t h64 = (*h) & 0xFFFFFFFFFFFFFFFF;
    uint_fast64_t l64 = (*l) & 0xFFFFFFFFFFFFFFFF;

    *to = (h64 << 32u) | l64;
}

// Get and return 32-bits type, whereas it contain 8-bits value,
// for avoid extra type casts.
static uint_fast32_t s_lookup(uint_fast32_t x,
    const uint_fast8_t sb_h[S_BLOCK_SIZE],
    const uint_fast8_t sb_l[S_BLOCK_SIZE])
{
    uint_fast32_t h = (x & 0x000000F0u) >> 4u;
    uint_fast32_t l =  x & 0x0000000Fu;

    uint_fast32_t new_h = sb_h[h];
    uint_fast32_t new_l = sb_l[l];

    uint_fast32_t res = (new_h << 4u) | new_l;

    return res;
}

// We sure that n < sizeof(uint_fast32_t).
static inline void rotate_left(uint_fast32_t * x, unsigned int n)
{
    uint_fast32_t xv = *x & 0xFFFFFFFFu;

    uint_fast32_t a = (xv << n) & 0xFFFFFFFFu;
    uint_fast32_t b = (xv >> (32 - n));

    *x = a | b;
}

// uint_fast32_t can be 32-bits, but not must.
static inline void sum_mod32(uint_fast32_t * res,
    uint_fast32_t x, uint_fast32_t y)
{
    *res = (x + y) & 0xFFFFFFFF;
}

static void s_boxes_subst(uint_fast32_t * res, uint_fast32_t x)
{
    *res = s_lookup(x & 0x000000FFu, s[1], s[0]) |
        (s_lookup((x & 0x0000FF00u) >>  8u, s[3], s[2]) <<  8u) |
        (s_lookup((x & 0x00FF0000u) >> 16u, s[5], s[4]) << 16u) |
        (s_lookup((x & 0xFF000000u) >> 24u, s[7], s[6]) << 24u);

    rotate_left(res, 11u);
}

void simple_substitution(uint_fast64_t * res,
    const uint_fast64_t * message)
{
    uint_fast32_t tmp;
    uint_fast32_t b, a;
    int i;

    set32_from64(&b, &a, message);

    for (i = 0; i < SINT_ROUNDS_1; ++i)
    {
        sum_mod32(&tmp, a, keys[i % KEYS]);
        s_boxes_subst(&tmp, tmp);
        tmp = tmp ^ b;
        b = a;
        a = tmp;
    }

    for (i = SINT_ROUNDS_2 - 1; i >= 0; --i)
    {
        // We sure that i < KEYS.
        sum_mod32(&tmp, a, keys[i]);
        s_boxes_subst(&tmp, tmp);
        tmp = tmp ^ b;
        b = a;
        a = tmp;
    }

    set64_from32(res, &a, &b);
}

void reverse_simple_substitution(uint_fast64_t * res,
    const uint_fast64_t * encripted)
{
    uint_fast32_t tmp;
    uint_fast32_t b, a;
    int i;

    set32_from64(&b, &a, encripted);

    for (i = 0; i < SINT_ROUNDS_2; ++i)
    {
        // We sure that i < KEYS.
        sum_mod32(&tmp, a, keys[i]);
        s_boxes_subst(&tmp, tmp);
        tmp = tmp ^ b;
        b = a;
        a = tmp;
    }

    for (i = SINT_ROUNDS_1 - 1; i >= 0; --i)
    {
        sum_mod32(&tmp, a, keys[i % KEYS]);
        s_boxes_subst(&tmp, tmp);
        tmp = tmp ^ b;
        b = a;
        a = tmp;
    }

    set64_from32(res, &a, &b);
}


