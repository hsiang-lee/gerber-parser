#ifndef crypto_aead_aegis128l_H
#define crypto_aead_aegis128l_H

#include <stddef.h>
#include <utils.h>

#define crypto_aead_aegis128l_KEYBYTES 16U

size_t crypto_aead_aegis128l_keybytes(void);

#define crypto_aead_aegis128l_NSECBYTES 0U

size_t crypto_aead_aegis128l_nsecbytes(void);

#define crypto_aead_aegis128l_NPUBBYTES 16U

size_t crypto_aead_aegis128l_npubbytes(void);

#define crypto_aead_aegis128l_ABYTES 32U

size_t crypto_aead_aegis128l_abytes(void);

#define crypto_aead_aegis128l_MESSAGEBYTES_MAX SODIUM_MIN(SODIUM_SIZE_MAX - crypto_aead_aegis128l_ABYTES, (1ULL << 61) - 1)

size_t crypto_aead_aegis128l_messagebytes_max(void);

int crypto_aead_aegis128l_encrypt(unsigned char *c, unsigned long long *clen_p, const unsigned char *m, unsigned long long mlen,
                                  const unsigned char *ad, unsigned long long adlen, const unsigned char *nsec, const unsigned char *npub,
                                  const unsigned char *k);

int crypto_aead_aegis128l_decrypt(unsigned char *m, unsigned long long *mlen_p, unsigned char *nsec, const unsigned char *c, unsigned long long clen,
                                  const unsigned char *ad, unsigned long long adlen, const unsigned char *npub, const unsigned char *k);

int crypto_aead_aegis128l_encrypt_detached(unsigned char *c, unsigned char *mac, unsigned long long *maclen_p, const unsigned char *m,
                                           unsigned long long mlen, const unsigned char *ad, unsigned long long adlen, const unsigned char *nsec,
                                           const unsigned char *npub, const unsigned char *k);

int crypto_aead_aegis128l_decrypt_detached(unsigned char *m, unsigned char *nsec, const unsigned char *c, unsigned long long clen,
                                           const unsigned char *mac, const unsigned char *ad, unsigned long long adlen, const unsigned char *npub,
                                           const unsigned char *k);

void crypto_aead_aegis128l_keygen(unsigned char k[crypto_aead_aegis128l_KEYBYTES]);

#endif
