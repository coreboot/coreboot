typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
	return __builtin_rdmsr(index);
}

static void main(void)
{
	msr_t msr;
	msr = rdmsr(0x12345678);
}

