
static void dump_spd_registers(int controllers, const struct mem_controller *ctrl)
{
	int n;
	for(n = 0; n < controllers; n++) {
		int i;
		print_debug("\n");
		activate_spd_rom(&ctrl[n]);
		for(i = 0; i < 4; i++) {
			unsigned device;
			device = ctrl[n].channel0[i];
			if (device) {
				int j;
				print_debug("dimm: ");
				print_debug_hex8(n);
				print_debug_char('.');
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
#if 0
					byte = status & 0xff;
					print_debug_hex8(byte);
#else
					print_debug_hex8(status & 0xff);
#endif
					print_debug_char(' ');
				}
				print_debug("\n");
			}
			device = ctrl[n].channel1[i];
			if (device) {
				int j;
				print_debug("dimm: ");
				print_debug_hex8(n);
				print_debug_char('.');
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
#if 0
					byte = status & 0xff;
					print_debug_hex8(byte);
#else
					print_debug_hex8(status & 0xff);
#endif
					print_debug_char(' ');
				}
				print_debug("\n");
			}
		}
	}
}
