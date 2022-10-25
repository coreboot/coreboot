/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <amdblocks/amd_pci_util.h>
#include <pc80/i8259.h>
#include <soc/amd_pci_int_defs.h>
#include <string.h>

/*
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 */
static uint8_t fch_pic_routing[FCH_IRQ_ROUTING_ENTRIES];
static uint8_t fch_apic_routing[FCH_IRQ_ROUTING_ENTRIES];

const struct pirq_struct *pirq_data_ptr;
u32 pirq_data_size;

/*
 * Read the FCH PCI_INTR registers 0xC00/0xC01 at a
 * given index and a given PIC (0) or IOAPIC (1) mode
 */
u8 read_pci_int_idx(u8 index, int mode)
{
	outb((mode << 7) | index, PCI_INTR_INDEX);
	return inb(PCI_INTR_DATA);
}

/*
 * Write a value to the FCH PCI_INTR registers 0xC00/0xC01
 * at a given index and PIC (0) or IOAPIC (1) mode
 */
void write_pci_int_idx(u8 index, int mode, u8 data)
{
	outb((mode << 7) | index, PCI_INTR_INDEX);
	outb(data, PCI_INTR_DATA);
}

static void init_fch_irq_map_tables(void)
{
	const struct fch_irq_routing *mb_irq_map;
	size_t mb_fch_irq_mapping_table_size;
	size_t i;

	mb_irq_map = mb_get_fch_irq_mapping(&mb_fch_irq_mapping_table_size);

	memset(fch_pic_routing, PIRQ_NC, sizeof(fch_pic_routing));
	memset(fch_apic_routing, PIRQ_NC, sizeof(fch_apic_routing));

	for (i = 0; i < mb_fch_irq_mapping_table_size; i++) {
		if (mb_irq_map[i].intr_index >= FCH_IRQ_ROUTING_ENTRIES) {
			printk(BIOS_WARNING,
			       "Invalid IRQ index %u in FCH IRQ routing table entry %zu\n",
			       mb_irq_map[i].intr_index, i);
			continue;
		}
		fch_pic_routing[mb_irq_map[i].intr_index] = mb_irq_map[i].pic_irq_num;
		fch_apic_routing[mb_irq_map[i].intr_index] = mb_irq_map[i].apic_irq_num;
	}
}

/*
 * Write the FCH PCI_INTR registers 0xC00/0xC01 with values
 * given in global variables intr_data and picr_data.
 * These variables are defined in mainboard.c
 */
void write_pci_int_table(void)
{
	uint8_t byte;
	size_t i, limit;
	const struct irq_idx_name *idx_name;

	init_fch_irq_map_tables();

	idx_name = sb_get_apic_reg_association(&limit);
	if (idx_name == NULL) {
		printk(BIOS_ERR, "Warning: Can't write PCI_INTR 0xC00/0xC01"
				" registers because\n"
				" 'irq_association'\ntables is NULL\n");
		return;
	}

	/* PIC IRQ routine */
	printk(BIOS_DEBUG, "PCI_INTR tables: Writing registers C00/C01 for"
				" PCI IRQ routing:\n"
				"PCI_INTR_INDEX\tname\t\t     PIC mode"
				"\tAPIC mode\n");
	/*
	 * Iterate table idx_name, indexes outside the table are ignored
	 * (assumed not connected within the chip). For each iteration,
	 * get the register index "byte" and the name of the associated
	 * IRQ source for printing.
	 */
	for (i = 0 ; i < limit; i++) {
		byte = idx_name[i].index;
		write_pci_int_idx(byte, 0, fch_pic_routing[byte]);
		printk(BIOS_DEBUG, "0x%02X\t\t%-20s 0x%02X\t",
				byte, idx_name[i].name,
				read_pci_int_idx(byte, 0));
		write_pci_int_idx(byte, 1, fch_apic_routing[byte]);
		printk(BIOS_DEBUG, "0x%02X\n", read_pci_int_idx(byte, 1));
	}
}

/*
 * Function to write the PCI config space Interrupt
 * registers based on the values given in PCI_INTR
 * table at I/O port 0xC00/0xC01
 */
