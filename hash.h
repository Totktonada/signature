#ifndef HASH_H_SENTRY
#define HASH_H_SENTRY

/* GOST R 34.11-94: Hash Function Algorithm. */

#include "u256.h"

typedef struct hasher_state
{
    u256_t length;
    u256_t sum;
    u256_t hash;
}
hasher_state;

void init_hasher(hasher_state * hs);

/* Last message must be placed in less significant bytes, i.e. in large
 * indexes of variable message. Unused bytes must be zero.
 * message_length in bytes. */
void make_hasher_step(hasher_state * hs, const u256_t message,
    unsigned int message_length);

/* Resulting hash placed in hs->hash. */
void get_hash(hasher_state * hs);

#endif // HASH_H_SENTRY
