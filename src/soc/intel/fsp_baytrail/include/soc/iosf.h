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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _BAYTRAIL_IOSF_H_
#define _BAYTRAIL_IOSF_H_

#include <stdint.h>
#include <soc/pci_devs.h>

/*
 * The Bay Trail SoC has a message network called IOSF Sideband. The access
 * routines are through 3 registers in PCI config space of 00:00.0:
 *  MCR - control register
 *  MDR - data register
 *  MCRX - control register extension
 * The extension register is only used for addresses that don't fit into the
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
#define  IOSF_REG_UPPER(x) (((~0xff) & (x)))
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
/* Some registers are per channel while the globals live in dunit 0 */
uint32_t iosf_dunit_ch0_read(int reg);
uint32_t iosf_dunit_ch1_read(int reg);
uint32_t iosf_punit_read(int reg);
uint32_t iosf_lpss_read(int reg);
void iosf_lpss_write(int reg, uint32_t val);

/* IOSF ports. */
#define IOSF_PORT_AUNIT		0x00 /* IO Arbiter unit */
#define IOSF_PORT_SYSMEMC	0x01 /* System Memory Controller */
#define IOSF_PORT_BUNIT		0x03 /* System Memory Arbiter/Bunit */
#define IOSF_PORT_DUNIT_CH1	0x07 /* DUNIT Channel 1 */
#define IOSF_PORT_USBPHY	0x43 /* USB PHY */
#define IOSF_PORT_USHPHY	0x61 /* USB XHCI PHY */
#define IOSF_PORT_LPSS		0xa0 /* LPSS - Low Power Subsystem */

/* Read and write opcodes differ per port. */
#define IOSF_OP_READ_SYSMEMC	0x10
#define IOSF_OP_WRITE_SYSMEMC	(IOSF_OP_READ_SYSMEMC | 1)
#define IOSF_OP_READ_BUNIT	0x10
#define IOSF_OP_WRITE_BUNIT	(IOSF_OP_READ_BUNIT | 1)
#define IOSF_OP_READ_LPSS	0x06
#define IOSF_OP_WRITE_LPSS	(IOSF_OP_READ_LPSS | 1)

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
 * LPSS Registers
 */
#define LPSS_SIO_DMA1_CTL		0x280
#define LPSS_I2C1_CTL			0x288
#define LPSS_I2C2_CTL			0x290
#define LPSS_I2C3_CTL			0x298
#define LPSS_I2C4_CTL			0x2a0
#define LPSS_I2C5_CTL			0x2a8
#define LPSS_I2C6_CTL			0x2b0
#define LPSS_I2C7_CTL			0x2b8
#define LPSS_SIO_DMA2_CTL		0x240
#define LPSS_PWM1_CTL			0x248
#define LPSS_PWM2_CTL			0x250
#define LPSS_HSUART1_CTL		0x258
#define LPSS_HSUART2_CTL		0x260
#define LPSS_SPI_CTL			0x268
# define LPSS_CTL_ACPI_INT_EN			(1 << 21)
# define LPSS_CTL_PCI_CFG_DIS			(1 << 20)
# define LPSS_CTL_SNOOP				(1 << 18)
# define LPSS_CTL_NOSNOOP			(1 << 19)
# define LPSS_CTL_PM_CAP_PRSNT			(1 <<  1)

/*
 * SCC Registers
 */
#define SCC_SD_CTL			0x504
#define SCC_SDIO_CTL			0x508
#define SCC_MMC_CTL			0x50c
# define SCC_CTL_PCI_CFG_DIS			(1 << 0)
# define SCC_CTL_ACPI_INT_EN			(1 << 1)

/*
 * CCU Registers
 */

#define PLT_CLK_CTRL_0			0x3c
#define PLT_CLK_CTRL_1			0x40
#define PLT_CLK_CTRL_2			0x44
#define PLT_CLK_CTRL_3			0x48
#define PLT_CLK_CTRL_4			0x4c
#define PLT_CLK_CTRL_5			0x50
# define PLT_CLK_CTRL_19P2MHZ_FREQ		(0 <<  1)
# define PLT_CLK_CTRL_25MHZ_FREQ		(1 <<  1)
# define PLT_CLK_CTRL_SELECT_FREQ		(1 <<  0)

/*
 * USBPHY Registers
 */
#define USBPHY_COMPBG				0x7f04
#define USBPHY_PER_PORT_LANE0			0x4100
#define USBPHY_PER_PORT_RCOMP_HS_PULLUP0	0x4122
#define USBPHY_PER_PORT_LANE1			0x4200
#define USBPHY_PER_PORT_RCOMP_HS_PULLUP1	0x4222
#define USBPHY_PER_PORT_LANE2			0x4300
#define USBPHY_PER_PORT_RCOMP_HS_PULLUP2	0x4322
#define USBPHY_PER_PORT_LANE3			0x4400
#define USBPHY_PER_PORT_RCOMP_HS_PULLUP3	0x4422

/*
 * USHPHY Registers
 */
#define USHPHY_CDN_PLL_CONTROL			0x03c0
#define USHPHY_CDN_VCO_START_CAL_POINT		0x0054
#define USHPHY_CCDRLF				0x8040
#define USHPHY_PEAKING_AMP_CONFIG_DIAG		0x80a8
#define USHPHY_OFFSET_COR_CONFIG_DIAG		0x80b0
#define USHPHY_VGA_GAIN_CONFIG_DIAG		0x8080
#define USHPHY_REE_DAC_CONTROL			0x80b8
#define USHPHY_CDN_U1_POWER_STATE_DEF		0x0000

#endif /* _BAYTRAIL_IOSF_H_ */
