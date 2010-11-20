#include "raminit.h"

void dump_spd_registers(void)
{
#if CONFIG_DEBUG_RAM_SETUP
	int i;
	print_debug("\n");
	for(i = 0; i < DIMM_SOCKETS; i++) {
		unsigned device;
		device = DIMM0 + i;
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
				status = smbus_read_byte(device, j);
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
#endif
}
