/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc
 * Copyright (C) 2018 Jonathan Neuschäfer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// "return" to a payload. a0: FDT, a1: entry point
	.global riscvpayload
riscvpayload:
	/* Load the entry point */
	mv	t0, a1
	csrw	mepc, t0
	csrr	t0, mstatus

	/* Set mstatus.MPP (the previous privilege mode) to supervisor mode */
	li	t1, ~(3<<11)
	and	t0, t0, t1
	li	t2,  (1<<11)
	or	t0, t0, t2
	csrw	mstatus, t0

	/* Pass the right arguments and jump! */
	mv	a1, a0
	csrr	a0, mhartid
	mret
