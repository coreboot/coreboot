/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _VENDORCODE_AMD_OPENSIL_CONSOLE
#define _VENDORCODE_AMD_OPENSIL_CONSOLE

#include <types.h>

void HostDebugService(size_t MsgLevel, const char *SilPrefix, const char *Message,
		      const char *Function, size_t Line, ...);

#endif
