/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_EBDA_H
#define __ARCH_EBDA_H

#include <stdint.h>
#include <stddef.h>

#define X86_BDA_SIZE		0x200
#define X86_BDA_BASE		((void *)0x400)
#define X86_EBDA_SEGMENT	((void *)0x40e)
#define X86_EBDA_LOWMEM		((void *)0x413)

#define DEFAULT_EBDA_LOWMEM	(1024 << 10)
#define DEFAULT_EBDA_SEGMENT	0xF600
#define DEFAULT_EBDA_SIZE	0x400

void setup_default_ebda(void);

#endif
