/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/spi.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/gspi.h>
#include <intelblocks/spi.h>
#include <soc/pci_devs.h>
#include <spi-generic.h>

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{ .ctrlr = &fast_spi_flash_ctrlr, .bus_start = 0, .bus_end = 0 },
#if !ENV_SMM && CONFIG(SOC_INTEL_COMMON_BLOCK_GSPI)
	{ .ctrlr = &gspi_ctrlr, .bus_start = 1,
	  .bus_end =  1 + (CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX - 1)},
#endif
};

const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);

#define MMIO_BIOS_GPR0	0x98

union spi_bios_gpr0 {
	struct {
		/* Specified write protection is enabled */
		/*
		 * This field corresponds to flash address bits 26:12
		 * and specifies the lower limit of protected range.
		 */
		uint32_t protect_range_base:15;

		/* Specifies read protection is enabled */
		uint32_t read_protect_en:1;

		/*
		 * This field corresponds to flash address bits 26:12
		 * and specifies the upper limit of the protected range
		 */
		uint32_t protect_range_limit:15;

		uint32_t write_protect_en:1;
	} __packed fields;

	uint32_t data;
};

/* Read SPI BAR 0 from PCI configuration space */
static uintptr_t get_spi_bar(pci_devfn_t dev)
{
	uintptr_t bar;

	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	assert(bar != 0);
	/*
	 * Bits 31-12 are the base address as per EDS for SPI,
	 * Don't care about 0-11 bit
	 */
	return bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
}

static uint32_t spi_read_bar(pci_devfn_t dev, uint32_t offset)
{
	return read32p(get_spi_bar(dev) + offset);
}

static uint32_t spi_read_bios_gpr0(void)
{
	return spi_read_bar(PCH_DEV_SPI, MMIO_BIOS_GPR0);
}

static uint32_t spi_get_wp_cse_ro_start_offset(union spi_bios_gpr0 bios_gpr0)
{
	return bios_gpr0.fields.protect_range_base << 12;
}

static uint32_t spi_get_wp_cse_ro_limit(union spi_bios_gpr0 bios_gpr0)
{
	return bios_gpr0.fields.protect_range_limit << 12 | 0xfff;
}

bool is_spi_wp_cse_ro_en(void)
{
	union spi_bios_gpr0 bios_gpr0;

	bios_gpr0.data = spi_read_bios_gpr0();
	return !!bios_gpr0.fields.write_protect_en;
}

void spi_get_wp_cse_ro_range(uint32_t *base, uint32_t *limit)
{
	union spi_bios_gpr0 bios_gpr0;

	bios_gpr0.data = spi_read_bios_gpr0();
	*base = spi_get_wp_cse_ro_start_offset(bios_gpr0);
	*limit = spi_get_wp_cse_ro_limit(bios_gpr0);
}

static int spi_dev_to_bus(struct device *dev)
{
	return spi_soc_devfn_to_bus(dev->path.pci.devfn);
}

static struct spi_bus_operations spi_bus_ops = {
	.dev_to_bus			= &spi_dev_to_bus,
};

