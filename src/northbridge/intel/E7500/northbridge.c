#include <mem.h>
#include <pci.h>
#include <arch/io.h>
#include <part/sizeram.h>
#include <printk.h>

struct mem_range *sizeram(void)
{
	static struct mem_range mem[4];
	uint16_t tolm, remapbase, remaplimit;
	uint8_t  drb;
	
	/* FIXME do  some of the configuration here instead of
	 * just reading it all out, and reporting it.
	 */
	/* Read the ram configruation registers */
	pcibios_read_config_word(0, 0, 0xc4, &tolm);
	pcibios_read_config_word(0, 0, 0xc6, &remapbase);
	remapbase &= 0x1FF;
	pcibios_read_config_word(0, 0, 0xc8, &remaplimit);
	remaplimit &= 0x1FF;
	pcibios_read_config_byte(0, 0, 0x67, &drb);
	
	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 768;
	/* Convert size in 64K bytes to size in K bytes */
	mem[1].sizek = (tolm << 6) - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	if ((drb << 16) > (tolm << 6)) {
		mem[2].basek = 4096*1024;
		mem[2].sizek = (drb << 16) - mem[2].basek;
		/* I know that the remap window always immediately follows
		 * the real top of memory.
		 */
		if (remapbase < remaplimit) {
			mem[2].sizek = (remaplimit << 16) - mem[2].basek;
		}
	}
	mem[3].basek = 0;
	mem[3].sizek = 0;
	
	return &mem;
}
