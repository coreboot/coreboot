/* this is a shrunken cpuid. */

static unsigned int cpuid(unsigned int op)
{
	unsigned int ret;
	unsigned dummy2,dummy3,dummy4;

	asm volatile ( 
		"cpuid" 
		: "=a" (ret), "=b" (dummy2), "=c" (dummy3), "=d" (dummy4)
		: "a" (op)
		);

	return ret;
}

static int is_cpu_rev_a0(void)
{
	return (cpuid(1) & 0xffff) == 0x0f10;
}

static int is_cpu_pre_c0(void)
{
	return (cpuid(1) & 0xffef) < 0x0f48;
}
