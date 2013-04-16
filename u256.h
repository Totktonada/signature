#ifndef U256_H_SENTRY
#define U256_H_SENTRY

#include <stdint.h>

/* u256_t x -> uint_fast64_t x[4]
 * x[3]x[2]x[1]x[0] -- from more to less qwords. */
typedef uint_fast64_t u256_t[4];

void concat_u256_from_bytes(u256_t res, const unsigned char x[32]);
// res[31]res[30]...res[0] -- from more to less significant bytes.
void split_u256_to_bytes(unsigned char res[32], const u256_t x);

void concat_u256_from_words(u256_t res, const uint_fast16_t x[16]);
// res[15]res[14]...res[0] -- from more to less significant bytes.
void split_u256_to_words(uint_fast16_t res[16], const u256_t x);

void concat_u256_from_dwords(u256_t res, const uint_fast32_t x[8]);
// res[7]res[6]...res[0] -- from more to less significant bytes.
void split_u256_to_dwords(uint_fast32_t res[8], const u256_t x);

static inline void concat_u256_from_qwords(u256_t res, uint_fast64_t x3,
    uint_fast64_t x2, uint_fast64_t x1, uint_fast64_t x0)
{
    res[0] = x0;
    res[1] = x1;
    res[2] = x2;
    res[3] = x3;
}

static inline void u256_set(u256_t res, const u256_t x)
{
    res[0] = x[0];
    res[1] = x[1];
    res[2] = x[2];
    res[3] = x[3];
}

static inline void xor_u256(u256_t res, const u256_t x, const u256_t y)
{
    res[0] = x[0] ^ y[0];
    res[1] = x[1] ^ y[1];
    res[2] = x[2] ^ y[2];
    res[3] = x[3] ^ y[3];
}

static inline void u256_zeroize(u256_t res)
{
    res[0] = 0;
    res[1] = 0;
    res[2] = 0;
    res[3] = 0;
}

// Ignore possible carry in the most significant qword (res[3]).
static inline void u256_add_qword(u256_t res, const u256_t a,
    uint_fast64_t b)
{
    res[0] = a[0] + b;
    res[1] = a[1] + (res[0] < a[0]);
    res[2] = a[2] + (res[1] < a[1]);
    res[3] = a[3] + (res[2] < a[2]);
}

// Ignore possible carry in the most significant qword (res[3]).
static inline void u256_add(u256_t res, const u256_t a, const u256_t b)
{
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1] + (res[0] < a[0]);
    res[2] = a[2] + b[2] + (res[1] < a[1]);
    res[3] = a[3] + b[3] + (res[2] < a[2]);
}

#include <gmp.h>

static inline void mpz_set_u256(mpz_t res, const u256_t x)
{
    mpz_set_ui(res, x[3] & 0xFFFFFFFFFFFFFFFFu);
    mpz_mul_2exp(res, res, 64);
    mpz_add_ui(res, res, x[2] & 0xFFFFFFFFFFFFFFFFu);
    mpz_mul_2exp(res, res, 64);
    mpz_add_ui(res, res, x[1] & 0xFFFFFFFFFFFFFFFFu);
    mpz_mul_2exp(res, res, 64);
    mpz_add_ui(res, res, x[0] & 0xFFFFFFFFFFFFFFFFu);
}

#endif // U256_H_SENTRY
