#define rdtsc(low,high) \
     __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

static void __rdtsc_delay(unsigned long loops)
{
	unsigned long bclock, now;
	
	rdtscl(bclock);
	do
	{
		rdtscl(now);
	}
	while((now-bclock) < loops);
}

static void __rdtsc_delay2(unsigned long loops, unsigned pm_io)
{
	unsigned long bclock, now;
	
	rdtscl(bclock);
	do
	{
		outl((1 << 11) | ( 1 << 8), pm_io + 0x00);
		rdtscl(now);
	}
	while((now-bclock) < loops);
}

