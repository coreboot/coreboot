/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
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

#include <console/console.h>
#include <soc/iomap.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/systemagent.h>

static uint32_t get_bar(device_t dev, unsigned int index)
{
	uint32_t bar;

	bar = pci_read_config32(dev, index);

	/* If not enabled return 0 else strip enabled bit */
	return (bar & 1) ? (bar & ~1) : 0;
}

static int mc_add_fixed_mmio_resources(device_t dev, int index)
{
	unsigned long addr;

	/* PCI extended config region */
	addr = ALIGN_DOWN(get_bar(dev, PCIEXBAR), 256*MiB) / KiB;
	mmio_resource(dev, index++, addr, CONFIG_SA_PCIEX_LENGTH / KiB);

	/* Memory Controller Hub */
	addr = ALIGN_DOWN(get_bar(dev, MCHBAR), 32*KiB) / KiB;
	mmio_resource(dev, index++, addr, MCH_BASE_SIZE / KiB);

	return index;
}

static bool is_imr_enabled(uint32_t imr_base_reg)
{
	return !!(imr_base_reg & (1 << 31));
}

static void imr_resource(device_t dev, int idx, uint32_t base, uint32_t mask)
{
	uint32_t base_k, size_k;
	/* Bits 28:0 encode the base address bits 38:10, hence the KiB unit. */
	base_k = (base & 0x0fffffff);
	/* Bits 28:0 encode the AND mask used for comparison, in KiB. */
	size_k = ((~mask & 0x0fffffff) + 1);
	/*
	 * IMRs sit in lower DRAM. Mark them cacheable, otherwise we run
	 * out of MTRRs. Memory reserved by IMRs is not usable for host
	 * so mark it reserved.
	 */
	reserved_ram_resource(dev, idx, base_k, size_k);
}

static int mc_add_imr_resources(device_t dev, int index)
{
	uint8_t *mchbar;
	size_t i, imr_offset;
	uint32_t base, mask;

	mchbar = (void *)(ALIGN_DOWN(get_bar(dev, MCHBAR), 32*KiB));

	for (i = 0; i < MCH_NUM_IMRS; i++) {
		imr_offset = i * MCH_IMR_PITCH;
		base = read32(mchbar + imr_offset + MCHBAR_IMR0BASE);
		mask = read32(mchbar + imr_offset + MCHBAR_IMR0MASK);

		if (is_imr_enabled(base))
			imr_resource(dev, index++, base, mask);
	}

	return index;
}


static int mc_add_dram_resources(device_t dev, int index)
{
	unsigned long base_k, size_k;
	uint32_t bgsm, bdsm, tolud, tseg;
	uint64_t touud;

	bgsm  = ALIGN_DOWN(pci_read_config32(dev, BGSM), MiB);
	bdsm  = ALIGN_DOWN(pci_read_config32(dev, BDSM), MiB);
	tolud = ALIGN_DOWN(pci_read_config32(dev, TOLUD), MiB);
	tseg  = ALIGN_DOWN(pci_read_config32(dev, TSEG), MiB);

	/* TOUUD is naturally a 64 bit integer */
	touud = pci_read_config32(dev, TOUUD + sizeof(uint32_t));
	touud <<= 32;
	touud |= ALIGN_DOWN(pci_read_config32(dev, TOUUD), MiB);

	/* 0 -> 0xa0000: 640kb of DOS memory. Not enough for anybody nowadays */
	ram_resource(dev, index++, 0, 640);

	/* 0xa0000 - 0xbffff: legacy VGA */
	mmio_resource(dev, index++, 640, 128);

	/* 0xc0000 -> 0xfffff: leave without e820 entry, as it has special uses
	 * 0x100000 -> top_of_ram
	 */
	base_k = 1024;
	size_k = (tseg / KiB) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* TSEG -> BGSM */
	reserved_ram_resource(dev, index++, tseg / KiB, (bgsm - tseg) / KiB);

	/* BGSM -> BDSM */
	mmio_resource(dev, index++, bgsm / KiB, (bdsm - bgsm) / KiB);

	/* BDSM -> TOLUD */
	mmio_resource(dev, index++, bdsm / KiB, (tolud - bdsm) / KiB);

	/* 4G -> TOUUD */
	base_k = 4ULL*GiB / KiB;
	size_k = (touud / KiB) - base_k;
	ram_resource(dev, index++, base_k, size_k);


	return index;
}

static void northbridge_read_resources(device_t dev)
{

	int index = 0;
	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* Add all fixed MMIO resources. */
	index = mc_add_fixed_mmio_resources(dev, index);

	/* Calculate and add DRAM resources. */
	index = mc_add_dram_resources(dev, index);

	/* Add the isolated memory ranges (IMRs). */
	mc_add_imr_resources(dev, index);

}

static struct device_operations northbridge_ops = {
	.read_resources   = northbridge_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = DEVICE_NOOP,
	.enable           = DEVICE_NOOP
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_NB,
	PCI_DEVICE_ID_INTEL_GLK_NB,
	0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops     = &northbridge_ops,
	.vendor  = PCI_VENDOR_ID_INTEL,
	.devices  = pci_device_ids,
};
