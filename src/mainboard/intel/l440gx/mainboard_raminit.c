#include <arch/cache_ram.h>
#include <sdram.h>
#include <smbus.h>
#include <printk.h>
#include <pci.h>
#include <ramtest.h>

#define I440GX_BUS 0
#define I440GX_DEVFN ((0 << 3) | 0)

static void debug_raminit(void)
{
	int i;
	printk_debug("\n440GX registers");
	for(i = 0; i < 256; i++) {
		unsigned char byte;
		if ((i & 0x0f) == 0) {
			printk_debug("\n%02x: ", i);
		}
		pcibios_read_config_byte(I440GX_BUS, I440GX_DEVFN, i, &byte);
		printk_debug("%02x ", byte);
	}
	printk_debug("\n");
}

void cache_ram_start(void)
{
	displayinit();
	printk_info("Hello from cache_ram_start\n");
	printk_debug("Calling pci_set_method\n");
	pci_set_method();
	printk_debug("Calling smbus_enable\n");
	smbus_enable();
	printk_debug("Calling smbus_setup\n");
	smbus_setup();
	printk_debug("Calling dump_spd_registers\n");
	dump_spd_registers();
	printk_debug("Calling sdram_initialize\n");
	sdram_initialize();
	printk_debug("Calling debug_raminit\n");
	debug_raminit();
	printk_debug("Calling sdram_initialize_ecc\n");
	sdram_initialize_ecc();
	printk_debug("Calling ramcheck\n");
#if 0
	ramcheck(0, 128*1024, 30);
	ramcheck(128*1024, (128+64)*1024, 30);
	ramcheck(256*1024, (256+64)*1024, 30);
	ramcheck((256+128)*1024, 512*1024, 30);
	ramcheck(512*1024, 640*1024, 30);
	ramcheck(1024*1024, (1024*1024) + (128*1024), 30);
	ramcheck(64*1024*1024, (64*1024*1024) + (128*1024), 30);
	ramcheck(127*1024*1024, 128*1024*1024, 30);
#endif
	printk_debug("ram is setup\n");
}
