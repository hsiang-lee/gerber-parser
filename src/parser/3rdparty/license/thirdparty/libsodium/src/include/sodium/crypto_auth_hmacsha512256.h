#ifndef crypto_auth_hmacsha512256_H
#define crypto_auth_hmacsha512256_H

#include <stddef.h>
#include "crypto_auth_hmacsha512.h"

#define crypto_auth_hmacsha512256_BYTES 32U

size_t crypto_auth_hmacsha512256_bytes(void);

#define crypto_auth_hmacsha512256_KEYBYTES 32U

size_t crypto_auth_hmacsha512256_keybytes(void);

int crypto_auth_hmacsha512256(unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *k);

int crypto_auth_hmacsha512256_verify(const unsigned char *h, const unsigned char *in, unsigned long long inlen, const unsigned char *k);

/* ------------------------------------------------------------------------- */

typedef crypto_auth_hmacsha512_state crypto_auth_hmacsha512256_state;

size_t crypto_auth_hmacsha512256_statebytes(void);

int crypto_auth_hmacsha512256_init(crypto_auth_hmacsha512256_state *state, const unsigned char *key, size_t keylen);

int crypto_auth_hmacsha512256_update(crypto_auth_hmacsha512256_state *state, const unsigned char *in, unsigned long long inlen);

int crypto_auth_hmacsha512256_final(crypto_auth_hmacsha512256_state *state, unsigned char *out);

void crypto_auth_hmacsha512256_keygen(unsigned char k[crypto_auth_hmacsha512256_KEYBYTES]);

#endif
