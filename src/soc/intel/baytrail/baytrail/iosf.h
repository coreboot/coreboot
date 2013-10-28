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
#define  IOSF_BYTE_EN_0 0x10
#define  IOSF_BYTE_EN_1 0x20
#define  IOSF_BYTE_EN_2 0x40
#define  IOSF_BYTE_EN_3 0x80
#define  IOSF_BYTE_EN \
	  (IOSF_BYTE_EN_0 | IOSF_BYTE_EN_1 | IOSF_BYTE_EN_2 | IOSF_BYTE_EN_3)
#define MDR_REG 0xd4
#define MCRX_REG 0xd8

uint32_t iosf_bunit_read(int reg);
void iosf_bunit_write(int reg, uint32_t val);
uint32_t iosf_dunit_read(int reg);
void iosf_dunit_write(int reg, uint32_t val);
/* Some registers are per channel while the gloals live in dunit 0 */
uint32_t iosf_dunit_ch0_read(int reg);
uint32_t iosf_dunit_ch1_read(int reg);
uint32_t iosf_punit_read(int reg);
void iosf_punit_write(int reg, uint32_t val);

/* IOSF ports. */
#define IOSF_PORT_AUNIT		0x00 /* IO Arbiter unit */
#define IOSF_PORT_SYSMEMC	0x01 /* System Memory Controller */
#define IOSF_PORT_DUNIT_CH0	0x07 /* DUNIT Channel 0 */
#define IOSF_PORT_CPU_BUS	0x02 /* CPU Bus Interface Controller */
#define IOSF_PORT_BUNIT		0x03 /* System Memroy Arbiter/Bunit */
#define IOSF_PORT_PMC		0x04 /* Power Management Controller */
#define IOSF_PORT_GFX		0x06 /* Graphics Adapter */
#define IOSF_PORT_DUNIT_CH1	0x07 /* DUNIT Channel 1 */
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

/*
 * DUNIT Registers.
 */

#define DRP			0x00
# define DRP_DIMM0_RANK0_EN	(0x01 << 0)
# define DRP_DIMM0_RANK1_EN	(0x01 << 1)
# define DRP_DIMM1_RANK0_EN	(0x01 << 2)
# define DRP_DIMM1_RANK1_EN	(0x01 << 3)
# define DRP_RANK_MASK (DRP_DIMM0_RANK0_EN | DRP_DIMM0_RANK1_EN | \
			DRP_DIMM1_RANK0_EN | DRP_DIMM1_RANK1_EN)
#define DTR0			0x01
# define DTR0_SPEED_MASK	0x03
# define DTR0_SPEED_800		0x00
# define DTR0_SPEED_1066	0x01
# define DTR0_SPEED_1333	0x02
# define DTR0_SPEED_1600	0x03

/*
 * PUNIT Registers
 */
#define SB_BIOS_CONFIG			0x06
# define  SB_BIOS_CONFIG_ECC_EN			(1 << 31)
# define  SB_BIOS_CONFIG_DUAL_CH_DIS		(1 << 30)
# define  SB_BIOS_CONFIG_EFF_ECC		(1 << 29)
# define  SB_BIOS_CONFIG_EFF_DUAL_CH_DIS	(1 << 28)
# define  SB_BIOS_CONFIG_PERF_MODE		(1 << 17)
# define  SB_BIOS_CONFIG_PDM_MODE		(1 << 16)
# define  SB_BIOS_CONFIG_DDRIO_PWRGATE		(1 << 8)
# define  SB_BIOS_CONFIG_GFX_TURBO_DIS		(1 << 7)
# define  SB_BIOS_CONFIG_PCIE_PLLOFFOK		(1 << 1)
# define  SB_BIOS_CONFIG_USB_CACHING_EN		(1 << 0)
#define BIOS_RESET_CPL			0x05
# define  BIOS_RESET_CPL_ALL_DONE		(1 << 1)
# define  BIOS_RESET_CPL_RESET_DONE		(1 << 0)
#define PUNIT_PWRGT_CONTROL		0x60
#define PUNIT_PWRGT_STATUS		0x61
#define PUNIT_GPU_EC_VIRUS		0xd2

#endif /* _BAYTRAIL_IOSF_H_ */
