/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#include <exception.h>
#include <libpayload.h>

/* The argc value to pass to main() */
unsigned int main_argc;
/* The argv value to pass to main() */
char *main_argv[MAX_ARGC_COUNT];

/*
 * This is our C entry function - set up the system
 * and jump into the payload entry point.
 */
void start_main(void);
void start_main(void)
{
	extern int main(int argc, char **argv);

	/* Gather system information. */
	lib_get_sysinfo();

	/* Optionally set up the consoles. */
#if !IS_ENABLED(CONFIG_LP_SKIP_CONSOLE_INIT)
	console_init();
#endif

	exception_init();
	/*
	 * Any other system init that has to happen before the
	 * user gets control goes here
	 */

	/*
	 * Go to the entry point.
	 * In the future we may care about the return value.
	 */

	(void) main(main_argc, (main_argc != 0) ? main_argv : NULL);

	/*
	 * Returning here will go to the _leave function to return
	 * us to the original context.
	 */
}
