static void spd_set_memclk(void)
{
	unsigned min_cycle_time, min_latency;
	unsigned device;
	int new_cycle_time, new_latency;
	int index;
	int latency;

	min_cycle_time = 0x50;
	min_latency = 2;
	device = 0x50;
	new_latency = 5;
	new_cycle_time = 0xa0;
	latency = 23;

	for(index = 0; index < 3; index++, latency++) {
		unsigned long loops;
		unsigned address = index;

		loops = 1000000;
		do {
		} while(--loops);
		if (loops) {
			continue;
		}

		__builtin_outb(device, 0x10e4);

		__builtin_outb(address & 0xFF, 0x10e8);

		loops = 1000000;
		while(--loops)
			;
	}

	if (new_cycle_time > min_cycle_time) {
		min_cycle_time = new_cycle_time;
	}
	if (new_latency > min_latency) {
		min_latency = new_latency;
	}
}

