#include <arch/io.h>

void early_mainboard_init(void)
{
	extern void enable_serial(void);
	enable_serial();
}
