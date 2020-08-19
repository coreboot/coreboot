/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_SMBUS__LIB_H
#define SOC_INTEL_COMMON_BLOCK_SMBUS__LIB_H

/* SMBus IO Base Address */
#define SMBUS_IO_BASE	0xefa0

/* PCI Configuration Space : SMBus */
#define HOSTC	0x40
#define HST_EN	(1 << 0)
/* SMBus I/O bits. */
#define SMBHSTSTAT	0x0
#define SMBHSTCTL	0x2
#define SMBHSTCMD	0x3
#define SMBXMITADD	0x4
#define SMBHSTDAT0	0x5

#define SMBUS_TIMEOUT	15	/* 15ms */

#endif	/* SOC_INTEL_COMMON_BLOCK_SMBUS__LIB_H */
