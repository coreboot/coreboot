/*
 * This code is derived from the Opteron boards' debug.c.
 * It should go away either there or here, depending what fits better.
 */

static void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	print_debug("\n");
	for(i = 0; i < 4; i++) {
		unsigned device;
		device = ctrl->channel0[i];
		if (device) {
			int j;
			print_debug("dimm: ");
			print_debug_hex8(i);
			print_debug(".0: ");
			print_debug_hex8(device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					print_debug("\n");
					print_debug_hex8(j);
					print_debug(": ");
				}
				status = spd_read_byte(device, j);
				if (status < 0) {
					print_debug("bad device\n");
					break;
				}
				byte = status & 0xff;
				print_debug_hex8(byte);
				print_debug_char(' ');
			}
			print_debug("\n");
		}
		device = ctrl->channel1[i];
		if (device) {
			int j;
			print_debug("dimm: ");
			print_debug_hex8(i);
			print_debug(".1: ");
			print_debug_hex8(device);
			for(j = 0; j < 256; j++) {
				int status;
				unsigned char byte;
				if ((j & 0xf) == 0) {
					print_debug("\n");
					print_debug_hex8(j);
					print_debug(": ");
				}
				status = spd_read_byte(device, j);
				if (status < 0) {
					print_debug("bad device\n");
					break;
				}
				byte = status & 0xff;
				print_debug_hex8(byte);
				print_debug_char(' ');
			}
			print_debug("\n");
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
