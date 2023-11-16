/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef MAINBOARD_EMULATION_QEMU_SBSA_CHIP_H
#define MAINBOARD_EMULATION_QEMU_SBSA_CHIP_H

#include <types.h>

struct mainboard_emulation_qemu_sbsa_config {
	uint32_t vgic_maintenance_interrupt;
	uint32_t performance_interrupt_gsiv;
};

#endif
