#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	unsigned char bus_pxhd_1;
	unsigned char bus_pxhd_2;
	unsigned char bus_pxhd_3;
	unsigned char bus_pxhd_4;
	unsigned char bus_ich5r_1;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	{
		device_t dev;

		/* ich5r */
		dev = dev_find_slot(0, PCI_DEVFN(0x1e,0));
		if (dev) {
			bus_ich5r_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 0:1f.0, using defaults\n");

			bus_ich5r_1 = 9;
		}
		/* pxhd-1 */
		dev = dev_find_slot(2, PCI_DEVFN(0x0,0));
		if (dev) {
			bus_pxhd_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);

		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:00.1, using defaults\n");

			bus_pxhd_1 = 3;
		}
		/* pxhd-2 */
		dev = dev_find_slot(2, PCI_DEVFN(0x00,2));
		if (dev) {
			bus_pxhd_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);

		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:02.0, using defaults\n");

			bus_pxhd_2 = 4;
		}

		/* pxhd-3 */
		dev = dev_find_slot(5, PCI_DEVFN(0x0,0));
		if (dev) {
			bus_pxhd_3 = pci_read_config8(dev, PCI_SECONDARY_BUS);

		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:00.1, using defaults\n");

			bus_pxhd_3 = 6;
		}
		/* pxhd-4 */
		dev = dev_find_slot(5, PCI_DEVFN(0x00,2));
		if (dev) {
			bus_pxhd_4 = pci_read_config8(dev, PCI_SECONDARY_BUS);

		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:02.0, using defaults\n");

			bus_pxhd_4 = 7;
		}

	}

	mptable_write_buses(mc, NULL, &bus_isa);

	/* IOAPIC handling */

	smp_write_ioapic(mc, 2, 0x20, IO_APIC_ADDR);
	{
		struct resource *res;
		device_t dev;
		/* pxhd apic 3 */
		dev = dev_find_slot(2, PCI_DEVFN(0x00,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x03, 0x20, res->base);
			}
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 2:00.1\n");
		}
		/* pxhd apic 4 */
		dev = dev_find_slot(2, PCI_DEVFN(0x00,3));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x04, 0x20, res->base);
			}
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 2:00.3\n");
		}
		/* pxhd apic 5 */
		dev = dev_find_slot(5, PCI_DEVFN(0x00,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x05, 0x20, res->base);
			}
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 5:00.1\n");
		}
		/* pxhd apic 8 */
		dev = dev_find_slot(5, PCI_DEVFN(0x00,3));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x08, 0x20, res->base);
			}
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 5:00.3\n");
		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, 0x2, 0);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x74, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x76, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x77, 0x02, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x75, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x74, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x7c, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x7d, 0x02, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pxhd_2, 0x08, 0x04, 0x06);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pxhd_2, 0x09, 0x04, 0x07);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pxhd_3, 0x08, 0x05, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_pxhd_4, 0x08, 0x08, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		(bus_isa - 1), 0x04, 0x02, 0x10);

	/* Standard local interrupt assignments */
	mptable_lintsrc(mc, bus_isa);

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

