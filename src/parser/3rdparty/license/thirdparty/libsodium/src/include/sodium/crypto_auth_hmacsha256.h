#ifndef crypto_auth_hmacsha256_H
#define crypto_auth_hmacsha256_H

#include <stddef.h>
#include "crypto_hash_sha256.h"

#define crypto_auth_hmacsha256_BYTES 32U

size_t crypto_auth_hmacsha256_bytes(void);

#define crypto_auth_hmacsha256_KEYBYTES 32U

size_t crypto_auth_hmacsha256_keybytes(void);

int crypto_auth_hmacsha256(unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *k);

int crypto_auth_hmacsha256_verify(const unsigned char *h, const unsigned char *in, unsigned long long inlen, const unsigned char *k);

/* ------------------------------------------------------------------------- */

typedef struct crypto_auth_hmacsha256_state {
  crypto_hash_sha256_state ictx;
  crypto_hash_sha256_state octx;
} crypto_auth_hmacsha256_state;

size_t crypto_auth_hmacsha256_statebytes(void);

int crypto_auth_hmacsha256_init(crypto_auth_hmacsha256_state *state, const unsigned char *key, size_t keylen);

int crypto_auth_hmacsha256_update(crypto_auth_hmacsha256_state *state, const unsigned char *in, unsigned long long inlen);

int crypto_auth_hmacsha256_final(crypto_auth_hmacsha256_state *state, unsigned char *out);

void crypto_auth_hmacsha256_keygen(unsigned char k[crypto_auth_hmacsha256_KEYBYTES]);

#endif
