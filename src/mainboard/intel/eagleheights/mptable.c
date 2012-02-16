/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2009 Thomas Jourdan <thomas.jourdan@gmail.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

// Generate MP-table IRQ numbers for PCI devices.
#define IO_APIC0 2

#define INT_A	0
#define INT_B	1
#define INT_C	2
#define INT_D	3
#define PCI_IRQ(dev, intLine)	(((dev)<<2) | intLine)

#define PIRQ_A 16
#define PIRQ_B 17
#define PIRQ_C 18
#define PIRQ_D 19
#define PIRQ_E 20
#define PIRQ_F 21
#define PIRQ_G 22
#define PIRQ_H 23

// RCBA
#define RCBA 0xF0

#define RCBA_D31IP 0x3100
#define RCBA_D30IP 0x3104
#define RCBA_D29IP 0x3108
#define RCBA_D28IP 0x310C
#define RCBA_D31IR 0x3140
#define RCBA_D30IR 0x3142
#define RCBA_D29IR 0x3144
#define RCBA_D28IR 0x3146

static void *smp_write_config_table(void *v)
{
        struct mp_config_table *mc;
	unsigned char bus_chipset, bus_pci;
	unsigned char bus_pcie_a, bus_pcie_a1, bus_pcie_b;
	int bus_isa, i;
	uint32_t pin, route;
	device_t dev;
	struct resource *res;
	unsigned long rcba;

	dev = dev_find_slot(0, PCI_DEVFN(0x1F,0));
	res = find_resource(dev, RCBA);
	if (!res) {
	  return NULL;
	}
	rcba = res->base;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

        smp_write_processors(mc);

	/* Get bus numbers */
	bus_chipset = 0;

	/* PCI */
	dev = dev_find_slot(0, PCI_DEVFN(0x1E,0));
	if (dev) {
	  bus_pci = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
	  printk(BIOS_DEBUG, "ERROR - could not find PCI 0:1e.0, using defaults\n");
	  bus_pci = 6;
	}

	dev = dev_find_slot(0, PCI_DEVFN(2,0));
	if(dev) {
	  bus_pcie_a = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
	  printk(BIOS_DEBUG, "ERROR - could not find PCIe Port A  0:2.0, using defaults\n");
	  bus_pcie_a = 1;
	}

	dev = dev_find_slot(0, PCI_DEVFN(3,0));
	if(dev) {
	  bus_pcie_a1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
	  printk(BIOS_DEBUG, "ERROR - could not find PCIe Port B 0:3.0, using defaults\n");
	  bus_pcie_a1 = 2;
	}

	dev = dev_find_slot(0, PCI_DEVFN(0x1C,0));
	if(dev) {
	  bus_pcie_b = pci_read_config8(dev, PCI_SECONDARY_BUS);
	} else {
	  printk(BIOS_DEBUG, "ERROR - could not find PCIe Port B 0:3.0, using defaults\n");
	  bus_pcie_b = 3;
	}

	mptable_write_buses(mc, NULL, &bus_isa);

	/*I/O APICs: APIC ID Version State Address*/
	smp_write_ioapic(mc, 2, 0x20, IO_APIC_ADDR);

	mptable_add_isa_interrupts(mc, bus_isa, IO_APIC0, 0);

	/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, bus_isa);

	/* Internal PCI device for i3100 */

	/* EDMA
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chipset, PCI_IRQ(1, INT_A), IO_APIC0, PIRQ_A);

	/* PCIe Port A
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chipset, PCI_IRQ(2, INT_A), IO_APIC0, PIRQ_A);

	/* PCIe Port A1
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chipset, PCI_IRQ(3, INT_A), IO_APIC0, PIRQ_A);

	/* PCIe Port B
	 */
	for(i = 0; i < 4; i++) {
	  pin = (read32(rcba + RCBA_D28IP) >> (i * 4)) & 0x0F;
	  if(pin > 0) {
	    pin -= 1;
	    route = PIRQ_A + ((read16(rcba + RCBA_D28IR) >> (pin * 4)) & 0x07);
	    smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chipset, PCI_IRQ(28, pin), IO_APIC0, route);
	  }
	}

	/* USB 1.1 : device 29, function 0, 1
	 */
	for(i = 0; i < 2; i++) {
	  pin = (read32(rcba + RCBA_D29IP) >> (i * 4)) & 0x0F;
	  if(pin > 0) {
	    pin -= 1;
	    route = PIRQ_A + ((read16(rcba + RCBA_D29IR) >> (pin * 4)) & 0x07);
	    smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chipset, PCI_IRQ(29, pin), IO_APIC0, route);
	  }
	}

	/* USB 2.0 : device 29, function 7
	*/
	pin = (read32(rcba + RCBA_D29IP) >> (7 * 4)) & 0x0F;
	if(pin > 0) {
	  pin -= 1;
	  route = PIRQ_A + ((read16(rcba + RCBA_D29IR) >> (pin * 4)) & 0x07);
	  smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chipset, PCI_IRQ(29, pin), IO_APIC0, route);
	}

	/* SATA : device 31 function 2
	   SMBus : device 31 function 3
	   Performance counters : device 31 function 4
	 */
	for(i = 2; i < 5; i++) {
	  pin = (read32(rcba + RCBA_D31IP) >> (i * 4)) & 0x0F;
	  if(pin > 0) {
	    pin -= 1;
	    route = PIRQ_A + ((read16(rcba + RCBA_D31IR) >> (pin * 4)) & 0x07);
	    smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chipset, PCI_IRQ(31, pin), IO_APIC0, route);
	  }
	}

	/* SLOTS */

	/* PCIe 4x slot A
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a, PCI_IRQ(0, INT_A), IO_APIC0, PIRQ_A);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a, PCI_IRQ(0, INT_B), IO_APIC0, PIRQ_B);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a, PCI_IRQ(0, INT_C), IO_APIC0, PIRQ_C);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a, PCI_IRQ(0, INT_D), IO_APIC0, PIRQ_D);

	/* PCIe 4x slot A1
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a1, PCI_IRQ(0, INT_A), IO_APIC0, PIRQ_A);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a1, PCI_IRQ(0, INT_B), IO_APIC0, PIRQ_B);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a1, PCI_IRQ(0, INT_C), IO_APIC0, PIRQ_C);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_a1, PCI_IRQ(0, INT_D), IO_APIC0, PIRQ_D);

	/* PCIe 4x slot B
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_b, PCI_IRQ(0, INT_A), IO_APIC0, PIRQ_A);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_b, PCI_IRQ(0, INT_B), IO_APIC0, PIRQ_B);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_b, PCI_IRQ(0, INT_C), IO_APIC0, PIRQ_C);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pcie_b, PCI_IRQ(0, INT_D), IO_APIC0, PIRQ_D);

	/* PCI slot
	 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, PCI_IRQ(0, INT_A), IO_APIC0, PIRQ_A);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, PCI_IRQ(0, INT_B), IO_APIC0, PIRQ_B);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, PCI_IRQ(0, INT_C), IO_APIC0, PIRQ_C);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_pci, PCI_IRQ(0, INT_D), IO_APIC0, PIRQ_D);

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
