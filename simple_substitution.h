#ifndef SIMPLE_SUBSTITUTION_H_SENTRY
#define SIMPLE_SUBSTITUTION_H_SENTRY

/* GOST 28147-89 algorithm in simple substitution mode.
 * Key: 256 bits.
 * I/O block size: 64 bits. */

// res and message can be one variable.
void simple_substitution(uint_fast64_t * res,
    const uint_fast64_t * message);

// res and encripted can be one variable.
void reverse_simple_substitution(uint_fast64_t * res,
    const uint_fast64_t * encripted);

#endif // SIMPLE_SUBSTITUTION_H_SENTRY
