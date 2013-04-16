#ifndef TEST_UTILS_H_SENTRY
#define TEST_UTILS_H_SENTRY

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

//     Defines and forward declatations
// ===================================================================

#define STR_EQUAL(str1, str2) (strcmp((str1), (str2)) == 0)

void help(FILE * stream, const char * prog_name);

//     Functions
// ===================================================================

static inline void check_help_arg(int argc, char ** argv)
{
    if (argc == 2 && STR_EQUAL(argv[1], "--help"))
    {
        help(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
}

static inline void wrong_args(const char * prog_name)
{
    fprintf(stderr, "Bad arguments. Try `%s --help`.\n", prog_name);
    help(stdout, prog_name);
    exit(EXIT_FAILURE);
}

static inline void check_args_count(int argc, char ** argv,
    int count)
{
    if (argc != count)
    {
        wrong_args(argv[0]);
    }
}

/* Get subcommand. */
static inline bool get_mode(char ** argv, const char * true_pattern,
    const char * false_pattern)
{
    if (STR_EQUAL(argv[1], true_pattern))
    {
        return true;
    }
    else if (STR_EQUAL(argv[1], false_pattern))
    {
        return false;
    }
    else
    {
        wrong_args(argv[0]);
    }

    /* Not possible */
    return true;
}

#endif // TEST_UTILS_H_SENTRY
