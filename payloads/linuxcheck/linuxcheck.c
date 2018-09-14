/*
 * This file is part of the coreinfo project.
 *
 * Copyright (C) 2018 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <libpayload-config.h>
#include <libpayload.h>
#include "linuxcheck.h"

extern struct console_output_driver *console_out;
extern struct sysinfo_t lib_sysinfo;

int main(void)
{
	int ret, i;

	buts("Greetings from linuxcheck, via hard-coded calls to serial functions.\n");
	if (console_out == NULL)
		buts("Bad news: console_out is NULL\n");
	if (lib_sysinfo.serial == NULL)
		buts("Bad news: lib_sysinfo.serial is NULL. Very little will work well.\n");
	ret = lib_get_sysinfo();
	if (ret) {
		buts("lib_get_sysinfo() is non-zero");
		hex32(ret);
		buts("\n");
	}

	buts("The next line should be puts works\n");
	puts("puts works\n");
	buts("If you did not see puts works, then you have a console issues\n");
	buts("The next line should be 'printf works'\n");
	printf("printf works\n");
	buts(" ... if you did not see printf works, then you have a printf issue\n");
	printf("Number of memory ranges: %d\n", lib_sysinfo.n_memranges);
	for (i = 0; i < lib_sysinfo.n_memranges; i++) {
		printf("%d: base 0x%08llx size 0x%08llx type 0x%x\n", i, lib_sysinfo.memrange[i].base, lib_sysinfo.memrange[i].size, lib_sysinfo.memrange[i].type);
	}
	buts("Now we will halt. Bye");
	halt();
	return 0;
}
