#ifndef crypto_aead_aes256gcm_H
#define crypto_aead_aes256gcm_H

/*
 * WARNING: Despite being the most popular AEAD construction due to its
 * use in TLS, safely using AES-GCM in a different context is tricky.
 *
 * No more than ~ 350 GB of input data should be encrypted with a given key.
 * This is for ~ 16 KB messages -- Actual figures vary according to
 * message sizes.
 *
 * In addition, nonces are short and repeated nonces would totally destroy
 * the security of this scheme.
 *
 * Nonces should thus come from atomic counters, which can be difficult to
 * set up in a distributed environment.
 *
 * Unless you absolutely need AES-GCM, use crypto_aead_xchacha20poly1305_ietf_*()
 * instead. It doesn't have any of these limitations.
 * Or, if you don't need to authenticate additional data, just stick to
 * crypto_secretbox().
 */

#include <stddef.h>
#include <utils.h>

int crypto_aead_aes256gcm_is_available(void);

#define crypto_aead_aes256gcm_KEYBYTES 32U

size_t crypto_aead_aes256gcm_keybytes(void);

#define crypto_aead_aes256gcm_NSECBYTES 0U

size_t crypto_aead_aes256gcm_nsecbytes(void);

#define crypto_aead_aes256gcm_NPUBBYTES 12U

size_t crypto_aead_aes256gcm_npubbytes(void);

#define crypto_aead_aes256gcm_ABYTES 16U

size_t crypto_aead_aes256gcm_abytes(void);

#define crypto_aead_aes256gcm_MESSAGEBYTES_MAX SODIUM_MIN(SODIUM_SIZE_MAX - crypto_aead_aes256gcm_ABYTES, (16ULL * ((1ULL << 32) - 2ULL)))

size_t crypto_aead_aes256gcm_messagebytes_max(void);

typedef struct crypto_aead_aes256gcm_state_ {
  unsigned char opaque[512];
} crypto_aead_aes256gcm_state;

size_t crypto_aead_aes256gcm_statebytes(void);

int crypto_aead_aes256gcm_encrypt(unsigned char *c, unsigned long long *clen_p, const unsigned char *m, unsigned long long mlen,
                                  const unsigned char *ad, unsigned long long adlen, const unsigned char *nsec, const unsigned char *npub,
                                  const unsigned char *k);

int crypto_aead_aes256gcm_decrypt(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec, const unsigned char *c, unsigned long long clen,
                                  const unsigned char *ad, unsigned long long adlen, const unsigned char *npub, const unsigned char *k);

int crypto_aead_aes256gcm_encrypt_detached(unsigned char *c, unsigned char *mac, unsigned long long *maclen_p, const unsigned char *m,
                                           unsigned long long mlen, const unsigned char *ad, unsigned long long adlen, const unsigned char *nsec,
                                           const unsigned char *npub, const unsigned char *k);

int crypto_aead_aes256gcm_decrypt_detached(unsigned char *m, unsigned char *nsec, const unsigned char *c, unsigned long long clen,
                                           const unsigned char *mac, const unsigned char *ad, unsigned long long adlen, const unsigned char *npub,
                                           const unsigned char *k);

/* -- Precomputation interface -- */

int crypto_aead_aes256gcm_beforenm(crypto_aead_aes256gcm_state *ctx_, const unsigned char *k);

int crypto_aead_aes256gcm_encrypt_afternm(unsigned char *c, unsigned long long *clen_p, const unsigned char *m, unsigned long long mlen,
                                          const unsigned char *ad, unsigned long long adlen, const unsigned char *nsec, const unsigned char *npub,
                                          const crypto_aead_aes256gcm_state *ctx_);

int crypto_aead_aes256gcm_decrypt_afternm(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec, const unsigned char *c,
                                          unsigned long long clen, const unsigned char *ad, unsigned long long adlen, const unsigned char *npub,
                                          const crypto_aead_aes256gcm_state *ctx_);

int crypto_aead_aes256gcm_encrypt_detached_afternm(unsigned char *c, unsigned char *mac, unsigned long long *maclen_p, const unsigned char *m,
                                                   unsigned long long mlen, const unsigned char *ad, unsigned long long adlen,
                                                   const unsigned char *nsec, const unsigned char *npub, const crypto_aead_aes256gcm_state *ctx_);

int crypto_aead_aes256gcm_decrypt_detached_afternm(unsigned char *m, unsigned char *nsec, const unsigned char *c, unsigned long long clen,
                                                   const unsigned char *mac, const unsigned char *ad, unsigned long long adlen,
                                                   const unsigned char *npub, const crypto_aead_aes256gcm_state *ctx_);

void crypto_aead_aes256gcm_keygen(unsigned char k[crypto_aead_aes256gcm_KEYBYTES]);

#endif
