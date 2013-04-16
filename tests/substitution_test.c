#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "substitution_test.h"
#include "test_utils.h"
#include "../ioutils.h"
#include "../u256.h"
#include "../substitution.h"

void help(FILE * stream, const char * prog_name)
{
    fprintf(stream,
        "Usage: %s action file1 >> file2\n"
        "Actions: \"enc\" or \"dec\".\n", prog_name);
}

int main(int argc, char ** argv)
{
    check_help_arg(argc, argv);
    check_args_count(argc, argv, 3);
    bool mode_enc = get_mode(argv, "enc", "dec");

    u256_t key;

    // Random key.
    concat_u256_from_qwords(key,
        0x4FAF51BE5C3D20EFu,
        0x49BA3FCE54C6A33Eu,
        0x5B7E3D72E95B3FC4u,
        0xCE42D79ABA591FFEu);

    reader_state rs;
    uint_fast64_t qword;

    if (open_reader(&rs, argv[2]))
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

        if (mode_enc)
        {
            substitution(&qword, key, qword);
        }
        else
        {
            reverse_substitution(&qword, key, qword);
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
