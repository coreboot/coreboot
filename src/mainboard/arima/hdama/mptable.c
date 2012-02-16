#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <cpu/x86/lapic.h>
#include <arch/cpu.h>
#include <arch/io.h>

#define HT_INIT_CONTROL 0x6c
#define HTIC_BIOSR_Detect  (1<<5)

static unsigned node_link_to_bus(unsigned node, unsigned link)
{
	device_t dev;
	unsigned reg;

	dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));
	if (!dev) {
		return 0xff;
	}
	for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
		uint32_t config_map;
		unsigned dst_node;
		unsigned dst_link;
		unsigned bus_base;
		config_map = pci_read_config32(dev, reg);
		if ((config_map & 3) != 3) {
			continue;
		}
		dst_node = (config_map >> 4) & 7;
		dst_link = (config_map >> 8) & 3;
		bus_base = (config_map >> 16) & 0xff;
#if 0
		printk(BIOS_DEBUG, "node.link=bus: %d.%d=%d 0x%2x->0x%08x\n",
			dst_node, dst_link, bus_base,
			reg, config_map);
#endif
		if ((dst_node == node) && (dst_link == link))
		{
			return bus_base;
		}
	}
	return 0xff;
}

static unsigned max_apicid(void)
{
	unsigned max;
	device_t dev;
	max = 0;
	for(dev = all_devices; dev; dev = dev->next) {
		if (dev->path.type != DEVICE_PATH_APIC)
			continue;
		if (dev->path.apic.apic_id > max) {
			max = dev->path.apic.apic_id;
		}
	}
	return max;
}

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	unsigned char bus_chain_0;
	unsigned char bus_8131_1;
	unsigned char bus_8131_2;
	unsigned char bus_8111_1;
	unsigned apicid_base;
	unsigned apicid_8111;
	unsigned apicid_8131_1;
	unsigned apicid_8131_2;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	apicid_base = max_apicid() + 1;
	apicid_8111 = apicid_base;
	apicid_8131_1 = apicid_base + 1;
	apicid_8131_2 = apicid_base + 2;
	{
		device_t dev;

		/* HT chain 0 */
		bus_chain_0 = node_link_to_bus(0, 0);
		if (bus_chain_0 == 0xff) {
			printk(BIOS_DEBUG, "ERROR - cound not find bus for node 0 chain 0, using defaults\n");
			bus_chain_0 = 0;
		}

		/* 8111 */
		dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x03,0));
		if (dev) {
			bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:03.0, using defaults\n", bus_chain_0);
			bus_8111_1 = 4;
		}
		/* 8131-1 */
		dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x01,0));
		if (dev) {
			bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:01.0, using defaults\n", bus_chain_0);
			bus_8131_1 = 2;
		}
		/* 8131-2 */
		dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x02,0));
		if (dev) {
			bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI %02x:02.0, using defaults\n", bus_chain_0);
			bus_8131_2 = 3;
		}
	}

	mptable_write_buses(mc, NULL, &bus_isa);

	/* IOAPIC handling */
	smp_write_ioapic(mc, apicid_8111, 0x11, IO_APIC_ADDR);
	{
		device_t dev;
		struct resource *res;
		/* 8131 apic 3 */
		dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x01,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, apicid_8131_1, 0x11, res->base);
			}
		}
		/* 8131 apic 4 */
		dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x02,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, apicid_8131_2, 0x11, res->base);
			}
		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, apicid_8111, 0);

	/* Standard local interrupt assignments */
	mptable_lintsrc(mc, bus_isa);

	/* PCI Ints:	     Type    Trigger                Polarity                 Bus ID      PCIDEVNUM|IRQ  APIC ID PIN# */
	/* On board nics */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x03<<2)|0, apicid_8111, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x04<<2)|0, apicid_8111, 0x13);
	/* On board SATA */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x05<<2)|0, apicid_8111, 0x11);

	/* PCI Slot 1 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x01<<2)|0, apicid_8111, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x01<<2)|1, apicid_8111, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x01<<2)|2, apicid_8111, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x01<<2)|3, apicid_8111, 0x10);

	/* PCI Slot 2 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x02<<2)|0, apicid_8111, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x02<<2)|1, apicid_8111, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x02<<2)|2, apicid_8111, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x02<<2)|3, apicid_8111, 0x11);

	/* PCI Slot 3 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x01<<2)|0, apicid_8111, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x01<<2)|1, apicid_8111, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x01<<2)|2, apicid_8111, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x01<<2)|3, apicid_8111, 0x10);

	/* PCI Slot 4 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x02<<2)|0, apicid_8111, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x02<<2)|1, apicid_8111, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x02<<2)|2, apicid_8111, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x02<<2)|3, apicid_8111, 0x11);

	/* PCI Slot 5 */
	// FIXME get the irqs right, it's just hacked to work for now
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x05<<2)|0, apicid_8111, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x05<<2)|1, apicid_8111, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x05<<2)|2, apicid_8111, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x05<<2)|3, apicid_8111, 0x10);

	/* PCI Slot 6 */
	// FIXME get the irqs right, it's just hacked to work for now
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x04<<2)|0, apicid_8111, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x04<<2)|1, apicid_8111, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x04<<2)|2, apicid_8111, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x04<<2)|3, apicid_8111, 0x13);

	/* There is no extension information... */

	/* Compute the checksums */
	return mptable_finalize(mc);
}

static void reboot_if_hotswap(void)
{
	/* Hack patch work around for hot swap enable 33mhz problem */
	device_t dev;
	uint32_t data;
	unsigned long htic;
	int reset;

	unsigned bus_chain_0 = node_link_to_bus(0, 0);

	reset = 0;
	printk(BIOS_DEBUG, "Looking for bad PCIX MHz input\n");
	dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x02,0));
	if (!dev)
		printk(BIOS_DEBUG, "Couldn't find %02x:02.0 \n", bus_chain_0);
	else {
		data = pci_read_config32(dev, 0xa0);
		if(!(((data>>16)&0x03)==0x03)) {
			reset=1;
			printk(BIOS_DEBUG, "Bad PCIX MHz - Reset\n");
		}
	}
	printk(BIOS_DEBUG, "Looking for bad Hot Swap Enable\n");
	dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x01,0));
	if (!dev)
		printk(BIOS_DEBUG, "Couldn't find %02x:01.0 \n", bus_chain_0);
	else {
		data = pci_read_config32(dev, 0x48);
		if(data & 0x0c) {
			reset=1;
			printk(BIOS_DEBUG, "Bad Hot Swap start - Reset\n");
		}
	}
	if(reset) {
		/* enable cf9 */
		dev = dev_find_slot(node_link_to_bus(0, 0), PCI_DEVFN(0x04,3));
		pci_write_config8(dev, 0x41, 0xf1);
		/* reset */
		dev = dev_find_slot(0, PCI_DEVFN(0x18,0));
		htic = pci_read_config32(dev, HT_INIT_CONTROL);
		htic &= ~HTIC_BIOSR_Detect;
		pci_write_config32(dev, HT_INIT_CONTROL, htic);
		outb(0x0e, 0x0cf9);
	}
	else {
		printk(BIOS_DEBUG, "OK 133MHz & Hot Swap is off\n");
	}
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	reboot_if_hotswap();

	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}

