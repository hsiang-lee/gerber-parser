#ifndef crypto_kdf_hkdf_sha512_H
#define crypto_kdf_hkdf_sha512_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "crypto_kdf.h"
#include "crypto_auth_hmacsha512.h"

#define crypto_kdf_hkdf_sha512_KEYBYTES crypto_auth_hmacsha512_BYTES

size_t crypto_kdf_hkdf_sha512_keybytes(void);

#define crypto_kdf_hkdf_sha512_BYTES_MIN 0U

size_t crypto_kdf_hkdf_sha512_bytes_min(void);

#define crypto_kdf_hkdf_sha512_BYTES_MAX (0xff * crypto_auth_hmacsha512_BYTES)

size_t crypto_kdf_hkdf_sha512_bytes_max(void);

int crypto_kdf_hkdf_sha512_extract(unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES], const unsigned char *salt, size_t salt_len,
                                   const unsigned char *ikm, size_t ikm_len) ;

void crypto_kdf_hkdf_sha512_keygen(unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES]) ;

int crypto_kdf_hkdf_sha512_expand(unsigned char *out, size_t out_len, const char *ctx, size_t ctx_len,
                                  const unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES]) ;

/* ------------------------------------------------------------------------- */

typedef struct crypto_kdf_hkdf_sha512_state {
  crypto_auth_hmacsha512_state st;
} crypto_kdf_hkdf_sha512_state;

size_t crypto_kdf_hkdf_sha512_statebytes(void);

int crypto_kdf_hkdf_sha512_extract_init(crypto_kdf_hkdf_sha512_state *state, const unsigned char *salt, size_t salt_len) ;

int crypto_kdf_hkdf_sha512_extract_update(crypto_kdf_hkdf_sha512_state *state, const unsigned char *ikm, size_t ikm_len) ;

int crypto_kdf_hkdf_sha512_extract_final(crypto_kdf_hkdf_sha512_state *state, unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES])
    ;

#endif
