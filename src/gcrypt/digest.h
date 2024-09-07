#ifndef aetherguard_GCRYPT_DIGEST_H
#define aetherguard_GCRYPT_DIGEST_H

/*
    digest.h -- header file digest.c
    Copyright (C) 2007-2022 Guus Sliepen <guus@aetherguard-vpn.org>

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

#include <gcrypt.h>

#include "../legacy.h"

typedef enum gcry_md_algos md_algo_t;

typedef struct digest {
	md_algo_t algo;
	nid_t nid;
	size_t maclength;
	gcry_md_hd_t hmac;
} digest_t;

#endif
