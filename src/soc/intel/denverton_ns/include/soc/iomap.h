/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_IOMAP_H_
#define _DENVERTON_NS_IOMAP_H_

/*
 * Memory Mapped IO bases.
 */

/* Northbridge BARs */
#define DEFAULT_MCHBAR 0xfed10000		    /* 16 KB */

/* Southbridge internal device IO BARs (Set to match FSP settings) */
#define DEFAULT_PMBASE 0x1800
#define DEFAULT_ACPI_BASE DEFAULT_PMBASE
#define ACPI_BASE_ADDRESS DEFAULT_PMBASE
#define DEFAULT_TCO_BASE 0x400

#define HPET_BASE_ADDRESS	0xfed00000

/* Southbridge internal device MEM BARs (Set to match FSP settings) */
#define DEFAULT_PCR_BASE 0xfd000000
#define DEFAULT_PWRM_BASE 0xfe000000
#define DEFAULT_HPET_ADDR CONFIG_HPET_ADDRESS
#define DEFAULT_SPI_BASE 0xfed01000

/* "VTD PLATFORM CONFIGURATION" (Set to match FSP settings) */
#define RMRR_USB_BASE_ADDRESS	0x3e2e0000
#define RMRR_USB_LIMIT_ADDRESS	0x3e2fffff

#endif /* _DENVERTON_NS_IOMAP_H_ */
