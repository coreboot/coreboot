/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *               2012 secunet Security Networks AG
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SOUTHBRIDGE_INTEL_I82801GX_I82801IX_H
#define SOUTHBRIDGE_INTEL_I82801GX_I82801IX_H

#ifndef __ACPI__
#ifndef __ASSEMBLER__
#include "chip.h"
#endif
#endif

#define DEFAULT_TBAR		0xfed1b000
#define DEFAULT_RCBA		0xfed1c000
#define DEFAULT_PMBASE		0x00000500 /* Speedstep code has this hardcoded, too. */
#define DEFAULT_TCOBASE		(DEFAULT_PMBASE + 0x60)
#define DEFAULT_GPIOBASE	0x00000580


#define APM_CNT		0xb2

#define PM1_STS		0x00
#define   PWRBTN_STS	(1 <<  8)
#define   RTC_STS	(1 << 10)
#define PM1_EN		0x02
#define   PWRBTN_EN	(1 <<  8)
#define   GBL_EN	(1 <<  5)
#define PM1_CNT		0x04
#define   SCI_EN	(1 << 0)
#define PM_LV2		0x14
#define PM_LV3		0x15
#define PM_LV4		0x16
#define PM_LV5		0x17
#define PM_LV6		0x18
#define GPE0_STS	0x20
#define SMI_EN		0x30
#define   PERIODIC_EN	(1 << 14)
#define   TCO_EN	(1 << 13)
#define   APMC_EN	(1 <<  5)
#define   BIOS_EN	(1 <<  2)
#define   EOS		(1 <<  1)
#define   GBL_SMI_EN	(1 <<  0)
#define SMI_STS		0x34
#define ALT_GP_SMI_EN	0x38
#define ALT_GP_SMI_STS	0x3a


#define DEBUG_PERIODIC_SMIS	0

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif


/* D31:F0 LPC bridge */
#define D31F0_PMBASE		0x40
#define D31F0_ACPI_CNTL		0x44
#define D31F0_GPIO_BASE		0x48
#define D31F0_GPIO_CNTL		0x4c
#define D31F0_PIRQA_ROUT	0x60
#define D31F0_PIRQB_ROUT	0x61
#define D31F0_PIRQC_ROUT	0x62
#define D31F0_PIRQD_ROUT	0x63
#define D31F0_SERIRQ_CNTL	0x64
#define D31F0_PIRQE_ROUT	0x68
#define D31F0_PIRQF_ROUT	0x69
#define D31F0_PIRQG_ROUT	0x6a
#define D31F0_PIRQH_ROUT	0x6b
#define D31F0_LPC_IODEC		0x80
#define D31F0_LPC_EN		0x82
#define D31F0_GEN1_DEC		0x84
#define D31F0_GEN_PMCON_1	0xa0
#define D31F0_GEN_PMCON_3	0xa4
#define D31F0_C5_EXIT_TIMING	0xa8
#define D31F0_CxSTATE_CNF	0xa9
#define D31F0_C4TIMING_CNT	0xaa
#define D31F0_GPIO_ROUT		0xb8
#define D31F0_RCBA		0xf0

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)


/* D31:F2 SATA */
#define D31F2_IDE_TIM_PRI	0x40
#define D31F2_IDE_TIM_SEC	0x42
#define D31F2_SIDX		0xa0
#define D31F2_SDAT		0xa4


/* D30:F0 PCI-to-PCI bridge */
#define D30F0_SMLT		0x1b


/* D28:F0-5 PCIe root ports */
#define D28Fx_XCAP		0x42
#define D28Fx_SLCAP		0x54


#define SMBUS_IO_BASE		0x0400

/* PCI Configuration Space (D31:F3): SMBus */
#define SMB_BASE		0x20
#define HOSTC			0x40

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

