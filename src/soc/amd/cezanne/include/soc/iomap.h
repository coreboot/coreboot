/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_IOMAP_H
#define AMD_CEZANNE_IOMAP_H

/* FCH AL2AHB Registers */
#define ALINK_AHB_ADDRESS		0xfedc0000

#define APU_DMAC0_BASE			0xfedc7000
#define APU_DMAC1_BASE			0xfedc8000
#define APU_UART0_BASE			0xfedc9000
#define APU_UART1_BASE			0xfedca000

/* MMIO Ranges */
#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define NCP_ERR				0x00f0
#define SMB_BASE_ADDR			0x0b00

#endif /* AMD_CEZANNE_IOMAP_H */
