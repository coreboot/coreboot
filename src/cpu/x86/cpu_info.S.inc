/* SPDX-License-Identifier: GPL-2.0-or-later */

/* Push struct cpu_info */
.macro push_cpu_info index=$0
#if CONFIG(COOP_MULTITASKING)
	push	$0	/* *thread */
#endif
	push	\index	/* index */
	push	$0	/* *cpu */
.endm
