/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _BAYTRAIL_IOSF_H_
#define _BAYTRAIL_IOSF_H_

#include <stdint.h>
#include <baytrail/pci_devs.h>

/*
 * The Bay Trail SoC has a message network called IOSF Sideband. The access
 * routines are through 3 registers in PCI config space of 00:00.0:
 *  MCR - control register
 *  MDR - data register
 *  MCRX - control register extension
 * The extension regist is only used for addresses that don't fit into the
 * 8 bit register address.
 */

#ifndef PCI_DEV
#define PCI_DEV(SEGBUS, DEV, FN) ( \
        (((SEGBUS) & 0xFFF) << 20) | \
        (((DEV) & 0x1F) << 15) | \
        (((FN)  & 0x07) << 12))
#endif
#define IOSF_PCI_DEV PCI_DEV(0,SOC_DEV,SOC_FUNC)

#define MCR_REG 0xd0
#define  IOSF_OPCODE(x) ((x) << 24)
#define  IOSF_PORT(x) ((0xff & (x)) << 16)
#define  IOSF_REG(x) ((0xff & (x)) << 8)
#define  IOSF_BYTE_EN 0xf0
#define MDR_REG 0xd4
#define MCRX_REG 0xd8

uint32_t iosf_bunit_read(int reg);
void iosf_bunit_write(int reg, uint32_t val);

/* IOSF ports. */
#define IOSF_PORT_AUNIT		0x00 /* IO Arbiter unit */
#define IOSF_PORT_SYSMEMC	0x01 /* System Memory Controller */
#define IOSF_PORT_CPU_BUS	0x02 /* CPU Bus Interface Controller */
#define IOSF_PORT_BUNIT		0x03 /* System Memroy Arbiter/Bunit */
#define IOSF_PORT_PMC		0x04 /* Power Management Controller */
#define IOSF_PORT_GFX		0x06 /* Graphics Adapter */
#define IOSF_PORT_SYSMEMIO	0x0c /* System Memory IO */
#define IOSF_PORT_USBPHY	0x43 /* USB PHY */
#define IOSF_PORT_SATAPHY	0xa3 /* SATA PHY */
#define IOSF_PORT_PCIEPHY	0xa3 /* PCIE PHY */

/* Read and write opcodes differ per port. */
#define IOSF_OP_READ_AUNIT	0x10
#define IOSF_OP_WRITE_AUNIT	(IOSF_OP_READ_AUNIT | 1)
#define IOSF_OP_READ_SYSMEMC	0x10
#define IOSF_OP_WRITE_SYSMEMC	(IOSF_OP_READ_SYSMEMC | 1)
#define IOSF_OP_READ_CPU_BUS	0x10
#define IOSF_OP_WRITE_CPU_BUS	(IOSF_OP_READ_CPU_BUS | 1)
#define IOSF_OP_READ_BUNIT	0x10
#define IOSF_OP_WRITE_BUNIT	(IOSF_OP_READ_BUNIT | 1)
#define IOSF_OP_READ_PMC	0x06
#define IOSF_OP_WRITE_PMC	(IOSF_OP_READ_PMC | 1)
#define IOSF_OP_READ_GFX	0x00
#define IOSF_OP_WRITE_GFX	(IOSF_OP_READ_GFX | 1)
#define IOSF_OP_READ_SYSMEMIO	0x06
#define IOSF_OP_WRITE_SYSMEMIO	(IOSF_OP_READ_SYSMEMIO | 1)
#define IOSF_OP_READ_USBPHY	0x06
#define IOSF_OP_WRITE_USBPHY	(IOSF_OP_READ_USBPHY | 1)
#define IOSF_OP_READ_SATAPHY	0x00
#define IOSF_OP_WRITE_SATAPHY	(IOSF_OP_READ_SATAPHY | 1)
#define IOSF_OP_READ_PCIEPHY	0x00
#define IOSF_OP_WRITE_PCIEPHY	(IOSF_OP_READ_PCIEPHY | 1)


/*
 * BUNIT Registers.
 */

#define BNOCACHE		0x23
/* BMBOUND has a 128MiB granularity. Highest address is 0xf8000000. */
#define BUNIT_BMBOUND		0x25
/* BMBOUND_HI describes the available ram above 4GiB. It has a
 * 256MiB granularity. Physical address bits 35:28 are compared with 31:24
 * bits in the BMBOUND_HI register. Also note that since BMBOUND has 128MiB
 * granularity care needs to be taken with the e820 map to account for a hole
 * in the ram. */
#define BUNIT_BMBOUND_HI	0x26
#define BUNIT_MMCONF_REG	0x27
/* The SMMRR registers define the SMM region in MiB granularity. */
#define BUNIT_SMRRL		0x2e
#define BUNIT_SMRRH		0x2f
# define BUNIT_SMRR_ENABLE	(1 << 31)



#endif /* _BAYTRAIL_IOSF_H_ */
