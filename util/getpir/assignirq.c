/* checkpir.c : This software is released under GPL
   For Linuxbios use only
   Aug 26 2001 , Nikolai Vladychevski, <niko@isl.net.mx>
*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <pci/pci.h>
#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100
#define IRQ_SLOT_COUNT 128

struct irq_info {
	u8 bus, devfn;			/* Bus, device and function */
	struct {
		u8 link;		/* IRQ line ID, chipset dependent, 0=not routed */
		u16 bitmap;		/* Available IRQs */
	} __attribute__((packed)) irq[4];
	u8 slot;			/* Slot number, 0=onboard */
	u8 rfu;
} __attribute__((packed));

struct irq_routing_table {
	u32 signature;			/* PIRQ_SIGNATURE should be here */
	u16 version;			/* PIRQ_VERSION */
	u16 size;			/* Table size in bytes */
	u8 rtr_bus, rtr_devfn;		/* Where the interrupt router lies */
	u16 exclusive_irqs;		/* IRQs devoted exclusively to PCI usage */
	u16 rtr_vendor, rtr_device;	/* Vendor and device ID of interrupt router */
	u32 miniport_data;		/* Crap */
	u8 rfu[11];
	u8 checksum;			/* Modulo 256 checksum must give zero */
#ifndef IRQ_SLOT_COUNT
#if (__GNUC__ < 3)
	struct irq_info slots[1];
#else
	struct irq_info slots[];
#endif // __GNUC__ < 3
#else
  struct irq_info slots[IRQ_SLOT_COUNT];
#endif // ! IRQ_SLOT_COUNT
} __attribute__((packed));



#define PIRQ_SIGNATURE  (('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERS 0x0100


struct irq_routing_table rt;

struct pci_access *pacc;
struct pci_dev *dev;

// work to minimize: irqused and num* used

unsigned int irqused[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int numa = 0, numb = 0, numbc = 0, numd = 0;

int
getint(u16 m){
    int mask = m, i;
    unsigned int min = (unsigned int) -1;
    unsigned int minindex;
    /* find the least-used interrupt compatible with mask */
    printf("getint mask 0x%x\n", mask);
    for(minindex = 0, i = 0; i < 16; i++, mask>>=1) {
	printf("Check %d\n", i);
	if (! (mask & 1))
	    continue;
	printf("Irqused[%d] is %d\n", i, irqused[i]);
	if (irqused[i] < min) {
	    minindex = i;
	    min = irqused[i];
	    irqused[i]++;
	}
	printf("end of loop minindex is %d\n", minindex);
    }
    return minindex;
}
int calc_checksum(struct irq_routing_table *rt) {
    long sum=0,i;
    u8 *addr,sum2=0;
    
    addr= (u8 *) rt;
    for (i=0;i<rt->size;i++) sum2 += addr[i];
    return(sum2);
}	

int 
findpir() {
    FILE *fmem, *fpir;
    size_t rcount=0;
    unsigned long b,p,pir=PIRQ_SIGNATURE;
    unsigned long count;
    int i,valid=1,print=0;
    char cksum=0;
    unsigned char *ptr;
    unsigned short ts;

   printf("Opening memory...\n");
   fmem=fopen("/dev/mem","r"); 
   do {
	rcount=fread(&b,1,4,fmem);
	if (rcount>0) {
	    if (b==pir) {
		valid=1;
		printf("Found PCI IRQ Routing table signature at %x bytes from top of the memory\nValidating../\n",count);
		rt.signature=PIRQ_SIGNATURE;
		ptr=(char*) &rt;
		ptr=ptr+4;   // signature was read, advance 4 bytes
		rcount=fread(ptr,1,sizeof(struct irq_routing_table)-4,fmem);
	        count=count+rcount;
		printf("Version is:%d Table size:%d\n",rt.version,rt.size);
		if (rt.version!=PIRQ_VERS) {printf("Invalid version\n");valid=0;}
		if (rt.size<32) {printf(" Invalid table size\n"); valid=0;}
		if (rt.size%16) {printf (" Invalid table size (not a multiple of 16)\n"); valid=0;}
		if (valid)
		    break;
#if 0		
	        if (valid) {
			//	read slot entries
			ts=(rt.size-32)/16;
			printf("Reading %d slot entries...\n",ts);
			for (i=0;i<ts;i++) {
			    rcount=fread(&rt.slots[i],1,16,fmem);
	        	    count=count+rcount;
			}
			print=1;
		        break;
		}
#endif
	    } else {
	        count=count+rcount;
	    }
	}
   } while (rcount>0);
   if (!calc_checksum(&rt)) printf("Checksum is ok!\n");
   printf("Closing memory\n");
   fclose(fmem);
   return valid;
}
main () {
    u8 sum,newsum, pin, interrupt, line;
    u16 mask;
    int numdevices, i;
    struct irq_info *slot;

   if (getuid()) { perror("Run me as root, I need access to /dev/mem"); exit(1);}

    if (iopl(3) != 0) {
        perror("Can not set io priviliage");
        exit(1);
    }

    if (! findpir()) {
	printf("can't find the pir\n");
	exit(1);
    }
    pacc = pci_alloc();

    pci_init(pacc);
    pci_scan_bus(pacc);

    printf("Validating checksum, file: irq_tables.c that was in ./ at compile time...\n");
    printf("(no other tests are done)\n");
    if (!sum) { printf("Checksum for IRQ Routing table is ok. You can use it in LinuxBios now\n"); }
    else {
	newsum=rt.checksum-sum;
    	printf("BAD CHECKSUM for IRQ Routing table !!!!\n");
	printf("If you want to make it valid, change the checksum to: %#x\n",newsum);
    }
    /* now try assigning interrupts */
    numdevices = (rt.size-32)/16;
    printf("There are %d devices\n", numdevices);
    for(slot = rt.slots, i = 0; i < numdevices; i++, slot++) {
	printf("bus 0x%x devfn 0x%x\n", slot->bus, slot->devfn);
	dev = pci_get_dev(pacc, slot->bus, slot->devfn>>3, 0);
	if (! dev) {
	    printf("no dev at this place\n");
	}
	/* get the interrupt pin */
	pin = pci_read_byte(dev, PCI_INTERRUPT_PIN);
	printf("pin is %d\n", pin);
	if ((pin < 1) || (pin > 4)) {
	    printf("invalid pin\n");
	    continue;
	}

	mask = slot->irq[pin-1].bitmap;
	printf("irq mask for this pin is 0x%x\n", mask);
	
	interrupt = getint(mask);
	printf("interrupt is 0x%x\n", interrupt);
	line = pci_read_byte(dev, PCI_INTERRUPT_LINE);
	printf("would set reg 0x%x to 0x%x, currently is 0x%x\n", 
			PCI_INTERRUPT_LINE, interrupt, line);
	pci_free_dev(dev);
    }
}
