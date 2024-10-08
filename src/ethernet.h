#ifndef aetherguard_ETHERNET_H
#define aetherguard_ETHERNET_H

/*
    ethernet.h -- missing Ethernet related definitions
    Copyright (C) 2005 Ivo Timmermans
                  2006 Guus Sliepen <guus@aetherguard-vpn.org>

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

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#ifndef ETH_HLEN
#define ETH_HLEN 14
#endif

#ifndef ETHER_TYPE_LEN
#define ETHER_TYPE_LEN 2
#endif


#ifndef ARPHRD_ETHER
#define ARPHRD_ETHER 1
#endif

#ifndef ETH_P_IP
#define ETH_P_IP 0x0800
#endif

#ifndef ETH_P_ARP
#define ETH_P_ARP 0x0806
#endif

#ifndef ETH_P_IPV6
#define ETH_P_IPV6 0x86DD
#endif

#ifndef ETH_P_8021Q
#define ETH_P_8021Q 0x8100
#endif

#ifndef ETH_P_MAX
#define ETH_P_MAX 0xFFFF
#endif

#ifndef HAVE_STRUCT_ETHER_HEADER
PACKED(struct ether_header {
	uint8_t ether_dhost[ETH_ALEN];
	uint8_t ether_shost[ETH_ALEN];
	uint16_t ether_type;
});
#endif

STATIC_ASSERT(sizeof(struct ether_header) == 14, "ether_header has incorrect size");

#ifndef HAVE_STRUCT_ARPHDR
PACKED(struct arphdr {
	uint16_t ar_hrd;
	uint16_t ar_pro;
	uint8_t ar_hln;
	uint8_t ar_pln;
	uint16_t ar_op;
});
#define ARPOP_REQUEST 1
#define ARPOP_REPLY 2
#define ARPOP_RREQUEST 3
#define ARPOP_RREPLY 4
#define ARPOP_InREQUEST 8
#define ARPOP_InREPLY 9
#define ARPOP_NAK 10
#endif

STATIC_ASSERT(sizeof(struct arphdr) == 8, "arphdr has incorrect size");

#ifndef HAVE_STRUCT_ETHER_ARP
PACKED(struct ether_arp {
	struct  arphdr ea_hdr;
	uint8_t arp_sha[ETH_ALEN];
	uint8_t arp_spa[4];
	uint8_t arp_tha[ETH_ALEN];
	uint8_t arp_tpa[4];
});
#define arp_hrd ea_hdr.ar_hrd
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op ea_hdr.ar_op
#endif

STATIC_ASSERT(sizeof(struct ether_arp) == 28, "ether_arp has incorrect size");

#endif
