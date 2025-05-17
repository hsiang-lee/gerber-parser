#ifndef crypto_core_hchacha20_H
#define crypto_core_hchacha20_H

#include <stddef.h>

#define crypto_core_hchacha20_OUTPUTBYTES 32U

size_t crypto_core_hchacha20_outputbytes(void);

#define crypto_core_hchacha20_INPUTBYTES 16U

size_t crypto_core_hchacha20_inputbytes(void);

#define crypto_core_hchacha20_KEYBYTES 32U

size_t crypto_core_hchacha20_keybytes(void);

#define crypto_core_hchacha20_CONSTBYTES 16U

size_t crypto_core_hchacha20_constbytes(void);

int crypto_core_hchacha20(unsigned char *out, const unsigned char *in, const unsigned char *k, const unsigned char *c);

#endif
