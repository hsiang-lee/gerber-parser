#ifndef crypto_pwhash_argon2id_H
#define crypto_pwhash_argon2id_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <utils.h>

#define crypto_pwhash_argon2id_ALG_ARGON2ID13 2

int crypto_pwhash_argon2id_alg_argon2id13(void);

#define crypto_pwhash_argon2id_BYTES_MIN 16U

size_t crypto_pwhash_argon2id_bytes_min(void);

#define crypto_pwhash_argon2id_BYTES_MAX SODIUM_MIN(SODIUM_SIZE_MAX, 4294967295U)

size_t crypto_pwhash_argon2id_bytes_max(void);

#define crypto_pwhash_argon2id_PASSWD_MIN 0U

size_t crypto_pwhash_argon2id_passwd_min(void);

#define crypto_pwhash_argon2id_PASSWD_MAX 4294967295U

size_t crypto_pwhash_argon2id_passwd_max(void);

#define crypto_pwhash_argon2id_SALTBYTES 16U

size_t crypto_pwhash_argon2id_saltbytes(void);

#define crypto_pwhash_argon2id_STRBYTES 128U

size_t crypto_pwhash_argon2id_strbytes(void);

#define crypto_pwhash_argon2id_STRPREFIX "$argon2id$"

const char *crypto_pwhash_argon2id_strprefix(void);

#define crypto_pwhash_argon2id_OPSLIMIT_MIN 1U

size_t crypto_pwhash_argon2id_opslimit_min(void);

#define crypto_pwhash_argon2id_OPSLIMIT_MAX 4294967295U

size_t crypto_pwhash_argon2id_opslimit_max(void);

#define crypto_pwhash_argon2id_MEMLIMIT_MIN 8192U

size_t crypto_pwhash_argon2id_memlimit_min(void);

#define crypto_pwhash_argon2id_MEMLIMIT_MAX ((SIZE_MAX >= 4398046510080U) ? 4398046510080U : (SIZE_MAX >= 2147483648U) ? 2147483648U : 32768U)

size_t crypto_pwhash_argon2id_memlimit_max(void);

#define crypto_pwhash_argon2id_OPSLIMIT_INTERACTIVE 2U

size_t crypto_pwhash_argon2id_opslimit_interactive(void);

#define crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE 67108864U

size_t crypto_pwhash_argon2id_memlimit_interactive(void);

#define crypto_pwhash_argon2id_OPSLIMIT_MODERATE 3U

size_t crypto_pwhash_argon2id_opslimit_moderate(void);

#define crypto_pwhash_argon2id_MEMLIMIT_MODERATE 268435456U

size_t crypto_pwhash_argon2id_memlimit_moderate(void);

#define crypto_pwhash_argon2id_OPSLIMIT_SENSITIVE 4U

size_t crypto_pwhash_argon2id_opslimit_sensitive(void);

#define crypto_pwhash_argon2id_MEMLIMIT_SENSITIVE 1073741824U

size_t crypto_pwhash_argon2id_memlimit_sensitive(void);

int crypto_pwhash_argon2id(unsigned char *const out, unsigned long long outlen, const char *const passwd, unsigned long long passwdlen,
                           const unsigned char *const salt, unsigned long long opslimit, size_t memlimit, int alg) 
;

int crypto_pwhash_argon2id_str(char out[crypto_pwhash_argon2id_STRBYTES], const char *const passwd, unsigned long long passwdlen,
                               unsigned long long opslimit, size_t memlimit)  ;

int crypto_pwhash_argon2id_str_verify(const char *str, const char *const passwd, unsigned long long passwdlen) 
;

int crypto_pwhash_argon2id_str_needs_rehash(const char *str, unsigned long long opslimit, size_t memlimit) 
;

#endif
