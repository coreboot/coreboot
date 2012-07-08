#ifndef __IPMI_KCS_H
#define __IPMI_KCS_H
/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#define IPMI_NETFN_CHASSIS 0x00
#define IPMI_NETFN_BRIDGE 0x02
#define IPMI_NETFN_SENSOREVENT 0x04
#define IPMI_NETFN_APPLICATION 0x06
#define IPMI_NETFN_FIRMWARE 0x08
#define IPMI_NETFN_STORAGE 0x0a
#define IPMI_NETFN_TRANSPORT 0x0c

#define IPMI_CMD_ACPI_POWERON 0x06

extern int ipmi_kcs_message(int port, int netfn, int lun, int cmd,
			    const unsigned char *inmsg, int inlen,
			    unsigned char *outmsg, int outlen);
#endif
