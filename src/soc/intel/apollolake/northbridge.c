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
 */

#include <console/console.h>
#include <soc/iomap.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/northbridge.h>
#include <soc/pci_ids.h>

static uint32_t get_bar(device_t dev, unsigned int index)
{
	uint32_t bar;

	bar = pci_read_config32(dev, index);

	/* If not enabled return 0 else strip enabled bit */
	return (bar & 1) ? (bar & ~1) : 0;
}

static int mc_add_fixed_mmio_resources(device_t dev, int index)
{
	/* PCI extended config region */
	mmio_resource(dev, index++, ALIGN_DOWN(get_bar(dev, PCIEXBAR), 256*MiB),
						PCIEX_SIZE / KiB);

	/* Memory Controller Hub */
	mmio_resource(dev, index++, ALIGN_DOWN(get_bar(dev, MCHBAR), 32*KiB),
						MCH_BASE_SIZE / KiB);

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

	/* 0 - > 0xa0000: 640kb of DOS memory. Not enough for anybody nowadays */
	ram_resource(dev, index++, 0, 640);

	/* 0xc0000 -> top_of_ram, skipping the legacy VGA region */
	base_k = 768;
	size_k = (tseg / KiB) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* TSEG -> BGSM */
	reserved_ram_resource(dev, index++, tseg / KiB, (bgsm - tseg) / KiB);

	/* BGSM -> BDSM */
	mmio_resource(dev, index++, bgsm / KiB, (bdsm - bgsm) / KiB);

	/* BDSM -> TOLUD */
	mmio_resource(dev, index++, tolud / KiB, (tolud - bdsm) / KiB);

	/* 4G -> TOUUD */
	base_k = 4ULL*GiB / KiB;
	size_k = (touud / KiB) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* 0xa0000 - 0xbffff: legacy VGA */
	mmio_resource(dev, index++, 640, 128);

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
	mc_add_dram_resources(dev, index);
}

static struct device_operations northbridge_ops = {
	.read_resources   = northbridge_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = DEVICE_NOOP,
	.enable           = DEVICE_NOOP
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops     = &northbridge_ops,
	.vendor  = PCI_VENDOR_ID_INTEL,
	.device  = PCI_DEVICE_ID_APOLLOLAKE_NB
};
