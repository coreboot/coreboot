#include <mem.h>
#include <pci.h>
#include <cpu/p5/io.h>
#include <part/sizeram.h>

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	u32 size;
	/* Use the PCI top memory register */
	pcibios_read_config_dword(0, 0, 0x9c, &size);
	/* Convert size in bytes to size in K */
	size = size >> 10;

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = size - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	return mem;
}

