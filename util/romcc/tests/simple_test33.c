static void main(void)
{
	unsigned long loops0, loops1, loops2;
	unsigned long accum;

	accum = 0;

	loops0 = 10;
	do {
		unsigned short val;
		val = __builtin_inw(0x10e0);
		if (((val & 0x08) == 0)  || (val == 1)) {
			break;
		}
	} while(--loops0);
	if (loops0 < 0) return;
	accum += loops0;


	loops1 = 20;
	do {
		unsigned short val;
		val = __builtin_inw(0x10e0);
		if (((val & 0x08) == 0)  || (val == 1)) {
			break;
		}
	} while(--loops1);

	loops2 = 30;
	do {
		unsigned short val;
		val = __builtin_inw(0x10e0);
		if (((val & 0x08) == 0)  || (val == 1)) {
			break;
		}
	} while(--loops2);

	accum += loops1 + loops0;
}


