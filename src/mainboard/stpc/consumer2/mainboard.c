/*
 * stpc consumerII final mainboard setup
 * primarily sets the irq routing
 *
 * many values in this file are
 * hardcoded for the consumerII eval motherboard
 *
 *   by
 * Steve M. Gehlbach <steve@kesa.com>
 * edit with ts=4
 *
 */
#include <printk.h>

#include <cpu/p5/io.h>
#include <arch/pirq_routing.h>
#include <pci.h>

////////////////////////////////////////////////////////////////
//                                                            //
// SET LIST OF INTERRUPTS HERE BY SETTING IRQ1,2,3.           //
// THE LIST MUST BE UNIQUE!!!!                                //
// You only need as many IRQs as boards                       //
// plugged in, the max is 3 boards (3 slots),                 //
// but all 3 must be defined and unique to avoid compile      //
// errors.                                                    //
//                                                            //
////////////////////////////////////////////////////////////////

#define IRQ1 9
#define IRQ2 10
#define IRQ3 11

//
//---------------------------------------------------------------
//

#if ! (IRQ1 && IRQ2 && IRQ3)
#error "irq_tables.c: IRQ1 IRQ2 IRQ3 must be defined even if not used."
#endif

#if (IRQ1 == IRQ2)
#error "irq_tables.c: Interrupts must be unique!"
#endif

#if IRQ2 == IRQ3
#error "irq_tables.c: Interrupts must be unique!"
#endif

#if IRQ1 == IRQ3
#error "irq_tables.c: Interrupts must be unique!"
#endif

extern struct irq_routing_table intel_irq_routing_table;

extern void stpc_conf_writeb(u8 data, u8 port);
extern u8 stpc_conf_readb( u8 port);

#define CONFIG_CMD(bus,devfn, where)   (0x80000000 | (bus << 16) | (devfn << 8) | (where & ~3))
#define INTERRUPT_PIN 0x3d
#define INTERRUPT_LINE 0x3c
#define BUS 0

static int pci_conf1_read_config_word(unsigned char bus, int devfn, int where, u16 * value) {
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	*value = inw(0xCFC + (where & 2));
	return 0;
}

static int pci_conf1_write_config_byte(unsigned char bus, int devfn, int where, u8 value) {
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	outb(value, 0xCFC + (where & 3));
	return 0;
}

static int pci_conf1_read_config_byte(unsigned char bus, int devfn, int where, u8 * value) {
	outl(CONFIG_CMD(bus, devfn, where), 0xCF8);
	*value = inb(0xCFC + (where & 3));
	return 0;
}
//
// calculate the irq_tables checksum
//
int calc_checksum(struct irq_routing_table *rt) {
	long i;
	u8 *addr,sum2=0;
	    
	addr= (u8 *) rt;
	for (i=0;i<rt->size;i++) sum2 += addr[i];
	return(sum2);
}       

