void main(void)
{
	unsigned long addr, start, stop;
	start = 0x00100000;
	stop = 0x00180000;


	for(addr = start; addr < stop ;) {
		unsigned char ch;
		const char *str = "\r";
		while((ch = *str++) != '\0') {
			while(__builtin_inb(0x3f))
				;
			__builtin_outb(ch, 0x3f8);

			while(__builtin_inb(0x3f))
				;
		}
		asm (
			"jmp 2f\n\t"
			"1:\n\t"
			"testl  $0xffff, %0\n\t"
			"jz	3f\n\t"
			"movnti %0, (%0)\n\t"
			"add $4, %0\n\t"
			"2:\n\t"
			"cmp %2, %0\n\t"
			"jl 1b\n\t"
			"3:\n\t"
			: "=b" (addr)		   /* outputs */
			: "0" (addr), "r" (stop)   /* intputs */
			: /* clobbers */
			);
	};
}
