static void spd_set_memclk(void)
{

        unsigned min;
        unsigned device;


	min = 0x250;


        for(device = 0x80; device <= 0x81; device += 1)
        {
                unsigned cur;
                int latency;
		unsigned long loops;

		cur = 5 | 0xa0;
		latency = __builtin_inw(0xab);

                if (latency > 0x250) {
			loops = 1000000;
			while(--loops)
				;
			if (!loops) {
				goto end;
			}

			loops = 1000000;
			while(--loops)
				;
		end:
			;

                }
		loops = 1000000;
		while(--loops)
			;

                if (latency > 0x250) {
			;
                }

                if (cur > 0x250) {
                }
        }

}
