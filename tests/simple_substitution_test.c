#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../simple_substitution.h"

#define READ_SUCCESS(value) ((value) > 0)
#define READ_ERROR(value) ((value) == -1)
#define READ_EOF(value) ((value) == 0)

#define WRITE_ERROR(value) ((value) == -1)

#define STR_EQUAL(str1, str2) (strcmp((str1), (str2)) == 0)

#define BYTES_TO_WRITE 8u

void help(FILE * stream, const char * prog_name)
{
    fprintf(stream,
        "Usage: cat file1 | %s action >> file2\n"
        "Actions: \"enc\" or \"dec\".\n", prog_name);
}

void wrong_args(const char * prog_name)
{
    fprintf(stderr, "Bad arguments.\n");
    help(stdout, prog_name);
    exit(EXIT_FAILURE);
}

void read64_step(unsigned char * c, int * read_value, int * cnt, int fd)
{
    if (READ_SUCCESS(*read_value))
    {
        *read_value = read(fd, c, sizeof(unsigned char));
    }

    if (READ_SUCCESS(*read_value))
    {
        ++(*cnt);
    }
    else
    {
        *c = 0;
    }
}

/* Little endian files only. */
/* That is ugly code, yes. But more or less simple. */
uint_fast64_t read64(uint_fast64_t * res, int fd, int * read_value)
{
    int cnt = 0;
    unsigned char c;

    *res = 0;

    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c));
    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c) << 8u);
    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c) << 16u);
    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c) << 24u);
    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c) << 32u);
    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c) << 40u);
    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c) << 48u);
    read64_step(&c, read_value, &cnt, fd);
    *res = (*res) | (((uint_fast64_t) c) << 56u);

    if (cnt == 0)
    {
        return *read_value;
    }
    else
    {
        return cnt;
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        wrong_args(argv[0]);
    }

    bool mode_enc;

    if (STR_EQUAL(argv[1], "enc"))
    {
        mode_enc = true;
    }
    else if (STR_EQUAL(argv[1], "dec"))
    {
        mode_enc = false;
    }
    else
    {
        wrong_args(argv[0]);
    }

    uint_fast64_t buf;
    // Stub for READ_SUCCESS(read_value) be true.
    int read_value = 1;
    int write_value;

    while (READ_SUCCESS(read64(&buf, STDIN_FILENO, &read_value)))
    {
        if (mode_enc)
        {
            simple_substitution(&buf, &buf);
        }
        else
        {
            reverse_simple_substitution(&buf, &buf);
        }

        // TODO: checks for uint_fast64_t is not 64-bits, write *less* bits.
        write_value = write(STDOUT_FILENO, &buf, BYTES_TO_WRITE);

        if (WRITE_ERROR(write_value))
        {
            perror("write");
            return EXIT_FAILURE;
        }
    }

    if (READ_ERROR(read_value))
    {
        perror("read");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
