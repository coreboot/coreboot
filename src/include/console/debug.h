/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CONSOLE_DEBUG_H_
#define _CONSOLE_DEBUG_H_

#if CONFIG(DEBUG_FUNC)
#include <console/console.h>

#define FUNC_ENTER() \
	printk(BIOS_SPEW, "%s:%s:%d: ENTER\n", __FILE__, __func__, __LINE__)

#define FUNC_EXIT() \
	printk(BIOS_SPEW, "%s:%s:%d: EXIT\n", __FILE__, __func__, __LINE__)

#else /* FUNC_DEBUG */

#define FUNC_ENTER()
#define FUNC_EXIT()

#endif /* FUNC_DEBUG */

#endif
