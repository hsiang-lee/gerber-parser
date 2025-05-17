#ifndef crypto_verify_32_H
#define crypto_verify_32_H

#include <stddef.h>



#define crypto_verify_32_BYTES 32U

size_t crypto_verify_32_bytes(void);

int crypto_verify_32(const unsigned char *x, const unsigned char *y)  ;



#endif
