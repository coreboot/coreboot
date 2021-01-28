/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <pc80/keyboard.h>

#include "q35.h"

static const unsigned char qemu_q35_irqs[] = {
	10, 10, 11, 11,
	10, 10, 11, 11,
};

static void qemu_nb_init(struct device *dev)
{
	/* Map memory at 0xc0000 - 0xfffff */
	int i;
	uint8_t v = pci_read_config8(dev, D0F0_PAM(0));
	v |= 0x30;
	pci_write_config8(dev, D0F0_PAM(0), v);
	pci_write_config8(dev, D0F0_PAM(1), 0x33);
	pci_write_config8(dev, D0F0_PAM(2), 0x33);
	pci_write_config8(dev, D0F0_PAM(3), 0x33);
	pci_write_config8(dev, D0F0_PAM(4), 0x33);
	pci_write_config8(dev, D0F0_PAM(5), 0x33);
	pci_write_config8(dev, D0F0_PAM(6), 0x33);

	/* This sneaked in here, because Qemu does not emulate a SuperIO chip. */
	pc_keyboard_init(NO_AUX_DEVICE);

	/* setup IRQ routing for pci slots */
	for (i = 0; i < 25; i++)
		pci_assign_irqs(pcidev_on_root(i, 0), qemu_q35_irqs + (i % 4));
	/* setup IRQ routing southbridge devices */
	for (i = 25; i < 32; i++)
		pci_assign_irqs(pcidev_on_root(i, 0), qemu_q35_irqs);
}

static void qemu_nb_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	/* reserve mmconfig */
	fixed_mem_resource(dev, 2, CONFIG_MMCONF_BASE_ADDRESS >> 10, 0x10000000 >> 10,
			   IORESOURCE_RESERVE);

	if (CONFIG(ARCH_RAMSTAGE_X86_64)) {
		/* Reserve page tables in DRAM. FIXME: Remove once x86_64 page tables reside in CBMEM */
		reserved_ram_resource(dev, 0, CONFIG_ARCH_X86_64_PGTBL_LOC / KiB,
			(6 * 0x1000) / KiB);
	}
}


static struct device_operations nb_operations = {
	.read_resources   = qemu_nb_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = qemu_nb_init,
};

static const struct pci_driver nb_driver __pci_driver = {
	.ops = &nb_operations,
	.vendor = 0x8086,
	.device = 0x29c0,
};
