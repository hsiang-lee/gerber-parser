
#ifndef crypto_box_curve25519xchacha20poly1305_H
#define crypto_box_curve25519xchacha20poly1305_H

#include <stddef.h>
#include "crypto_stream_xchacha20.h"

#define crypto_box_curve25519xchacha20poly1305_SEEDBYTES 32U

size_t crypto_box_curve25519xchacha20poly1305_seedbytes(void);

#define crypto_box_curve25519xchacha20poly1305_PUBLICKEYBYTES 32U

size_t crypto_box_curve25519xchacha20poly1305_publickeybytes(void);

#define crypto_box_curve25519xchacha20poly1305_SECRETKEYBYTES 32U

size_t crypto_box_curve25519xchacha20poly1305_secretkeybytes(void);

#define crypto_box_curve25519xchacha20poly1305_BEFORENMBYTES 32U

size_t crypto_box_curve25519xchacha20poly1305_beforenmbytes(void);

#define crypto_box_curve25519xchacha20poly1305_NONCEBYTES 24U

size_t crypto_box_curve25519xchacha20poly1305_noncebytes(void);

#define crypto_box_curve25519xchacha20poly1305_MACBYTES 16U

size_t crypto_box_curve25519xchacha20poly1305_macbytes(void);

#define crypto_box_curve25519xchacha20poly1305_MESSAGEBYTES_MAX \
  (crypto_stream_xchacha20_MESSAGEBYTES_MAX - crypto_box_curve25519xchacha20poly1305_MACBYTES)

size_t crypto_box_curve25519xchacha20poly1305_messagebytes_max(void);

int crypto_box_curve25519xchacha20poly1305_seed_keypair(unsigned char *pk, unsigned char *sk, const unsigned char *seed);

int crypto_box_curve25519xchacha20poly1305_keypair(unsigned char *pk, unsigned char *sk);

int crypto_box_curve25519xchacha20poly1305_easy(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n,
                                                const unsigned char *pk, const unsigned char *sk);

int crypto_box_curve25519xchacha20poly1305_open_easy(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n,
                                                     const unsigned char *pk, const unsigned char *sk);

int crypto_box_curve25519xchacha20poly1305_detached(unsigned char *c, unsigned char *mac, const unsigned char *m, unsigned long long mlen,
                                                    const unsigned char *n, const unsigned char *pk, const unsigned char *sk);

int crypto_box_curve25519xchacha20poly1305_open_detached(unsigned char *m, const unsigned char *c, const unsigned char *mac, unsigned long long clen,
                                                         const unsigned char *n, const unsigned char *pk, const unsigned char *sk);

/* -- Precomputation interface -- */

int crypto_box_curve25519xchacha20poly1305_beforenm(unsigned char *k, const unsigned char *pk, const unsigned char *sk);

int crypto_box_curve25519xchacha20poly1305_easy_afternm(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n,
                                                        const unsigned char *k);

int crypto_box_curve25519xchacha20poly1305_open_easy_afternm(unsigned char *m, const unsigned char *c, unsigned long long clen,
                                                             const unsigned char *n, const unsigned char *k);

int crypto_box_curve25519xchacha20poly1305_detached_afternm(unsigned char *c, unsigned char *mac, const unsigned char *m, unsigned long long mlen,
                                                            const unsigned char *n, const unsigned char *k);

int crypto_box_curve25519xchacha20poly1305_open_detached_afternm(unsigned char *m, const unsigned char *c, const unsigned char *mac,
                                                                 unsigned long long clen, const unsigned char *n, const unsigned char *k);

/* -- Ephemeral SK interface -- */

#define crypto_box_curve25519xchacha20poly1305_SEALBYTES \
  (crypto_box_curve25519xchacha20poly1305_PUBLICKEYBYTES + crypto_box_curve25519xchacha20poly1305_MACBYTES)

size_t crypto_box_curve25519xchacha20poly1305_sealbytes(void);

int crypto_box_curve25519xchacha20poly1305_seal(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *pk);

int crypto_box_curve25519xchacha20poly1305_seal_open(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *pk,
                                                     const unsigned char *sk);

#endif
