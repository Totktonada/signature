#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "hash_test.h"
#include "test_utils.h"
#include "../hash.h"
#include "../ioutils.h"

void help(FILE * stream, const char * prog_name)
{
    fprintf(stream, "Usage: %s file.\n", prog_name);
}

int main(int argc, char ** argv)
{
    check_help_arg(argc, argv);

    if (argc != 2)
    {
        wrong_args(argv[0]);
    }

    u256_t hash;

    if (get_file_hash(hash, argv[1]))
    {
        return EXIT_FAILURE;
    }

    printf_u256(hash);

    return EXIT_SUCCESS;
}
