#include <console/console.h>
#include <cpu/x86/cache.h>

void x86_enable_cache(void)
{
	post_code(0x60);
	printk_info("Enabling cache\n");
	enable_cache();
}

