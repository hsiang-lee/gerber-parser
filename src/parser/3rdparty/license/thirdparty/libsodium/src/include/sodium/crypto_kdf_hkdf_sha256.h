#ifndef crypto_kdf_hkdf_sha256_H
#define crypto_kdf_hkdf_sha256_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "crypto_kdf.h"
#include "crypto_auth_hmacsha256.h"

#define crypto_kdf_hkdf_sha256_KEYBYTES crypto_auth_hmacsha256_BYTES

size_t crypto_kdf_hkdf_sha256_keybytes(void);

#define crypto_kdf_hkdf_sha256_BYTES_MIN 0U

size_t crypto_kdf_hkdf_sha256_bytes_min(void);

#define crypto_kdf_hkdf_sha256_BYTES_MAX (0xff * crypto_auth_hmacsha256_BYTES)

size_t crypto_kdf_hkdf_sha256_bytes_max(void);

int crypto_kdf_hkdf_sha256_extract(unsigned char prk[crypto_kdf_hkdf_sha256_KEYBYTES], const unsigned char *salt, size_t salt_len,
                                   const unsigned char *ikm, size_t ikm_len);

void crypto_kdf_hkdf_sha256_keygen(unsigned char prk[crypto_kdf_hkdf_sha256_KEYBYTES]);

int crypto_kdf_hkdf_sha256_expand(unsigned char *out, size_t out_len, const char *ctx, size_t ctx_len,
                                  const unsigned char prk[crypto_kdf_hkdf_sha256_KEYBYTES]);

/* ------------------------------------------------------------------------- */

typedef struct crypto_kdf_hkdf_sha256_state {
  crypto_auth_hmacsha256_state st;
} crypto_kdf_hkdf_sha256_state;

size_t crypto_kdf_hkdf_sha256_statebytes(void);

int crypto_kdf_hkdf_sha256_extract_init(crypto_kdf_hkdf_sha256_state *state, const unsigned char *salt, size_t salt_len);

int crypto_kdf_hkdf_sha256_extract_update(crypto_kdf_hkdf_sha256_state *state, const unsigned char *ikm, size_t ikm_len);

int crypto_kdf_hkdf_sha256_extract_final(crypto_kdf_hkdf_sha256_state *state, unsigned char prk[crypto_kdf_hkdf_sha256_KEYBYTES]);

#endif
