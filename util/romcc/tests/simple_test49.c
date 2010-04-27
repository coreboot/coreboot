
static void main(void)
{
	int i;
	i = __builtin_inb(0x1234);
	if (i == 23) {
		for(;;) {
			int j;
			j = __builtin_inb(0xabcd);
			__builtin_outb(j, 0xef90);
		}
	}
	__builtin_outb(i, 0x5678);

}
