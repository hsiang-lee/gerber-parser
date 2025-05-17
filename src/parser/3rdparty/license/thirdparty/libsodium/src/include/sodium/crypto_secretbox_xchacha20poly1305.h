#ifndef crypto_secretbox_xchacha20poly1305_H
#define crypto_secretbox_xchacha20poly1305_H

#include <stddef.h>
#include "crypto_stream_xchacha20.h"

#define crypto_secretbox_xchacha20poly1305_KEYBYTES 32U

size_t crypto_secretbox_xchacha20poly1305_keybytes(void);

#define crypto_secretbox_xchacha20poly1305_NONCEBYTES 24U

size_t crypto_secretbox_xchacha20poly1305_noncebytes(void);

#define crypto_secretbox_xchacha20poly1305_MACBYTES 16U

size_t crypto_secretbox_xchacha20poly1305_macbytes(void);

#define crypto_secretbox_xchacha20poly1305_MESSAGEBYTES_MAX (crypto_stream_xchacha20_MESSAGEBYTES_MAX - crypto_secretbox_xchacha20poly1305_MACBYTES)

size_t crypto_secretbox_xchacha20poly1305_messagebytes_max(void);

int crypto_secretbox_xchacha20poly1305_easy(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n,
                                            const unsigned char *k) ;

int crypto_secretbox_xchacha20poly1305_open_easy(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n,
                                                 const unsigned char *k)  ;

int crypto_secretbox_xchacha20poly1305_detached(unsigned char *c, unsigned char *mac, const unsigned char *m, unsigned long long mlen,
                                                const unsigned char *n, const unsigned char *k) ;

int crypto_secretbox_xchacha20poly1305_open_detached(unsigned char *m, const unsigned char *c, const unsigned char *mac, unsigned long long clen,
                                                     const unsigned char *n, const unsigned char *k) 
;

#endif
