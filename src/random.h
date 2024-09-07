#ifndef aetherguard_RANDOM_H
#define aetherguard_RANDOM_H

#include "system.h"

extern void random_init(void);
extern void random_exit(void);
extern void randomize(void *vout, size_t outlen);

#endif // aetherguard_RANDOM_H
