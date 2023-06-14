/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/asm.h>

ENTRY(smc)
	/* W0, X1-X7 passed as arguments. Function ID is always W0. */
	smc	#0
	ret			/* X0 passed back as return value */
ENDPROC(smc)
