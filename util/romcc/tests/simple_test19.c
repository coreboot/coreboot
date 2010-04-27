static void hlt(void)
{
	__builtin_hlt();
}

typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
	return __builtin_rdmsr(index);
}

static void wrmsr(unsigned long index, msr_t msr)
{
	__builtin_wrmsr(index, msr.lo, msr.hi);
}

static void main(void)
{
	msr_t msr;
	msr = rdmsr(0x1234);
	msr.lo &= ~(1<<5);
	wrmsr(0x1234, msr);
}
