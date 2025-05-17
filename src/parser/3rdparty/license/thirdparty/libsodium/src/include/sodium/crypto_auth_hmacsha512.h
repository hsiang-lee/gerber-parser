#ifndef crypto_auth_hmacsha512_H
#define crypto_auth_hmacsha512_H

#include <stddef.h>
#include "crypto_hash_sha512.h"

#define crypto_auth_hmacsha512_BYTES 64U

size_t crypto_auth_hmacsha512_bytes(void);

#define crypto_auth_hmacsha512_KEYBYTES 32U

size_t crypto_auth_hmacsha512_keybytes(void);

int crypto_auth_hmacsha512(unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *k);

int crypto_auth_hmacsha512_verify(const unsigned char *h, const unsigned char *in, unsigned long long inlen, const unsigned char *k);

/* ------------------------------------------------------------------------- */

typedef struct crypto_auth_hmacsha512_state {
  crypto_hash_sha512_state ictx;
  crypto_hash_sha512_state octx;
} crypto_auth_hmacsha512_state;

size_t crypto_auth_hmacsha512_statebytes(void);

int crypto_auth_hmacsha512_init(crypto_auth_hmacsha512_state *state, const unsigned char *key, size_t keylen);

int crypto_auth_hmacsha512_update(crypto_auth_hmacsha512_state *state, const unsigned char *in, unsigned long long inlen);

int crypto_auth_hmacsha512_final(crypto_auth_hmacsha512_state *state, unsigned char *out);

void crypto_auth_hmacsha512_keygen(unsigned char k[crypto_auth_hmacsha512_KEYBYTES]);

#endif
