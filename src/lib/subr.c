/*
 * Bootstrap code for the INTEL 
 * $Id$
 *
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <cpu/p5/io.h>

#include <cpu/p5/macros.h>
#include <printk.h>
#include <pci.h>
#include <subr.h>
#include <linux/asm-i386/msr.h>

#ifdef SERIAL_CONSOLE
#include <serial_subr.h>
#endif

// kludgy but this is only used here ...
static char *vidmem;		/* The video buffer, should be replaced by symbol in ldscript.ld */
static int line_number;

#define LINES  	25		/* Number of lines and   */
#define COLS   	80		/*   columns on display  */
#define VIDBUFFER 0x20000;

#ifdef VIDEO_SCROLLING
static void scroll()
{
	int i;

	memcpy(vidmem, vidmem + COLS * 2, (LINES - 1) * COLS * 2);
	for (i = (LINES - 1) * COLS * 2; i < LINES * COLS * 2; i += 2)
		vidmem[i] = ' ';
}
#endif

// initialize the display
void displayinit(void)
{
	line_number = 0;
	vidmem = (char *) VIDBUFFER;
	memset(vidmem, 0, 64 * 1024);
#ifdef SERIAL_CONSOLE
#endif
}

void display(char msg[])
{
	int i = 0;

#ifdef SERIAL_CONSOLE
	ttys0_tx_string(msg);
#endif

#ifdef VIDEO_BIOS_WORKS
	while (msg[i] != 0) {
		vidmem[(line_number + 1) * COLS * 2 + i * 2] = msg[i];
		vidmem[(line_number + 1) * COLS * 2 + i * 2 + 1] = 0x07;
		i++;
	}
	(line_number >= (LINES - 1)) ? scroll() : line_number++;
#else /* VIDEO_BIOS_WORKS */
	while (msg[i] != 0)
		vidmem[(line_number++)] = msg[i++];

	/* vidmem[(line_number++)]= 0; */
#endif /* VIDEO_BIOS_WORKS */
}

void printnibl(unsigned long nibl)
{
	static char c[2];
	nibl &= 0xf;
	c[0] = nibl;
	c[1] = 0;
	if ((nibl >= 0) && (nibl <= 9))
		c[0] += '0';
	else
		c[0] += ('A' - 10);
	display(c);

}

void printchar(unsigned char x)
{
	int i;
	for (i = 4; i >= 0; i -= 4)
		printnibl(x >> i);
	display(" ");
}

void printint(unsigned long x)
{
	int i;
	display(" 0x");
	for (i = 28; i >= 0; i -= 4)
		printnibl(x >> i);
	display(" ");
}

void error(char errmsg[])
{
	display(errmsg);

/* what a bad idea -- this wipes out the most recent POST!
 * comment out unless someone comes up with a great reason to keep it.
	intel_post(0xee);
 */

	while (1);		/* Halt */
}

/*
 *    Write POST information
 */
void intel_post(char value)
{
	outb(value, 0x80);
#ifdef SERIAL_POST
	printk(KERN_INFO "POST: 0x%02x\n", value);
#endif
}

void intel_cache_on(unsigned long base, unsigned long totalram)
{
	intel_post(0x60);

	/* we need an #ifdef i586 here at some point ... */
	__asm__ __volatile__("mov %cr0, %eax\n\t"
			     "and $0x9fffffff,%eax\n\t"
			     "mov %eax, %cr0\n\t");
	/* turns out cache isn't really on until you set MTRR registers on 
	 * 686 and later. 
	 * NOTHING FANCY. Linux does a much better job anyway. 
	 * so absolute minimum needed to get it going. 
	 */
	/* OK, linux it turns out does nothing. We have to do it ... */
#ifdef i686
	// totalram here is in linux sizing, i.e. units of KB. 
	// set_mtrr is responsible for getting it into the right units!
	intel_set_mtrr(base, totalram);
#endif
	intel_post(0x62);
}

void intel_interrupts_on()
{

	/* this is so interrupts work. This is very limited scope -- 
	 * linux will do better later, we hope ...
	 */
	/* this is the first way we learned to do it. It fails on real SMP 
	 * stuff. So we have to do things differently ... 
	 * see the Intel mp1.4 spec, page A-3
	 */
#ifdef SMP
	unsigned long reg, *regp;
#define SVR 0xfee000f0
#define LVT1 0xfee00350
#define LVT2 0xfee00360
#define APIC_ENABLED 0x100
	regp = (unsigned long *) SVR;
	reg = *regp;
	reg &= (~0xf0);
	reg |= APIC_ENABLED;
	*regp = reg;

	regp = (unsigned long *) LVT1;
	reg = *regp;
	reg &= 0xfffe00ff;
	reg |= 0x5700;
	*regp = reg;

	regp = (unsigned long *) LVT2;
	reg = *regp;
	reg &= 0xfffe00ff;
	reg |= 0x5400;
	*regp = reg;
#else
	unsigned long low, high;
	rdmsr(0x1b, low, high);
	low &= ~0x800;
	wrmsr(0x1b, low, high);
#endif

}


/* These functions should be chip-set independent -tds */
void intel_zero_irq_settings(void)
{
	struct pci_dev *pcidev;
//	unsigned char irq;
	unsigned char line;
  
	pcidev = pci_devices;
  
	while (pcidev) {
		pci_read_config_byte(pcidev, 0x3d, &line);
		if (line) {
			pci_write_config_byte(pcidev, 0x3c, 0);
		}
		pcidev = pcidev->next;
	}
}

void intel_check_irq_routing_table(void)
{
#if (defined(L440BX) || defined (SIS630))
	const u8 *addr;
	const struct irq_routing_table *rt;
	int i;
	u8 sum;

	rt = &intel_irq_routing_table;
	addr = (u8 *)rt;

	sum = 0;
	for (i=0; i<rt->size; i++)
		sum += addr[i];

	printk(KERN_DEBUG "%s:%6d:%s() - "
	       "irq_routing_table located at: 0x%p\n",
	       __FILE__, __LINE__, __FUNCTION__, addr);

	sum = (unsigned char)(rt->checksum-sum);

	if (sum != rt->checksum) {
		printk(KERN_WARNING "%s:%6d:%s() - "
		       "checksum is: 0x%02x but should be: 0x%02x\n",
		       __FILE__, __LINE__, __FUNCTION__, rt->checksum, sum);
	}

	if (rt->signature != PIRQ_SIGNATURE || rt->version != PIRQ_VERSION ||
	    rt->size % 16 || rt->size < sizeof(struct irq_routing_table)) {
		printk(KERN_WARNING "%s:%6d:%s() - "
		       "Interrupt Routing Table not valid\n",
		       __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	sum = 0;
	for (i=0; i<rt->size; i++)
		sum += addr[i];

	if (sum) {
		printk(KERN_WARNING "%s:%6d:%s() - "
		       "checksum error in irq routing table\n",
		       __FILE__, __LINE__, __FUNCTION__);
	}
#endif /* #if defined(L440BX) */
}

#define RTABLE_DEST 0xf0000

void intel_copy_irq_routing_table(void)
{
#ifdef SIS630
	memcpy((char *) RTABLE_DEST, &intel_irq_routing_table, intel_irq_routing_table.size);
#endif
}

