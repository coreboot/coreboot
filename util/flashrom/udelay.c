#include <sys/time.h>
#include <stdio.h>
#include "flash.h"

// count to a billion. Time it. If it's < 1 sec, count to 10B, etc.
unsigned long micro = 1;

void myusec_delay(int time)
{
	volatile unsigned long i;
	for (i = 0; i < time * micro; i++) ;
}

void myusec_calibrate_delay()
{
	int count = 1000;
	unsigned long timeusec;
	struct timeval start, end;
	int ok = 0;

	printf("Calibrating delay loop... ");

	while (!ok) {
		gettimeofday(&start, 0);
		myusec_delay(count);
		gettimeofday(&end, 0);
		timeusec = 1000000 * (end.tv_sec - start.tv_sec) +
		    (end.tv_usec - start.tv_usec);
		count *= 2;
		if (timeusec < 1000000 / 4)
			continue;
		ok = 1;
	}

	// compute one microsecond. That will be count / time
	micro = count / timeusec;

	printf_debug("%ldM loops per second. ", (unsigned long)micro);
	printf("ok\n");
}
