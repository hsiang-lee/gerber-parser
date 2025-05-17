#ifndef crypto_secretbox_H
#define crypto_secretbox_H

#include <stddef.h>

#include "crypto_secretbox_xsalsa20poly1305.h"

#define crypto_secretbox_KEYBYTES crypto_secretbox_xsalsa20poly1305_KEYBYTES

size_t crypto_secretbox_keybytes(void);

#define crypto_secretbox_NONCEBYTES crypto_secretbox_xsalsa20poly1305_NONCEBYTES

size_t crypto_secretbox_noncebytes(void);

#define crypto_secretbox_MACBYTES crypto_secretbox_xsalsa20poly1305_MACBYTES

size_t crypto_secretbox_macbytes(void);

#define crypto_secretbox_PRIMITIVE "xsalsa20poly1305"

const char *crypto_secretbox_primitive(void);

#define crypto_secretbox_MESSAGEBYTES_MAX crypto_secretbox_xsalsa20poly1305_MESSAGEBYTES_MAX

size_t crypto_secretbox_messagebytes_max(void);

int crypto_secretbox_easy(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *k)
    ;

int crypto_secretbox_open_easy(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k)
     ;

int crypto_secretbox_detached(unsigned char *c, unsigned char *mac, const unsigned char *m, unsigned long long mlen, const unsigned char *n,
                              const unsigned char *k) ;

int crypto_secretbox_open_detached(unsigned char *m, const unsigned char *c, const unsigned char *mac, unsigned long long clen,
                                   const unsigned char *n, const unsigned char *k) 
;

void crypto_secretbox_keygen(unsigned char k[crypto_secretbox_KEYBYTES]) ;

/* -- NaCl compatibility interface ; Requires padding -- */

#define crypto_secretbox_ZEROBYTES crypto_secretbox_xsalsa20poly1305_ZEROBYTES

size_t crypto_secretbox_zerobytes(void);

#define crypto_secretbox_BOXZEROBYTES crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES

size_t crypto_secretbox_boxzerobytes(void);

int crypto_secretbox(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *k)
    ;

int crypto_secretbox_open(unsigned char *m, const unsigned char *c, unsigned long long clen, const unsigned char *n, const unsigned char *k)
     ;

#endif
