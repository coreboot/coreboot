/* checkpir.c : This software is released under GPL
   For Linuxbios use only
   Aug 26 2001 , Nikolai Vladychevski, <niko@isl.net.mx>
*/
/* modified to try assigning IRQs by rgm */

#include <printk.h>
#include <pci.h>
#include <subr.h>
#include <string.h>
#include <arch/i386_subr.h>

#include <arch/pirq_routing.h>

/* from linux: */
/*
 * Never use: 0, 1, 2 (timer, keyboard, and cascade)
 * Avoid using: 13, 14 and 15 (FP error and IDE).
 * Penalize: 3, 4, 6, 7, 12 
 * (known ISA uses: serial, floppy, parallel and mouse)
 */
/* I am leaving 3 and 4 at never use ... */
/* M is Max, P is Penalize */
#define M ((unsigned int) -1)
#define P 32
unsigned int irqused[16] = {M, M, M, M,  // 0-3
			    M, 0, P, P,  // 4-7
			    M, 0, 0, 0,  // 8-11
			    P, M, M, M}; // 12-15

int numa = 0, numb = 0, numbc = 0, numd = 0;

/* start at 8 since things seem to like to use high ints consider
 * wrapping later ...
 */
int
getint(u16 m){
    int mask = m>>8, i;
    unsigned int min = (unsigned int) -1;
    unsigned int minindex;
    /* find the least-used interrupt compatible with mask */
    printk_err("getint mask 0x%x\n", mask);
    for(minindex = 8, i = 8; i < 16; i++, mask>>=1) {
	printk_err("Check %d\n", i);
	if (! (mask & 1))
	    continue;
	printk_err("Irqused[%d] is %d\n", i, irqused[i]);
	if (irqused[i] < min) {
	    minindex = i;
	    min = irqused[i];
	    irqused[i]++;
	}
	printk_err("end of loop minindex is %d\n", minindex);
    }
    return minindex;
}

void
assignirq(void) {
  struct pci_dev *pci_find_slot(unsigned int bus, unsigned int devfn);
  struct pci_dev *dev;
  const struct irq_routing_table *rt;
  u8 sum,newsum, pin, interrupt, line;
  u16 mask;
  int numdevices, i;
  struct irq_info *slot;

  rt = &intel_irq_routing_table;
    /* now try assigning interrupts */
    numdevices = (rt->size-32)/16;
    printk_err("There are %d devices\n", numdevices);
    for(slot = rt->slots, i = 0; i < numdevices; i++, slot++) {
	printk_err("bus 0x%x devfn 0x%x\n", slot->bus, slot->devfn);
	printk_err("  %x:%x.%x\n", slot->bus, slot->devfn>>3, slot->devfn&7);
	dev = pci_find_slot(slot->bus, slot->devfn);
	if (! dev) {
	    printk_err("no dev at this place\n");
	}
	/* get the interrupt pin */
	pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin);
	printk_err("pin is %d\n", pin);
	if ((pin < 1) || (pin > 4)) {
	    printk_err("invalid pin\n");
	    continue;
	}

	mask = slot->irq[pin-1].bitmap;
	printk_err("irq mask for this pin is 0x%x\n", mask);
	
	interrupt = getint(mask);
	printk_err("interrupt is 0x%x\n", interrupt);
	pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &line);
	printk_err("would set reg 0x%x to 0x%x, currently is 0x%x\n", 
			PCI_INTERRUPT_LINE, interrupt, line);
	pci_write_config_byte(dev, PCI_INTERRUPT_LINE, interrupt);
    }
}
