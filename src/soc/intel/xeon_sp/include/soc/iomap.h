/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#ifndef _SOC_IOMAP_H_
#define _SOC_IOMAP_H_

#define MAP_ENTRY(reg_, is_64_, is_limit_, mask_bits_, desc_) \
	{                                                           \
		.reg = reg_, .is_64_bit = is_64_, .is_limit = is_limit_,  \
		.mask_bits = mask_bits_, .description = desc_,            \
	}

#define MAP_ENTRY_BASE_64(reg_, desc_) MAP_ENTRY(reg_, 1, 0, 0, desc_)
#define MAP_ENTRY_LIMIT_64(reg_, mask_bits_, desc_) MAP_ENTRY(reg_, 1, 1, mask_bits_, desc_)
#define MAP_ENTRY_BASE_32(reg_, desc_) MAP_ENTRY(reg_, 0, 0, 0, desc_)
#define MAP_ENTRY_LIMIT_32(reg_, mask_bits_, desc_) MAP_ENTRY(reg_, 0, 1, mask_bits_, desc_)

// SPI BAR0 MMIO base address
#define SPI_BASE_ADDRESS             0xfe010000
#define SPI_BASE_SIZE                0x1000

#define ACPI_BASE_ADDRESS            0x500

/* Video RAM */
#define VGA_BASE_ADDRESS             0xa0000
#define VGA_BASE_SIZE                0x20000

/* High Performance Event Timer */
#define HPET_BASE_ADDRESS            0xfed00000

#define P2SB_BAR                     CONFIG_PCR_BASE_ADDRESS

#endif /* _SOC_IOMAP_H_ */
