#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "ioutils_test.h"
#include "test_utils.h"
#include "../ioutils.h"

void help(FILE * stream, const char * prog_name)
{
    fprintf(stream, "Usage: %s file.\n", prog_name);
}

int main(int argc, char ** argv)
{
    check_help_arg(argc, argv);
    check_args_count(argc, argv, 2);

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
            if (!rs.err && rs.eof)
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
