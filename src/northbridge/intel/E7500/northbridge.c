#include <mem.h>
#include <pci.h>
#include <arch/io.h>
#include <part/sizeram.h>
#include <printk.h>
#include <pc80/mc146818rtc.h>

struct mem_range *sizeram(void)
{
	static struct mem_range mem[4];
	uint16_t tolm, remapbase, remaplimit, drb16;
	uint16_t tolm_r, remapbase_r, remaplimit_r;
	uint8_t  drb;
	int remap_high;
	
	/* Calculate and report the top of low memory and 
	 * any remapping.
	 */
	/* Test if the remap memory high option is set */
        remap_high = 0;
        if(get_option(&remap_high, "remap_memory_high")){
                remap_high = 0;
        }
	pcibios_read_config_byte(0, 0, 0x67, &drb);
	drb16 = (uint16_t)drb;
	if(remap_high && (drb16 > 0x08)) {
		/* We only come here if we have at least 512MB of memory,
		 * so it is safe to hard code tolm.
		 */
		tolm = 0x2000;
		if(drb16 > 0x0040) {
			/* There is more than 4GB of memory put
			 * the remap window at the end of ram.
			 */
			remapbase = drb16;
			remaplimit = remapbase + 0x38;
		}
		else {
			remapbase = 0x0040;
			remaplimit = remapbase + (drb16-8);
		}
	}
	else {
		tolm = (uint16_t)((pci_memory_base >> 16)&0x0f800);
		if((tolm>>8) >= (drb16<<2)) {
			tolm = (drb16<<10);
			remapbase = 0x3ff;
			remaplimit = 0;
		}
		else {
			remapbase = drb16;
			remaplimit = remapbase + ((0x0040-(tolm>>10))-1);
		}
	}
	/* Write the ram configruation registers,
	 * preserving the reserved bits.
	 */
	pcibios_read_config_word(0, 0, 0xc4, &tolm_r);
	tolm |= (tolm_r & 0x7ff); 
	pcibios_write_config_word(0, 0, 0xc4, tolm);
	pcibios_read_config_word(0, 0, 0xc6, &remapbase_r);
	remapbase |= (remapbase_r & 0xfc00);
	pcibios_write_config_word(0, 0, 0xc6, remapbase);
	pcibios_read_config_word(0, 0, 0xc8, &remaplimit_r);
	remaplimit |= (remaplimit_r & 0xfc00);
	pcibios_write_config_word(0, 0, 0xc8, remaplimit);

#if 0
    printk_debug("mem info tolm = %x, drb = %x, pci_memory_base = %x, remap = %x-%x\n",tolm,drb,pci_memory_base,remapbase,remaplimit);
#endif
	
	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 768;
	/* Convert size in 64K bytes to size in K bytes */
	mem[1].sizek = (tolm << 6) - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	if ((drb << 16) > (tolm << 6)) {
		/* We don't need to consider the remap window
		 * here because we put it immediately after the
		 * rest of ram.
		 * All we must do is calculate the amount
		 * of unused memory and report it at 4GB.
		 */
		mem[2].basek = 4096*1024;
		mem[2].sizek = (drb << 16) - (tolm << 6);
	}
	mem[3].basek = 0;
	mem[3].sizek = 0;
	
	return (struct mem_range *) &mem;
}
