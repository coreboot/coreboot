
static void dump_spd_registers(const struct mem_controller *ctrl)
{
	int i;
	print_debug("\r\n");
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
					print_debug("\r\n");
					print_debug_hex8(j);
					print_debug(": ");
				}
				status = spd_read_byte(device, j);
				if (status < 0) {
					print_debug("bad device\r\n");
					break;
				}
				byte = status & 0xff;
				print_debug_hex8(byte);
				print_debug_char(' ');
			}
			print_debug("\r\n");
		}
#if 0  /* Enable this if you have 2 memory channels */
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
					print_debug("\r\n");
					print_debug_hex8(j);
					print_debug(": ");
				}
				status = spd_read_byte(device, j);
				if (status < 0) {
					print_debug("bad device\r\n");
					break;
				}
				byte = status & 0xff;
				print_debug_hex8(byte);
				print_debug_char(' ');
			}
			print_debug("\r\n");
		}
#endif		
	}
}

#if 0
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
#endif
