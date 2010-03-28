#include <arch/io.h>
#include <reset.h>

void soft_reset(void)
{
        outb(0x04, 0xcf9);
}
void hard_reset(void)
{
        outb(0x02, 0xcf9);
        outb(0x06, 0xcf9);
}
