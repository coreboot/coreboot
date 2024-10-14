/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int isa_bus;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc);

	smp_write_processors(mc);

	mptable_write_buses(mc, nullptr, &isa_bus);

	/* I/O APICs:	APIC ID	Version	State		Address */
	u8 ioapic_id = smp_write_ioapic_from_hw(mc, IO_APIC_ADDR);

	/* Legacy Interrupts */

	mptable_add_isa_interrupts(mc, isa_bus, ioapic_id, 0);

	/* Builtin devices on Bus 0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x02, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1f, 1, ioapic_id, 0x13);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 0, ioapic_id, 0x17);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 1, ioapic_id, 0x13);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 2, ioapic_id, 0x12);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1d, 3, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1b, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1c, 0, ioapic_id, 0x10);
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x1c, 1, ioapic_id, 0x11);

	/* Firewire 4:0.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x4, 0x00, 0, ioapic_id, 0x10);

	/* Old riser card */
	// riser slot top 5:8.0
	smp_write_pci_intsrc(mc, mp_INT, 0x5, 0x08, 0, ioapic_id, 0x14);
	// riser slot middle 5:9.0
	smp_write_pci_intsrc(mc, mp_INT, 0x5, 0x09, 0, ioapic_id, 0x15);
	// riser slot bottom 5:a.0
	smp_write_pci_intsrc(mc, mp_INT, 0x5, 0x0a, 0, ioapic_id, 0x16);

	/* New Riser Card */
	smp_write_pci_intsrc(mc, mp_INT, 0x5, 0x0c, 0, ioapic_id, 0x14);
	smp_write_pci_intsrc(mc, mp_INT, 0x5, 0x0d, 0, ioapic_id, 0x15);
	smp_write_pci_intsrc(mc, mp_INT, 0x5, 0x0e, 0, ioapic_id, 0x16);

	/* Onboard Ethernet */
	smp_write_pci_intsrc(mc, mp_INT, 0x1, 0x00, 0, ioapic_id, 0x10);

	/* Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	mptable_lintsrc(mc, isa_bus);

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
