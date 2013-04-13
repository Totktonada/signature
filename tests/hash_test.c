#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "hash_test.h"
#include "../hash.h"
#include "../ioutils.h"

void help(FILE * stream, const char * prog_name)
{
    fprintf(stream, "Usage: %s file.\n", prog_name);
}

void wrong_args(const char * prog_name)
{
    fprintf(stderr, "Bad arguments.\n");
    help(stdout, prog_name);
    exit(EXIT_FAILURE);
}

void print_error(FILE * stream, const char * msg)
{
    if (msg != NULL)
    {
        fprintf(stream, "%s\n", msg);
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        wrong_args(argv[0]);
    }

    reader_state rs;
    u256_t message;

    if (open_reader(&rs, argv[1]))
    {
        print_error(stderr, rs.err_msg);
        return EXIT_FAILURE;
    }

    hasher_state hs;
    init_hasher(&hs);

    do
    {
        int cnt = read_u256(&rs, message);

        if (cnt < 1)
        {
            if (rs.eof)
            {
                break;
            }

            print_error(stderr, rs.err_msg);
            return EXIT_FAILURE;
        }

        make_hasher_step(&hs, message, cnt);
    }
    while (!rs.eof);

    close_reader(&rs);

    // As defined in GOST R 34.11-94.
    if (rs.filesize == 0)
    {
        make_hasher_step(&hs, message, 0u);
    }

    get_hash(&hs);

    printf("0x%llX%llX%llX%llX\n",
        (unsigned long long int) hs.hash[3],
        (unsigned long long int) hs.hash[2],
        (unsigned long long int) hs.hash[1],
        (unsigned long long int) hs.hash[0]);

    return EXIT_SUCCESS;
}
