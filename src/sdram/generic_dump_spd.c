#include <printk.h>
#include <smbus.h>

void dump_spd_registers(void)
{
	unsigned device;
	device = SMBUS_MEM_DEVICE_START;
	printk_debug("\n");
	while(device <= SMBUS_MEM_DEVICE_END) {
		int status = 0;
		int i;
		printk_debug("dimm %02x", device);
		for(i = 0; (i < 256) && (status == 0); i++) {
			unsigned char byte;
			if ((i % 20) == 0) {
				printk_debug("\n%3d: ", i);
			}
			status = smbus_read_byte(device, i, &byte);
			if (status != 0) {
				printk_debug("bad device\n");
				continue;
			}
			printk_debug("%02x ", byte);
		}
		device += SMBUS_MEM_DEVICE_INC;
		printk_debug("\n");
	}
}
