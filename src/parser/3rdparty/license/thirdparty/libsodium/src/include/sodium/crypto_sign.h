#ifndef crypto_sign_H
#define crypto_sign_H

/*
 * THREAD SAFETY: crypto_sign_keypair() is thread-safe,
 * provided that sodium_init() was called before.
 *
 * Other functions, including crypto_sign_seed_keypair() are always thread-safe.
 */

#include <stddef.h>

#include "crypto_sign_ed25519.h"

typedef crypto_sign_ed25519ph_state crypto_sign_state;

size_t crypto_sign_statebytes(void);

#define crypto_sign_BYTES crypto_sign_ed25519_BYTES

size_t crypto_sign_bytes(void);

#define crypto_sign_SEEDBYTES crypto_sign_ed25519_SEEDBYTES

size_t crypto_sign_seedbytes(void);

#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_PUBLICKEYBYTES

size_t crypto_sign_publickeybytes(void);

#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_SECRETKEYBYTES

size_t crypto_sign_secretkeybytes(void);

#define crypto_sign_MESSAGEBYTES_MAX crypto_sign_ed25519_MESSAGEBYTES_MAX

size_t crypto_sign_messagebytes_max(void);

#define crypto_sign_PRIMITIVE "ed25519"

const char *crypto_sign_primitive(void);

int crypto_sign_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed) ;

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) ;

int crypto_sign(unsigned char *sm, unsigned long long *smlen_p, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
    ;

int crypto_sign_open(unsigned char *m, unsigned long long *mlen_p, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk)
     ;

int crypto_sign_detached(unsigned char *sig, unsigned long long *siglen_p, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
    ;

int crypto_sign_verify_detached(const unsigned char *sig, const unsigned char *m, unsigned long long mlen, const unsigned char *pk)
     ;

int crypto_sign_init(crypto_sign_state *state);

int crypto_sign_update(crypto_sign_state *state, const unsigned char *m, unsigned long long mlen) ;

int crypto_sign_final_create(crypto_sign_state *state, unsigned char *sig, unsigned long long *siglen_p, const unsigned char *sk)
    ;

int crypto_sign_final_verify(crypto_sign_state *state, const unsigned char *sig, const unsigned char *pk) 
;

#endif
