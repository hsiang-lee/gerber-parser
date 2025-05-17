#ifndef crypto_core_salsa2012_H
#define crypto_core_salsa2012_H

#include <stddef.h>




#define crypto_core_salsa2012_OUTPUTBYTES 64U

size_t crypto_core_salsa2012_outputbytes(void);

#define crypto_core_salsa2012_INPUTBYTES 16U

size_t crypto_core_salsa2012_inputbytes(void);

#define crypto_core_salsa2012_KEYBYTES 32U

size_t crypto_core_salsa2012_keybytes(void);

#define crypto_core_salsa2012_CONSTBYTES 16U

size_t crypto_core_salsa2012_constbytes(void);


int crypto_core_salsa2012(unsigned char *out, const unsigned char *in,
                          const unsigned char *k, const unsigned char *c)
            ;



#endif
