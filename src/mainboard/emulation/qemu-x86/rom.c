#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <cbfs.h>

/*
 */
void udelay(int usecs) 
{
	int i;
	for(i = 0; i < usecs; i++)
		outb(i&0xff, 0x80);
}

void main(void)
{
	int i;
	void uart_init(void);
	void (*start_address)();
	outb(5, 0x80);
	
	uart_init();
	start_address = cbfs_load_stage("fallback/coreboot_ram");
	start_address();
	
}
