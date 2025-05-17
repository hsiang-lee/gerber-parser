#ifndef crypto_verify_16_H
#define crypto_verify_16_H

#include <stddef.h>



#define crypto_verify_16_BYTES 16U

size_t crypto_verify_16_bytes(void);

int crypto_verify_16(const unsigned char *x, const unsigned char *y)  ;



#endif
