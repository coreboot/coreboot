/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOUTHBRIDGE_INTEL_I82371EB_I82371EB_H
#define SOUTHBRIDGE_INTEL_I82371EB_I82371EB_H

#if !defined(__ACPI__)

void enable_pm(void);
void i82371eb_early_init(void);

#endif

/* If 'cond' is true this macro sets the bit(s) specified by 'bits' in the
 * 'reg' variable, otherwise it clears those bits.
 *
 * Examples:
 * reg16 = ONOFF(conf->ide0_enable, reg16, (1 << 5));
 * reg16 = ONOFF(conf->ide0_enable, reg16, (FOO | BAR));
 */
/* TODO: Move into some global header file? */
#define ONOFF(cond,reg,bits) ((cond) ? ((reg) | (bits)) : ((reg) & ~(bits)))

#define XBCS		0x4e	/* X-Bus chip select register */
#define GENCFG		0xb0	/* General configuration register */
#define   GPO2223	(1<<28) /* GPO22/23 */
#define RTCCFG		0xcb	/* Real time clock configuration register */
#define   RTC_POS_DECODE (1<<5)
#define   UPPER_RAM_EN	(1<<2)
#define   RTC_ENABLE	(1<<0)

/* IDE */
#define IDETIM_PRI	0x40	/* IDE timing register, primary channel */
#define IDETIM_SEC	0x42	/* IDE timing register, secondary channel */
#define UDMACTL		0x48	/* Ultra DMA/33 control register */
#define UDMATIM		0x4a	/* Ultra DMA/33 timing register */

/* SMBus */
#define SMBBA		0x90	/* SMBus base address */
#define   SMBUS_IO_BASE	0x0f00
#define SMBHSTCFG	0xd2	/* SMBus host configuration */

/* Power management (ACPI) I/O ports, offset from PM_IO_BASE below */
#define PMSTS		0x00	/* Power Management Status */
#define PMEN		0x02	/* Power Management Resume Enable */
#define   PWRBTN_EN	(1<<8)
#define   GBL_EN	(1<<5)
#define PMCNTRL		0x04	/* Power Management Control */
#define   SUS_EN	(1<<13)	/* S0-S5 trigger */
#define   SUS_TYP_MSK	(7<<10)
#define   SUS_TYP_S0	(5<<10)
#define   SUS_TYP_S1	(4<<10)
#define   SUS_TYP_S2	(3<<10) /* S2 may also map as 2<<10 */
#define   SUS_TYP_S3	(1<<10)
#define   SUS_TYP_S5	(0<<10)
#define   SCI_EN	(1<<0)
#define PMTMR		0x08	/* Power Management Timer */
#define GPSTS		0x0c	/* General Purpose Status */
#define GPEN		0x0e	/* General Purpose Enable */
#define   THRM_EN	(1<<0)
#define PCNTRL		0x10	/* Processor control */
#define GLBSTS		0x18	/* Global Status */
#define   IRQ_RSM_STS	(1<<11)
#define   EXTSMI_STS	(1<<10)
#define   GSTBY_STS	(1<<8)
#define   GP_STS	(1<<7)
#define   PM1_STS	(1<<6)
#define   APM_STS	(1<<5)
#define   DEV_STS	(1<<4)
#define   LEGACY_USB_STS (1<<1)
#define   BIOS_STS	(1<<0)
#define DEVSTS		0x1c	/* Device Status */
#define GLBEN		0x20	/* Global Enable */
#define   EXTSMI_EN	(1<<10)	/* EXTSMI# signal triggers SMI */
#define   GSTBY_EN	(1<<8)
#define   BM_TRP_EN	(1<<1)
#define   BIOS_EN	(1<<1)	/* GBL_RLS write triggers SMI */
#define   LEGACY_USB_EN	(1<<0)	/* Keyboard controller access triggers SMI */
#define GLBCTL		0x28	/* Global Control */
#define   EOS		(1<<16)	/* End of SMI */
#define   SMI_EN	(1<<0)	/* SMI enable */
#define DEVCTL		0x2c	/* Device Control */
#define   TRP_EN_DEV12	(1<<24)	/* SMI on dev12 trap */
#define GPO0		0x34
#define GPO1		0x35
#define GPO2		0x36
#define GPO3		0x37

/* Power management (ACPI) PCI registers */
#define PMBA		0x40	/* Power management base address */
#define   DEFAULT_PMBASE	0xe400
#define   PM_IO_BASE DEFAULT_PMBASE
#define DEVRESA		0x5c	/* Device resource A */
#define DEVRESB		0x60	/* Device resource B */
#define DEVRESC		0x64	/* Device resource C */
#define DEVRESD		0x52	/* Device resource D */
#define DEVRESE		0x68	/* Device resource E */
#define DEVRESF		0x6c	/* Device resource F */
#define DEVRESG		0x70	/* Device resource G */
#define DEVRESH		0x74	/* Device resource H */
#define DEVRESI		0x78	/* Device resource I */
#define DEVRESJ		0x7c	/* Device resource J */
#define PMREGMISC	0x80	/* Miscellaneous power management */
#define   PMIOSE	(1 << 0) /* PM I/O Space Enable */

/* Bit definitions */
#define EXT_BIOS_ENABLE_1MB	(1 << 9)  /* 1-Meg Extended BIOS Enable */
#define EXT_BIOS_ENABLE		(1 << 7)  /* Extended BIOS Enable */
#define LOWER_BIOS_ENABLE	(1 << 6)  /* Lower BIOS Enable */
#define WRITE_PROTECT_ENABLE	(1 << 2)  /* Write Protect Enable */
#define SMB_HST_EN		(1 << 0)  /* Host Interface Enable */
#define IDE_DECODE_ENABLE	(1 << 15) /* IDE Decode Enable */
#define DTE0			(1 << 3)  /* DMA Timing Enable Only, drive 0 */
#define DTE1			(1 << 7)  /* DMA Timing Enable Only, drive 1 */
#define PSDE0			(1 << 0)  /* Primary Drive 0 UDMA/33 */
#define PSDE1			(1 << 1)  /* Primary Drive 1 UDMA/33 */
#define SSDE0			(1 << 2)  /* Secondary Drive 0 UDMA/33 */
#define SSDE1			(1 << 3)  /* Secondary Drive 1 UDMA/33 */
#define ISA			(1 << 0)  /* Select ISA */

#endif /* SOUTHBRIDGE_INTEL_I82371EB_I82371EB_H */
