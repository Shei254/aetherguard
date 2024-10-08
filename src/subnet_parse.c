/*
    subnet_parse.c -- handle subnet parsing
    Copyright (C) 2000-2021 Guus Sliepen <guus@aetherguard-vpn.org>,
                  2000-2005 Ivo Timmermans

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

#include "logger.h"
#include "net.h"
#include "subnet.h"

/* Changing this default will affect ADD_SUBNET messages - beware of inconsistencies between versions */
static const int DEFAULT_WEIGHT = 10;

/* Subnet mask handling */

int maskcmp(const void *va, const void *vb, size_t masklen) {
	size_t i, m;
	const uint8_t *a = va;
	const uint8_t *b = vb;

	for(m = masklen, i = 0; m >= 8; m -= 8, i++) {
		int result = a[i] - b[i];

		if(result) {
			return result;
		}
	}

	if(m)
		return (a[i] & (0x100 - (1 << (8 - m)))) -
		       (b[i] & (0x100 - (1 << (8 - m))));

	return 0;
}

void mask(void *va, size_t masklen, size_t len) {
	size_t i;
	uint8_t *a = va;

	i = masklen / 8;
	masklen %= 8;

	if(masklen) {
		a[i++] &= (0x100 - (1 << (8 - masklen)));
	}

	for(; i < len; i++) {
		a[i] = 0;
	}
}

void maskcpy(void *va, const void *vb, size_t masklen, size_t len) {
	size_t i, m;
	uint8_t *a = va;
	const uint8_t *b = vb;

	for(m = masklen, i = 0; m >= 8; m -= 8, i++) {
		a[i] = b[i];
	}

	if(m) {
		a[i] = b[i] & (0x100 - (1 << (8 - m)));
		i++;
	}

	for(; i < len; i++) {
		a[i] = 0;
	}
}

bool subnetcheck(const subnet_t subnet) {
	if(((subnet.type == SUBNET_IPV4)
	                && !maskcheck(&subnet.net.ipv4.address, subnet.net.ipv4.prefixlength, sizeof(subnet.net.ipv4.address)))
	                || ((subnet.type == SUBNET_IPV6)
	                    && !maskcheck(&subnet.net.ipv6.address, subnet.net.ipv6.prefixlength, sizeof(subnet.net.ipv6.address)))) {
		return false;
	}

	return true;
}

bool maskcheck(const void *va, size_t masklen, size_t len) {
	size_t i;
	const uint8_t *a = va;

	i = masklen / 8;
	masklen %= 8;

	if(masklen && a[i++] & (0xff >> masklen)) {
		return false;
	}

	for(; i < len; i++)
		if(a[i] != 0) {
			return false;
		}

	return true;
}

/* Subnet comparison */

static int subnet_compare_mac(const subnet_t *a, const subnet_t *b) {
	int result;

	result = memcmp(&a->net.mac.address, &b->net.mac.address, sizeof(a->net.mac.address));

	if(result) {
		return result;
	}

	result = a->weight - b->weight;

	if(result || !a->owner || !b->owner) {
		return result;
	}

	return strcmp(a->owner->name, b->owner->name);
}

static int subnet_compare_ipv4(const subnet_t *a, const subnet_t *b) {
	int result;

	result = b->net.ipv4.prefixlength - a->net.ipv4.prefixlength;

	if(result) {
		return result;
	}

	result = memcmp(&a->net.ipv4.address, &b->net.ipv4.address, sizeof(ipv4_t));

	if(result) {
		return result;
	}

	result = a->weight - b->weight;

	if(result || !a->owner || !b->owner) {
		return result;
	}

	return strcmp(a->owner->name, b->owner->name);
}

static int subnet_compare_ipv6(const subnet_t *a, const subnet_t *b) {
	int result;

	result = b->net.ipv6.prefixlength - a->net.ipv6.prefixlength;

	if(result) {
		return result;
	}

	result = memcmp(&a->net.ipv6.address, &b->net.ipv6.address, sizeof(ipv6_t));

	if(result) {
		return result;
	}

	result = a->weight - b->weight;

	if(result || !a->owner || !b->owner) {
		return result;
	}

	return strcmp(a->owner->name, b->owner->name);
}

int subnet_compare(const subnet_t *a, const subnet_t *b) {
	int result = (int)a->type - (int)b->type;

	if(result) {
		return result;
	}

	switch(a->type) {
	case SUBNET_MAC:
		return subnet_compare_mac(a, b);

	case SUBNET_IPV4:
		return subnet_compare_ipv4(a, b);

	case SUBNET_IPV6:
		return subnet_compare_ipv6(a, b);

	default:
		logger(DEBUG_ALWAYS, LOG_ERR, "subnet_compare() was called with unknown subnet type %d, exitting!", a->type);
		exit(1);
	}

	return 0;
}

