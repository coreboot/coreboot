#include <pci.h>
#include <cpu/p5/io.h>
#include <part/sizeram.h>

unsigned long sizeram(void)
{
	unsigned long size;
	/* Use the PCI top memory register */
	pcibios_read_config_dword(0, 0, 0x9c, &size);
	/* Convert size in bytes to size in K */
	size = size >> 10;
	return size;
}

