static void main(void)
{
	unsigned long a,b,c, d;
	volatile unsigned long *val = (volatile unsigned long *)0x1234;
	a = val[0];
	b = val[1];
	d = val[2];
	c = a*b;
	val[3] = c;
	a = c / d;
}
