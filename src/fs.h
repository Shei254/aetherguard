#ifndef aetherguard_FS_H
#define aetherguard_FS_H

#include "system.h"

typedef enum {
	DIR_CACHE       = 1 << 0,
	DIR_CONFBASE    = 1 << 1,
	DIR_CONFDIR     = 1 << 2,
	DIR_HOSTS       = 1 << 3,
	DIR_INVITATIONS = 1 << 4,
} aetherguard_dir_t;

// Create one or multiple directories inside aetherguardd configuration directory
extern bool makedirs(aetherguard_dir_t dirs);

// Open file. If it does not exist, create a new file with the specified access mode.
extern FILE *fopenmask(const char *filename, const char *mode, mode_t perms) ATTR_DEALLOCATOR(fclose);

// Get absolute path to a possibly nonexistent file or directory
extern char *absolute_path(const char *path) ATTR_MALLOC;

#endif // aetherguard_FS_H
