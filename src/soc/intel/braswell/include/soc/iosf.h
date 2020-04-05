/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_IOSF_H_
#define _SOC_IOSF_H_

#include <stdint.h>
#if ENV_RAMSTAGE
#include <device/device.h>
#include <reg_script.h>
#endif	/* ENV_RAMSTAGE */
#include <soc/pci_devs.h>

/*
 * The SoC has a message network called IOSF Sideband. The access
 * routines are through 3 registers in PCI config space of 00:00.0:
 *  MCR - control register
 *  MDR - data register
 *  MCRX - control register extension
 * The extension register is only used for addresses that don't fit
 * into the 8 bit register address.
 */

#ifndef PCI_DEV
#define PCI_DEV(SEGBUS, DEV, FN) ( \
	(((SEGBUS) & 0xFFF) << 20) | \
	(((DEV) & 0x1F) << 15) | \
	(((FN)  & 0x07) << 12))
#endif
#define IOSF_PCI_DEV PCI_DEV(0, SOC_DEV, SOC_FUNC)

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
uint32_t iosf_punit_read(int reg);
void iosf_punit_write(int reg, uint32_t val);
uint32_t iosf_score_read(int reg);
void iosf_score_write(int reg, uint32_t val);
uint32_t iosf_lpss_read(int reg);
void iosf_lpss_write(int reg, uint32_t val);
uint32_t iosf_port58_read(int reg);
void iosf_port58_write(int reg, uint32_t val);
uint32_t iosf_scc_read(int reg);
void iosf_scc_write(int reg, uint32_t val);
uint32_t iosf_usbphy_read(int reg);
void iosf_usbphy_write(int reg, uint32_t val);

#if ENV_RAMSTAGE
uint64_t reg_script_read_iosf(struct reg_script_context *ctx);
void reg_script_write_iosf(struct reg_script_context *ctx);
#endif	/* ENV_RAMSTAGE */

/* IOSF ports. */
#define IOSF_PORT_AUNIT		0x00 /* IO Arbiter unit */
#define IOSF_PORT_CPU_BUS	0x02 /* CPU Bus Interface Controller */
#define IOSF_PORT_BUNIT		0x03 /* System Memory Arbiter/Bunit */
#define IOSF_PORT_PMC		0x04 /* Power Management Controller */
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
#define IOSF_PORT_USBPHY	0x43 /* USB PHY */
#define IOSF_PORT_LPSS		0xa0 /* LPSS - Low Power Subsystem */
#define IOSF_PORT_0xa2		0xa2
#define IOSF_PORT_SSUS		0xa8 /* SUS */
#define IOSF_PORT_CCU		0xa9 /* Clock control unit. */

/* Read and write opcodes differ per port. */
#define IOSF_OP_READ_BUNIT	0x10
#define IOSF_OP_WRITE_BUNIT	(IOSF_OP_READ_BUNIT | 1)
#define IOSF_OP_READ_PMC	0x06
#define IOSF_OP_WRITE_PMC	(IOSF_OP_READ_PMC | 1)
#define IOSF_OP_READ_SCORE	0x06
#define IOSF_OP_WRITE_SCORE	(IOSF_OP_READ_SCORE | 1)
#define IOSF_OP_READ_LPSS	0x06
#define IOSF_OP_WRITE_LPSS	(IOSF_OP_READ_LPSS | 1)
#define IOSF_OP_READ_0x58	0x06
#define IOSF_OP_WRITE_0x58	(IOSF_OP_READ_0x58 | 1)
#define IOSF_OP_READ_SCC        0x06
#define IOSF_OP_WRITE_SCC       (IOSF_OP_READ_SCC | 1)
#define IOSF_OP_READ_USBPHY     0x06
#define IOSF_OP_WRITE_USBPHY    (IOSF_OP_READ_USBPHY | 1)

/*
 * BUNIT Registers.
 */

/* BMBOUND has a 128MiB granularity. Highest address is 0xf8000000. */
#define BUNIT_BMBOUND		0x25
/*
 * BMBOUND_HI describes the available RAM above 4GiB. It has a
 * 256MiB granularity. Physical address bits 35:28 are compared with 31:24
 * bits in the BMBOUND_HI register. Also note that since BMBOUND has 128MiB
 * granularity care needs to be taken with the e820 map to account for a hole
 * in the RAM.
 */
#define BUNIT_BMBOUND_HI	0x26
#define BUNIT_MMCONF_REG	0x27
#define BUNIT_BMISC		0x28
/* The SMMRR registers define the SMM region in MiB granularity. */
#define BUNIT_SMRWAC	0x2d
#define BUNIT_SMRRL		0x2e
#define BUNIT_SMRRH		0x2f

/* SA ID bits. */
#define SAI_IA_UNTRUSTED	(1 << 0)
#define SAI_IA_SMM			(1 << 2)
#define SAI_IA_BOOT			(1 << 4)

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
#define SCC_MMC_CTL			0x500
# define SCC_CTL_PCI_CFG_DIS			(1 << 0)
# define SCC_CTL_ACPI_INT_EN			(1 << 1)

/*
 * LPE Registers
 */
#define LPE_PCICFGCTR1			0x0500
# define LPE_PCICFGCTR1_PCI_CFG_DIS		(1 << 0)
# define LPE_PCICFGCTR1_ACPI_INT_EN		(1 << 1)

/*
 * USBPHY Registers
 */
#define USBPHY_COMPBG			0x7f04

/*
 * IO Sideband Function
 */

#if ENV_RAMSTAGE
#define REG_SCRIPT_IOSF(cmd_, unit_, reg_, mask_, value_, timeout_) \
	_REG_SCRIPT_ENCODE_RAW(REG_SCRIPT_COMMAND_##cmd_,      \
			       REG_SCRIPT_TYPE_IOSF,           \
			       REG_SCRIPT_SIZE_32,             \
			       reg_, mask_, value_, timeout_, unit_)
#define REG_IOSF_READ(unit_, reg_) \
	REG_SCRIPT_IOSF(READ, unit_, reg_, 0, 0, 0)
#define REG_IOSF_WRITE(unit_, reg_, value_) \
	REG_SCRIPT_IOSF(WRITE, unit_, reg_, 0, value_, 0)
#define REG_IOSF_RMW(unit_, reg_, mask_, value_) \
	REG_SCRIPT_IOSF(RMW, unit_, reg_, mask_, value_, 0)
#define REG_IOSF_OR(unit_, reg_, value_) \
	REG_IOSF_RMW(unit_, reg_, 0xffffffff, value_)
#define REG_IOSF_POLL(unit_, reg_, mask_, value_, timeout_) \
	REG_SCRIPT_IOSF(POLL, unit_, reg_, mask_, value_, timeout_)
#endif	/* ENV_RAMSTAGE */

#endif /* _SOC_IOSF_H_ */
