static void main(void)
{
	asm("cpuid"
		::: "eax", "ebx", "ecx", "edx");
}
