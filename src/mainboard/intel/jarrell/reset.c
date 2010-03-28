#include <arch/io.h>
#include <reset.h>
#include <arch/romcc_io.h>

void soft_reset(void)
{
        outb(0x04, 0xcf9);
}

void hard_reset(void)
{
        outb(0x02, 0xcf9);
        outb(0x06, 0xcf9);
}

void full_reset(void)
{
	/* Enable power on after power fail... */
	unsigned byte;
	byte = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
	byte &= 0xfe;
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, byte);
        outb(0x0e, 0xcf9);
}


