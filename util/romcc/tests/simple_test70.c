static void main(void)
{
	int i;
	int dest;

	goto start;
 foo:
	__builtin_outl(dest, 0x5678);
	if (dest == 2) goto middle;
	goto head;

start:
	dest = 1;
	goto foo;
 head:
	for(i = 0; i < 10; i++) {
		dest = 2;
		goto foo;
	middle:
		__builtin_outl(i, 0x1234);
	}
}
