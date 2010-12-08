#include <arch/io.h>
#include <reset.h>

void hard_reset(void)
{
        /* Try rebooting through port 0xcf9 */
        outb((0 <<3)|(1<<2)|(1<<1), 0xcf9);
}
