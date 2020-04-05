/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_SPI_H__
#define __SOC_SPI_H__

#include <spi_flash.h>
#include <spi-generic.h>

#define SPISTS		0x3020 /* Status register */
#define SPICTL		0x3022 /* Control register */
#define SPIADDR		0x3024 /* Flash chip select and 24-bit address */
#define SPIDATA		0x3028 /* 64-byte send/receive data buffer */
#define SPIBBAR		0x3070 /* BIOS base address */
#define SPIPREOP	0x3074 /* Prefix opcode table */
#define SPITYPE		0x3076 /* Opcode type table */
#define SPIOPMENU	0x3078 /* Opcode table */
#define SPIPBR0		0x3080 /* Protected BIOS range */
#define SPIPBR1		0x3084
#define SPIPBR2		0x3088

struct flash_ctrlr {
	uint8_t rsvd_0x00[0x3020];/* 0x00 */
	uint16_t status;	  /* 0x3020: Status register */
	uint16_t control;	  /* 0x3022: Control register */
	uint32_t address;	  /* 0x3024: Chip select and 24-bit address */
	uint8_t data[64];	  /* 0x3028: 64-byte send/receive data buffer */
	uint8_t rsvd_0x68[8];	  /* 0x3068 */
	uint32_t bbar;		  /* 0x3070: BIOS base address */
	uint8_t prefix[2];	  /* 0x3074: Prefix opcode table */
	uint16_t type;		  /* 0x3076: Opcode type table */
	uint8_t opmenu[8];	  /* 0x3078: Opcode table */
	uint32_t pbr[3];	  /* 0x3080: Protected BIOS range */
};

/* 0x3020: SPISTS */
#define SPISTS_CLD		0x8000 /* Lock SPI controller configuration */
#define SPISTS_BA		0x0008 /* Access is blocked */
#define SPISTS_CD		0x0004 /* Cycle done */
#define SPISTS_CIP		0x0001 /* Cycle in progress */

/* 0x3022: SPICTL */
#define SPICTL_SMIEN		0x8000 /* Assert SMI_B at cycle done */
#define SPICTL_DC		0x4000 /* Cycle contains data */
#define SPICTL_DBCNT		0x3f00 /* Data byte count - 1, 1 - 64 bytes */
#define SPICTL_DBCNT_SHIFT	8
#define SPICTL_COPTR		0x0070 /* Opcode menu index, 0 - 7 */
#define SPICTL_COPTR_SHIFT	4
#define SPICTL_SOPTR		0x0008 /* Prefix index, 0 - 1 */
#define SPICTL_SOPTR_SHIFT	3
#define SPICTL_ACS		0x0004 /* Atomic cycle sequence */
#define SPICTL_CG		0x0002 /* Cycle go */
#define SPICTL_AR		0x0001 /* Access request */

/* 0x3076: SPITYPE */
#define SPITYPE_ADDRESS		0x0002 /* 3-byte address required */
#define SPITYPE_PREFIX		0x0001 /* Prefix required, write/erase cycle */

/*
 * 0x3080: PBR0
 * 0x3084: PBR1
 * 0x3088: PBR2
 */
#define SPIPBR_WPE		0x80000000 /* Write protect enable */
#define SPIPBR_PRL		0x00fff000 /* Protected range limit */
#define SPIPBR_PRB_SHIFT	12
#define SPIPBR_PRB		0x00000fff /* Protected range base */

struct spi_context {
	volatile struct flash_ctrlr *ctrlr;
	uint16_t control;
	uint16_t prefix;
};

extern const struct spi_ctrlr spi_driver;

void spi_bios_base(uint32_t bios_base_address);
void spi_controller_lock(void);
void spi_display(volatile struct flash_ctrlr *ctrlr);
const char *spi_opcode_name(int opcode);
int spi_protection(uint32_t address, uint32_t length);

#endif /* __SOC_SPI_H__ */
