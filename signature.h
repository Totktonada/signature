#ifndef SIGNATURE_H_SENTRY
#define SIGNATURE_H_SENTRY

#include <gmp.h>

typedef struct signature_attr
{
    mpz_t p;
    mpz_t q;
    mpz_t a;
    mpz_t x; // secret key
    mpz_t y; // public key
}
signature_attr;

typedef struct signature
{
    mpz_t r;
    mpz_t s;
}
signature;

// Do not change keys pair (x and y).
void gen_signature_attr(signature_attr * attr);

// Do not change keys pair (x and y).
void free_signature_attr(signature_attr * attr);

// Do not change any variables except keys pair (x and y).
// Required initialized attributes (invoke gen_signature_attr before).
void gen_keys_pair(signature_attr * attr);

// Do not change any variables except keys pair (x and y).
void free_keys_pair(signature_attr * attr);

// Set sign to signature of hash with attr.
void make_signature(signature * sign, const signature_attr * attr,
    const mpz_t hash);

/* Return value: is sign signature of hash with attr.
 * true -- signature test passed.
 * false -- signature test failed. */
bool test_signature(const signature * sign, const signature_attr * attr,
    const mpz_t hash);

#endif // SIGNATURE_H_SENTRY
