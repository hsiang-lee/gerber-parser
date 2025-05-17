
#ifndef sodium_version_H
#define sodium_version_H

#define SODIUM_VERSION_STRING "1.0.19"

#define SODIUM_LIBRARY_VERSION_MAJOR 26
#define SODIUM_LIBRARY_VERSION_MINOR 1

const char *sodium_version_string(void);

int sodium_library_version_major(void);

int sodium_library_version_minor(void);

int sodium_library_minimal(void);

#endif
