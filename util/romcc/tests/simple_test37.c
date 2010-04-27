static void main(void)
{
	unsigned csbase, csmask;

	csbase = 0x40;
	csmask = 0xfe00;

	__builtin_outl(csbase, 0x40);
	__builtin_outl(csmask, 0x60);
}
