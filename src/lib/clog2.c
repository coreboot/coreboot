#include <console/console.h>

unsigned long log2(unsigned long x)
{
        // assume 8 bits per byte.
        unsigned long i = 1 << (sizeof(x)*8 - 1);
        unsigned long pow = sizeof(x) * 8 - 1;

        if (! x) {
                printk_warning("%s called with invalid parameter of 0\n",
			__FUNCTION__);
                return -1;
        }
        for(; i > x; i >>= 1, pow--)
                ;

        return pow;
}
