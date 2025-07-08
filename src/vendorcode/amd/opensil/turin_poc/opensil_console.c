/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <console/console.h>
#include "opensil_console.h"
#include <SilCommon.h>

static int translate_opensil_debug_level(size_t MsgLevel)
{
	switch (MsgLevel) {
	case SIL_TRACE_ERROR:
		return BIOS_ERR;
	case SIL_TRACE_WARNING:
		return BIOS_WARNING;
	case SIL_TRACE_INFO:
		return BIOS_DEBUG;
	case SIL_TRACE_ENTRY:
	case SIL_TRACE_EXIT:
	case SIL_TRACE_RAW:
	case SIL_TRACE_VERBOSE:
		return BIOS_SPEW;
	default:
		return BIOS_NEVER;
	}
}

void HostDebugService(size_t MsgLevel, const char *SilPrefix, const char *Message,
				 const char *Function, size_t Line, ...)
{
	if (!CONFIG(OPENSIL_DEBUG_OUTPUT))
		return;

	const int loglevel = translate_opensil_debug_level(MsgLevel);

	/* print formatted prefix */
	if (CONFIG(OPENSIL_DEBUG_PREFIX)) {
		switch (MsgLevel) {
		case SIL_TRACE_RAW: break; // Raw print do nothing
		case SIL_TRACE_ENTRY:
			printk(loglevel, "%s Enter %s:%zu:", SilPrefix, Function, Line);
			break;
		case SIL_TRACE_EXIT:
			printk(loglevel, "%s Exit %s:%zu:", SilPrefix, Function, Line);
			break;
		case SIL_TRACE_ERROR:
		case SIL_TRACE_WARNING:
		case SIL_TRACE_INFO:
		case SIL_TRACE_VERBOSE:
		/* fallthrough */
		default:
			printk(loglevel, "%s%s:%zu:", SilPrefix, Function, Line);
			break;
		}
	}

	/* print formatted message */
	va_list args;
	va_start(args, Line);
	vprintk(loglevel, Message, args);
	va_end(args);
}
