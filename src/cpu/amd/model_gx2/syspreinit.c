/* StartTimer1
 *
 * Entry: none
 * Exit: Starts Timer 1 for port 61 use
 * Destroys: Al,
 */
static void StartTimer1(void)
{
	outb(0x56, 0x43);
	outb(0x12, 0x41);
}

void SystemPreInit(void)
{
	/* they want a jump ... */
#if !CONFIG_CACHE_AS_RAM
	__asm__ __volatile__("jmp .+2\ninvd\njmp .+2\n");
#endif
	StartTimer1();
}
