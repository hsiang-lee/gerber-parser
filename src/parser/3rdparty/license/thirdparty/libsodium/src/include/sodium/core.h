
#ifndef sodium_core_H
#define sodium_core_H

int sodium_init(void) ;

int sodium_set_misuse_handler(void (*handler)(void));

void sodium_misuse(void) ;

#endif
