#ifndef crypto_core_ristretto255_H
#define crypto_core_ristretto255_H

#include <stddef.h>

#define crypto_core_ristretto255_BYTES 32

size_t crypto_core_ristretto255_bytes(void);

#define crypto_core_ristretto255_HASHBYTES 64

size_t crypto_core_ristretto255_hashbytes(void);

#define crypto_core_ristretto255_SCALARBYTES 32

size_t crypto_core_ristretto255_scalarbytes(void);

#define crypto_core_ristretto255_NONREDUCEDSCALARBYTES 64

size_t crypto_core_ristretto255_nonreducedscalarbytes(void);

int crypto_core_ristretto255_is_valid_point(const unsigned char *p);

int crypto_core_ristretto255_add(unsigned char *r, const unsigned char *p, const unsigned char *q);

int crypto_core_ristretto255_sub(unsigned char *r, const unsigned char *p, const unsigned char *q);

int crypto_core_ristretto255_from_hash(unsigned char *p, const unsigned char *r);

void crypto_core_ristretto255_random(unsigned char *p);

void crypto_core_ristretto255_scalar_random(unsigned char *r);

int crypto_core_ristretto255_scalar_invert(unsigned char *recip, const unsigned char *s);

void crypto_core_ristretto255_scalar_negate(unsigned char *neg, const unsigned char *s);

void crypto_core_ristretto255_scalar_complement(unsigned char *comp, const unsigned char *s);

void crypto_core_ristretto255_scalar_add(unsigned char *z, const unsigned char *x, const unsigned char *y);

void crypto_core_ristretto255_scalar_sub(unsigned char *z, const unsigned char *x, const unsigned char *y);

void crypto_core_ristretto255_scalar_mul(unsigned char *z, const unsigned char *x, const unsigned char *y);

/*
 * The interval `s` is sampled from should be at least 317 bits to ensure almost
 * uniformity of `r` over `L`.
 */

void crypto_core_ristretto255_scalar_reduce(unsigned char *r, const unsigned char *s);

#endif
