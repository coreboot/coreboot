static void main(void)
{
	unsigned long a,b,c, d;
	volatile unsigned long *val = (volatile unsigned long *)0x1234;
	a = val[0];
	b = val[1];
	c = a*b;
	val[2] = c;
	d = val[3];
	a = c / d;
	b = c % d;
	val[4] = a;
	val[5] = b;
}