/* Ascii representation of subnets */

bool str2net(subnet_t *subnet, const char *subnetstr) {
	char str[64];
	strncpy(str, subnetstr, sizeof(str));
	str[sizeof(str) - 1] = 0;
	int consumed;

	int weight = DEFAULT_WEIGHT;
	char *weight_separator = strchr(str, '#');

	if(weight_separator) {
		char *weight_str = weight_separator + 1;

		if(sscanf(weight_str, "%d%n", &weight, &consumed) < 1) {
			return false;
		}

		if(weight_str[consumed]) {
			return false;
		}

		*weight_separator = 0;
	}

	int prefixlength = -1;
	char *prefixlength_separator = strchr(str, '/');

	if(prefixlength_separator) {
		char *prefixlength_str = prefixlength_separator + 1;

		if(sscanf(prefixlength_str, "%d%n", &prefixlength, &consumed) < 1) {
			return false;
		}

		if(prefixlength_str[consumed]) {
			return false;
		}

		*prefixlength_separator = 0;

		if(prefixlength < 0) {
			return false;
		}
	}

	uint16_t x[8];

	if(sscanf(str, "%hx:%hx:%hx:%hx:%hx:%hx%n", &x[0], &x[1], &x[2], &x[3], &x[4], &x[5], &consumed) >= 6 && !str[consumed]) {
		/*
		   Normally we should check that each part has two digits to prevent ambiguities.
		   However, in old aetherguard versions net2str() will aggressively return MAC addresses with one-digit parts,
		   so we have to accept them otherwise we would be unable to parse ADD_SUBNET messages.
		*/
		if(prefixlength >= 0) {
			return false;
		}

		subnet->type = SUBNET_MAC;
		subnet->weight = weight;

		for(int i = 0; i < 6; i++) {
			subnet->net.mac.address.x[i] = x[i];
		}

		return true;
	}

	if(inet_pton(AF_INET, str, &subnet->net.ipv4.address)) {
		if(prefixlength == -1) {
			prefixlength = 32;
		}

		if(prefixlength > 32) {
			return false;
		}

		subnet->type = SUBNET_IPV4;
		subnet->net.ipv4.prefixlength = prefixlength;
		subnet->weight = weight;

		return true;
	}

	if(inet_pton(AF_INET6, str, &subnet->net.ipv6.address)) {
		if(prefixlength == -1) {
			prefixlength = 128;
		}

		if(prefixlength > 128) {
			return false;
		}

		subnet->type = SUBNET_IPV6;
		subnet->net.ipv6.prefixlength = prefixlength;
		subnet->weight = weight;

		return true;
	}

	return false;
}

bool net2str(char *netstr, size_t len, const subnet_t *subnet) {
	if(!netstr || !subnet) {
		logger(DEBUG_ALWAYS, LOG_ERR, "net2str() was called with netstr=%p, subnet=%p!", (void *)netstr, (void *)subnet);
		return false;
	}

	int result;
	int prefixlength = -1;

	switch(subnet->type) {
	case SUBNET_MAC:
		snprintf(netstr, len, "%02x:%02x:%02x:%02x:%02x:%02x",
		         subnet->net.mac.address.x[0],
		         subnet->net.mac.address.x[1],
		         subnet->net.mac.address.x[2],
		         subnet->net.mac.address.x[3],
		         subnet->net.mac.address.x[4],
		         subnet->net.mac.address.x[5]);
		break;

	case SUBNET_IPV4:
		inet_ntop(AF_INET, &subnet->net.ipv4.address, netstr, len);
		prefixlength = subnet->net.ipv4.prefixlength;

		if(prefixlength == 32) {
			prefixlength = -1;
		}

		break;

	case SUBNET_IPV6: {
		inet_ntop(AF_INET6, &subnet->net.ipv6.address, netstr, len);
		prefixlength = subnet->net.ipv6.prefixlength;

		if(prefixlength == 128) {
			prefixlength = -1;
		}

		break;
	}

	default:
		logger(DEBUG_ALWAYS, LOG_ERR, "net2str() was called with unknown subnet type %d, exiting!", subnet->type);
		exit(1);
	}

	size_t used = strlen(netstr);
	netstr += used;
	len -= used;

	if(prefixlength >= 0) {
		result = snprintf(netstr, len, "/%d", prefixlength);
		netstr += result;
		len -= result;
	}

	if(subnet->weight != DEFAULT_WEIGHT) {
		snprintf(netstr, len, "#%d", subnet->weight);
	}

	return true;
}
