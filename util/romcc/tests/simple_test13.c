static void outb(unsigned char value, unsigned short port)
{
        __builtin_outb(value, port);
}

static void uart_init(void)
{

	int a;
	if (1 == 1) {
		a = 1;
		outb(a, 0x3f8);
	} else {
		a = 2;
		outb(a, 0x3f8);
	}
	outb(a, 0x3f8);
}

static void main(void)
{
	uart_init();
}
