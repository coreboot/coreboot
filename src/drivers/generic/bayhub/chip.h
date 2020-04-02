/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi_device.h>

/*
 * Bayhub BG720 PCI to eMMC bridge
 */
struct drivers_generic_bayhub_config {
	/* 1 to enable power-saving mode, 0 to disable */
	int power_saving;
};
