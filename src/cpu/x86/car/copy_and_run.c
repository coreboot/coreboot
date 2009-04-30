/* Copyright (C) 2009 coresystems GmbH
   (Written by Patrick Georgi <patrick.georgi@coresystems.de> for coresystems GmbH
*/

void copy_and_run_core(u8 *src, u8 *dst, unsigned long ilen, unsigned ebp);

extern u8 _liseg, _iseg, _eiseg;

static void copy_and_run(unsigned cpu_reset)
{
	uint8_t *src, *dst; 
        unsigned long ilen;


	src = &_liseg;
	dst = &_iseg;
	ilen = &_eiseg - dst;

	if (cpu_reset == 1) cpu_reset = -1;
	else cpu_reset = 0;

	copy_and_run_core(src, dst, ilen, cpu_reset);
}
