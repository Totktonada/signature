#ifndef SUBSTITUTION_H_SENTRY
#define SUBSTITUTION_H_SENTRY

#include "u256.h"

/* GOST 28147-89 algorithm in simple substitution mode.
 * Key: 256 bits.
 * I/O block size: 64 bits. */

// res and message can be one variable.
void substitution(uint_fast64_t * res,
    u256_t key, uint_fast64_t message);

// res and encripted can be one variable.
void reverse_substitution(uint_fast64_t * res,
    u256_t key, uint_fast64_t encripted);

#endif // SUBSTITUTION_H_SENTRY
