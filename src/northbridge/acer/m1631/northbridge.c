#include <pci.h>
#include <printk.h>

// FIX ME!
unsigned long sizeram()
{
	extern void cache_disable(void), cache_enable(void);
	int i;
        struct pci_dev *pcidev;
	volatile unsigned char *cp; 
	u32 ram;
	unsigned long size;
	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (! pcidev)
		return 0;
	printk("Acer sizeram pcidev %p\n", pcidev);

	/* now read and print registers for ram ...*/
	for(i = 0x6c; i < 0x78; i++) {

		pci_read_config_dword(pcidev, i, &ram);
		size = (1 << (((ram >> 20) & 0x7))) * (0x400000);
		printk("0x%x 0x%x, size 0x%x\n", i, ram, size);
	}
	printk("so is the first one double-sided? \n");
	cache_disable();
	pci_read_config_dword(pcidev, 0x6c, &ram);
	size = (1 << (((ram >> 20) & 0x7))) * (0x400000);
	printk("set cp to 0x%x\n", size);
	cp = (char *) size;
	printk("cp is now %p\n", cp);
	*cp = 0x55;
	// how odd. 
	// what happens is if there is a 2nd row, then it will 
	// read back REGARDLESS of the settings of the bits in the
	// register! We verified this with the arium ...
	// RGM 4/10/01
	if (*cp == 0x55) {
		ram |= 0x1800000;		
		printk("Jam 0x%x into 0x6c\n", ram);
		pci_write_config_dword(pcidev, 0x6c, ram);
		printk("@ cp now is 0x%x\n", *cp);
		// set the base address for the next dram slot 
		// (if there is any ... )
		cp += size;
	}
	printk("cp now is 0x%x\n", cp);
	// now do the other two banks. 
#define INIT_MCR 0xf663f83c
	for(i = 0x70; i < 0x78; i += 4) {
		u32 temp;
		unsigned long size, cas, offset;
		printk("OK, let's try the other two banks\n");
		pci_read_config_dword(pcidev, i, &temp);
		pci_write_config_dword(pcidev, i, INIT_MCR);
		printk("Slot 0x%x: set to 0x%x\n", i, INIT_MCR);
		// anyone home?
		printk("Slot 0x%x: set value at %p\n", i, cp);
		*cp = 0x55;
		printk("Slot 0x%x: value at %p is 0x%x\n", cp, *cp);
		if (*cp != 0x55) {
			printk("Nothing in slot 0x%x\n", i);
			pci_write_config_dword(pcidev, i, temp);
			continue;
		}
		// get the cas bank size. 
		for(cas = 0, offset = 0x800; ;cas++, offset <<= 1) {
			*cp = 0;
			printk("Slot %x: check at %p\n", i, (cp+offset));
			*(cp + offset) = cas + 1;
			printk("Slot %x: cas %d, *cp %d\n", i, cas, *cp);
			if (*cp)
				break;
			if (cas > 2)
				break;
		}
		printk("Slot 0x%x: cas is 0x%x\n", i, cas);
		// now set the cas value into bits 19:16
		cas <<= 16;
		pci_read_config_dword(pcidev, i, &temp);
		temp &= ~0xf0000;
		temp |= cas;
		pci_write_config_dword(pcidev, i, temp);
		// now size it. 
		for(*cp = 0, offset = 0x400000, size = 0; 
		    size < 7; offset <<= 1, size++) {
			*(cp + offset) = 4;
			if (*cp)
				break;
		}
		printk("Slot 0x%x: size 0x%x\n", i, size);
		// fix up size bits
		temp &= ~0x700000;
		temp |= size << 20;
		temp |= 1;
		temp &= ~0x1000;
		pci_write_config_dword(pcidev, i, temp);
		printk("Slot 0x%x: before banks wrote 0x%x\n", i, temp);
		// now see what the banks are. 
		*cp = 0;
		*(cp + 0x1000) = 5;
		*(cp + 0x2000) = 6;
		*(cp + 0x4000) = 7;
		if (*cp) {
			printk("Slot 0x%x: two banks\n", i);
			// only two banks
			temp &= ~1;
			pci_write_config_dword(pcidev, i, temp);
		}
		else
			printk("Slot 0x%x: four banks\n", i);
		// compute real size
		size = (1<<size) * 0x400000;
		printk("Slot 0x%x: size is 0x%x\n", i, size);
		// advance cp for the next area to check
		cp += size;
		// is it two-sided
		temp |= 0x1800000;
		pci_write_config_dword(pcidev, i, temp);
		*cp = 0xaa;
		printk("Slot 0x%x: value at %p is 0x%x\n", i, cp, *cp);
		if (*cp != 0xaa) { // two side
			cp += size;
			temp &= ~0x1800000;
			pci_write_config_dword(pcidev, i, temp);
			printk("Slot %d: one-sided\n", i);
		}
	}
	cache_enable();
	return 0; //64*1024*1024;
}


#ifdef HAVE_FRAMEBUFFER

void intel_framebuffer_on()
{
}
#endif

void
final_northbridge_fixup()
{
    printk("SET THAT BIT!\n");
    /* set bit 4 of north bridge register d4 to 1 */
}




