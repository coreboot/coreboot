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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
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
#define  IOSF_REG_UPPER(x) (((~0xff) & (x)))
#define  IOSF_BYTE_EN_0 0x10
#define  IOSF_BYTE_EN_1 0x20
#define  IOSF_BYTE_EN_2 0x40
#define  IOSF_BYTE_EN_3 0x80
#define  IOSF_BYTE_EN \
	  (IOSF_BYTE_EN_0 | IOSF_BYTE_EN_1 | IOSF_BYTE_EN_2 | IOSF_BYTE_EN_3)
#define MDR_REG 0xd4
#define MCRX_REG 0xd8

uint32_t iosf_aunit_read(int reg);
void iosf_aunit_write(int reg, uint32_t val);
uint32_t iosf_cpu_bus_read(int reg);
void iosf_cpu_bus_write(int reg, uint32_t val);
uint32_t iosf_bunit_read(int reg);
void iosf_bunit_write(int reg, uint32_t val);
uint32_t iosf_dunit_read(int reg);
void iosf_dunit_write(int reg, uint32_t val);
/* Some registers are per channel while the globals live in dunit 0 */
uint32_t iosf_dunit_ch0_read(int reg);
uint32_t iosf_dunit_ch1_read(int reg);
uint32_t iosf_punit_read(int reg);
void iosf_punit_write(int reg, uint32_t val);
uint32_t iosf_usbphy_read(int reg);
void iosf_usbphy_write(int reg, uint32_t val);
uint32_t iosf_ushphy_read(int reg);
void iosf_ushphy_write(int reg, uint32_t val);
uint32_t iosf_sec_read(int reg);
void iosf_sec_write(int reg, uint32_t val);
uint32_t iosf_port45_read(int reg);
void iosf_port45_write(int reg, uint32_t val);
uint32_t iosf_port46_read(int reg);
void iosf_port46_write(int reg, uint32_t val);
uint32_t iosf_port47_read(int reg);
void iosf_port47_write(int reg, uint32_t val);
uint32_t iosf_port55_read(int reg);
void iosf_port55_write(int reg, uint32_t val);
uint32_t iosf_port58_read(int reg);
void iosf_port58_write(int reg, uint32_t val);
uint32_t iosf_port59_read(int reg);
void iosf_port59_write(int reg, uint32_t val);
uint32_t iosf_port5a_read(int reg);
void iosf_port5a_write(int reg, uint32_t val);
uint32_t iosf_lpss_read(int reg);
void iosf_lpss_write(int reg, uint32_t val);
uint32_t iosf_ccu_read(int reg);
void iosf_ccu_write(int reg, uint32_t val);
uint32_t iosf_score_read(int reg);
void iosf_score_write(int reg, uint32_t val);
uint32_t iosf_scc_read(int reg);
void iosf_scc_write(int reg, uint32_t val);
uint32_t iosf_porta2_read(int reg);
void iosf_porta2_write(int reg, uint32_t val);
uint32_t iosf_ssus_read(int reg);
void iosf_ssus_write(int reg, uint32_t val);

