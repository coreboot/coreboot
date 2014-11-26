#ifdef CONFIG_BOOTBLOCK_CPU_INIT
#include CONFIG_BOOTBLOCK_CPU_INIT
#endif

// I'm disappointed that we let this kind of thing creep in.
// we null out functions with this kind of stuff, AND weak symbols,
// AND empty cpp function defines. What's next? Ouija boards?
#if 0
#ifdef CONFIG_BOOTBLOCK_MAINBOARD_INIT
#include CONFIG_BOOTBLOCK_MAINBOARD_INIT
#else
static void bootblock_mainboard_init(void)
{
}
#endif
#endif
