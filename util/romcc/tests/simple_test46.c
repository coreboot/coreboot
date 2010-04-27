static void spd_set_memclk(void)
{
	static const int indicies[] = { 26, 23, 9 };
	int new_cycle_time, new_latency;
	int index;
	unsigned min_cycle_time, min_latency;
	unsigned device;

	min_cycle_time = 0x50;
	min_latency = 2;
	device = 0x50;
	new_cycle_time = 0xa0;
	new_latency = 5;

	for(index = 0; index < 3; index++) {
		unsigned long loops;
		unsigned long address;
		address = indicies[index];
		loops = 1000000;
		do {
		} while(--loops);
		if (loops < 0) {
			continue;
		}

		__builtin_outb(device, 0x10e4);
		__builtin_outb(address, 0x10e8);

		loops = 1000000;
		if ((loops?0:-1) < 0) {
			continue;
		}
	}

	if (new_cycle_time > min_cycle_time) {
		min_cycle_time = new_cycle_time;
	}
	if (new_latency > min_latency) {
		min_latency = new_latency;
	}
}
