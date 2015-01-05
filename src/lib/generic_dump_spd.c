/*
 * This code is derived from the Opteron boards' debug.c.
 * It should go away either there or here, depending what fits better.
 */

static void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	printk(BIOS_DEBUG, "\n");
	for(i = 0; i < 4; i++) {
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.0: %02x", i, device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0)
					printk(BIOS_DEBUG, "\n%02x: ", j);
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
		device = ctrl->channel1[i];
		if (device) {
			int j;
			printk(BIOS_DEBUG, "dimm: %02x.1: %02x", i, device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0)
					printk(BIOS_DEBUG, "\n%02x: ");
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
}

#if 0
void dump_spd_registers(void)
{
	unsigned device;
	device = SMBUS_MEM_DEVICE_START;
	printk(BIOS_DEBUG, "\n");
	while(device <= SMBUS_MEM_DEVICE_END) {
		int status = 0;
		int i;
		printk(BIOS_DEBUG, "dimm %02x", device);
		for(i = 0; (i < 256) && (status == 0); i++) {
			unsigned char byte;
			if ((i % 20) == 0) {
				printk(BIOS_DEBUG, "\n%3d: ", i);
			}
			status = smbus_read_byte(device, i, &byte);
			if (status != 0) {
				printk(BIOS_DEBUG, "bad device\n");
				continue;
			}
			printk(BIOS_DEBUG, "%02x ", byte);
		}
		device += SMBUS_MEM_DEVICE_INC;
		printk(BIOS_DEBUG, "\n");
	}
}
#endif
