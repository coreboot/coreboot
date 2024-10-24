/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/ioapic.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/acpi.h>
#include <soc/util.h>
#include <soc/pci_devs.h>
#include <stdint.h>

static uintptr_t xeonsp_ioapic_bases[CONFIG_MAX_SOCKET * MAX_IIO_STACK + 1];

size_t soc_get_ioapic_info(const uintptr_t *ioapic_bases[])
{
	struct device *dev = NULL;
	int index = 0;

	*ioapic_bases = xeonsp_ioapic_bases;

	/*
	 * Stack 0 has non-PCH IOAPIC and PCH IOAPIC.
	 * The IIO IOAPIC is placed at 0x1000 from the reported base.
	 */
	xeonsp_ioapic_bases[index++] = IO_APIC_ADDR;

	while ((dev = dev_find_class(PCI_CLASS_SYSTEM_PIC << 8, dev))) {
		if (!is_pci_ioapic(dev))
			continue;

		u16 abar = pci_read_config16(dev, APIC_ABAR);
		if (!abar)
			continue;
		const u32 addr = IO_APIC_ADDR | ((abar & 0xfff) << 8);

		printk(BIOS_DEBUG, "%s: %s: IOAPIC Address: 0x%x\n",
		       __func__, dev_path(dev), addr);
		xeonsp_ioapic_bases[index++] = addr;
	}

	return index;
}
