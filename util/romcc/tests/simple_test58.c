static void main(void)
{
	const char *str;
	unsigned char ch;
	str = "one\r\n";
	while((ch = *str++) != '\0') {
		__builtin_outb(ch, 0x3f0);
	}
}
