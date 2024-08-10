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

	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, isa_bus, 0x00, MP_APIC_ALL, 0x00);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x00, MP_APIC_ALL, 0x01);
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x01, 0x00, ioapic_id, 0x10); /* PCIe root 0.01.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x02, 0x00, ioapic_id, 0x10); /* VGA       0.02.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1b, 0x00, ioapic_id, 0x16); /* HD Audio  0:1b.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x00, ioapic_id, 0x11); /* PCIe      0:1c.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x01, ioapic_id, 0x10); /* PCIe      0:1c.1 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x02, ioapic_id, 0x12); /* PCIe      0:1c.2 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x03, ioapic_id, 0x13); /* PCIe      0:1c.3 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x00, ioapic_id, 0x15); /* USB       0:1d.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x01, ioapic_id, 0x13); /* USB       0:1d.1 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x02, ioapic_id, 0x12); /* USB       0:1d.2 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x03, ioapic_id, 0x10); /* USB       0:1d.3 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1f, 0x00, ioapic_id, 0x12); /* LPC       0:1f.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1f, 0x01, ioapic_id, 0x13); /* IDE       0:1f.1 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1f, 0x03, ioapic_id, 0x10); /* SATA      0:1f.3 */
	smp_write_pci_intsrc(mc, mp_INT, 0x03, 0x03, 0x00, ioapic_id, 0x13); /* Firewire  3:03.0 */

	mptable_lintsrc(mc, isa_bus);
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
