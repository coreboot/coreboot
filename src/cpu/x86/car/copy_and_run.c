/* Copyright (C) 2009 coresystems GmbH
   (Written by Patrick Georgi <patrick.georgi@coresystems.de> for coresystems GmbH
*/

void cbfs_and_run_core(char*, unsigned ebp);

static void copy_and_run(unsigned cpu_reset)
{
	if (cpu_reset == 1) cpu_reset = -1;
	else cpu_reset = 0;

	cbfs_and_run_core(CONFIG_CBFS_PREFIX "/coreboot_ram", cpu_reset);
}
