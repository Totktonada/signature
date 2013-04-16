#include <stdbool.h>
#include <gmp.h>
#include "signature.h"

static inline void init_h(mpz_t h, const mpz_t hash, const mpz_t q)
{
    if (mpz_divisible_p(hash, q))
    {
        mpz_init_set_ui(h, 1u);
    }
    else
    {
        mpz_init_set(h, hash);
    }
}

// Get pseudo-random k: 0 < k < q. 
static inline void get_key(mpz_t k, gmp_randstate_t rs,
    const mpz_t q)
{
    do
    {
        mpz_urandomm(k, rs, q);
    }
    while (mpz_sgn(k) == 0);
}

// Do not change keys pair (x and y).
void gen_signature_attr(signature_attr * attr)
{
    // TODO: generate.
    mpz_init_set_str(attr->p,
        "EE8172AE 8996608F B69359B8 9EB82A69"
        "854510E2 977A4D63 BC97322C E5DC3386"
        "EA0A12B3 43E9190F 23177539 84583978"
        "6BB0C345 D165976E F2195EC9 B1C379E3", 16);

    mpz_init_set_str(attr->q,
        "98915E7E C8265EDF CDA31E88 F24809DD"
        "B064BDC7 285DD50D 7289F0AC 6F49DD2D", 16);

    mpz_init_set_str(attr->a,
        "9E960315 00C8774A 869582D4 AFDE2127"
        "AFAD2538 B4B6270A 6F7C8837 B50D50F2"
        "06755984 A49E5093 04D648BE 2AB5AAB1"
        "8EBE2CD4 6AC3D849 5B142AA6 CE23E21C", 16);
}

// Do not change keys pair (x and y).
void free_signature_attr(signature_attr * attr)
{
    mpz_clear(attr->p);
    mpz_clear(attr->q);
    mpz_clear(attr->a);
}

// Do not change any variables except keys pair (x and y).
// Required initialized attributes (invoke gen_signature_attr before).
void gen_keys_pair(signature_attr * attr)
{
    // TODO: generate.
    mpz_init_set_str(attr->x,
        "30363145 38303830 34363045 42353244"
        "35324234 31413237 38324331 38443046", 16);

    // y = (a ^ x) % p;
    mpz_init(attr->y);
    mpz_powm_sec(attr->y, attr->a, attr->x, attr->p);
}

// Do not change any variables except keys pair (x and y).
void free_keys_pair(signature_attr * attr)
{
    mpz_clear(attr->x);
    mpz_clear(attr->y);
}

// Set sign to signature of hash with attr.
void make_signature(signature * sign, const signature_attr * attr,
    const mpz_t hash)
{
    mpz_t h;
    mpz_t k;
    gmp_randstate_t rs;

    init_h(h, hash, attr->q);
    mpz_init(k);
    gmp_randinit_mt(rs);
    // TODO: use /dev/random or CryptGetRandom().
    gmp_randseed_ui(rs, 0xF4A83EB93CD13B26u);

    do
    {
        do
        {
            get_key(k, rs, attr->q);

            // r = ((a ^ k) % p) % q;
            mpz_powm_sec(sign->r, attr->a, k, attr->p);
            mpz_mod(sign->r, sign->r, attr->q);
        }
        while (mpz_sgn(sign->r) == 0);

        // s = (r * x + k * h) % q;
        mpz_mul(sign->s, sign->r, attr->x);
        mpz_addmul(sign->s, k, h);
        mpz_mod(sign->s, sign->s, attr->q);
    }
    while (mpz_sgn(sign->s) == 0);

    gmp_randclear(rs);
    mpz_clear(k);
    mpz_clear(h);
}

/* Return value: is sign signature of hash with attr.
 * true -- signature test passed.
 * false -- signature test failed. */
bool test_signature(const signature * sign, const signature_attr * attr,
    const mpz_t hash)
{
    if (mpz_sgn(sign->s) <= 0 ||
        mpz_sgn(sign->r) <= 0 ||      
        mpz_cmp(sign->s, attr->q) >= 0 ||
        mpz_cmp(sign->r, attr->q) >= 0)
    {
        return false;
    }

    mpz_t h;
    mpz_t v;
    mpz_t z1;
    mpz_t z2;
    mpz_t u;

    init_h(h, hash, attr->q);
    mpz_init(v);
    mpz_init(z1);
    mpz_init(z2);
    mpz_init(u);

    // v = (h ^ (q - 2)) % q;
    mpz_sub_ui(v, attr->q, 2u);
    mpz_powm_sec(v, h, v, attr->q);

    // z1 = (s * v) % q;
    mpz_mul(z1, sign->s, v);
    mpz_mod(z1, z1, attr->q);

    // z2 = ((q - r) * v) % q;
    mpz_sub(z2, attr->q, sign->r);
    mpz_mul(z2, z2, v);
    mpz_mod(z2, z2, attr->q);

    // u = (((a ^ z1) * (y ^ z2)) % p) % q;
    mpz_powm_sec(z1, attr->a, z1, attr->p);
    mpz_powm_sec(z2, attr->y, z2, attr->p);
    mpz_mul(u, z1, z2);
    mpz_mod(u, u, attr->p);
    mpz_mod(u, u, attr->q);

    bool res = (mpz_cmp(sign->r, u) == 0);

    mpz_clear(u);
    mpz_clear(z2);
    mpz_clear(z1);
    mpz_clear(v);
    mpz_clear(h);

    return res;
}
