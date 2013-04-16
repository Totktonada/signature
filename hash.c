#include <stdint.h>
#include "hash.h"
#include "u256.h"
#include "substitution.h"

// res and x can be pointers to one memory area.
static void transform_a(u256_t res, const u256_t x)
{
    if (res == x)
    {
        // This possible unnecessary copying, but I do not know,
        // make compiler copy of inline function parameters as
        // this need here.
        u256_t y;
        concat_u256_from_qwords(y, x[3], x[2], x[1], x[0]);
        concat_u256_from_qwords(res, y[0] ^ y[1], y[3], y[2], y[1]);
    }
    else
    {
        concat_u256_from_qwords(res, x[0] ^ x[1], x[3], x[2], x[1]);
    }
}

// res and x can be pointers to one memory area.
static void transform_p(u256_t res, const u256_t x)
{
    unsigned char xb[32];
    unsigned char yb[32];

    split_u256_to_bytes(xb, x);

    for (unsigned int k = 0; k < 8; ++k)
    {
        for (unsigned int i = 0; i < 4; ++i)
        {
            yb[4 * k + i] = xb[8 * i + k];
        }
    }

    concat_u256_from_bytes(res, yb);
}

static void gen_keys(u256_t k[4], const u256_t m, const u256_t h)
{
    u256_t c;
    u256_t u;
    u256_t v;
    u256_t w;

    concat_u256_from_qwords(c,
        0xFF00FFFF000000FF,
        0xFF0000FF00FFFF00,
        0x00FF00FF00FF00FF,
        0xFF00FF00FF00FF00);

    xor_u256(w, m, h);

    transform_p(k[0], w);

    transform_a(u, h);
    transform_a(v, m);
    transform_a(v, v);
    xor_u256(w, u, v);
    transform_p(k[1], w);

    transform_a(u, u);
    xor_u256(u, u, c);
    transform_a(v, v);
    transform_a(v, v);
    xor_u256(w, u, v);
    transform_p(k[2], w);

    transform_a(u, u);
    transform_a(v, v);
    transform_a(v, v);
    xor_u256(w, u, v);
    transform_p(k[3], w);
}

// res and x can be pointers to one memory area.
static void transform_psi(u256_t res, const u256_t x)
{
    static uint_fast16_t xw[16];

    split_u256_to_words(xw, x);

    uint_fast16_t tmp = xw[0] ^ xw[1] ^ xw[2] ^ xw[3] ^ xw[12] ^ xw[15];

    for (unsigned int i = 0; i < 15; ++i)
    {
        xw[i] = xw[i + 1];
    }

    xw[15] = tmp;

    concat_u256_from_words(res, xw);
}

// Transformation: PSI^61(H xor PSI(M xor PSI^12(S))).
static void transform_mixing(u256_t res,
    const u256_t m, const u256_t h, const u256_t s)
{
    u256_t tmp;

    transform_psi(tmp, s);

    for (unsigned int i = 0; i < 11; ++i)
    {
        transform_psi(tmp, tmp);
    }

    xor_u256(tmp, tmp, m);
    transform_psi(tmp, tmp);
    xor_u256(tmp, tmp, h);

    for (unsigned int i = 0; i < 60; ++i)
    {
        transform_psi(tmp, tmp);
    }

    transform_psi(res, tmp);
}

static void hash_step(u256_t res, const u256_t m, const u256_t h)
{
    u256_t keys[4];
    u256_t s;

    gen_keys(keys, m, h);

    substitution(s + 0, keys[0], h[0]);
    substitution(s + 1, keys[1], h[1]);
    substitution(s + 2, keys[2], h[2]);
    substitution(s + 3, keys[3], h[3]);

    transform_mixing(res, m, h, s);
}

void init_hasher(hasher_state * hs)
{
    u256_zeroize(hs->length);
    u256_zeroize(hs->sum);
    u256_zeroize(hs->hash);
}

/* Last message must be placed in less significant bytes, i.e. in large
 * indexes of variable message. Unused bytes must be zero.
 * message_length in bytes. */
void make_hasher_step(hasher_state * hs, const u256_t message,
    unsigned int message_length)
{
    u256_add_qword(hs->length, hs->length, 8u * message_length);
    u256_add(hs->sum, hs->sum, message);
    hash_step(hs->hash, message, hs->hash);
}

/* Resulting hash placed in hash and hs->hash. */
void get_hash(u256_t hash, hasher_state * hs)
{
    hash_step(hs->hash, hs->length, hs->hash);
    hash_step(hs->hash, hs->sum, hs->hash);
    u256_set(hash, hs->hash);
}
