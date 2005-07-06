#include <sys/io.h>
#include <stdio.h>
#include <pci/pci.h>
#include <stdlib.h>

static int enable_flash_sis630(struct pci_dev *dev, char *name)
{
	char b;

	/* get io privilege access PCI configuration space */
	if (iopl(3) != 0) {
		perror("Can not set io priviliage");
		exit(1);
	}

	/* Enable 0xFFF8000~0xFFFF0000 decoding on SiS 540/630 */
	outl(0x80000840, 0x0cf8);
	b = inb(0x0cfc) | 0x0b;
	outb(b, 0xcfc);
	/* Flash write enable on SiS 540/630 */
	outl(0x80000845, 0x0cf8);
	b = inb(0x0cfd) | 0x40;
	outb(b, 0xcfd);

	/* The same thing on SiS 950 SuperIO side */
	outb(0x87, 0x2e);
	outb(0x01, 0x2e);
	outb(0x55, 0x2e);
	outb(0x55, 0x2e);

	if (inb(0x2f) != 0x87) {
		outb(0x87, 0x4e);
		outb(0x01, 0x4e);
		outb(0x55, 0x4e);
		outb(0xaa, 0x4e);
		if (inb(0x4f) != 0x87) {
			printf("Can not access SiS 950\n");
			return -1;
		}
		outb(0x24, 0x4e);
		b = inb(0x4f) | 0xfc;
		outb(0x24, 0x4e);
		outb(b, 0x4f);
		outb(0x02, 0x4e);
		outb(0x02, 0x4f);
	}

	outb(0x24, 0x2e);
	printf("2f is %#x\n", inb(0x2f));
	b = inb(0x2f) | 0xfc;
	outb(0x24, 0x2e);
	outb(b, 0x2f);

	outb(0x02, 0x2e);
	outb(0x02, 0x2f);

	return 0;
}

static int enable_flash_e7500(struct pci_dev *dev, char *name)
{
	/* register 4e.b gets or'ed with one */
	unsigned char old, new;
	/* if it fails, it fails. There are so many variations of broken mobos
	 * that it is hard to argue that we should quit at this point. 
	 */

	old = pci_read_byte(dev, 0x4e);

	new = old | 1;

	if (new == old)
		return 0;

	pci_write_byte(dev, 0x4e, new);

	if (pci_read_byte(dev, 0x4e) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x4e, new, name);
		return -1;
	}
	return 0;
}

static int enable_flash_ich4(struct pci_dev *dev, char *name)
{
	/* register 4e.b gets or'ed with one */
	unsigned char old, new;
	/* if it fails, it fails. There are so many variations of broken mobos
	 * that it is hard to argue that we should quit at this point. 
	 */

	old = pci_read_byte(dev, 0x4e);

	new = old | 1;

	if (new == old)
		return 0;

	pci_write_byte(dev, 0x4e, new);

	if (pci_read_byte(dev, 0x4e) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x4e, new, name);
		return -1;
	}
	return 0;
}

static int enable_flash_vt8235(struct pci_dev *dev, char *name)
{
	unsigned char old, new, val;
	unsigned int base;
	int ok;

	/* get io privilege access PCI configuration space */
	if (iopl(3) != 0) {
		perror("Can not set io priviliage");
		exit(1);
	}

	old = pci_read_byte(dev, 0x40);

	new = old | 0x10;

	if (new == old)
		return 0;

	ok = pci_write_byte(dev, 0x40, new);
	if (ok != 0) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       old, new, name);
	}

	/* enable GPIO15 which is connected to write protect. */
	base = ((pci_read_byte(dev, 0x88) & 0x80) | pci_read_byte(dev, 0x89) << 8);
	val = inb(base + 0x4d);
	val |= 0x80;
	outb(val, base + 0x4d);

	if (ok != 0) {
		return -1;
	} else {
		return 0;
	}
}

static int enable_flash_vt8231(struct pci_dev *dev, char *name)
{
	unsigned char val;

	val = pci_read_byte(dev, 0x40);
	val |= 0x10;
	pci_write_byte(dev, 0x40, val);

	if (pci_read_byte(dev, 0x40) != val) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x40, val, name);
		return -1;
	}
	return 0;
}

static int enable_flash_cs5530(struct pci_dev *dev, char *name)
{
	unsigned char new;

	pci_write_byte(dev, 0x52, 0xee);

	new = pci_read_byte(dev, 0x52);

	if (new != 0xee) {
		printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x52, new, name);
		return -1;
	}
	return 0;
}

static int enable_flash_sc1100(struct pci_dev *dev, char *name)
{
	unsigned char new;

	pci_write_byte(dev, 0x52, 0xee);

	new = pci_read_byte(dev, 0x52);

	if (new != 0xee) {
		printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x52, new, name);
		return -1;
	}
	return 0;
}

