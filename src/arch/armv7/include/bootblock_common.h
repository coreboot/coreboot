#if CONFIG_CPU_HAS_BOOTBLOCK_INIT
void bootblock_cpu_init(void);
#else
static void __attribute__((unused)) bootblock_cpu_init(void)
{
}
#endif

#if CONFIG_MAINBOARD_HAS_BOOTBLOCK_INIT
void bootblock_mainboard_init(void);
#else
static void __attribute__((unused)) bootblock_mainboard_init(void)
{
}
#endif
