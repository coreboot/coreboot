#include <mem.h>
#include <part/sizeram.h>
#include <printk.h>
#include <pci.h>
#include <pciconf.h>

/* 
 * Automatic memory configuration by SONE Takeshi <ts1@tsn.or.jp>, 05/19/03
 */

static unsigned long find_size(unsigned long addr, unsigned long minimum)
{
    unsigned long i;
    unsigned long maximum;
    volatile long *p;

    /* First, see if there is any RAM. */
    p = (long *) (addr + minimum);
    *p = 0x12345678;
    p = (long *) (addr + minimum + 8);
    *p = 0x87654321;
    p = (long *) (addr + minimum);
    if (*p != 0x12345678)
        return 0; /* No memory */

    maximum = (0xffUL << 23) - addr;

    /* Write to addresses with only one address bit on,
     * in increasing order, from address 8 (assuming 64-bit bus),
     * then read address zero to see if it gets wrap-around.
     * This way we can detect missing address bit due to incorrect
     * MA mapping, or the size of bank when MA mapping is correct. */

    for (i = 8; i < maximum; i <<= 1) {
        if (i < minimum)
            continue;
        p = (long *) (addr + i);
        *p = 0x89abcdef;
        p = (long *) addr;
        if (*p == 0x89abcdef)
            return i;
    }
    return maximum;
}

static void set_ma_mapping(struct pci_dev *pcidev, int bank, int type)
{
    unsigned char reg, val;
    int shift;

    reg = 0x58 + bank/4;
    if (bank%4 >= 2)
        shift = 0;
    else
        shift = 4;

    pci_read_config_byte(pcidev, reg, &val);
    val &= ~(0xf << shift);
    val |= type << shift;
    pci_write_config_byte(pcidev, reg, val);
}

static int get_ma_mapping(struct pci_dev *pcidev, int bank)
{
    unsigned char reg, val;
    int shift;

    reg = 0x58 + bank/4;
    if (bank%4 >= 2)
        shift = 0;
    else
        shift = 4;

    pci_read_config_byte(pcidev, reg, &val);
    return (val >> shift) & 0xf;
}

