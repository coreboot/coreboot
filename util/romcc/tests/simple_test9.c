

void main(void)
{
	static const char msg[] = "hello world\r\n";
	char *str;
	char ch;
	str = msg;
	do {
		ch = *str++;
	} while(ch);
}
