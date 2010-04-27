static void spd_set_memclk(void)
{
        unsigned min_cycle_time;
        unsigned device;
	int new_cycle_time, new_latency;
	int index;
	int latency;

        min_cycle_time = 0x50;
	device = 0x50;
	new_cycle_time = 0xa0;
	new_latency = 5;


	latency = 0;
	for(index = 0; index < 3; index++, latency++) {
		unsigned long loops;
		loops = 1000000;
		do {
			unsigned short val;
			val = __builtin_inw(0x10e0);
		} while(--loops);
		if (!loops) {
			continue;
		}

		__builtin_outb(device, 0xe4);
		__builtin_outb(index, 0xe8);

		loops = 1000000;
		while(--loops)
			;
	}
	if (new_latency > 4){
		return;
	}

	if (new_cycle_time > min_cycle_time) {
		min_cycle_time = new_cycle_time;
	}
}
