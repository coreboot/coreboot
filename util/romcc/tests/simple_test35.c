static void main(void)
{
	__builtin_msr_t msr;
	msr = __builtin_rdmsr(0xC001001A);
	while(__builtin_inb(0x3fd))
		;
        __builtin_outb(msr.hi, 0x3f8);

}
