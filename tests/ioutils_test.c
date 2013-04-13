#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "ioutils_test.h"
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
    uint_fast64_t qword;

    if (open_reader(&rs, argv[1]))
    {
        print_error(stderr, rs.err_msg);
        return EXIT_FAILURE;
    }

    do
    {
        if (read_qword(&rs, &qword) < 1)
        {
            if (rs.eof)
            {
                break;
            }

            print_error(stderr, rs.err_msg);
            return EXIT_FAILURE;
        }

        // TODO: checks for uint_fast64_t is not 64-bits, write *less*
        // bits.
        if (write(STDOUT_FILENO, &qword, 8u) == -1)
        {
            perror("write");
            return EXIT_FAILURE;
        }
    }
    while (!rs.eof);

    close_reader(&rs);

    return EXIT_SUCCESS;
}
