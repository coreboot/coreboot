/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <pc80/keyboard.h>

static const unsigned char qemu_i440fx_irqs[] = {
	11, 10, 10, 11,
	11, 10, 10, 11,
};

#define D0F0_PAM(x)		(0x59 + (x)) /* 0-6 */

static void qemu_nb_init(struct device *dev)
{
	/* Map memory at 0xc0000 - 0xfffff */
	int i;
	pci_or_config8(dev, D0F0_PAM(0), 0x30);
	for (i = 1; i <= 6; i++)
		pci_write_config8(dev, D0F0_PAM(i), 0x33);

	/* This sneaked in here, because Qemu does not emulate a SuperIO chip. */
	pc_keyboard_init(NO_AUX_DEVICE);

	/* setup IRQ routing */
	for (i = 0; i < 32; i++) {
		struct device *d = pcidev_on_root(i, 0);
		if (d)
			pci_assign_irqs(d, qemu_i440fx_irqs + (i % 4));
	}
}

static void qemu_nb_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	if (CONFIG(ARCH_RAMSTAGE_X86_64)) {
		/* Reserve page tables in DRAM. FIXME: Remove once x86_64 page tables reside in CBMEM */
		reserved_ram_range(dev, 0, CONFIG_ARCH_X86_64_PGTBL_LOC, 6 * 0x1000);
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
	.device = 0x1237,
};
