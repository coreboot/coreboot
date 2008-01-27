/*
 * This software and ancillary information (herein called SOFTWARE )
 * called LinuxBIOS          is made available under the terms described
 * here.  The SOFTWARE has been approved for release with associated
 * LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
 * been authored by an employee or employees of the University of
 * California, operator of the Los Alamos National Laboratory under
 * Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
 * U.S. Government has rights to use, reproduce, and distribute this
 * SOFTWARE.  The public may copy, distribute, prepare derivative works
 * and publicly display this SOFTWARE without charge, provided that this
 * Notice and any statement of authorship are reproduced on all copies.
 * Neither the Government nor the University makes any warranty, express
 * or implied, or assumes any liability or responsibility for the use of
 * this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
 * such modified SOFTWARE should be clearly marked, so as not to confuse
 * it with the version available from LANL.
 */
 /*
 * This file is part of the coreboot project.
 *
 *  (c) Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef PCI_BIOS_H
#define PCI_BIOS_H

enum {
	PCI_BIOS_PRESENT	= 0xB101,
	FIND_PCI_DEVICE		= 0xB102,
	FIND_PCI_CLASS_CODE	= 0xB103,
	GENERATE_SPECIAL_CYCLE	= 0xB106,
	READ_CONFIG_BYTE	= 0xB108,
	READ_CONFIG_WORD	= 0xB109,
	READ_CONFIG_DWORD	= 0xB10A,
	WRITE_CONFIG_BYTE	= 0xB10B,
	WRITE_CONFIG_WORD	= 0xB10C,
	WRITE_CONFIG_DWORD	= 0xB10D,
	GET_IRQ_ROUTING_OPTIONS	= 0xB10E,
	SET_PCI_IRQ		= 0xB10F
};

enum {
	SUCCESSFUL		= 0x00,
	FUNC_NOT_SUPPORTED	= 0x81,
	BAD_VENDOR_ID		= 0x83,
	DEVICE_NOT_FOUND	= 0x86,
	BAD_REGISTER_NUMBER	= 0x87,
	SET_FAILED		= 0x88,
	BUFFER_TOO_SMALL	= 0x89
};

int pcibios_handler(void);

#endif /* PCI_BIOS_H */

