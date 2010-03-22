#undef DEBUG_LOG2

#ifdef DEBUG_LOG2
#include <console/console.h>
#endif

#include <lib.h>

/* Assume 8 bits per byte */
#define CHAR_BIT 8

unsigned long log2(unsigned long x)
{
        // assume 8 bits per byte.
        unsigned long i = 1ULL << (sizeof(x)* CHAR_BIT - 1ULL);
        unsigned long pow = sizeof(x) * CHAR_BIT - 1ULL;

        if (! x) {
#ifdef DEBUG_LOG2
                printk(BIOS_WARNING, "%s called with invalid parameter of 0\n",
			__func__);
#endif
                return -1;
        }
        for(; i > x; i >>= 1, pow--)
                ;

        return pow;
}
