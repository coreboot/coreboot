#include <pci.h>
#include <printk.h>

static 
void refresh_set(int turn_it_on)
{
        struct pci_dev *pcidev;
	u32 ref;

	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (! pcidev) // won't happen but ...
		return;

	pci_read_config_dword(pcidev, 0x7c, &ref);
//	printk(KERN_INFO __FUNCTION__ "refresh was 0x%lx onoff is %d\n", 
//		ref, turn_it_on);
	if (turn_it_on)
		ref |= (1 << 19);
	else
		ref &= ~(1 << 19);
			
	pci_write_config_dword(pcidev, 0x7c, ref);
	pci_read_config_dword(pcidev, 0x7c, &ref);
//	printk(KERN_INFO __FUNCTION__ "refresh is now 0x%lx\n", ref);
}
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
	for(i = 0x6c; i < 0x78; i += 4) {

		pci_read_config_dword(pcidev, i, &ram);
		size = (1 << (((ram >> 20) & 0x7))) * (0x400000);
		printk("0x%x 0x%x, size 0x%x\n", i, ram, size);
	}
	pci_read_config_dword(pcidev, 0x6c, &ram);
	size = (1 << (((ram >> 20) & 0x7))) * (0x400000);
	if ((ram & 0x1800000) == 0x1800000)
		size <<= 1;
	printk("size in 0x6c is 0x%x\n", size);
#if 1
	printk("NOT SIZING OTHER TWO SLOTS. ONLY ONE SLOT RIGHT NOW\n");
	return size/1024;
#endif
	cp = (unsigned char *) size;
	// now do the other two banks. 
#define INIT_MCR 0xf663b83c
	for(i = 0x70; i < 0x78; i += 4) {
		u32 temp;
		u8 c1, c2;
		unsigned long size, cas, offset;
		printk("OK, let's try the other two banks\n");
		pci_read_config_dword(pcidev, i, &temp);
		pci_write_config_dword(pcidev, i, INIT_MCR);
		printk("Slot 0x%x: set to 0x%x\n", i, INIT_MCR);
		// anyone home?
		cache_disable();
		printk("Slot 0x%x: set value at %p\n", i, cp);
		refresh_set(0);
		*cp = 0x55;
		*(cp + 8) = 0xaa;
		c1 = *cp;
		c2 = *(cp + 8);
		refresh_set(1);
		cache_enable();
		printk("Slot 0x%x: value at %p is 0x%x\n", i, cp, c1);
		printk("Slot 0x%x: value at %p is 0x%x\n", i, cp+8, c2);

		if ((c1 != 0x55) || (c2 != 0xaa)) {
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
		temp &= 0xe07fffff;
		pci_write_config_dword(pcidev, i, temp);
		printk("Disabled it\n");
		// enable other side. 
		temp |= 0x11800000;
		printk("Slot 0x%x: enable second side\n", i);
		refresh_set(0);
		// RONNIE: hangs here !
		pci_write_config_dword(pcidev, i, temp);
		printk("Slot %d: DONE enable second side\n", i);
		cache_disable();

		*cp = 0xaa;
		*(cp + 8) = 0x55;
		c1 = *cp;
		refresh_set(1);
		cache_enable();
		printk("Slot 0x%x: value at %p is 0x%x\n", i, cp, c1);
		if (c1 == 0xaa) { // two side
			cp += size;
			printk("Slot 0x%x: two-sided\n", i);
		} else { // one side
			temp &= ~0x1800000;
			printk("Slot 0x%x: one side\n", i);
		}
		// turn the first slot back on
		temp |= 0x6000000;
		pci_write_config_dword(pcidev, i, temp);
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




