/* SPDX-License-Identifier: GPL-2.0-only */

.section ".fit_pointer", "a", @progbits
	.code32
/* This will get updated by ifittool later on to point to the cbfs 'intel_fit' file. */
.global fit_pointer
fit_pointer:
.long 0
.long 0
