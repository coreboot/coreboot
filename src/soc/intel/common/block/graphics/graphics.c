/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corp.
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

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/graphics.h>
#include <soc/pci_devs.h>

/* SoC Overrides */
__weak void graphics_soc_init(struct device *dev)
{
	/*
	 * User needs to implement SoC override in case wishes
	 * to perform certain specific graphics initialization
	 * along with pci_dev_init(dev)
	 */
	pci_dev_init(dev);
}

static int is_graphics_disabled(struct device *dev)
{
	/* Check if Graphics PCI device is disabled */
	if (!dev || !dev->enabled)
		return 1;

	return 0;
}

static uintptr_t graphics_get_bar(struct device *dev, unsigned long index)
{
	struct resource *gm_res;

	gm_res = find_resource(dev, index);
	if (!gm_res)
		return 0;

	return gm_res->base;
}

uintptr_t graphics_get_memory_base(void)
{
	uintptr_t memory_base;
	struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);

	if (is_graphics_disabled(dev))
		return 0;
	/*
	 * GFX PCI config space offset 0x18 know as Graphics
	 * Memory Range Address (GMADR)
	 */
	memory_base = graphics_get_bar(dev, PCI_BASE_ADDRESS_2);
	if (!memory_base)
		die_with_post_code(POST_HW_INIT_FAILURE,
				   "GMADR is not programmed!");

	return memory_base;
}

static uintptr_t graphics_get_gtt_base(void)
{
	static uintptr_t gtt_base;
	struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);

	if (is_graphics_disabled(dev))
		die("IGD is disabled!");
	/*
	 * GFX PCI config space offset 0x10 know as Graphics
	 * Translation Table Memory Mapped Range Address
	 * (GTTMMADR)
	 */
	if (!gtt_base) {
		gtt_base = graphics_get_bar(dev, PCI_BASE_ADDRESS_0);
		if (!gtt_base)
			die_with_post_code(POST_HW_INIT_FAILURE,
					   "GTTMMADR is not programmed!");
	}
	return gtt_base;
}

uint32_t graphics_gtt_read(unsigned long reg)
{
	return read32((void *)(graphics_get_gtt_base() + reg));
}

void graphics_gtt_write(unsigned long reg, uint32_t data)
{
	write32((void *)(graphics_get_gtt_base() + reg), data);
}

void graphics_gtt_rmw(unsigned long reg, uint32_t andmask, uint32_t ormask)
{
	uint32_t val = graphics_gtt_read(reg);
	val &= andmask;
	val |= ormask;
	graphics_gtt_write(reg, val);
}

