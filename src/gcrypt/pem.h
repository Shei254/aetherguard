#ifndef aetherguard_GCRYPT_PEM_H
#define aetherguard_GCRYPT_PEM_H

/*
    pem.h -- PEM encoding and decoding
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

#include "../system.h"

bool pem_decode(FILE *fp, const char *header, uint8_t *buf, size_t size, size_t *outsize);
bool pem_encode(FILE *fp, const char *header, uint8_t *buf, size_t size);

#endif // aetherguard_GCRYPT_PEM_H
