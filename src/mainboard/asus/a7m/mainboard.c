#include <pci_ids.h>
#include <pci.h>
#include <arch/io.h>
#include <printk.h>
#include <arch/pirq_routing.h>
#include <pc80/mc146818rtc.h>

#define CHECKSUM 0x8d

static void pci_routing_fixup(void)
{
	struct pci_dev *dev;

        dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C686, 0);
	if (dev != NULL) {
		/* initialize PCI interupts */
		pci_write_config_byte(dev, 0x51, 0x76);
		pci_write_config_byte(dev, 0x55, 0xc0);
		pci_write_config_byte(dev, 0x56, 0x0a);
		pci_write_config_byte(dev, 0x57, 0xc0);
	}
}

static void a7m_northbridge_fixup(void)
{
	struct pci_dev *dev;

        dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_761_1, 0);
	if (dev != NULL) {
		/* load addresses and limits */
		pci_write_config_byte(dev, 0x1c, 0xd1);
		pci_write_config_byte(dev, 0x1d, 0xd1);
		pci_write_config_byte(dev, 0x20, 0x80);
		pci_write_config_byte(dev, 0x21, 0xf5);
		pci_write_config_byte(dev, 0x22, 0x50);
		pci_write_config_byte(dev, 0x23, 0xf7);
		pci_write_config_byte(dev, 0x24, 0x70);
		pci_write_config_byte(dev, 0x25, 0xf7);
		pci_write_config_byte(dev, 0x26, 0x70);
		pci_write_config_byte(dev, 0x27, 0xf7);
	}
}

static void pci_functions_fixup(void)
{
	struct pci_dev *dev;
	struct pci_dev dev_cpy;

        dev = pci_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_82C686, 0);
	if (dev != NULL) {
		printk_debug("PCI function 0 found\n");
		/* initialize PCI misc */
		pci_write_config_byte(dev, 0x70, 0x43);
		pci_write_config_byte(dev, 0x71, 0x10);
		pci_write_config_byte(dev, 0x72, 0x40);
		pci_write_config_byte(dev, 0x73, 0x80);
		pci_write_config_byte(dev, 0x7a, 0x60);
		memcpy(&dev_cpy,dev,sizeof(dev_cpy));
		dev=&dev_cpy;
		dev->devfn|=1;  /* increment to function 1 */
	}
	else
		printk_debug("PCI function 1 Not found\n");
}


static void pci_eth0_fixup(void)
{
	struct pci_dev *dev;

        dev = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82557, 0);
	if (dev != NULL) {
		/* initialize PCI Intel ethernet controler interupt line */
		pci_write_config_byte(dev, 0x3c, 0x0c);
		pci_write_config_byte(dev, 0x3d, 0x01);
		/* The following were added ???? */
		pci_write_config_byte(dev, 0x04, 0x07);
		pci_write_config_byte(dev, 0x0c, 0x08);
		pci_write_config_byte(dev, 0x0d, 0x20);
	}
	else
		printk_debug("PCI eth0 Not found\n");
}

static void lpc_generic_range_enable(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_VIPER_7410,0);
	if (dev != NULL) {
		/* Send io addres 0x0c00 the LPC bus */
		pci_write_config_dword(dev, 0x58, 0x0c01);
	}
}

const struct irq_routing_table via_irq_routing_table = {
        PIRQ_SIGNATURE, /* u32 signature */
        PIRQ_VERSION,   /* u16 version   */
        32+16*9,        /* there can be total 9 devices on the bus */
        0x00,           /* Bus 0 */
        0x20,           /* Device 1, Function 0 */
        0x0000,         /* reserve IRQ for PCI */
        0x1106,         /* VIA Technologies */
        0x0586,         /* VT82C686 ISA Bridge */
        0x00,           /*  u32 miniport_data  - "crap" */
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
        CHECKSUM,       /*  u8 checksum       - mod 256 checksum must give zero */
        {  /* bus, devfn, {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu  */
                {0x00, 0x20, {{0x01, 0x1eb8}, {0x02, 0x1eb8}, {0x03, 0x1eb8}, {0x05, 0x1eb8}}, 0x00, 0x00},
                {0x00, 0x60, {{0x01, 0x1eb8}, {0x02, 0x1eb8}, {0x03, 0x1eb8}, {0x05, 0x1eb8}}, 0x01, 0x00},
                {0x00, 0x58, {{0x02, 0x1eb8}, {0x03, 0x1eb8}, {0x05, 0x1eb8}, {0x01, 0x1eb8}}, 0x02, 0x00},
                {0x00, 0x50, {{0x03, 0x1eb8}, {0x05, 0x1eb8}, {0x01, 0x1eb8}, {0x02, 0x1eb8}}, 0x03, 0x00},
                {0x00, 0x48, {{0x05, 0x1eb8}, {0x01, 0x1eb8}, {0x02, 0x1eb8}, {0x03, 0x1eb8}}, 0x04, 0x00},
                {0x00, 0x68, {{0x05, 0x1eb8}, {0x01, 0x1eb8}, {0x02, 0x1eb8}, {0x03, 0x1eb8}}, 0x05, 0x00},
                {0x00, 0x38, {{0x03, 0x1eb8}, {0x05, 0x1eb8}, {0x01, 0x1eb8}, {0x02, 0x1eb8}}, 0x00, 0x00},
                {0x00, 0x28, {{0x02, 0x1eb8}, {0x03, 0x1eb8}, {0x05, 0x1eb8}, {0x01, 0x1eb8}}, 0x00, 0x00},
                {0x01, 0x28, {{0x01, 0x1eb8}, {0x02, 0x1eb8}, {0x03, 0x1eb8}, {0x05, 0x1eb8}}, 0x06, 0x00},
        }
};

	 
void load_irq_routing_table(void)
{
	unsigned char *bios_ram;
	int cnt;
	unsigned char *irq_table;

	printk_debug("Loading irq routing table\n");
	irq_table=(unsigned char *)&via_irq_routing_table;
	bios_ram=(unsigned char *)0x0f0000;
	for(cnt=0;cnt<176;cnt++)
		bios_ram[cnt]=irq_table[cnt];
	printk_debug("irq routing table loaded\n");		
	return;
}

static unsigned int pci_table[12]=
	{0x80000000,0x80000800,0x80002000,0x80002100,0x80002200,
	0x80002300,0x80002400,0x80002800,0x80004800,0x80012800,0,0};
	 
void pci_dump(void)
{
	unsigned int addr;
	int cntb,cnta;
	unsigned char chr;

	printk_info("Printing pci table\n");
	for(cnta=0;cnta<10;cnta++)
	{
		for(cntb=0;cntb<256;cntb++)
		{
			if((cntb%16)==0)
				printk_info("\n");
			addr=pci_table[cnta]+cntb;
			chr=intel_conf_readb(addr);
			printk_info("%2.2x ",chr);
		}
		printk_info("\n\n");
	}
	printk_info("pci table done\n");		
	return;
}

void mainboard_fixup(void)
{
	pci_routing_fixup();
	pci_eth0_fixup();
	keyboard_on();
	a7m_northbridge_fixup();
	southbridge_fixup();
	pci_functions_fixup();
	load_irq_routing_table();
//	rtc_init(0);
//	pci_dump();
}
