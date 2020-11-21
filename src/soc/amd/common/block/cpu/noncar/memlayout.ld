/* SPDX-License-Identifier: GPL-2.0-only */

#if ENV_SEPARATE_VERSTAGE && CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)
	#include "memlayout_psp_verstage.ld"
#else
	#include "memlayout_x86.ld"
#endif /* ENV_SEPARATE_VERSTAGE && CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK) */
