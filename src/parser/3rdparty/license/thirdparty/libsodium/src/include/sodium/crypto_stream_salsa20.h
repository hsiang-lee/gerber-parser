#ifndef crypto_stream_salsa20_H
#define crypto_stream_salsa20_H

/*
 *  WARNING: This is just a stream cipher. It is NOT authenticated encryption.
 *  While it provides some protection against eavesdropping, it does NOT
 *  provide any security against active attacks.
 *  Unless you know what you're doing, what you are looking for is probably
 *  the crypto_box functions.
 */

#include <stddef.h>
#include <stdint.h>
#include <utils.h>

#define crypto_stream_salsa20_KEYBYTES 32U

size_t crypto_stream_salsa20_keybytes(void);

#define crypto_stream_salsa20_NONCEBYTES 8U

size_t crypto_stream_salsa20_noncebytes(void);

#define crypto_stream_salsa20_MESSAGEBYTES_MAX SODIUM_SIZE_MAX

size_t crypto_stream_salsa20_messagebytes_max(void);

int crypto_stream_salsa20(unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k) ;

int crypto_stream_salsa20_xor(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *k)
    ;

int crypto_stream_salsa20_xor_ic(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, uint64_t ic,
                                 const unsigned char *k) ;

void crypto_stream_salsa20_keygen(unsigned char k[crypto_stream_salsa20_KEYBYTES]) ;

#endif
