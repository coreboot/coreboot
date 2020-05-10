/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PC80_I8259_H
#define PC80_I8259_H

/*
 * IRQ numbers and common usage
 * If an IRQ does not say it is 'Reserved'
 * then it can be used by a device, though
 * some systems may not adhere to this map.
 */
/* PIC IRQs */
#define IRQ_DIS		0x1F	/* IRQ is disabled */
#define IRQ_0		0x00	/* Reserved - Timer IRQ */
#define IRQ_1		0x01	/* Keyboard controller */
#define IRQ_2		0x02	/* Reserved - Cascade to Slave PIC */
#define IRQ_3		0x03	/* Serial Port 2 & 4 */
#define IRQ_4		0x04	/* Serial Port 1 & 3 */
#define IRQ_5		0x05	/* Parallel Port 2 & 3 or Sound Card */
#define IRQ_6		0x06	/* Floppy Disk Controller */
#define IRQ_7		0x07	/* Parallel Port 1 */
#define IRQ_8		0x08	/* Reserved - RTC */
#define IRQ_9		0x09	/* Reserved - ACPI System Control Interrupt */
#define IRQ_10		0x0A	/* Free or SCSI or NIC */
#define IRQ_11		0x0B	/* Free or SCSI or NIC */
#define IRQ_12		0x0C	/* PS/2 Mouse */
#define IRQ_13		0x0D	/* Reserved - CPU Floating Point Unit */
#define IRQ_14		0x0E	/* Primary ATA */
#define IRQ_15		0x0F	/* Secondary ATA */

#define MASTER_PIC_ICW1		0x20
#define SLAVE_PIC_ICW1		0xa0
#define   ICW_SELECT		(1 << 4)
#define   OCW_SELECT		(0 << 4)
#define   ADI			(1 << 2)
#define   SNGL			(1 << 1)
#define   IC4			(1 << 0)

#define MASTER_PIC_ICW2		0x21
#define SLAVE_PIC_ICW2		0xa1
#define   INT_VECTOR_MASTER	0x20
#define   IRQ0			0x00
#define   IRQ1			0x01
#define   INT_VECTOR_SLAVE	0x28
#define   IRQ8			0x00
#define   IRQ9			0x01

#define MASTER_PIC_ICW3		0x21
#define   CASCADED_PIC		(1 << 2)

#define MASTER_PIC_ICW4		0x21
#define SLAVE_PIC_ICW4		0xa1
#define   MICROPROCESSOR_MODE	(1 << 0)

#define SLAVE_PIC_ICW3		0xa1
#define    SLAVE_ID		0x02

#define MASTER_PIC_OCW1		0x21
#define SLAVE_PIC_OCW1		0xa1
#define    IRQ2			(1 << 2)
#define    ALL_IRQS		0xff

#define ELCR1			0x4d0
#define ELCR2			0x4d1

#define IRQ_LEVEL_TRIGGERED	1
#define IRQ_EDGE_TRIGGERED	0

u16 pic_read_irq_mask(void);
void pic_write_irq_mask(u16 mask);
void pic_irq_enable(u8 int_num, u8 mask);
void setup_i8259(void);
void i8259_configure_irq_trigger(int int_num, int is_level_triggered);

#endif /* PC80_I8259_H */
