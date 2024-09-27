/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_IOMAP_H_
#define _SOC_SNOWRIDGE_IOMAP_H_

#define TCO_BASE_ADDRESS 0x400

/**
 * @brief PMC PciCfgSpace is not PCI compliant. Intel FSP will hide the PMC controller to avoid
 * external software or OS from corrupting the BAR addresses. Intel® FSP will program the PMC
 * controller I/O and MMIO BARs with the following addresses. Use these addresses in the
 * bootloader code instead of reading from the PMC controller. This process takes place during
 * Intel FSP-S, @sa SnowRidge FSP Integration Guide.
 */
#define ACPI_BASE_ADDRESS     0x00000500
#define PCH_PWRM_BASE_ADDRESS 0xfe000000

#define SPI_BASE_ADDRESS 0xfe010000
#define SPI_BASE_SIZE    0x00001000

#define RESERVED_BASE_ADDRESS 0xfc000000 /**< PCH reserved. */
#define RESERVED_BASE_SIZE    0x02c00000

#endif // _SOC_SNOWRIDGE_IOMAP_H_
