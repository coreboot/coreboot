/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <string.h>
#include <stdint.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include <southbridge/amd/common/amd_pci_util.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;

	/*
	 * By the time this function gets called, the IOAPIC registers
	 * have been written so they can be read to get the correct
	 * APIC ID and Version
	 */
	u8 ioapic_id = (io_apic_read(VIO_APIC_VADDR, 0x00) >> 24);
	u8 ioapic_ver = (io_apic_read(VIO_APIC_VADDR, 0x01) & 0xFF);

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);
	memcpy(mc->mpc_oem, "AMD     ", 8);

	smp_write_processors(mc);

	mptable_write_buses(mc, NULL, &bus_isa);

	/* I/O APICs:   APIC ID Version State   Address */
	smp_write_ioapic(mc, ioapic_id, ioapic_ver, VIO_APIC_VADDR);

	/* I/O Ints:    Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
	smp_write_lintsrc(mc, (type), MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, bus_isa, (intr), (apicid), (pin));

	mptable_add_isa_interrupts(mc, bus_isa, ioapic_id, 0);

	/* PCI interrupts are level triggered, and are
	 * associated with a specific bus/device/function tuple.
	 */
#define PCI_INT(bus, dev, fn, pin) \
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, (bus), (((dev)<<2)|(fn)), ioapic_id, (pin))

	/* APU Internal Graphic Device*/
	PCI_INT(0x0, 0x01, 0x0, intr_data_ptr[0x02]);
	PCI_INT(0x0, 0x01, 0x1, intr_data_ptr[0x03]);

	//PCI_INT(0x0, 0x14, 0x1, 0x11); /* IDE. */
	PCI_INT(0x0, 0x14, 0x0, 0x10);
	/* Southbridge HD Audio: */
	PCI_INT(0x0, 0x14, 0x2, 0x12);

	PCI_INT(0x0, 0x12, 0x0, intr_data_ptr[0x30]); /* USB */
	PCI_INT(0x0, 0x12, 0x1, intr_data_ptr[0x31]);
	PCI_INT(0x0, 0x13, 0x0, intr_data_ptr[0x32]);
	PCI_INT(0x0, 0x13, 0x1, intr_data_ptr[0x33]);
	PCI_INT(0x0, 0x16, 0x0, intr_data_ptr[0x34]);
	PCI_INT(0x0, 0x16, 0x1, intr_data_ptr[0x35]);

	/* sata */
	PCI_INT(0x0, 0x11, 0x0, intr_data_ptr[0x41]);

	/* on board NIC & Slot PCIE.  */

	/* PCI slots */
	struct device *dev = pcidev_on_root(0x14, 4);
	if (dev && dev->enabled) {
		u8 bus_pci = dev->link_list->secondary;
		/* PCI_SLOT 0. */
		PCI_INT(bus_pci, 0x5, 0x0, 0x14);
		PCI_INT(bus_pci, 0x5, 0x1, 0x15);
		PCI_INT(bus_pci, 0x5, 0x2, 0x16);
		PCI_INT(bus_pci, 0x5, 0x3, 0x17);

		/* PCI_SLOT 1. */
		PCI_INT(bus_pci, 0x6, 0x0, 0x15);
		PCI_INT(bus_pci, 0x6, 0x1, 0x16);
		PCI_INT(bus_pci, 0x6, 0x2, 0x17);
		PCI_INT(bus_pci, 0x6, 0x3, 0x14);

		/* PCI_SLOT 2. */
		PCI_INT(bus_pci, 0x7, 0x0, 0x16);
		PCI_INT(bus_pci, 0x7, 0x1, 0x17);
		PCI_INT(bus_pci, 0x7, 0x2, 0x14);
		PCI_INT(bus_pci, 0x7, 0x3, 0x15);
	}

	/* PCIe PortA */
	PCI_INT(0x0, 0x15, 0x0, 0x10);
	/* PCIe PortB */
	PCI_INT(0x0, 0x15, 0x1, 0x11);
	/* PCIe PortC */
	PCI_INT(0x0, 0x15, 0x2, 0x12);
	/* PCIe PortD */
	PCI_INT(0x0, 0x15, 0x3, 0x13);

	/*Local Ints:   Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN# */
	IO_LOCAL_INT(mp_ExtINT, 0x0, MP_APIC_ALL, 0x0);
	IO_LOCAL_INT(mp_NMI, 0x0, MP_APIC_ALL, 0x1);
	/* There is no extension information... */

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
