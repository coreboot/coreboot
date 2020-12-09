/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_IOMAP_H
#define AMD_CEZANNE_IOMAP_H

/* MMIO Ranges */
#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define NCP_ERR				0x00f0
#define SMB_BASE_ADDR			0x0b00

#endif /* AMD_CEZANNE_IOMAP_H */
