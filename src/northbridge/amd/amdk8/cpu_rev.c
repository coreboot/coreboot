#include <arch/cpu.h>
static int is_cpu_rev_a0(void)
{
	return (cpuid_eax(1) & 0xffef) == 0x0f00;
}

static int is_cpu_pre_c0(void)
{
	return (cpuid_eax(1) & 0xffef) < 0x0f48;
}

static int is_cpu_c0(void)
{
	return (cpuid_eax(1) & 0xffef) == 0x0f48;
}

static int is_cpu_pre_b3(void)
{
	return (cpuid_eax(1) & 0xffef) < 0x0f41;
}

static int is_cpu_b3(void)
{
	return (cpuid_eax(1) & 0xffef) == 0x0f41;
}
