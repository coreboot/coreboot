#include <arch/io.h>
#include <southbridge/intel/82801.h>

void ich3_hard_reset(void)
{
	/* Try rebooting through port 0xcf9 */
	outb((0 <<3)|(1<<2)|(1<<1), 0xcf9);
}

