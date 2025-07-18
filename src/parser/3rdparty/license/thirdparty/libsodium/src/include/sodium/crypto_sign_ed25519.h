#ifndef crypto_sign_ed25519_H
#define crypto_sign_ed25519_H

#include <stddef.h>
#include <utils.h>
#include "crypto_hash_sha512.h"

typedef struct crypto_sign_ed25519ph_state {
  crypto_hash_sha512_state hs;
} crypto_sign_ed25519ph_state;

size_t crypto_sign_ed25519ph_statebytes(void);

#define crypto_sign_ed25519_BYTES 64U

size_t crypto_sign_ed25519_bytes(void);

#define crypto_sign_ed25519_SEEDBYTES 32U

size_t crypto_sign_ed25519_seedbytes(void);

#define crypto_sign_ed25519_PUBLICKEYBYTES 32U

size_t crypto_sign_ed25519_publickeybytes(void);

#define crypto_sign_ed25519_SECRETKEYBYTES (32U + 32U)

size_t crypto_sign_ed25519_secretkeybytes(void);

#define crypto_sign_ed25519_MESSAGEBYTES_MAX (SODIUM_SIZE_MAX - crypto_sign_ed25519_BYTES)

size_t crypto_sign_ed25519_messagebytes_max(void);

int crypto_sign_ed25519(unsigned char *sm, unsigned long long *smlen_p, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
    ;

int crypto_sign_ed25519_open(unsigned char *m, unsigned long long *mlen_p, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk)
     ;

int crypto_sign_ed25519_detached(unsigned char *sig, unsigned long long *siglen_p, const unsigned char *m, unsigned long long mlen,
                                 const unsigned char *sk) ;

int crypto_sign_ed25519_verify_detached(const unsigned char *sig, const unsigned char *m, unsigned long long mlen, const unsigned char *pk)
     ;

int crypto_sign_ed25519_keypair(unsigned char *pk, unsigned char *sk) ;

int crypto_sign_ed25519_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed) ;

int crypto_sign_ed25519_pk_to_curve25519(unsigned char *curve25519_pk, const unsigned char *ed25519_pk) 
;

int crypto_sign_ed25519_sk_to_curve25519(unsigned char *curve25519_sk, const unsigned char *ed25519_sk) ;

int crypto_sign_ed25519_sk_to_seed(unsigned char *seed, const unsigned char *sk) ;

int crypto_sign_ed25519_sk_to_pk(unsigned char *pk, const unsigned char *sk) ;

int crypto_sign_ed25519ph_init(crypto_sign_ed25519ph_state *state) ;

int crypto_sign_ed25519ph_update(crypto_sign_ed25519ph_state *state, const unsigned char *m, unsigned long long mlen) ;

int crypto_sign_ed25519ph_final_create(crypto_sign_ed25519ph_state *state, unsigned char *sig, unsigned long long *siglen_p, const unsigned char *sk)
    ;

int crypto_sign_ed25519ph_final_verify(crypto_sign_ed25519ph_state *state, const unsigned char *sig, const unsigned char *pk)
     ;

#endif
