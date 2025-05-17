#ifndef crypto_kdf_blake2b_H
#define crypto_kdf_blake2b_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define crypto_kdf_blake2b_BYTES_MIN 16

size_t crypto_kdf_blake2b_bytes_min(void);

#define crypto_kdf_blake2b_BYTES_MAX 64

size_t crypto_kdf_blake2b_bytes_max(void);

#define crypto_kdf_blake2b_CONTEXTBYTES 8

size_t crypto_kdf_blake2b_contextbytes(void);

#define crypto_kdf_blake2b_KEYBYTES 32

size_t crypto_kdf_blake2b_keybytes(void);

int crypto_kdf_blake2b_derive_from_key(unsigned char *subkey, size_t subkey_len, uint64_t subkey_id, const char ctx[crypto_kdf_blake2b_CONTEXTBYTES],
                                       const unsigned char key[crypto_kdf_blake2b_KEYBYTES]) ;

#endif
