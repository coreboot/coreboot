static void spd_set_drb(void)
{
	unsigned char ch;
	char *str;
	str = "test_string";
	ch = *str;
	__builtin_outb(ch, 0xab);
}

