/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _CPC710_PCI_H_
#define _CPC710_PCI_H_

#define CPC710_PCI32_CONFIG		(CONFIG_PCIC0_CFGADDR & 0xfff00000)
#define CPC710_PCI32_MEM_SIZE		0xf8000000
#define CPC710_PCI32_MEM_BASE		0xc0000000
#define CPC710_PCI32_IO_SIZE		0xf8000000
#define CPC710_PCI32CONFIG_IO_BASE		0x80000000

//#define CPC710_PCI64_CONFIG		0xff400000
//#define CPC710_PCI64_MEM_SIZE		0xf8000000
//#define CPC710_PCI64_MEM_BASE		0xc8000000
//#define CPC710_PCI64_IO_SIZE		0xf8000000
//#define CPC710_PCI64CONFIG_IO_BASE		0x88000000

#define CPC710_PCIL0_PSEA		0xf6110
#define CPC710_PCIL0_PCIDG		0xf6120
#define CPC710_PCIL0_INTACK		0xf7700
#define CPC710_PCIL0_PIBAR		0xf7800
#define CPC710_PCIL0_PMBAR		0xf7810
#define CPC710_PCIL0_CRR		0xf7ef0
#define CPC710_PCIL0_PR			0xf7f20
#define CPC710_PCIL0_ACR		0xf7f30
#define CPC710_PCIL0_MSIZE		0xf7f40
#define CPC710_PCIL0_IOSIZE		0xf7f60
#define CPC710_PCIL0_SMBAR		0xf7f80
#define CPC710_PCIL0_SIBAR		0xf7fc0
#define CPC710_PCIL0_CTLRW		0xf7fd0
#define CPC710_PCIL0_CFGADDR		0xf8000	/* little endian */
#define CPC710_PCIL0_CFGDATA		0xf8010	/* little endian */
#define CPC710_PCIL0_PSSIZE		0xf8100
#define CPC710_PCIL0_BARPS		0xf8120
#define CPC710_PCIL0_PSBAR		0xf8140
#define CPC710_PCIL0_BPMDLK		0xf8200
#define CPC710_PCIL0_TPMDLK		0xf8210
#define CPC710_PCIL0_BIODLK		0xf8220
#define CPC710_PCIL0_TIODLK		0xf8230
#define CPC710_PCIL0_INTSET		0xf8310


#endif