/* IOSF ports. */
#define IOSF_PORT_AUNIT		0x00 /* IO Arbiter unit */
#define IOSF_PORT_SYSMEMC	0x01 /* System Memory Controller */
#define IOSF_PORT_DUNIT_CH0	0x07 /* DUNIT Channel 0 */
#define IOSF_PORT_CPU_BUS	0x02 /* CPU Bus Interface Controller */
#define IOSF_PORT_BUNIT		0x03 /* System Memory Arbiter/Bunit */
#define IOSF_PORT_PMC		0x04 /* Power Management Controller */
#define IOSF_PORT_GFX		0x06 /* Graphics Adapter */
#define IOSF_PORT_DUNIT_CH1	0x07 /* DUNIT Channel 1 */
#define IOSF_PORT_SYSMEMIO	0x0c /* System Memory IO */
#define IOSF_PORT_USBPHY	0x43 /* USB PHY */
#define IOSF_PORT_SEC		0x44 /* SEC */
#define IOSF_PORT_0x45		0x45
#define IOSF_PORT_0x46		0x46
#define IOSF_PORT_0x47		0x47
#define IOSF_PORT_SCORE		0x48 /* SCORE */
#define IOSF_PORT_0x55		0x55
#define IOSF_PORT_0x58		0x58
#define IOSF_PORT_0x59		0x59
#define IOSF_PORT_0x5a		0x5a
#define IOSF_PORT_USHPHY	0x61 /* USB XHCI PHY */
#define IOSF_PORT_SCC		0x63 /* Storage Control Cluster */
#define IOSF_PORT_LPSS		0xa0 /* LPSS - Low Power Subsystem */
#define IOSF_PORT_0xa2		0xa2
#define IOSF_PORT_SATAPHY	0xa3 /* SATA PHY */
#define IOSF_PORT_PCIEPHY	0xa3 /* PCIE PHY */
#define IOSF_PORT_SSUS		0xa8 /* SUS */
#define IOSF_PORT_CCU		0xa9 /* Clock control unit. */

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
#define IOSF_OP_READ_SEC	0x04
#define IOSF_OP_WRITE_SEC	(IOSF_OP_READ_SEC | 1)
#define IOSF_OP_READ_0x45	0x06
#define IOSF_OP_WRITE_0x45	(IOSF_OP_READ_0x45 | 1)
#define IOSF_OP_READ_0x46	0x06
#define IOSF_OP_WRITE_0x46	(IOSF_OP_READ_0x46 | 1)
#define IOSF_OP_READ_0x47	0x06
#define IOSF_OP_WRITE_0x47	(IOSF_OP_READ_0x47 | 1)
#define IOSF_OP_READ_SCORE	0x06
#define IOSF_OP_WRITE_SCORE	(IOSF_OP_READ_SCORE | 1)
#define IOSF_OP_READ_0x55	0x04
#define IOSF_OP_WRITE_0x55	(IOSF_OP_READ_0x55 | 1)
#define IOSF_OP_READ_0x58	0x06
#define IOSF_OP_WRITE_0x58	(IOSF_OP_READ_0x58 | 1)
#define IOSF_OP_READ_0x59	0x06
#define IOSF_OP_WRITE_0x59	(IOSF_OP_READ_0x59 | 1)
#define IOSF_OP_READ_0x5a	0x04
#define IOSF_OP_WRITE_0x5a	(IOSF_OP_READ_0x5a | 1)
#define IOSF_OP_READ_USHPHY	0x06
#define IOSF_OP_WRITE_USHPHY	(IOSF_OP_READ_USHPHY | 1)
#define IOSF_OP_READ_SCC	0x06
#define IOSF_OP_WRITE_SCC	(IOSF_OP_READ_SCC | 1)
#define IOSF_OP_READ_LPSS	0x06
#define IOSF_OP_WRITE_LPSS	(IOSF_OP_READ_LPSS | 1)
#define IOSF_OP_READ_0xa2	0x06
#define IOSF_OP_WRITE_0xa2	(IOSF_OP_READ_0xa2 | 1)
#define IOSF_OP_READ_SATAPHY	0x00
#define IOSF_OP_WRITE_SATAPHY	(IOSF_OP_READ_SATAPHY | 1)
#define IOSF_OP_READ_PCIEPHY	0x00
#define IOSF_OP_WRITE_PCIEPHY	(IOSF_OP_READ_PCIEPHY | 1)
#define IOSF_OP_READ_SSUS	0x10
#define IOSF_OP_WRITE_SSUS	(IOSF_OP_READ_SSUS | 1)
#define IOSF_OP_READ_CCU	0x06
#define IOSF_OP_WRITE_CCU	(IOSF_OP_READ_CCU | 1)

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
#define BUNIT_SMRCP		0x2b
#define BUNIT_SMRRAC		0x2c
#define BUNIT_SMRWAC		0x2d
#define BUNIT_SMRRL		0x2e
#define BUNIT_SMRRH		0x2f
# define BUNIT_SMRR_ENABLE	(1 << 31)

/* SA ID bits. */
#define SAI_IA_UNTRUSTED	(1 << 0)
#define SAI_IA_SMM		(1 << 2)
#define SAI_IA_BOOT		(1 << 4)

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
# define  SB_BIOS_CONFIG_PS2_EN_VNN		(1 << 3)
# define  SB_BIOS_CONFIG_PS2_EN_VCC		(1 << 2)
# define  SB_BIOS_CONFIG_PCIE_PLLOFFOK		(1 << 1)
# define  SB_BIOS_CONFIG_USB_CACHING_EN		(1 << 0)
#define BIOS_RESET_CPL			0x05
# define  BIOS_RESET_CPL_ALL_DONE		(1 << 1)
# define  BIOS_RESET_CPL_RESET_DONE		(1 << 0)
#define PUNIT_PWRGT_CONTROL		0x60
#define PUNIT_PWRGT_STATUS		0x61
#define PUNIT_GPU_EC_VIRUS		0xd2

#define PUNIT_SOC_POWER_BUDGET		0x02
#define PUNIT_SOC_ENERGY_CREDIT		0x03
#define PUNIT_PTMC			0x80
#define PUNIT_GFXT			0x88
#define PUNIT_VEDT			0x89
#define PUNIT_ISPT			0x8c
#define PUNIT_PTPS			0xb2
#define PUNIT_TE_AUX0			0xb5
#define PUNIT_TE_AUX1			0xb6
#define PUNIT_TE_AUX2			0xb7
#define PUNIT_TE_AUX3			0xb8
#define PUNIT_TTE_VRIccMax		0xb9
#define PUNIT_TTE_VRHot			0xba
#define PUNIT_TTE_XXPROCHOT		0xbb
#define PUNIT_TTE_SLM0			0xbc
#define PUNIT_TTE_SLM1			0xbd
#define PUNIT_TTE_SWT			0xbf

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

/*
 * LPE Registers
 */
#define LPE_PCICFGCTR1			0x0500
# define LPE_PCICFGCTR1_PCI_CFG_DIS		(1 << 0)
# define LPE_PCICFGCTR1_ACPI_INT_EN		(1 << 1)

#endif /* _BAYTRAIL_IOSF_H_ */
