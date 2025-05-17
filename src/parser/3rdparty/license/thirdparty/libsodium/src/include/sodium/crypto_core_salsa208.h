#ifndef crypto_core_salsa208_H
#define crypto_core_salsa208_H

#include <stddef.h>

#define crypto_core_salsa208_CONSTBYTES 16U

int crypto_core_salsa208(unsigned char *out, const unsigned char *in, const unsigned char *k, const unsigned char *c);

#endif
