static void print(char *str)
{
	while(1) {
		unsigned char ch;
		ch = *str;
		if (ch == '\0') {
			break;
		}
		__builtin_outb(ch, 0x1234);
		str += 1;
	}
}

static void main(void)
{
	print("hello world\r\n");
	print("how are you today\r\n");
}
