static void main(void)
{
	volatile unsigned long *val = (volatile unsigned long *)0x1234;
	int i;
	if (val[0] > 25) {
		i = 7;
	}
	val[1] = i;

}
