#include <cpu/p6/msr.h>

void ndelay(unsigned long ns)
{
        unsigned long long count;
        unsigned long long stop;
        unsigned long long ticks;

        /* FIXME calibrate this... don't just estimage 2Ghz */
        ticks = ns << 1;
        rdtscll(count);
        stop = ticks + count;
        while(stop > count) {
                rdtscll(count);
        }
}


