#include <pci.h>
#include <arch/io.h>
#include <part/sizeram.h>
#include <printk.h>

unsigned long sizeram(void)
{
	unsigned long size;
	unsigned short word;
	
	/* Read TOLM */
	/* How should we handle > 4GB of ram? */
	pcibios_read_config_word(0, 0, 0xc4, &word);
	/* Convert size in 128K bytes to size in K bytes */
	size = word << 5;
	return size;

}
