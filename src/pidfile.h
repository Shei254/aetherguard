#ifndef aetherguard_PIDFILE_H
#define aetherguard_PIDFILE_H

#include "system.h"

typedef struct pidfile_t {
	int pid;
	char host[129];
	char port[129];
	char cookie[65];
} pidfile_t;

extern pidfile_t *read_pidfile(void) ATTR_MALLOC;
extern bool write_pidfile(const char *controlcookie, const char *address);

#endif // aetherguard_PIDFILE_H
