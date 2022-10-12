/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFSTOOL_CONSOLE_H_
#define _CBFSTOOL_CONSOLE_H_

#include <stdio.h>
#include <commonlib/loglevel.h>

/* Message output */
extern int verbose;
#define ERROR(...) fprintf(stderr, "E: " __VA_ARGS__)
#define WARN(...) fprintf(stderr, "W: " __VA_ARGS__)
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#define INFO(...) do { if (verbose > 0) fprintf(stderr, "INFO: " __VA_ARGS__); } while (0)
#define DEBUG(...) do { if (verbose > 1) fprintf(stderr, "DEBUG: " __VA_ARGS__); } while (0)


#define printk(lvl, ...) \
	do {						\
		if ((lvl) <= BIOS_ERR) {		\
			ERROR(__VA_ARGS__);		\
		} else if ((lvl) <= BIOS_NOTICE) {	\
			WARN(__VA_ARGS__);		\
		} else if ((lvl) <= BIOS_INFO) {	\
			INFO(__VA_ARGS__);		\
		} else if ((lvl) <= BIOS_DEBUG) {	\
			DEBUG(__VA_ARGS__);		\
		}					\
	} while (0)

#endif
