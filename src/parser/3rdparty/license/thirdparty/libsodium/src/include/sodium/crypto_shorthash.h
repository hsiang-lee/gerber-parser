#ifndef crypto_shorthash_H
#define crypto_shorthash_H

#include <stddef.h>

#include "crypto_shorthash_siphash24.h"

#define crypto_shorthash_BYTES crypto_shorthash_siphash24_BYTES

size_t crypto_shorthash_bytes(void);

#define crypto_shorthash_KEYBYTES crypto_shorthash_siphash24_KEYBYTES

size_t crypto_shorthash_keybytes(void);

#define crypto_shorthash_PRIMITIVE "siphash24"

const char *crypto_shorthash_primitive(void);

int crypto_shorthash(unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *k) ;

void crypto_shorthash_keygen(unsigned char k[crypto_shorthash_KEYBYTES]) ;

#endif
