/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_FAST_SPI_H
#define SOC_INTEL_COMMON_BLOCK_FAST_SPI_H

#include <stdint.h>
#include <stddef.h>

/*
 * Disable the BIOS write protect and Enable Prefetching and Caching.
 */
void fast_spi_init(void);
/*
 * Minimal set of commands to read WPSR from SPI.
 * Returns 0 on success, < 0 on failure.
 */
int fast_spi_flash_read_wpsr(u8 *sr);
/*
 * Set FAST_SPIBAR BIOS Control BILD bit.
 */
void fast_spi_set_bios_interface_lock_down(void);
/*
 * Set FAST_SPIBAR BIOS Control LE bit.
 */
void fast_spi_set_lock_enable(void);
/*
 * Set FAST_SPIBAR BIOS Control EISS bit.
 */
void fast_spi_set_eiss(void);
/*
 * Set FAST_SPI opcode menu.
 */
void fast_spi_set_opcode_menu(void);
/*
 * Lock FAST_SPIBAR.
 */
void fast_spi_lock_bar(void);
/*
 * Set FAST_SPIBAR Soft Reset Data Register value.
 */
void fast_spi_set_strap_msg_data(uint32_t soft_reset_data);
/*
 * Returns bios_start and fills in size of the BIOS region.
 */
size_t fast_spi_get_bios_region(size_t *bios_size);
/*
 * Cache the memory-mapped BIOS region as write-protect type.
 */
void fast_spi_cache_bios_region(void);
/*
 * Program temporary BAR for FAST_SPI in case any of the stages before ramstage
 * need to access FAST_SPI MMIO regs. Ramstage will assign a new BAR during PCI
 * enumeration. Also, Disable the BIOS write protect and Enable Prefetching and
 * Caching.
 */
void fast_spi_early_init(uintptr_t spi_base_address);

/*
 * Fast SPI flash controller structure to allow SoCs to define bus-controller
 * mapping.
 */
extern const struct spi_ctrlr fast_spi_flash_ctrlr;

#endif	/* SOC_INTEL_COMMON_BLOCK_FAST_SPI_H */
