/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef __AMDBLOCKS_ACPIMMIO_MAP_H__
#define __AMDBLOCKS_ACPIMMIO_MAP_H__

/* IO index/data for accessing PMIO prior to enabling MMIO decode */
#define PM_INDEX			0xcd6
#define PM_DATA				0xcd7

/* Earlier devices enable the decode in PMx24 instead.  All discrete FCHs and
 * the Kabini SoC fall into this category.  Kabini's successor, Mullins, uses
 * this newer method.
 */

#define ACPIMMIO_DECODE_REGISTER_24 0x24
#define   PM_24_ACPIMMIO_DECODE_EN	BIT(0)

#define ACPIMMIO_DECODE_REGISTER_04 0x4
#define   PM_04_BIOSRAM_DECODE_EN	BIT(0)
#define   PM_04_ACPIMMIO_DECODE_EN	BIT(1)


/* MMIO register blocks are at fixed offsets from 0xfed80000 and are enabled
 * in PMx24[0] (older implementations) and PMx04[1] (newer implementations).
 * PM registers are also accessible via IO CD6/CD7.
 *
 * All products do not support all blocks below, however AMD has avoided
 * redefining addresses and consumes new ranges as necessary.
 *
 * Definitions within each block are not guaranteed to remain consistent
 * across family/model products.
 */

#define AMD_SB_ACPI_MMIO_ADDR		0xfed80000
#define ACPIMMIO_SM_PCI_BASE		0xfed80000
#define ACPIMMIO_GPIO_BASE_100		0xfed80100
#define ACPIMMIO_SMI_BASE		0xfed80200
#define ACPIMMIO_PMIO_BASE		0xfed80300
#define ACPIMMIO_PMIO2_BASE		0xfed80400
#define ACPIMMIO_BIOSRAM_BASE		0xfed80500
#define ACPIMMIO_CMOSRAM_BASE		0xfed80600
#define ACPIMMIO_CMOS_BASE		0xfed80700
#define ACPIMMIO_ACPI_BASE		0xfed80800
#define ACPIMMIO_ASF_BASE		0xfed80900
#define ACPIMMIO_SMBUS_BASE		0xfed80a00
#define ACPIMMIO_WDT_BASE		0xfed80b00
#define ACPIMMIO_HPET_BASE		0xfed80c00
#define ACPIMMIO_IOMUX_BASE		0xfed80d00
#define ACPIMMIO_MISC_BASE		0xfed80e00
#define ACPIMMIO_DPVGA_BASE		0xfed81400
#define ACPIMMIO_GPIO0_BASE		0xfed81500
#define ACPIMMIO_GPIO1_BASE		0xfed81600
#define ACPIMMIO_GPIO2_BASE		0xfed81700
#define ACPIMMIO_XHCIPM_BASE		0xfed81c00
#define ACPIMMIO_ACDCTMR_BASE		0xfed81d00
#define ACPIMMIO_AOAC_BASE		0xfed81e00

#endif /* __AMDBLOCKS_ACPIMMIO_MAP_H__ */