void write_pci_cfg_irqs(void)
{
	struct device *dev = NULL;  /* Our current device to route IRQs */
	struct device *target_dev = NULL; /* the bridge a device may be
					   * connected to */
	u16 int_pin = 0;
	u16 target_pin = 0;
	u16 int_line = 0;
	u16 pci_intr_idx = 0; /* Index into PCI_INTR table, 0xC00/0xC01 */
	u16 devfn = 0;
	u32 i = 0;
	size_t limit;
	const struct irq_idx_name *idx_name;

	idx_name = sb_get_apic_reg_association(&limit);
	if (pirq_data_ptr == NULL) {
		printk(BIOS_WARNING, "Can't write PCI IRQ assignments"
				" because 'mainboard_pirq_data' structure does"
				" not exist\n");
		return;
	}

	/* Populate the PCI cfg space header with the IRQ assignment */
	printk(BIOS_DEBUG, "PCI_CFG IRQ: Write PCI config space IRQ"
				" assignments\n");

	for (dev = all_devices ; dev ; dev = dev->next) {
		/*
		 * Step 1: Get INT_PIN and device structure to look for in the
		 * PCI_INTR table pirq_data
		 */
		target_dev = NULL;
		target_pin = get_pci_irq_pins(dev, &target_dev);
		if (target_dev == NULL)
			continue;

		if (target_pin < 1)
			continue;

		/* Get the original INT_PIN for record keeping */
		int_pin = pci_read_config8(dev, PCI_INTERRUPT_PIN);
		if (int_pin < 1 || int_pin > 4)
			continue;	/* Device has invalid INT_PIN - skip  */

		devfn = target_dev->path.pci.devfn;

		/*
		 * Step 2: Use the INT_PIN and DevFn number to find the PCI_INTR
		 * register (0xC00) index for this device
		 */
		pci_intr_idx = 0xbad;	/* Will check to make sure it changed */
		for (i = 0 ; i < pirq_data_size ; i++) {
			if (pirq_data_ptr[i].devfn != devfn)
				continue;

			/* PIN_A is idx0 in pirq_data array but 1 in PCI reg */
			pci_intr_idx = pirq_data_ptr[i].PIN[target_pin - 1];
			printk(BIOS_SPEW, "\tFound this device in pirq_data"
					" table entry %d\n", i);
			break;
		}

		/*
		 * Step 3: Make sure we got a valid index and use it to get
		 * the IRQ number from the PCI_INTR register table
		 */
		if (pci_intr_idx == 0xbad) {
			/* Not on a bridge or in pirq_data table, skip it */
			printk(BIOS_SPEW, "PCI Devfn (0x%x) not found in"
						" pirq_data table\n", devfn);
			continue;
		} else if (pci_intr_idx == 0x1f) {
			/* Index found is not defined */
			printk(BIOS_SPEW, "Got index 0x1F (Not Connected),"
					" perhaps this device was"
					" defined wrong?\n");
			continue;
		}
		/*
		 * Find the name associated with register [pci_intr_idx]
		 * and print information.
		 */
		for (i = 0; i < limit; i++) {
			if (idx_name[i].index == pci_intr_idx)
				break;
		}
		if (i == limit) {
			printk(BIOS_SPEW, "Got register index 0x%02x"
					  " undefined in table irq_idx_name,\n"
					  " perhaps this device was"
					  " defined wrong?\n", pci_intr_idx);
			continue;
		}

		/* Find the value to program into the INT_LINE register from
		 *  the PCI_INTR registers
		 */
		int_line = read_pci_int_idx(pci_intr_idx, 0);
		if (int_line == PIRQ_NC) {	/* The IRQ found is disabled */
			printk(BIOS_SPEW, "Got IRQ 0x1F (disabled), perhaps"
					" this device was defined wrong?\n");
			continue;
		}

		/*
		 * Step 4: Program the INT_LINE register in this device's
		 * PCI config space with the IRQ number we found in step 3
		 * and make it Level Triggered
		 */
		pci_write_config8(dev, PCI_INTERRUPT_LINE, int_line);

		/* Set IRQ to level triggered since used by a PCI device */
		i8259_configure_irq_trigger(int_line, IRQ_LEVEL_TRIGGERED);

		/*
		 * Step 5: Print out debug info and move on to next device
		 */
		printk(BIOS_SPEW, "\tOrig INT_PIN\t: %d (%s)\n",
						int_pin, pin_to_str(int_pin));
		printk(BIOS_SPEW, "\tPCI_INTR idx\t: 0x%02x (%s)\n"
				  "\tINT_LINE\t: 0x%X (IRQ %d)\n",
				  pci_intr_idx, idx_name[i].name,
				  int_line, int_line);
	}	/* for (dev = all_devices) */
	printk(BIOS_DEBUG, "PCI_CFG IRQ: Finished writing PCI config space"
					" IRQ assignments\n");
}
