#ifndef IOUTILS_H_SENTRY
#define IOUTILS_H_SENTRY
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include "u256.h"

typedef struct reader_state
{
    bool err;
    const char * err_msg;
    off_t filesize;
    int fd;

    unsigned char * map; /* map area (== NULL, if munmap'ed) */
    off_t map_off;       /* offset of next map area in file */
    off_t off;           /* offset of next byte in map */
    long mapsize;        /* size of any map area */
    bool on_last_map;    /* Is current map area last of file? */
    bool eof;            /* End-of-file situation occured. */
}
reader_state;

/* Can produce error, rs->err shall be checked.
 * Returns resulting rs->err (false in normally case). */
bool open_reader(reader_state * rs, const char * filepath);

/* Can produce error, rs->err shall be checked.
 * Returns resulting rs->err (false in normally case). */
bool close_reader(reader_state * rs);

/* Can produce error, but can be involved after possible erroneous
 * operations (for example iterative) without rs->err checks.
 * Also, can be involved after end-of-file situation without
 * rs->eof checks. Do not forget check rs->err after series
 * of involves. If end-of-file situation occured in read process,
 * rs->eof will be set to true, check return value, if you interest
 * in last non-aliquot value (most significant bytes will be zeroize).
 * Returns 0 (and set rs->err to true) if error occured or no one bytes
 * has been readed, otherwise returns count of readed bytes. */
int read_qword(reader_state * rs, uint_fast64_t * res);

/* Similar to read_qword, but reads u256_t value.
 * Returns 0 (and set rs->err to true) if error occured or no one bytes
 * has been readed, otherwise returns count of readed bytes. */
int read_u256(reader_state * rs, u256_t res);

static inline void printf_u256(const u256_t x)
{
    printf("%016llX %016llX %016llX %016llX\n",
        (unsigned long long int) x[3],
        (unsigned long long int) x[2],
        (unsigned long long int) x[1],
        (unsigned long long int) x[0]);
}

#endif // IOUTILS_H_SENTRY
