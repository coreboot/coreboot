/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
.text
.global chipset_teardown_car
chipset_teardown_car:
	/* There's no CAR to tear down */
	ret
