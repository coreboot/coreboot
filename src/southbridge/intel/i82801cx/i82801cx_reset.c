#include <arch/io.h>
#include "i82801cx.h"

void i82801cx_hard_reset(void)
{
        /* Try rebooting through port 0xcf9 */
        // Hard reset without power cycle
        outb((0 <<3)|(1<<2)|(1<<1), 0xcf9);
}
