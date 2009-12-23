static unsigned long findstage(char* target)
{
	unsigned long entry;
	asm volatile (
		"mov $1f, %%esp\n\t"
		"jmp walkcbfs\n\t"
		"1:\n\t" : "=a" (entry) : "S" (target) : "ebx", "ecx", "edx", "edi", "ebp", "esp");
	return entry;
}

static void call(unsigned long addr)
{
	asm volatile ("jmp %0\n\t" : : "r" (addr));
}

static void main(void)
{
	const char* target1 = "fallback/romstage";
	unsigned long entry;
	entry = findstage(target1);
	if (entry) call(entry);
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

