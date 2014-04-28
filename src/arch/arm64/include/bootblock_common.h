#ifdef CONFIG_BOOTBLOCK_CPU_INIT
#include CONFIG_BOOTBLOCK_CPU_INIT
#endif

#ifdef CONFIG_BOOTBLOCK_MAINBOARD_INIT
#include CONFIG_BOOTBLOCK_MAINBOARD_INIT
#else
static void bootblock_mainboard_init(void)
{
}
#endif
