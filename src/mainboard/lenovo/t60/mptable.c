/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int isa_bus;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	mptable_write_buses(mc, NULL, &isa_bus);

	/* I/O APICs:	APIC ID	Version	State		Address */
	smp_write_ioapic(mc, 2, 0x20, VIO_APIC_VADDR);

	/* Legacy Interrupts */
	mptable_add_isa_interrupts(mc, isa_bus, 0x2, 0);

	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, isa_bus, 0x00, MP_APIC_ALL, 0x00);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, isa_bus, 0x00, MP_APIC_ALL, 0x01);
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x01, 0x00, 0x02, 0x10); /* PCIe root 0.01.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x02, 0x00, 0x02, 0x10); /* VGA       0.02.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1b, 0x00, 0x02, 0x11); /* HD Audio  0:1b.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x00, 0x02, 0x14); /* PCIe      0:1c.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x01, 0x02, 0x15); /* PCIe      0:1c.1 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x02, 0x02, 0x16); /* PCIe      0:1c.2 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1c, 0x03, 0x02, 0x17); /* PCIe      0:1c.3 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x00, 0x02, 0x10); /* USB       0:1d.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x01, 0x02, 0x11); /* USB       0:1d.1 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x02, 0x02, 0x12); /* USB       0:1d.2 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1d, 0x03, 0x02, 0x13); /* USB       0:1d.3 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1f, 0x00, 0x02, 0x17); /* LPC       0:1f.0 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1f, 0x01, 0x02, 0x10); /* IDE       0:1f.1 */
	smp_write_pci_intsrc(mc, mp_INT, 0x00, 0x1f, 0x02, 0x02, 0x10); /* SATA      0:1f.2 */
	smp_write_pci_intsrc(mc, mp_INT, 0x06, 0x00, 0x00, 0x02, 0x10); /* Cardbus   6:00.0 */

	mptable_lintsrc(mc, isa_bus);
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
