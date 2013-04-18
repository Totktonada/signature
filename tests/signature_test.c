#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include "signature_test.h"
#include "test_utils.h"
#include "../signature.h"
#include "../u256.h"
#include "../ioutils.h"

void help(FILE * stream, const char * prog_name)
{
    fprintf(stream,
        "Usage: %s action file file.sg\n"
        "Actions: \"make\" or \"test\".\n", prog_name);
}

void mpz_set_file_hash(mpz_t hash, const char * read_filepath)
{
    u256_t u256_hash;

    if (get_file_hash(u256_hash, read_filepath))
    {
        exit(EXIT_FAILURE);
    }

    mpz_set_u256(hash, u256_hash);
}

FILE * open_wrapper(const char * filepath, const char * mode)
{
    FILE * stream = fopen(filepath, mode);

    if (stream == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    return stream;
}

void close_wrapper(FILE * stream)
{
    if (fclose(stream) == EOF)
    {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

void mpz_out_wrapper(FILE * stream, mpz_t x)
{
    if (mpz_out_raw(stream, x) == 0)
    {
        fprintf(stderr, "mpz_out_raw error.\n");
        exit(EXIT_FAILURE);
    }
}

void mpz_inp_wrapper(mpz_t x, FILE * stream)
{
    if (mpz_inp_raw(x, stream) == 0)
    {
        fprintf(stderr, "mpz_imp_raw error.\n");
        exit(EXIT_FAILURE);
    }
}

void write_signature_file(const char * write_filepath, mpz_t hash)
{
    signature_attr attr;
    gen_signature_attr(&attr);
    gen_keys_pair(&attr);

    signature sign;
    mpz_init(sign.r);
    mpz_init(sign.s);

    make_signature(&sign, &attr, hash);

    FILE * stream = open_wrapper(write_filepath, "wb");
    mpz_out_wrapper(stream, sign.r);
    mpz_out_wrapper(stream, sign.s);
    close_wrapper(stream);

    mpz_clear(sign.r);
    mpz_clear(sign.s);

    free_keys_pair(&attr);
    free_signature_attr(&attr);
}

void test_signature_file(const char * read_filepath, mpz_t hash)
{
    signature_attr attr;
    gen_signature_attr(&attr);
    gen_keys_pair(&attr);

    signature sign;
    mpz_init(sign.r);
    mpz_init(sign.s);

    FILE * stream = open_wrapper(read_filepath, "rb");
    mpz_inp_wrapper(sign.r, stream);
    mpz_inp_wrapper(sign.s, stream);
    close_wrapper(stream);

    printf("Signature verification...");

    if (test_signature(&sign, &attr, hash))
    {
        printf(" [Ok]\n");
    }
    else
    {
        printf(" [Failed]\n");
    }

    mpz_clear(sign.r);
    mpz_clear(sign.s);

    free_signature_attr(&attr);
    free_keys_pair(&attr);
}

int main(int argc, char ** argv)
{
    check_help_arg(argc, argv);
    check_args_count(argc, argv, 4);
    bool mode_make = get_mode(argv, "make", "test");

    mpz_t hash;
    mpz_init(hash);
    mpz_set_file_hash(hash, argv[2]);

    if (mode_make)
    {
        write_signature_file(argv[3], hash);
    }
    else
    {
        test_signature_file(argv[3], hash);
    }

    mpz_clear(hash);

    return EXIT_SUCCESS;
}
