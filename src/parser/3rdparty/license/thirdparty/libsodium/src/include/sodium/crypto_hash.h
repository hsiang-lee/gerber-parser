#ifndef crypto_hash_H
#define crypto_hash_H

/*
 * WARNING: Unless you absolutely need to use SHA512 for interoperability,
 * purposes, you might want to consider crypto_generichash() instead.
 * Unlike SHA512, crypto_generichash() is not vulnerable to length
 * extension attacks.
 */

#include <stddef.h>

#include "crypto_hash_sha512.h"

#define crypto_hash_BYTES crypto_hash_sha512_BYTES

size_t crypto_hash_bytes(void);

int crypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen) ;

#define crypto_hash_PRIMITIVE "sha512"

const char *crypto_hash_primitive(void) ;

#endif
