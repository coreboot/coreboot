static void main(void)
{
	unsigned dev;
	dev = __builtin_inl(0xcd);
	if (dev == (0xffffffffU)) {
		do {
			__builtin_hlt();
		} while(1);
	}
}
