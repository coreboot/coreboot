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
