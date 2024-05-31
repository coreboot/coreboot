/* SPDX-License-Identifier: GPL-2.0-only */

.global chipset_teardown_car

.code32
chipset_teardown_car:
	/* Return to caller. */
	jmp	*%esp