static struct device_operations spi_dev_ops = {
	.read_resources			= pci_dev_read_resources,
	.set_resources			= pci_dev_set_resources,
	.enable_resources		= pci_dev_enable_resources,
	.scan_bus			= scan_generic_bus,
	.ops_spi_bus			= &spi_bus_ops,
	.ops_pci			= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_HWSEQ_SPI,
	PCI_DID_INTEL_MTL_GSPI0,
	PCI_DID_INTEL_MTL_GSPI1,
	PCI_DID_INTEL_MTL_GSPI2,
	PCI_DID_INTEL_SPT_SPI1,
	PCI_DID_INTEL_SPT_SPI2,
	PCI_DID_INTEL_SPT_SPI3,
	PCI_DID_INTEL_APL_SPI0,
	PCI_DID_INTEL_APL_SPI1,
	PCI_DID_INTEL_APL_SPI2,
	PCI_DID_INTEL_GLK_SPI0,
	PCI_DID_INTEL_GLK_SPI1,
	PCI_DID_INTEL_GLK_SPI2,
	PCI_DID_INTEL_CNL_SPI0,
	PCI_DID_INTEL_CNL_SPI1,
	PCI_DID_INTEL_CNL_SPI2,
	PCI_DID_INTEL_CNL_HWSEQ_SPI,
	PCI_DID_INTEL_CNP_H_SPI0,
	PCI_DID_INTEL_CNP_H_SPI1,
	PCI_DID_INTEL_CNP_H_SPI2,
	PCI_DID_INTEL_CNP_H_HWSEQ_SPI,
	PCI_DID_INTEL_LWB_SPI,
	PCI_DID_INTEL_LWB_SPI_SUPER,
	PCI_DID_INTEL_ICP_SPI0,
	PCI_DID_INTEL_ICP_SPI1,
	PCI_DID_INTEL_ICP_SPI2,
	PCI_DID_INTEL_ICP_HWSEQ_SPI,
	PCI_DID_INTEL_CMP_SPI0,
	PCI_DID_INTEL_CMP_SPI1,
	PCI_DID_INTEL_CMP_SPI2,
	PCI_DID_INTEL_CMP_HWSEQ_SPI,
	PCI_DID_INTEL_CMP_H_SPI0,
	PCI_DID_INTEL_CMP_H_SPI1,
	PCI_DID_INTEL_CMP_H_SPI2,
	PCI_DID_INTEL_CMP_H_HWSEQ_SPI,
	PCI_DID_INTEL_TGP_SPI0,
	PCI_DID_INTEL_TGP_GSPI0,
	PCI_DID_INTEL_TGP_GSPI1,
	PCI_DID_INTEL_TGP_GSPI2,
	PCI_DID_INTEL_TGP_GSPI3,
	PCI_DID_INTEL_TGP_GSPI4,
	PCI_DID_INTEL_TGP_GSPI5,
	PCI_DID_INTEL_TGP_GSPI6,
	PCI_DID_INTEL_TGP_H_SPI0,
	PCI_DID_INTEL_TGP_H_GSPI0,
	PCI_DID_INTEL_TGP_H_GSPI1,
	PCI_DID_INTEL_TGP_H_GSPI2,
	PCI_DID_INTEL_TGP_H_GSPI3,
	PCI_DID_INTEL_MCC_SPI0,
	PCI_DID_INTEL_MCC_GSPI0,
	PCI_DID_INTEL_MCC_GSPI1,
	PCI_DID_INTEL_MCC_GSPI2,
	PCI_DID_INTEL_JSP_SPI0,
	PCI_DID_INTEL_JSP_SPI1,
	PCI_DID_INTEL_JSP_SPI2,
	PCI_DID_INTEL_JSP_HWSEQ_SPI,
	PCI_DID_INTEL_ADP_P_HWSEQ_SPI,
	PCI_DID_INTEL_ADP_S_HWSEQ_SPI,
	PCI_DID_INTEL_ADP_M_N_HWSEQ_SPI,
	PCI_DID_INTEL_ADP_P_SPI0,
	PCI_DID_INTEL_ADP_P_SPI1,
	PCI_DID_INTEL_ADP_P_SPI2,
	PCI_DID_INTEL_ADP_P_SPI3,
	PCI_DID_INTEL_ADP_P_SPI4,
	PCI_DID_INTEL_ADP_P_SPI5,
	PCI_DID_INTEL_ADP_P_SPI6,
	PCI_DID_INTEL_ADP_S_SPI0,
	PCI_DID_INTEL_ADP_S_SPI1,
	PCI_DID_INTEL_ADP_S_SPI2,
	PCI_DID_INTEL_ADP_S_SPI3,
	PCI_DID_INTEL_ADP_S_SPI4,
	PCI_DID_INTEL_ADP_S_SPI5,
	PCI_DID_INTEL_ADP_S_SPI6,
	PCI_DID_INTEL_ADP_M_N_SPI0,
	PCI_DID_INTEL_ADP_M_N_SPI1,
	PCI_DID_INTEL_ADP_M_SPI2,
	PCI_DID_INTEL_SPR_HWSEQ_SPI,
	PCI_DID_INTEL_DNV_SPI,
	0
};

static const struct pci_driver pch_spi __pci_driver = {
	.ops				= &spi_dev_ops,
	.vendor				= PCI_VID_INTEL,
	.devices			= pci_device_ids,
};
