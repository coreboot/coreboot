/* Copyright (C) 2009 coresystems GmbH
   (Written by Patrick Georgi <patrick.georgi@coresystems.de> for coresystems GmbH
*/

#if CONFIG_CBFS == 1
void cbfs_and_run_core(char*, unsigned ebp);

static void copy_and_run(unsigned cpu_reset)
{
	if (cpu_reset == 1) cpu_reset = -1;
	else cpu_reset = 0;

# if USE_FALLBACK_IMAGE == 1
	cbfs_and_run_core("fallback/coreboot_ram", cpu_reset);
# else
	cbfs_and_run_core("normal/coreboot_ram", cpu_reset);
# endif
}

#else
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
#endif

