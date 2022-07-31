/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * EC communication interface for Embedded Controller.
 */

#ifndef _EC_STARLABS_EC_H
#define _EC_STARLABS_EC_H

/*
 * Define the expected value of the PNP base address that is fixed through
 * the BADRSEL register controlled within the EC domain by the EC Firmware.
 */
#define ITE_FIXED_ADDR		0x4e
#define NUVOTON_FIXED_ADDR	0x4e

/* Logical device number (LDN) assignments for ITE. */
#define ITE_SP1			0x01	/* Serial Port 1 (UART) */
#define ITE_SP2			0x02	/* Serial Port 2 (UART) */
#define ITE_SWUC		0x04	/* System Wake-Up Control (SWUC) */
#define ITE_KBCM		0x05	/* KBC / Mouse Interface */
#define ITE_KBCK		0x06	/* KBC / Keyboard Interface */
#define ITE_IR			0x0a	/* Consumer IR (CIR) */
#define ITE_SMFI		0x0f	/* Shared Memory / Flash Interface (SMFI) */
#define ITE_RTCT		0x10	/* RTC-like Timer (RCTC) */
#define ITE_PMC1		0x11	/* Power Management I/F Channel 1 (PMC1) */
#define ITE_PMC2		0x12	/* Power Management I/F Channel 2 (PMC2) */
#define ITE_SSPI		0x13	/* Serial Peripheral Interface (SSPI) */
#define ITE_PECI		0x14	/* Platform Environment Control Interface (PECI) */
#define ITE_PMC3		0x17	/* Power Management I/F Channel 3 (PMC3) */
#define ITE_PMC4		0x18	/* Power Management I/F Channel 4 (PMC4) */
#define ITE_PMC5		0x19	/* Power Management I/F Channel 5 (PMC5) */

/* Logical device number (LDN) assignments for Nuvoton. */
#define NUVOTON_MSWC		0x04	/* Mobile System Wake-Up Control (MSWC) */
#define NUVOTON_KBCM		0x05	/* KBC / Mouse Interface */
#define NUVOTON_KBCK		0x06	/* KBC / Keyboard Interface */
#define NUVOTON_SHM		0x0f	/* Shared Memory (SHM) */
#define NUVOTON_PM1		0x11	/* Power Management I/F Channel 1 (PM1) */
#define NUVOTON_PM2		0x12	/* Power Management I/F Channel 2 (PM2) */
#define NUVOTON_PM3		0x17	/* Power Management I/F Channel 3 (PM3) */
#define NUVOTON_ESHM		0x1d	/* Extended Shared Memory (ESHM) */
#define NUVOTON_PM4		0x1e	/* Power Management I/F Channel 3 (PM4) */

/* Host domain registers. */
#define ITE_CHIPID1		0x20	/* Device ID register 1 */
#define ITE_CHIPID2		0x21	/* Device ID register 2 */
#define NUVOTON_CHIPID		0x27	/* Device ID register */

/* EC RAM common offsets */
#define ECRAM_MAJOR_VERSION	0x00
#define ECRAM_MINOR_VERSION	0x01

/*
 * CMOS Settings
 */

/* Keyboard Backlight Timeout */
#define SEC_30			0x00
#define MIN_1			0x01
#define MIN_3			0x02
#define MIN_5			0x03
#define NEVER			0x04

/* Fn Ctrl Swap */
#define FN_CTRL			0x00
#define CTRL_FN			0x01

/* Max Charge Setting */
#define CHARGE_100		0x00
#define CHARGE_80		0xbb
#define CHARGE_60		0xaa

/* Fast Charge Setting */
#define CHARGE_RATE_NORMAL	0x00
#define CHARGE_RATE_FAST	0x01

/* Fan Mode Setting */
#define FAN_NORMAL		0x00
#define FAN_AGGRESSIVE		0xbb
#define FAN_QUIET		0xaa

/* Fn Lock State */
#define UNLOCKED		0x00
#define LOCKED			0x01

/* Trackpad State */
#define TRACKPAD_ENABLED	0x00
#define TRACKPAD_DISABLED	0x22

/* Keyboard Brightness Levels */
#define KBL_ON			0xdd
#define KBL_OFF			0xcc
#define KBL_LOW			0xbb
#define KBL_HIGH		0xaa

/* Keyboard Backlight State */
#define KBL_DISABLED		0x00
#define KBL_ENABLED		0xdd

uint16_t ec_get_version(void);

#endif
