#include <part/nvram.h>
#include <printk.h>

void nvram_on(void)
{
	printk_notice("Please turn on nvram\n");
	return;
}
