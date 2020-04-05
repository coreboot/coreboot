/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _BROADWELL_SMBUS_H_
#define _BROADWELL_SMBUS_H_

/* PCI Configuration Space (D31:F3): SMBus */
#define SMB_BASE		0x20
#define HOSTC			0x40
#define  HST_EN			(1 << 0)

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
#define SMBUS_SLAVE_ADDR	0x24

#endif
