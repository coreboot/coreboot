#include <mem.h>
#include <pci.h>
#include <arch/io.h>
#include <part/sizeram.h>
#include <printk.h>

struct mem_range *sizeram(void)
{
	unsigned long size;
	unsigned short word;
	
	/* Read TOM */
	/* How should we handle > 4GB of ram? */
	pcibios_read_config_word(0, 0, 0xc4, &word);
	/* Convert size in 64K bytes to size in K bytes */
	size = word << 6;

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = size - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	return &mem;
}
