#ifndef aetherguard_INVITATION_H
#define aetherguard_INVITATION_H

/*
    invitation.h -- header for invitation.c.
    Copyright (C) 2013-2022 Guus Sliepen <guus@aetherguard-vpn.org>

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

int cmd_invite(int argc, char *argv[]);
int cmd_join(int argc, char *argv[]);

// Wait until data can be read from socket, or a timeout occurs.
// true if socket is ready, false on timeout.
bool wait_socket_recv(int fd);

#endif
