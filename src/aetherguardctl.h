#ifndef aetherguard_aetherguardCTL_H
#define aetherguard_aetherguardCTL_H

/*
    aetherguardctl.h -- header for aetherguardctl.c.
    Copyright (C) 2011-2022 Guus Sliepen <guus@aetherguard-vpn.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "system.h"
#include "ecdsa.h"

extern bool tty;
extern bool force;
extern char line[4096];
extern int fd;
extern char buffer[4096];
extern size_t blen;
extern bool confbasegiven;
extern char *aetherguard_conf;
extern char *hosts_dir;

#define VAR_SERVER 1    /* Should be in aetherguard.conf */
#define VAR_HOST 2      /* Can be in host config file */
#define VAR_MULTIPLE 4  /* Multiple statements allowed */
#define VAR_OBSOLETE 8  /* Should not be used anymore */
#define VAR_SAFE 16     /* Variable is safe when accepting invitations */

typedef struct {
	const char *name;
	int type;
} var_t;

extern const var_t variables[];

extern size_t rstrip(char *value);
extern char *get_my_name(bool verbose) ATTR_MALLOC;
extern bool connect_aetherguardd(bool verbose);
extern bool sendline(int fd, const char *format, ...) ATTR_FORMAT(printf, 2, 3);
extern bool recvline(int fd, char *line, size_t len);
extern int check_port(const char *name);

#endif
