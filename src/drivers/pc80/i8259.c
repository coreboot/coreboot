/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <pc80/i8259.h>
#include <console/console.h>

/* Read the current PIC IRQ mask */
u16 pic_read_irq_mask(void)
{
	u16 mask;
	int i;

	mask = inb(MASTER_PIC_OCW1) | (inb(SLAVE_PIC_OCW1) << 8);

	printk(BIOS_DEBUG, "8259 PIC: OCW1 IRQ Mask: 0x%x\n", mask);
	printk(BIOS_SPEW, "\tEnabled IRQs (0 = Unmasked, 1 = Masked off):\n"
			"\t\tMaster\t\tSlave\n");
	for(i = 0; i <= 7; i++) {
		printk(BIOS_SPEW, "\t\tIRQ%X: %x\t\tIRQ%X: %x\n",
				i, (mask >> i) & 1, i + 8, (mask >> (i + 8)) & 1);
	}
	return mask;
}

/*
 * Write an IRQ mask to the PIC:
 * IRQA is bit 0xA in the 16 bit bitmask (OCW1)
 */
void pic_write_irq_mask(u16 mask)
{
	outb(mask, MASTER_PIC_OCW1);
	outb(mask >> 8, SLAVE_PIC_OCW1);
}

/*
 * The PIC IRQs default to masked off
 * Allow specific IRQs to be enabled (1)
 * or disabled by (0) the user
 */
void pic_irq_enable(u8 int_num, u8 mask)
{
    pic_write_irq_mask(pic_read_irq_mask() & ~(mask << int_num));
    pic_read_irq_mask();
}

void setup_i8259(void)
{
	/* A write to ICW1 starts the Interrupt Controller Initialization
	 * Sequence. This implicitly causes the following to happen:
	 *   - Interrupt Mask register is cleared
	 *   - Priority 7 is assigned to IRQ7 input
	 *   - Slave mode address is set to 7
	 *   - Special mask mode is cleared
	 *
	 * We send the initialization sequence to both the master and
	 * slave i8259 controller.
	 */
	outb(ICW_SELECT|IC4, MASTER_PIC_ICW1);
	outb(ICW_SELECT|IC4, SLAVE_PIC_ICW1);

	/* Now the interrupt controller expects us to write to ICW2. */
	outb(INT_VECTOR_MASTER | IRQ0, MASTER_PIC_ICW2);
	outb(INT_VECTOR_SLAVE  | IRQ8, SLAVE_PIC_ICW2);

	/* Now the interrupt controller expects us to write to ICW3.
	 *
	 * The normal scenario is to set up cascading on IRQ2 on the master
	 * i8259 and assign the slave ID 2 to the slave i8259.
	 */
	outb(CASCADED_PIC, MASTER_PIC_ICW3);
	outb(SLAVE_ID, SLAVE_PIC_ICW3);

	/* Now the interrupt controller expects us to write to ICW4.
	 *
	 * We switch both i8259 to microprocessor mode because they're
	 * operating as part of an x86 architecture based chipset
	 */
	outb(MICROPROCESSOR_MODE, MASTER_PIC_ICW2);
	outb(MICROPROCESSOR_MODE, SLAVE_PIC_ICW2);

	/* Now clear the interrupts through OCW1.
	 * First we mask off all interrupts on the slave interrupt controller
	 * then we mask off all interrupts but interrupt 2 on the master
	 * controller. This way the cascading stays alive.
	 */
	outb(ALL_IRQS, SLAVE_PIC_OCW1);
	outb(ALL_IRQS & ~IRQ2, MASTER_PIC_OCW1);
}

/**
 * @brief Configure IRQ triggering in the i8259 compatible Interrupt Controller.
 *
 * Switch a certain interrupt to be level / edge triggered.
 *
 * @param int_num legacy interrupt number (3-7, 9-15)
 * @param is_level_triggered 1 for level triggered interrupt, 0 for edge
 *        triggered interrupt
 */
void i8259_configure_irq_trigger(int int_num, int is_level_triggered)
{
	u16 int_bits = inb(ELCR1) | (((u16)inb(ELCR2)) << 8);

	if (is_level_triggered)
		int_bits |= (1 << int_num);
	else
		int_bits &= ~(1 << int_num);

	/* Write new values */
	outb((u8)(int_bits & 0xff), ELCR1);
	outb((u8)(int_bits >> 8), ELCR2);

#ifdef PARANOID_IRQ_TRIGGERS
	/* Try reading back the new values. This seems like an error but is not ... */
	if (inb(ELCR1) != (int_bits & 0xff)) {
		printk(BIOS_ERR, "%s: lower order bits are wrong: want 0x%x, got 0x%x\n",
				__func__, (int_bits & 0xff), inb(ELCR1));
	}

	if (inb(ELCR2) != (int_bits >> 8)) {
		printk(BIOS_ERR, "%s: higher order bits are wrong: want 0x%x, got 0x%x\n",
				__func__, (int_bits>>8), inb(ELCR2));
	}
#endif
}
