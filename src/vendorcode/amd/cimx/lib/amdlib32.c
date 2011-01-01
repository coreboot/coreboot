#include "amdlib32.h"

UINT8 ReadIo8 (IN UINT16 port)
{
	UINT8 value;
	__asm__ __volatile__ ("inb %w1, %b0" : "=a"(value) : "Nd" (port));
	return value;
}

UINT16 ReadIo16 (IN UINT16 port)
{
        UINT16 value;
        __asm__ __volatile__ ("inw %w1, %w0" : "=a"(value) : "Nd" (port));
        return value;
}

UINT32 ReadIo32 (IN UINT16 port)
{
        UINT32 value;
        __asm__ __volatile__ ("inl %w1, %0" : "=a"(value) : "Nd" (port));
        return value;
}

VOID WriteIo8 (IN UINT16 port, IN UINT8 value)
{
        __asm__ __volatile__ ("outb %b0, %w1" : : "a" (value), "Nd" (port));
}

VOID WriteIo16 (IN UINT16 port, IN UINT16 value)
{
        __asm__ __volatile__ ("outw %w0, %w1" : : "a" (value), "Nd" (port));
}

VOID WriteIo32 (IN UINT16 port, IN UINT32 value)
{
        __asm__ __volatile__ ("outl %0, %w1" : : "a" (value), "Nd" (port));
}

UINT64 ReadTSC(VOID)
{
	struct tsc_struct {
		unsigned lo;
		unsigned hi;
	} res;
	UINT64 ret;

	__asm__ __volatile__ (
			"rdtsc" 
			: "=a" (res.lo), "=d"(res.hi) /* outputs */
			);
	ret = res.hi;
	ret <<= 32;
	ret |= res.lo;
	return ret;
}

VOID CpuidRead(IN UINT32 op, IN OUT SB_CPUID_DATA* Data)
{
        asm volatile(
                "cpuid"
                : "=a" (Data->EAX_Reg),
                  "=b" (Data->EBX_Reg),
                  "=c" (Data->ECX_Reg),
                  "=d" (Data->EDX_Reg)
                : "0" (op));
}

static inline unsigned int cpuid_ecx(unsigned int op)
{
        unsigned int eax, ecx;

        __asm__("cpuid"
                : "=a" (eax), "=c" (ecx)
                : "0" (op)
                : "ebx", "edx" );
        return ecx;
}

//static inline unsigned get_core_num(void)
UINT8 ReadNumberOfCpuCores(VOID)
{
        return (cpuid_ecx(0x80000008) & 0xff);
}

