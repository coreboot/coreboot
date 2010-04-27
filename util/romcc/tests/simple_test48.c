
static void main(void)
{
	int i;
	i = __builtin_inb(0x1234);
	goto next;
	int j;
	j = __builtin_inb(0xabcd);
	__builtin_outb(j, 0xef90);
 next:
	__builtin_outb(i, 0x5678);

}
