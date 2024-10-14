/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	struct device *riser = nullptr, *firewire = nullptr;
	int firewire_bus = 0, riser_bus = 0, isa_bus;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc);

	smp_write_processors(mc);

	firewire = dev_find_device(0x104c, 0x8023, 0);
	if (firewire) {
		firewire_bus = firewire->upstream->secondary;
		printk(BIOS_SPEW, "Firewire device is on bus %x\n",
				firewire_bus);
	}

	// If a riser card is used, this riser is detected on bus 4, so its secondary bus is the
	// highest bus number on the pci bus.
	riser = dev_find_device(0x3388, 0x0021, 0);
	if (!riser)
		riser = dev_find_device(0x3388, 0x0022, 0);
	if (riser) {
		riser_bus = riser->downstream->secondary;
		printk(BIOS_SPEW, "Riser bus is %x\n", riser_bus);
	}

	mptable_write_buses(mc, nullptr, &isa_bus);

	/* I/O APICs:	APIC ID	Version	State		Address */
	u8 ioapic_id = smp_write_ioapic_from_hw(mc, IO_APIC_ADDR);

	/* Legacy Interrupts */

	/* I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x0, ioapic_id, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x1, ioapic_id, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x0, ioapic_id, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x3, ioapic_id, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x4, ioapic_id, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, isa_bus, 0x8, ioapic_id, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x9, ioapic_id, 0x9);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0xa, ioapic_id, 0xa);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0xb, ioapic_id, 0xb);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0xc, ioapic_id, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0xd, ioapic_id, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0xe, ioapic_id, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT | MP_IRQ_POLARITY_DEFAULT, isa_bus, 0xf, ioapic_id, 0xf);

	/* Builtin devices on Bus 0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x01, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x02, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1f, 1, ioapic_id, 0x13);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 0, ioapic_id, 0x17);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 1, ioapic_id, 0x13);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 2, ioapic_id, 0x12);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 3, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1b, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1c, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1c, 1, ioapic_id, 0x11);

	/* Internal PCI bus (Firewire, PCI slot) */
	if (firewire) {
		smp_write_pci_intsrc(mc, mp_INT, firewire_bus, 0x00, 0, ioapic_id, 0x10);
		smp_write_pci_intsrc(mc, mp_INT, firewire_bus, 0x01, 0, ioapic_id, 0x14);
	}

	if (riser) {
		/* Old riser card */
		// riser slot top 5:8.0
		smp_write_pci_intsrc(mc, mp_INT, riser_bus, 0x08, 0, ioapic_id, 0x14);
		// riser slot middle 5:9.0
		smp_write_pci_intsrc(mc, mp_INT, riser_bus, 0x09, 0, ioapic_id, 0x15);
		// riser slot bottom 5:a.0
		smp_write_pci_intsrc(mc, mp_INT, riser_bus, 0x0a, 0, ioapic_id, 0x16);

		/* New Riser Card */
		smp_write_pci_intsrc(mc, mp_INT, riser_bus, 0x0c, 0, ioapic_id, 0x14);
		smp_write_pci_intsrc(mc, mp_INT, riser_bus, 0x0d, 0, ioapic_id, 0x15);
		smp_write_pci_intsrc(mc, mp_INT, riser_bus, 0x0e, 0, ioapic_id, 0x16);
	}

	/* PCIe slot */
	smp_write_pci_intsrc(mc, mp_INT, 0x1, 0x00, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x1, 0x00, 1, ioapic_id, 0x11);

	/* Onboard Ethernet */
	smp_write_pci_intsrc(mc, mp_INT, 0x2, 0x00, 0, ioapic_id, 0x10);

	/* Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	mptable_lintsrc(mc, isa_bus);

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 1);
	return (unsigned long)smp_write_config_table(v);
}
