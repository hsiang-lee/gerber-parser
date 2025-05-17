
#ifndef randombytes_internal_random_H
#define randombytes_internal_random_H


#include "randombytes.h"




extern struct randombytes_implementation randombytes_internal_implementation;

/* Backwards compatibility with libsodium < 1.0.18 */
#define randombytes_salsa20_implementation randombytes_internal_implementation



#endif
