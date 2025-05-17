#ifndef crypto_box_H
#define crypto_box_H

/*
 * THREAD SAFETY: crypto_box_keypair() is thread-safe,
 * provided that sodium_init() was called before.
 *
 * Other functions are always thread-safe.
 */

#include <stddef.h>

#include "crypto_box_curve25519xsalsa20poly1305.h"

#define crypto_box_SEEDBYTES crypto_box_curve25519xsalsa20poly1305_SEEDBYTES

size_t crypto_box_seedbytes(void);

#define crypto_box_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES

size_t crypto_box_publickeybytes(void);

#define crypto_box_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES

size_t crypto_box_secretkeybytes(void);

#define crypto_box_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_NONCEBYTES

size_t crypto_box_noncebytes(void);

#define crypto_box_MACBYTES crypto_box_curve25519xsalsa20poly1305_MACBYTES

size_t crypto_box_macbytes(void);

#define crypto_box_MESSAGEBYTES_MAX crypto_box_curve25519xsalsa20poly1305_MESSAGEBYTES_MAX

size_t crypto_box_messagebytes_max(void);

#define crypto_box_PRIMITIVE "curve25519xsalsa20poly1305"

const char *crypto_box_primitive(void);

int crypto_box_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed);

int crypto_box_keypair(unsigned char *pk, unsigned char *sk);

int crypto_box_easy(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *pk,
                    const unsigned char *sk);

int crypto_box_open_easy(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *pk,
                         const unsigned char *sk);

int crypto_box_detached(unsigned char *c, unsigned char *mac, const unsigned char *m, unsigned long long mlen, const unsigned char *n,
                        const unsigned char *pk, const unsigned char *sk);

int crypto_box_open_detached(unsigned char *m, const unsigned char *c, const unsigned char *mac, unsigned long long clen, const unsigned char *n,
                             const unsigned char *pk, const unsigned char *sk);

/* -- Precomputation interface -- */

#define crypto_box_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES

size_t crypto_box_beforenmbytes(void);

int crypto_box_beforenm(unsigned char *k, const unsigned char *pk, const unsigned char *sk);

int crypto_box_easy_afternm(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *k);

int crypto_box_open_easy_afternm(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k);

int crypto_box_detached_afternm(unsigned char *c, unsigned char *mac, const unsigned char *m, unsigned long long mlen, const unsigned char *n,
                                const unsigned char *k);

int crypto_box_open_detached_afternm(unsigned char *m, const unsigned char *c, const unsigned char *mac, unsigned long long clen,
                                     const unsigned char *n, const unsigned char *k);

/* -- Ephemeral SK interface -- */

#define crypto_box_SEALBYTES (crypto_box_PUBLICKEYBYTES + crypto_box_MACBYTES)

size_t crypto_box_sealbytes(void);

int crypto_box_seal(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *pk);

int crypto_box_seal_open(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *pk, const unsigned char *sk);

/* -- NaCl compatibility interface ; Requires padding -- */

#define crypto_box_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_ZEROBYTES

size_t crypto_box_zerobytes(void);

#define crypto_box_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES

size_t crypto_box_boxzerobytes(void);

int crypto_box(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *pk,
               const unsigned char *sk);

int crypto_box_open(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *pk,
                    const unsigned char *sk);

int crypto_box_afternm(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *k);

int crypto_box_open_afternm(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k);

#endif