/* SMBus I/O bits. */
#define SMBHSTSTAT		0x0
#define SMBHSTCTL		0x2
#define SMBHSTCMD		0x3
#define SMBXMITADD		0x4
#define SMBHSTDAT0		0x5
#define SMBHSTDAT1		0x6
#define SMBBLKDAT		0x7
#define SMBTRNSADD		0x9
#define SMBSLVDATA		0xa
#define SMLINK_PIN_CTL		0xe
#define SMBUS_PIN_CTL		0xf

#define SMBUS_TIMEOUT		(10 * 1000 * 100)


#define RCBA8(x) *((volatile u8 *)(DEFAULT_RCBA + x))
#define RCBA16(x) *((volatile u16 *)(DEFAULT_RCBA + x))
#define RCBA32(x) *((volatile u32 *)(DEFAULT_RCBA + x))

#define RCBA_V0CTL		0x0014
#define RCBA_V1CAP		0x001c
#define RCBA_V1CTL		0x0020
#define RCBA_V1STS		0x0026
#define RCBA_PAT		0x0030
#define RCBA_ESD		0x0104
#define RCBA_ULD		0x0110
#define RCBA_ULBA		0x0118
#define RCBA_LCAP		0x01a4
#define RCBA_LCTL		0x01a8
#define RCBA_LSTS		0x01aa
#define RCBA_DMIC		0x0234
#define RCBA_RPFN		0x0238
#define RCBA_DMC		0x2010
#define RCBA_HPTC		0x3404
#define RCBA_BUC		0x3414
#define RCBA_FD			0x3418 /* Function Disable, see below. */
#define RCBA_CG			0x341c
#define RCBA_FDSW		0x3420
#define RCBA_MAP		0x35f0 /* UHCI cotroller #6 remapping */

#define BUC_LAND	(1 <<  5) /* LAN */
#define FD_SAD2		(1 << 25) /* SATA #2 */
#define FD_TTD		(1 << 24) /* Thermal Throttle */
#define FD_PE6D		(1 << 21) /* PCIe root port 6 */
#define FD_PE5D		(1 << 20) /* PCIe root port 5 */
#define FD_PE4D		(1 << 19) /* PCIe root port 4 */
#define FD_PE3D		(1 << 18) /* PCIe root port 3 */
#define FD_PE2D		(1 << 17) /* PCIe root port 2 */
#define FD_PE1D		(1 << 16) /* PCIe root port 1 */
#define FD_EHCI1D	(1 << 15) /* EHCI #1 */
#define FD_LBD		(1 << 14) /* LPC bridge */
#define FD_EHCI2D	(1 << 13) /* EHCI #2 */
#define FD_U5D		(1 << 12) /* UHCI #5 */
#define FD_U4D		(1 << 11) /* UHCI #4 */
#define FD_U3D		(1 << 10) /* UHCI #3 */
#define FD_U2D		(1 <<  9) /* UHCI #2 */
#define FD_U1D		(1 <<  8) /* UHCI #1 */
#define FD_U6D		(1 <<  7) /* UHCI #6 */
#define FD_HDAD		(1 <<  4) /* HD audio */
#define FD_SD		(1 <<  3) /* SMBus */
#define FD_SAD1		(1 <<  2) /* SATA #1 */


#define SKPAD_ACPI_S3_MAGIC	0xcafed00d
#define SKPAD_NORMAL_BOOT_MAGIC	0xcafebabe


#ifndef __ACPI__
#ifndef __ASSEMBLER__

static inline int lpc_is_mobile(const u16 devid)
{
	return (devid == 0x2917) || (devid == 0x2919);
}
#define LPC_IS_MOBILE(dev) lpc_is_mobile(pci_read_config16(dev, PCI_DEVICE_ID))

#if defined(__PRE_RAM__)
void enable_smbus(void);
int smbus_read_byte(unsigned device, unsigned address);
void i82801ix_early_init(void);
void i82801ix_dmi_setup(void);
void i82801ix_dmi_poll_vc1(void);
#endif

#endif
#endif

#endif
