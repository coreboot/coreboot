#include <console/console.h>
#include <arch/io.h>
#include <spd.h>
#include "raminit.h"
#include <spd.h>
#include <console/console.h>

#if CONFIG_DEBUG_RAM_SETUP
void dump_spd_registers(void)
{
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
				status = smbus_read_byte(device, j);
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
}

void dump_pci_device(unsigned dev)
{
	int i;
	printk(BIOS_DEBUG, "PCI: %02x:%02x.%02x\n", (dev >> 20) & 0xff, (dev >> 15) & 0x1f, (dev >> 12) & 7);

	for (i = 0; i <= 255; i++) {
		unsigned char val;
		val = pci_read_config8(dev, i);
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%02x: %02x", i, val);
		if ((i & 0x0f) == 0x0f)
			printk(BIOS_DEBUG, "\n");
	}
}
#endif