static unsigned long __sizeram(void)
{
	u8 sma_status, sma_size_bits;	
        struct pci_dev *pcidev;
        unsigned long memtop, highest, size, sma_size;
        int bank, i;
        static const unsigned char ma_table[] = {0, 8, 0xe};
        unsigned char ma_tmp, val;
        extern void cache_enable(void), cache_disable(void);

	pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (! pcidev)
		return 0;

        /* In assembly part, we have initialized all RAM chips,
         * brought the first equipped RAM bank to address zero,
         * and set correct MA mapping type of that bank.
         * Now, we have to detect the size of the first bank,
         * then configure rest of banks. */

        /* Cache must be disabled to detect the RAM. */
        cache_disable();

        /* Find the first bank configured by assembly part. */
        for (bank = 0; bank < 6; bank++) {
            pci_read_config_byte(pcidev, 0x5a + bank, &val);
            if (val != 0)
                break;
        }

        memtop = find_size(0, 1024*1024);
        memtop &= ~0x7fffff; /* Unit of 8MB */

        printk_info("Bank%d %dMB (MA type 0x%x)\n", bank,
                memtop>>20, get_ma_mapping(pcidev, bank));

        pci_write_config_byte(pcidev, 0x5a + bank, memtop>>23);

        for (bank++; bank < 6; bank++) {
            if (bank & 1) {
                /* We don't change MA mapping of this bank
                 * since it is shared with the previous bank.
                 * Most possibly this is the other side of a 
                 * double-sided DIMM. */
                size = find_size(memtop, 0);
                size &= ~0x7fffff;
                if (size) {
                    printk_info("Bank%d %dMB\n", bank, size>>20);
                    memtop += size;
                }
            } else {
                /* Try MA mapping types and find the one which gives
                 * highest address without wrap-around.
                 * It should be the correct mapping for the DIMM,
                 * and the returned address is the size of the DIMM. */
                highest = 0;
                ma_tmp = 0;
                for (i = 0; i < sizeof(ma_table)/sizeof(ma_table[0]); i++) {
                    set_ma_mapping(pcidev, bank, ma_table[i]);
                    size = find_size(memtop, 0);
                    printk_debug("bank %d MA 0x%x: %d bytes\n",
                            bank, ma_table[i], size);
                    if (size > highest) {
                        highest = size;
                        ma_tmp = ma_table[i];
                    }
                }
                highest &= ~0x7fffff;
                if (highest) {
                    printk_info("Bank%d %dMB (MA type 0x%x)\n",
                            bank, highest>>20, ma_tmp);
                    memtop += highest;
                }
                set_ma_mapping(pcidev, bank, ma_tmp);
            }
            pci_write_config_byte(pcidev, 0x5a + bank, memtop>>23);
        }
        /* As well as 6 bank registers above, it seems we have to fill
         * these 2 registers. */
        pci_write_config_byte(pcidev, 0x56, memtop>>23);
        pci_write_config_byte(pcidev, 0x57, memtop>>23);

        cache_enable();

        /* Frame buffer size */

	// Documentation on VT8601 - Pg 51 Rev 1.3 Sept 1999 says
	// Device 0 Offset FB - Frame buffer control
	// bit
	//  7  VGA Enable (0 disable, 1 enable)
	//  6  VGA Reset (write 1 to reset)
	// 5-4 Frame Buffer size
	//      00 None (default)
	//      01 2M
	//      10 4M
	//      11 8M
	//  3  CPU Direct Access Frame Buffer (0 disable, 1 enable)
	// 2-0 CPU Direct Access Frame Buffer Base Adddress <31:29>
	pci_read_config_byte(pcidev, 0xfb, &sma_status);
	sma_size_bits = (sma_status >> 4) & 0x03;
	if (sma_size_bits > 0)
		sma_size = (1024*1024) << sma_size_bits;
	else
		sma_size = 0;

        printk_info("Total %dMB + frame buffer %dMB\n",
                (memtop - sma_size)>>20, sma_size>>20);

        /* Turn on shadow DRAM at 0xC0000-0xFFFFF so we can write
         * PIRQ table, VGA BIOS, Bochs BIOS, etc. */
        printk_debug("Enabling shadow DRAM at 0xC0000-0xFFFFF: ");
        pci_write_config_byte(pcidev, 0x61, 0xff);
        pci_write_config_byte(pcidev, 0x62, 0xff);
        pci_write_config_byte(pcidev, 0x63, 0xf0);
        printk_debug("done\n");

	return (memtop - sma_size) >> 10; // return in kilo bytes
}

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 768;
	mem[1].sizek = __sizeram();
	mem[2].basek = 0;
	mem[2].sizek = 0;
	if (mem[1].sizek == 0) {
		mem[1].sizek = 64*1024;
	}
	mem[1].sizek -= mem[1].basek;
	return mem;
}

#ifdef HAVE_FRAMEBUFFER
void framebuffer_on()
{
#if 0 /* This code has not been working (always reads 0xffff)
       * and I still can bring up VGA (with original VGA BIOS under ADLO)
       * after disabling this. -- ts1 */
	unsigned long devfn;
	u16 command;

	devfn = PCI_DEVFN(0, 1);
	pcibios_read_config_word(0, devfn, 0x3e, &command);
	//command |= 0x08;
	command |= 0x0c;
	pcibios_write_config_word(0, devfn, 0x3e, command);
        printk_debug("wrote %02x\n", command);
	pcibios_read_config_word(0, devfn, 0x3e, &command);
        printk_debug("readback %02x\n", command);
#endif
}
#endif

/*
 * This fixup is based on capturing values from an Award bios.  Without
 * this fixup the DMA write performance is awful (i.e. hdparm -t /dev/hda is 20x
 * slower than normal, ethernet drops packets).
 * Apparently these registers govern some sort of bus master behavior.
 */
void northbridge_fixup(void)
{
	struct pci_dev *pcidev = pci_find_slot(0, PCI_DEVFN(0,0));

	if (pcidev) {
		pci_write_config_byte(pcidev, 0x70, 0xc0);
		pci_write_config_byte(pcidev, 0x71, 0x88);
		pci_write_config_byte(pcidev, 0x72, 0xec);
		pci_write_config_byte(pcidev, 0x73, 0x0c);
		pci_write_config_byte(pcidev, 0x74, 0x0e);
		pci_write_config_byte(pcidev, 0x75, 0x81);
		pci_write_config_byte(pcidev, 0x76, 0x52);
	}
}

