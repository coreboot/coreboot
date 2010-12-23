#include "raminit.h"
#include <spd.h>
#include <console/console.h>

void dump_spd_registers(void)
{
#if CONFIG_DEBUG_RAM_SETUP
	int i;
	printk(BIOS_DEBUG, "\n");
	for(i = 0; i < DIMM_SOCKETS; i++) {
		unsigned device;
		device = DIMM0 + i;
		if (device) {
			int j;
			printk(BIOS_DEBUG, "DIMM %d: %02x", i, device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					printk(BIOS_DEBUG, "\n%02x: ", j);
				}
				status = spd_read_byte(device, j);
				if (status < 0) {
					printk(BIOS_DEBUG, "bad device\n");
					break;
				}
				byte = status & 0xff;
				printk(BIOS_DEBUG, "%02x ", byte);
			}
			printk(BIOS_DEBUG, "\n");
		}
	}
#endif
}
