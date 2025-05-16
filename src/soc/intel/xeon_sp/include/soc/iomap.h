/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_IOMAP_H_
#define _SOC_IOMAP_H_

#define MAP_ENTRY(reg_, is_64_, is_limit_, mask_bits_, desc_) \
	{                                                           \
		.reg = reg_, .is_64_bit = is_64_, .is_limit = is_limit_,  \
		.mask_bits = mask_bits_, .description = desc_,            \
	}

#define MAP_ENTRY_BASE_64(reg_, desc_) MAP_ENTRY(reg_, 1, 0, 12, desc_)
#define MAP_ENTRY_LIMIT_64(reg_, mask_bits_, desc_) MAP_ENTRY(reg_, 1, 1, mask_bits_, desc_)
#define MAP_ENTRY_BASE_32(reg_, desc_) MAP_ENTRY(reg_, 0, 0, 12, desc_)
#define MAP_ENTRY_LIMIT_32(reg_, mask_bits_, desc_) MAP_ENTRY(reg_, 0, 1, mask_bits_, desc_)

// SPI BAR0 MMIO base address
#define SPI_BASE_ADDRESS             CONFIG_INTEL_SPI_BASE_ADDRESS
#define SPI_BASE_SIZE                0x1000

#define TCO_BASE_ADDRESS             0x400
#define ACPI_BASE_ADDRESS            CONFIG_INTEL_ACPI_BASE_ADDRESS
#define ACPI_BASE_SIZE		0x100

#define HECI1_BASE_ADDRESS	0xfed1a000

#define PCH_PWRM_BASE_ADDRESS   CONFIG_INTEL_PCH_PWRM_BASE_ADDRESS
#define PCH_PWRM_BASE_SIZE	0x10000

#define P2SB_BAR                     CONFIG_PCR_BASE_ADDRESS
#define GPIO_BASE_SIZE               0x10000

#endif /* _SOC_IOMAP_H_ */
