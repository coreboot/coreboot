static void main(void)
{
	int x = 25;
	do {
	} while(1);
	*((volatile int *)5) = x;
}
