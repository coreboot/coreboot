#include <arch/cpu.h>
static int is_cpu_rev_a0(void)
{
	return (cpuid_eax(1) & 0xfffef) == 0x0f00;
}
//AMD_D0_SUPPORT
static int is_cpu_pre_d0(void)
{
        return (cpuid_eax(1) & 0xfff0f) < 0x10f00;
}

static int is_cpu_d0(void)
{
        return (cpuid_eax(1) & 0xfff0f) == 0x10f00;
}

//AMD_E0_SUPPORT
static int is_cpu_pre_e0(void)
{
        return (cpuid_eax(1) & 0xfff0f) < 0x20f00;
}

static int is_cpu_e0(void)
{
        return (cpuid_eax(1) & 0xfff00) == 0x20f00;
}

static int is_cpu_pre_c0(void)
{
	return (cpuid_eax(1) & 0xfffef) < 0x0f48;
}

static int is_cpu_c0(void)
{
	return (cpuid_eax(1) & 0xfffef) == 0x0f48;
}

static int is_cpu_pre_b3(void)
{
	return (cpuid_eax(1) & 0xfffef) < 0x0f41;
}

static int is_cpu_b3(void)
{
	return (cpuid_eax(1) & 0xfffef) == 0x0f41;
}
