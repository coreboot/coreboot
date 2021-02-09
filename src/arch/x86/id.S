/* SPDX-License-Identifier: GPL-2.0-only */

#include <build.h>

.section ".id", "a", @progbits

ver:
	.asciz COREBOOT_VERSION
vendor:
	.asciz CONFIG_MAINBOARD_VENDOR
part:
	.asciz CONFIG_MAINBOARD_PART_NUMBER

#if ENV_X86_64
.long	0xffffffff - ver + 1		/* Reverse offset to the version */
.long	0xffffffff - vendor + 1		/* Reverse offset to the vendor id */
.long	0xffffffff - part + 1		/* Reverse offset to the part number */
#else
.long	- ver		/* Reverse offset to the version */
.long	- vendor	/* Reverse offset to the vendor id */
.long	- part		/* Reverse offset to the part number */
#endif

.long	CONFIG_ROM_SIZE /* Size of this romimage */
