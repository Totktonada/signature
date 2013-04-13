#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "ioutils.h"

#define MAP_SIZE_IN_PAGES 128

/* Can produce error, rs->err shall be checked.
 * Returns resulting rs->err (false in normally case).
 * Normally munmap'ed area of rs->map. SEGFAULT'ed at NULL rs->map. */
static bool munmap_wrapper(reader_state * rs)
{
    if (munmap(rs->map, rs->mapsize) == -1)
    {
        rs->err = true;
        rs->err_msg = NULL;
        perror("munmap");
        return rs->err;
    }

    rs->map = NULL;
    return rs->err;
}

/* Can produce error, rs->err shall be checked.
 * Returns resulting rs->err (false in normally case).
 * Normally mmap'ed area begins at rs->map_off file offset. */
static bool mmap_wrapper(reader_state * rs)
{
    rs->map = (unsigned char *) mmap(NULL, rs->mapsize, PROT_READ,
        MAP_PRIVATE, rs->fd, rs->map_off);

    if (rs->map == MAP_FAILED)
    {
        rs->err = true;
        rs->err_msg = NULL;
        perror("mmap");
        return rs->err;
    }

    return rs->err;
}

/* Can produce error, rs->err shall be checked.
 * Returns resulting rs->err (false in normally case). */
static bool get_next_map(reader_state * rs)
{
    if (rs->map != NULL)
    {
        if (munmap_wrapper(rs))
        {
            return rs->err;
        }
    }

    if (rs->on_last_map)
    {
        rs->err = true;
        rs->err_msg = "Can not map area after end of file.";
        return rs->err;
    }

    if (mmap_wrapper(rs))
    {
        return rs->err;
    }

    rs->off = 0;
    rs->map_off += rs->mapsize;
    rs->on_last_map = rs->filesize - rs->map_off < 1;

    return rs->err;
}

/* Can produce error, but can be involved after possible erroneous
 * operations (for example iterative) without rs->err checks.
 * Also, can be involved after end-of-file situation without
 * rs->eof checks. Do not forget check rs->err and rs->eof after series
 * of involves.
 * If rs->err already true or error occured at work then set rs->err and
 * returns '\0'. If rs->eof already true or end-of-file situation occured
 * then set rs->eof and returns '\0'. Normally returns next byte of file. */
static unsigned char get_next_byte(reader_state * rs)
{
    if (rs->err || rs->eof)
    {
        return '\0';
    }

    if (!rs->on_last_map &&
        (rs->map == NULL || rs->mapsize - rs->off < 1))
    {
        if (get_next_map(rs))
        {
            return '\0';
        }
    }

    rs->eof = rs->on_last_map &&
        (rs->filesize % rs->mapsize) - rs->off < 1;

    if (rs->eof)
    {
        if (rs->map != NULL)
        {
            if (munmap_wrapper(rs))
            {
                return '\0';
            }
        }

        return '\0';
    }

    unsigned char res = rs->map[rs->off];
    ++(rs->off);
    return res;
}

/* Can produce error, rs->err shall be checked.
 * Returns resulting rs->err (false in normally case). */
bool open_reader(reader_state * rs, const char * filepath)
{
    struct stat s;

    rs->err = false;
    rs->err_msg = NULL;

    if (stat(filepath, &s) == -1)
    {
        rs->err = true;
        rs->err_msg = NULL;
        perror("stat");
        return rs->err;
    }

    if (! S_ISREG(s.st_mode))
    {
        rs->err = true;
        rs->err_msg = "Not a regular file.";
        return rs->err;
    }

    rs->filesize = s.st_size;

    if (rs->filesize > 0)
    {
        rs->fd = open(filepath, O_RDONLY);

        if (rs->fd == -1)
        {
            rs->err = true;
            rs->err_msg = NULL;
            perror("open");
            return rs->err;
        }
    }

    rs->map = NULL;
    rs->map_off = 0;
    rs->off = 0;
    rs->mapsize = MAP_SIZE_IN_PAGES * sysconf(_SC_PAGESIZE);
    rs->on_last_map = rs->filesize - rs->map_off < 1;
    rs->eof = false;

    return rs->err;
}

/* Can produce error, rs->err shall be checked.
 * Returns resulting rs->err (false in normally case). */
bool close_reader(reader_state * rs)
{
    if (rs->map != NULL)
    {
        if (munmap_wrapper(rs))
        {
            return rs->err;
        }
    }

    if (rs->filesize > 0)
    {
        if (close(rs->fd) == -1)
        {
            rs->err = true;
            rs->err_msg = NULL;
            perror("close");
            return rs->err;
        }
    }

    return rs->err;
}

/* See comment to read_qword function, except return value. */
static inline void read_qword_only(reader_state * rs, uint_fast64_t * res)
{
    *res = (((uint_fast64_t) get_next_byte(rs))) |
        (((uint_fast64_t) get_next_byte(rs)) << 8u) |
        (((uint_fast64_t) get_next_byte(rs)) << 16u) |
        (((uint_fast64_t) get_next_byte(rs)) << 24u) |
        (((uint_fast64_t) get_next_byte(rs)) << 32u) |
        (((uint_fast64_t) get_next_byte(rs)) << 40u) |
        (((uint_fast64_t) get_next_byte(rs)) << 48u) |
        (((uint_fast64_t) get_next_byte(rs)) << 56u);
}

/* Can produce error, but can be involved after possible erroneous
 * operations (for example iterative) without rs->err checks.
 * Also, can be involved after end-of-file situation without
 * rs->eof checks. Do not forget check rs->err after series
 * of involves. If end-of-file situation occured in read process,
 * rs->eof will be set to true, check return value, if you interest
 * in last non-aliquot value (most significant bytes will be zeroize).
 * Returns 0 (and set rs->err to true) if error occured or no one bytes
 * has been readed, otherwise returns count of readed bytes. */
int read_qword(reader_state * rs, uint_fast64_t * res)
{
    off_t map_off = rs->map_off;
    off_t off = rs->off;

    read_qword_only(rs, res);

    if (!rs->err && rs->off == off && rs->map_off == map_off)
    {
        rs->err = true;
        rs->err_msg =
            "Can not read next value."
            " Maybe end-of-file situation check was forgotten?";
    }

    return rs->err ? 0 : (rs->map_off - map_off) + (rs->off - off);
}

/* Similar to read_qword, but reads u256_t value.
 * Returns 0 (and set rs->err to true) if error occured or no one bytes
 * has been readed, otherwise returns count of readed bytes. */
int read_u256(reader_state * rs, u256_t res)
{
    off_t map_off = rs->map_off;
    off_t off = rs->off;

    read_qword_only(rs, res + 0);
    read_qword_only(rs, res + 1);
    read_qword_only(rs, res + 2);
    read_qword_only(rs, res + 3);

    if (!rs->err && rs->off == off && rs->map_off == map_off)
    {
        rs->err = true;
        rs->err_msg =
            "Can not read next value."
            " Maybe end-of-file situation check was forgotten?";
    }

    return rs->err ? 0 : (rs->map_off - map_off) + (rs->off - off);
}