static int enable_flash_sis5595(struct pci_dev *dev, char *name)
{
	unsigned char new, newer;

	new = pci_read_byte(dev, 0x45);

	/* clear bit 5 */
	new &= (~0x20);
	/* set bit 2 */
	new |= 0x4;

	pci_write_byte(dev, 0x45, new);

	newer = pci_read_byte(dev, 0x45);
	if (newer != new) {
		printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x45, new, name);
		printf("Stuck at 0x%x\n", newer);
		return -1;
	}
	return 0;
}

static int enable_flash_amd8111(struct pci_dev *dev, char *name)
{
	/* register 4e.b gets or'ed with one */
	unsigned char old, new;
	/* if it fails, it fails. There are so many variations of broken mobos
	 * that it is hard to argue that we should quit at this point. 
	 */

	/* enable decoding at 0xffb00000 to 0xffffffff */
	old = pci_read_byte(dev, 0x43);
	new = old | 0xC0;
	if (new != old) {
		pci_write_byte(dev, 0x43, new);
		if (pci_read_byte(dev, 0x43) != new) {
			printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
			       0x43, new, name);
		}
	}

	old = pci_read_byte(dev, 0x40);
	new = old | 0x01;
	if (new == old)
		return 0;
	pci_write_byte(dev, 0x40, new);

	if (pci_read_byte(dev, 0x40) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x40, new, name);
		return -1;
	}
	return 0;
}

//By yhlu
static int enable_flash_ck804(struct pci_dev *dev, char *name)
{
        /* register 4e.b gets or'ed with one */
        unsigned char old, new;
        /* if it fails, it fails. There are so many variations of broken mobos
         * that it is hard to argue that we should quit at this point. 
         */

        //dump_pci_device(dev); 
        
        old = pci_read_byte(dev, 0x88);
        new = old | 0xc0;
        if (new != old) {
                pci_write_byte(dev, 0x88, new);
                if (pci_read_byte(dev, 0x88) != new) {
                        printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
                               0x88, new, name);
                }
        }

        old = pci_read_byte(dev, 0x6d);
        new = old | 0x01;
        if (new == old)
                return 0;
        pci_write_byte(dev, 0x6d, new);

        if (pci_read_byte(dev, 0x6d) != new) {
                printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
                       0x6d, new, name);
                return -1;
        }
        return 0;
}

typedef struct penable {
	unsigned short vendor, device;
	char *name;
	int (*doit) (struct pci_dev * dev, char *name);
} FLASH_ENABLE;

static FLASH_ENABLE enables[] = {
	{0x1039, 0x0630, "sis630", enable_flash_sis630},
	{0x8086, 0x2480, "E7500", enable_flash_e7500},
	{0x8086, 0x24c0, "ICH4", enable_flash_ich4},
	{0x1106, 0x8231, "VT8231", enable_flash_vt8231},
	{0x1106, 0x3177, "VT8235", enable_flash_vt8235},
	{0x1078, 0x0100, "CS5530", enable_flash_cs5530},
	{0x100b, 0x0510, "SC1100", enable_flash_sc1100},
	{0x1039, 0x0008, "SIS5595", enable_flash_sis5595},
	{0x1022, 0x7468, "AMD8111", enable_flash_amd8111},
        {0x10de, 0x0050, "NVIDIA CK804", enable_flash_ck804}, // LPC
        {0x10de, 0x0051, "NVIDIA CK804", enable_flash_ck804}, // Pro
        {0x10de, 0x00d3, "NVIDIA CK804", enable_flash_ck804}, // Slave, should not be here, to fix known bug for A01.
};

int enable_flash_write()
{
	int i;
	struct pci_access *pacc;
	struct pci_dev *dev = 0;
	FLASH_ENABLE *enable = 0;

	pacc = pci_alloc();	/* Get the pci_access structure */
	/* Set all options you want -- here we stick with the defaults */
	pci_init(pacc);		/* Initialize the PCI library */
	pci_scan_bus(pacc);	/* We want to get the list of devices */

	/* now let's try to find the chipset we have ... */
	for (i = 0; i < sizeof(enables) / sizeof(enables[0]) && (!dev);
	     i++) {
		struct pci_filter f;
		struct pci_dev *z;
		/* the first param is unused. */
		pci_filter_init((struct pci_access *) 0, &f);
		f.vendor = enables[i].vendor;
		f.device = enables[i].device;
		for (z = pacc->devices; z; z = z->next)
			if (pci_filter_match(&f, z)) {
				enable = &enables[i];
				dev = z;
			}
	}

	/* now do the deed. */
	if (enable) {
		printf("Enabling flash write on %s...", enable->name);
		if (enable->doit(dev, enable->name) == 0)
			printf("OK\n");
	}
	return 0;
}
