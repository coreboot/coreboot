
typedef __builtin_msr_t msr_t;

static msr_t rdmsr(unsigned long index)
{
        return __builtin_rdmsr(index);
}

static void uart_tx_byte(unsigned char data)
{
	while(!(__builtin_inb(0x3f8 + 0x05) & 0x20))
		;
        __builtin_outb(data, 0x3f8 + 0x00);

        while(!(__builtin_inb(0x3f8 + 0x05) & 0x40))
                ;
}


static void print_nibble(unsigned nibble)
{
	unsigned char digit;
	digit = nibble + '0';
	if (digit > '9') {
		digit += 39;
	}
	uart_tx_byte(digit);
}

static void print_debug_hex32(unsigned int value)
{
	print_nibble((value >> 28U) & 0x0fU);
	print_nibble((value >> 24U) & 0x0fU);
	print_nibble((value >> 20U) & 0x0fU);
	print_nibble((value >> 16U) & 0x0fU);
	print_nibble((value >> 12U) & 0x0fU);
	print_nibble((value >> 8U) & 0x0fU);
	print_nibble((value >> 4U) & 0x0fU);
	print_nibble(value & 0x0fU);
}

static void print_debug(const char *str)
{
	unsigned char ch;
	while((ch = *str++) != '\0') {
		uart_tx_byte(ch);
	}
}

static void main(void)
{
	unsigned long start, stop;
	msr_t msr;
	msr = rdmsr(0xC001001A);
	print_debug("TOP_MEM: ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\r\n");

	start = 0;
	stop = msr.lo;
        print_debug("Testing DRAM : ");
        print_debug_hex32(start);
        print_debug("-");
        print_debug_hex32(stop);
        print_debug("\r\n");

        print_debug("DRAM verify: ");
        print_debug_hex32(start);
        print_debug_hex32(stop);
}
