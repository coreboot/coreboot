static void main(void)
{
	static const int sdivisor = 20;
	const int *pdivisor;
	unsigned rdpreamble;
	unsigned divisor;
	pdivisor = &sdivisor;
	divisor = *pdivisor;
	rdpreamble = 0;

	if (divisor == 20) {
		rdpreamble = 18;
	}
	else {
		if (divisor == 15) {
			rdpreamble = 16;
		}
		else {
			if (divisor == 12) {
				rdpreamble = 15;
			}
		}
	}
}
