/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef _CBFSTOOL_CONSOLE_H_
#define _CBFSTOOL_CONSOLE_H_

#include <stdio.h>
#include <commonlib/loglevel.h>

/* Message output */
extern int verbose;
#define ERROR(...) { fprintf(stderr, "E: " __VA_ARGS__); }
#define WARN(...) { fprintf(stderr, "W: " __VA_ARGS__); }
#define LOG(...) { fprintf(stderr, __VA_ARGS__); }
#define INFO(...) { if (verbose > 0) fprintf(stderr, "INFO: " __VA_ARGS__); }
#define DEBUG(...) { if (verbose > 1) fprintf(stderr, "DEBUG: " __VA_ARGS__); }


#define printk(lvl, ...) \
	{						\
		if ((lvl) <= BIOS_ERR) {		\
			ERROR(__VA_ARGS__);		\
		} else if ((lvl) <= BIOS_NOTICE) {	\
			WARN(__VA_ARGS__);		\
		} else if ((lvl) <= BIOS_INFO) {	\
			INFO(__VA_ARGS__);		\
		} else if ((lvl) <= BIOS_DEBUG) {	\
			DEBUG(__VA_ARGS__);		\
		}					\
	}

#endif
