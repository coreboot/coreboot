#include "linux_syscall.h"
#include "linux_console.h"


static void main(void)
{
	static const int cpu[] = { 0, 1, 2, 3 };
	int i;
	for(i = 0; i < sizeof(cpu)/sizeof(cpu[0]); i++) {
		static const unsigned int register_values[] = {
			0x0000c144, 0x0000f8f8, 0x00000000,
			0x0000c14C, 0x0000f8f8, 0x00000001,
			0x0000c154, 0x0000f8f8, 0x00000002,
			0x0000c15C, 0x0000f8f8, 0x00000003,
			0x0000c164, 0x0000f8f8, 0x00000004,
			0x0000c16C, 0x0000f8f8, 0x00000005,
			0x0000c174, 0x0000f8f8, 0x00000006,
			0x0000c17C, 0x0000f8f8, 0x00000007,
		};
		int j;
		int max = sizeof(register_values)/sizeof(register_values[0]);
		for(j = 0; j < max; j += 3) {
			print_debug("val[");
			print_debug_hex8(j);
			print_debug("]: ");
			print_debug_hex32(register_values[j]);
			print_debug_char(' ');
			print_debug_hex32(register_values[j+1]);
			print_debug_char(' ');
			print_debug_hex32(register_values[j+2]);
			print_debug_char('\n');
		}
	}
	_exit(0);
}
