/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_FAST_SPI_H
#define SOC_INTEL_COMMON_BLOCK_FAST_SPI_H

#include <types.h>

/* Check if SPI transaction is pending */
int fast_spi_cycle_in_progress(void);
/*
 * Disable the BIOS write protect and Enable Prefetching and Caching.
 */
void fast_spi_init(void);
/*
 * Set FAST_SPIBAR BIOS Control BILD bit.
 */
void fast_spi_set_bios_interface_lock_down(void);
/*
 * Set FAST_SPIBAR BIOS Control LE bit.
 */
void fast_spi_set_lock_enable(void);
/*
 * Set FAST_SPIBAR BIOS Control Ext Bios LE bit.
 */
void fast_spi_set_ext_bios_lock_enable(void);
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
 * Set FAST_SPIBAR + DLOCK (0x0C) register bits to discrete lock the
 * FAST_SPI Protected Range (PR) registers.
 */
void fast_spi_pr_dlock(void);
/*
 * Set FAST_SPIBAR Soft Reset Data Register value.
 */
void fast_spi_set_strap_msg_data(uint32_t soft_reset_data);
/*
 * Returns bios_start and fills in size of the BIOS region.
 */
size_t fast_spi_get_bios_region(size_t *bios_size);
/*
 * Cache the memory-mapped BIOS region as write-protect type. In ramstage
 * this function needs to be called after the final MTRR solution has been
 * calculated.
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
/*
 * Clear SPI Synchronous SMI status bit and return its value.
 */
bool fast_spi_clear_sync_smi_status(void);
/*
 * Read SPI Write protect disable bit.
 */
bool fast_spi_wpd_status(void);
/*
 * Enable SPI Write protect.
 */
void fast_spi_enable_wp(void);
/*
 * Disable SPI Write protect.
 */
void fast_spi_disable_wp(void);
/*
 * Get base and size of extended BIOS decode window used at runtime in host address space. If
 * the BIOS region is not greater than 16MiB, then this function returns 0 for both base and
 * size.
 */
void fast_spi_get_ext_bios_window(uintptr_t *base, size_t *size);
/*
 * SOC function to get SPI PSF Destination Id
 */
uint32_t soc_get_spi_psf_destination_id(void);
/*
 * Add MTRR for extended BIOS region(when supported) to postcar frame
 */
struct postcar_frame;
void fast_spi_cache_ext_bios_postcar(struct postcar_frame *pcf);

#endif	/* SOC_INTEL_COMMON_BLOCK_FAST_SPI_H */
