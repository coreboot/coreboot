/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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

/*
 * Datasheet:
 *   - Name: Intel 440BX AGPset: 82443BX Host Bridge/Controller
 *   - URL: http://www.intel.com/design/chipsets/datashts/290633.htm
 *   - PDF: http://www.intel.com/design/chipsets/datashts/29063301.pdf
 *   - Order Number: 290633-001
 */

/*
 * Host-to-PCI Bridge Registers.
 * The values in parenthesis are the default values as per datasheet.
 * Any addresses between 0x00 and 0xff not listed below are either
 * Reserved or Intel Reserved and should not be touched.
 */
#define VID     0x00 /* Vendor Identification (0x8086). */
#define DID     0x02 /* Device Identification (0x7190/0x7192). */
#define PCICMD  0x04 /* PCI Command Register (0x006). */
#define PCISTS  0x06 /* PCI Status Register (0x0210/0x0200). */
#define RID     0x08 /* Revision Identification (0x00/0x01/0x02). */
#define SUBC    0x0a /* Sub-Class Code (0x00). */
#define BCC     0x0b /* Base Class Code (0x06). */
#define MLT     0x0d /* Master Latency Timer (0x00). */
#define HDR     0x0e /* Header Type (0x00). */
#define APBASE  0x10 /* Aperture Base Configuration (0x00000008). */
#define SVID    0x2c /* Subsystem Vendor Identification (0x0000). */
#define SID     0x2e /* Subsystem Identification (0x0000). */
#define CAPPTR  0x34 /* Capabilities Pointer (0xa0/0x00). */
#define NBXCFG  0x50 /* 440BX Configuration (0x0000:00S0_0000_000S_0S00b). */
#define DRAMC   0x57 /* DRAM Control (00S0_0000b). */
#define DRAMT   0x58 /* DRAM Timing (0x03). */
#define PAM     0x59 /* Programmable Attribute Map, 7 registers (0x00). */
#define DRB     0x60 /* DRAM Row Boundary, 8 registers (0x01). */
#define FDHC    0x68 /* Fixed SDRAM Hole Control (0x00). */
#define MBSC    0x69 /* Memory Buffer Strength Control (0x0000-0000-0000). */
#define SMRAM   0x72 /* System Management RAM Control (0x02). */
#define ESMRAMC 0x73 /* Extended System Management RAM Control (0x38). */
#define RPS     0x74 /* SDRAM Row Page Size (0x0000). */
#define SDRAMC  0x76 /* SDRAM Control Register (0x0000). */
#define PGPOL   0x78 /* Paging Policy Register (0x00). */
#define PMCR    0x7a /* Power Management Control Register (0000_S0S0b). */
#define SCRR    0x7b /* Suspend CBR Refresh Rate Register (0x0038). */
#define EAP     0x80 /* Error Address Pointer Register (0x00000000). */
#define ERRCMD  0x90 /* Error Command Register (0x80). */
#define ERRSTS  0x91 /* Error Status (0x0000). */
// TODO: AGP stuff.
#define MBFS    0xca /* Memory Buffer Frequency Select (0x000000). */
#define BSPAD   0xd0 /* BIOS Scratch Pad (0x000..000). */
#define DWTC    0xe0 /* DRAM Write Thermal Throttling Control (0x000..000). */
#define DRTC    0xe8 /* DRAM Read Thermal Throttling Control (0x000..000). */
#define BUFFC   0xf0 /* Buffer Control Register (0x0000). */

/* For convenience: */
#define DRB0    0x60
#define DRB1    0x61
#define DRB2    0x62
#define DRB3    0x63
#define DRB4    0x64
#define DRB5    0x65
#define DRB6    0x66
#define DRB7    0x67