void mainboard_fixup() {
	u8 pin, devfn, irq, irq_written, stpc_reg;
	u8 irq_list[] = { IRQ1, IRQ2, IRQ3 };
	// pirq(slot,pin); slot[0:2] pin[0:3]
	u8 pirq[3][4] = { {2,3,0,1},{1,2,3,0},{0,1,2,3}};
	u16 vendor_id, mask;
	int i,j;
	
	printk_debug("stpc mainboard_fixup()\n");

	mask = (1<<irq_list[0]) | (1<<irq_list[1]) | (1<<irq_list[2]);

	/* 
	 * Hardcoded for the stpc consumerII motherboard.
	 *
	 * There are 3 slots with dev # 0x1d, 0x1e, 0x1f in stpc consumer2 
	 * The registers in stpc and the routing table are setup to route the 8259 irqs
	 * to the pirqa..pirqd.  The consumer2 motherboard is wired as follows:
	 *
	 *                               INTA   INTB   INTC   INTD
	 *                               ----------------------------
	 * slot 1(P8): dev=0x1d  wiring: PIRQ_C PIRQ_D PIRQ_A PIRQ_B
	 * slot 2(P9): dev=0x1e  wiring: PIRQ_B PIRQ_C PIRQ_D PIRQ_A
	 * slot 3(P10): dev=0x1f wiring: PIRQ_A PIRQ_B PIRQ_C PIRQ_D
	 * 
	 */

	//
	// search all 3 slots for a device and program the pci INTERRUPT_LINE register
	// note: not needed by 2.4 but used by 2.2
	// also program the stpc pirq routing registers
	// and set the value in the bios-style routing table used by linux.
	// When done, calculate and store the routing table checksum.
	//
	j=0;
	for (i=0;i<3;i++) {
		// i= slot# -1
		// read the pin if the device exists
		devfn = PCI_DEVFN(0x1d+i,0);
		pci_conf1_read_config_word(BUS,devfn,0,&vendor_id);
		if (vendor_id == 0xffff) continue;

		pci_conf1_read_config_byte(BUS,devfn,INTERRUPT_PIN,&pin);
		if (!pin) continue;

		printk_debug("stpc mainboard_fixup(): found INT pin for dev 0x%x -> pin %d\n",0x1d+i,pin);
		pin--; // we start at 0: pin 0-3 -> INTA-D

		// get the next irq in our list
		irq = irq_list[j++];

		// program the pci registers
		pci_conf1_write_config_byte(BUS,devfn,INTERRUPT_LINE,irq);
		pci_conf1_read_config_byte(BUS,devfn,INTERRUPT_LINE,&irq_written);
		printk_debug("stpc mainboard_fixup(): wrote pci INTERRUPT_LINE for dev 0x%x -> irq %d (%d)\n", PCI_SLOT(devfn),irq,irq_written);

	// program the stpc config registers
    // based on slot and pin.
	// note that since the irq list is unique it is not possible to program
	// two stpc pirq registers with the same irq.  Doing this will screw up the stpc and
	// cause the interrupts to not work properly.
		stpc_reg = pirq[i][pin] + 0x52;
		switch (irq) {
			case 3: 
			case 4: 
			case 5: 
			case 6: 
			case 7:
				stpc_conf_writeb(stpc_conf_readb(0x56) | (1<<(irq+3)),0x56);
				printk_debug("stpc_mainboard_fixup(): wrote conf register 0x56= 0x%x;",stpc_conf_readb(0x56));
				break;
			case 9: 
			case 10: 
			case 11: 
			case 12:
				stpc_conf_writeb(stpc_conf_readb(0x57) | (1<<(irq-8)),0x57);
				printk_debug("stpc_mainboard_fixup(): wrote conf register 0x57= 0x%x;",stpc_conf_readb(0x57));
				break;
			case 14: 
			case 15:
				stpc_conf_writeb(stpc_conf_readb(0x57) | (1<<(irq-8)),0x57);
				printk_debug("stpc_mainboard_fixup(): wrote conf register 0x57= 0x%x;",stpc_conf_readb(0x57));
				break;
			default:
				continue;
		}
		stpc_conf_writeb(irq|0x80,stpc_reg);
		printk_debug(" conf register 0x%x= 0x%x\n",stpc_reg,stpc_conf_readb(stpc_reg));

	// set the routing table
		// The 0xf0 causes linux to consider the irq hard-wired and use the link value
		// as the actual irq, so it does not need a router for the stpc.
		// Otherwise a kernel patch would be needed to add the stpc router into arch/i386/kernel/pci-irq.c
		intel_irq_routing_table.slots[i].irq[pin].link = irq | 0xf0;
		intel_irq_routing_table.slots[i].irq[pin].bitmap = mask;
		printk_debug("stpc_mainboard_fixup(): routing_table.slots[%d].irq[%d].link,mask = 0x%x,0x%x\n",i,pin,
				intel_irq_routing_table.slots[i].irq[pin].link,
				intel_irq_routing_table.slots[i].irq[pin].bitmap
				);
	}

	//
	// Now...
	// set the checksum in the routing table;
	//
	intel_irq_routing_table.checksum = 0;
	intel_irq_routing_table.checksum = -calc_checksum( (struct irq_routing_table *) &intel_irq_routing_table);
	printk_debug("stpc mainboard_fixup(): checksum calculated= 0x%x\n",intel_irq_routing_table.checksum);

	printk_debug("stpc mainboard_fixup complete.\n");
}
void keyboard_on (void) {
	return;
}
