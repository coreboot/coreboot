#include <pci.h>
#include <arch/io.h>
#include <part/sizeram.h>
#include <printk.h>

unsigned long sizeram(void)
{
	unsigned long size;
	unsigned short word;
	
	printk_notice("\nsizeram!!!\n");
	/* Read TOM */
	/* How should we handle > 4GB of ram? */
	pcibios_read_config_word(0, 0, 0xc4, &word);
	/* Convert size in 64K bytes to size in K bytes */
	size = word << 6;
	return size;

}
