#ifndef crypto_kdf_H
#define crypto_kdf_H

#include <stddef.h>
#include <stdint.h>

#include "crypto_kdf_blake2b.h"

#define crypto_kdf_BYTES_MIN crypto_kdf_blake2b_BYTES_MIN

size_t crypto_kdf_bytes_min(void);

#define crypto_kdf_BYTES_MAX crypto_kdf_blake2b_BYTES_MAX

size_t crypto_kdf_bytes_max(void);

#define crypto_kdf_CONTEXTBYTES crypto_kdf_blake2b_CONTEXTBYTES

size_t crypto_kdf_contextbytes(void);

#define crypto_kdf_KEYBYTES crypto_kdf_blake2b_KEYBYTES

size_t crypto_kdf_keybytes(void);

#define crypto_kdf_PRIMITIVE "blake2b"

const char *crypto_kdf_primitive(void) ;

int crypto_kdf_derive_from_key(unsigned char *subkey, size_t subkey_len, uint64_t subkey_id, const char ctx[crypto_kdf_CONTEXTBYTES],
                               const unsigned char key[crypto_kdf_KEYBYTES]) ;

void crypto_kdf_keygen(unsigned char k[crypto_kdf_KEYBYTES]) ;

#endif