static const struct device_operations graphics_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= graphics_soc_init,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables	= graphics_soc_write_acpi_opregion,
#endif
	.scan_bus		= scan_generic_bus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_IGD_HD_505,
	PCI_DEVICE_ID_INTEL_APL_IGD_HD_500,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_1,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_2,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_3,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULX_4,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_1,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_2,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_3,
	PCI_DEVICE_ID_INTEL_CNL_GT2_ULT_4,
	PCI_DEVICE_ID_INTEL_GLK_IGD,
	PCI_DEVICE_ID_INTEL_GLK_IGD_EU12,
	PCI_DEVICE_ID_INTEL_WHL_GT1_ULT_1,
	PCI_DEVICE_ID_INTEL_WHL_GT2_ULT_1,
	PCI_DEVICE_ID_INTEL_KBL_GT1_SULTM,
	PCI_DEVICE_ID_INTEL_KBL_GT1_SHALM_1,
	PCI_DEVICE_ID_INTEL_KBL_GT1_SHALM_2,
	PCI_DEVICE_ID_INTEL_KBL_GT1_SSRVM,
	PCI_DEVICE_ID_INTEL_KBL_GT1F_DT2,
	PCI_DEVICE_ID_INTEL_KBL_GT2_SULXM,
	PCI_DEVICE_ID_INTEL_KBL_GT2_SULTM,
	PCI_DEVICE_ID_INTEL_KBL_GT2_SULTMR,
	PCI_DEVICE_ID_INTEL_KBL_GT2_SSRVM,
	PCI_DEVICE_ID_INTEL_KBL_GT2_SWSTM,
	PCI_DEVICE_ID_INTEL_KBL_GT2_SHALM,
	PCI_DEVICE_ID_INTEL_KBL_GT2_DT2P2,
	PCI_DEVICE_ID_INTEL_KBL_GT2F_SULTM,
	PCI_DEVICE_ID_INTEL_KBL_GT3E_SULTM_1,
	PCI_DEVICE_ID_INTEL_KBL_GT3E_SULTM_2,
	PCI_DEVICE_ID_INTEL_KBL_GT4_SHALM,
	PCI_DEVICE_ID_INTEL_AML_GT2_ULX,
	PCI_DEVICE_ID_INTEL_SKL_GT1F_DT2,
	PCI_DEVICE_ID_INTEL_SKL_GT1_SULTM,
	PCI_DEVICE_ID_INTEL_SKL_GT2_DT2P1,
	PCI_DEVICE_ID_INTEL_SKL_GT2_SULXM,
	PCI_DEVICE_ID_INTEL_SKL_GT2_SULTM,
	PCI_DEVICE_ID_INTEL_SKL_GT2_SHALM,
	PCI_DEVICE_ID_INTEL_SKL_GT2_SWKSM,
	PCI_DEVICE_ID_INTEL_SKL_GT3_SULTM,
	PCI_DEVICE_ID_INTEL_SKL_GT3E_SULTM_1,
	PCI_DEVICE_ID_INTEL_SKL_GT3E_SULTM_2,
	PCI_DEVICE_ID_INTEL_SKL_GT3FE_SSRVM,
	PCI_DEVICE_ID_INTEL_SKL_GT4_SHALM,
	PCI_DEVICE_ID_INTEL_SKL_GT4E_SWSTM,
	PCI_DEVICE_ID_INTEL_CFL_H_GT2,
	PCI_DEVICE_ID_INTEL_CFL_H_XEON_GT2,
	PCI_DEVICE_ID_INTEL_CFL_S_GT2_1,
	PCI_DEVICE_ID_INTEL_CFL_S_GT2_2,
	PCI_DEVICE_ID_INTEL_CFL_S_GT2_3,
	PCI_DEVICE_ID_INTEL_CFL_S_GT2_4,
	PCI_DEVICE_ID_INTEL_CFL_U_GT2,
	PCI_DEVICE_ID_INTEL_ICL_GT0_ULT,
	PCI_DEVICE_ID_INTEL_ICL_GT0_5_ULT,
	PCI_DEVICE_ID_INTEL_ICL_GT1_ULT,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULX_0,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULX_1,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULT_1,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULX_2,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULT_2,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULX_3,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULT_3,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULX_4,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULT_4,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULX_5,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULT_5,
	PCI_DEVICE_ID_INTEL_ICL_GT2_ULX_6,
	PCI_DEVICE_ID_INTEL_ICL_GT3_ULT,
	PCI_DEVICE_ID_INTEL_CML_GT1_ULT_1,
	PCI_DEVICE_ID_INTEL_CML_GT1_ULT_2,
	PCI_DEVICE_ID_INTEL_CML_GT2_ULT_1,
	PCI_DEVICE_ID_INTEL_CML_GT2_ULT_2,
	PCI_DEVICE_ID_INTEL_CML_GT1_ULT_3,
	PCI_DEVICE_ID_INTEL_CML_GT1_ULT_4,
	PCI_DEVICE_ID_INTEL_CML_GT2_ULT_5,
	PCI_DEVICE_ID_INTEL_CML_GT2_ULT_6,
	PCI_DEVICE_ID_INTEL_CML_GT2_ULT_3,
	PCI_DEVICE_ID_INTEL_CML_GT2_ULT_4,
	PCI_DEVICE_ID_INTEL_CML_GT1_ULX_1,
	PCI_DEVICE_ID_INTEL_CML_GT2_ULX_1,
	PCI_DEVICE_ID_INTEL_CML_GT1_S_1,
	PCI_DEVICE_ID_INTEL_CML_GT1_S_2,
	PCI_DEVICE_ID_INTEL_CML_GT2_S_1,
	PCI_DEVICE_ID_INTEL_CML_GT2_S_2,
	PCI_DEVICE_ID_INTEL_CML_GT1_H_1,
	PCI_DEVICE_ID_INTEL_CML_GT1_H_2,
	PCI_DEVICE_ID_INTEL_CML_GT2_H_1,
	PCI_DEVICE_ID_INTEL_CML_GT2_H_2,
	PCI_DEVICE_ID_INTEL_CML_GT2_S_G0,
	PCI_DEVICE_ID_INTEL_CML_GT2_S_P0,
	PCI_DEVICE_ID_INTEL_CML_GT2_H_R0,
	PCI_DEVICE_ID_INTEL_CML_GT2_H_R1,
	PCI_DEVICE_ID_INTEL_TGL_GT0,
	PCI_DEVICE_ID_INTEL_TGL_GT2_ULT,
	PCI_DEVICE_ID_INTEL_TGL_GT2_ULX,
	PCI_DEVICE_ID_INTEL_TGL_GT3_ULT,
	PCI_DEVICE_ID_INTEL_JSL_PRE_PROD_GT0,
	PCI_DEVICE_ID_INTEL_EHL_GT1_1,
	PCI_DEVICE_ID_INTEL_EHL_GT2_1,
	PCI_DEVICE_ID_INTEL_EHL_GT1_2,
	PCI_DEVICE_ID_INTEL_EHL_GT2_2,
	PCI_DEVICE_ID_INTEL_EHL_GT1_3,
	PCI_DEVICE_ID_INTEL_EHL_GT2_3,
	0,
};

static const struct pci_driver graphics_driver __pci_driver = {
	.ops		= &graphics_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
