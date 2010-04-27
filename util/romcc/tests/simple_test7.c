void main(void)
{
	int i;
	i = 0;
	do {
		int j;
		__builtin_outb(i, 0xab);
		j = i++;
		__builtin_outb(j, 0xdc);
	} while(i <= 9);

}
