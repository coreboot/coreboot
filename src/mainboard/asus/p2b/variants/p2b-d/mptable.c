/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>

static void *smp_write_config_table(void *v)
{
	int isa_bus;
	struct mp_config_table *mc;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc);

	smp_write_processors(mc);

	mptable_write_buses(mc, NULL, &isa_bus);

	/* External Intel 82093AA IOAPIC. */
	u8 ioapic_id = smp_write_ioapic_from_hw(mc, VIO_APIC_VADDR);

	/* Legacy Interrupts */
	mptable_add_isa_interrupts(mc, isa_bus, ioapic_id, 0);

	/* I/O Ints:         Type       Trigger                Polarity              Bus ID   IRQ   APIC ID      PIN# */
	smp_write_pci_intsrc(mc, mp_INT, 0x0, 0x04, 3, ioapic_id, 0x13); /* UHCI */

	/* Local Ints:       Type       Trigger                Polarity              Bus ID   IRQ   APIC ID      PIN# */
	mptable_lintsrc(mc, isa_bus);

	/* Compute the checksums. */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
