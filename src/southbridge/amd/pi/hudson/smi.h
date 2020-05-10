/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Utilities for SMI handlers and SMM setup
 */

#ifndef _SOUTHBRIDGE_AMD_PI_HUDSON_SMI_H
#define _SOUTHBRIDGE_AMD_PI_HUDSON_SMI_H

#include <device/mmio.h>

/* ACPI_MMIO_BASE + 0x200 -- leave this string here so grep catches it.
 * This is defined by AGESA, but we dpn't include AGESA headers to avoid
 * polluting the namesace.
 */
#define SMI_BASE 0xfed80200

#define SMI_REG_SMITRIG0	0x98
#define SMITRG0_EOS		(1 << 28)
#define SMITRG0_SMIENB		(1 << 31)

#define SMI_REG_CONTROL0	0xa0

enum smi_mode {
	SMI_MODE_DISABLE = 0,
	SMI_MODE_SMI = 1,
	SMI_MODE_NMI = 2,
	SMI_MODE_IRQ13 = 3,
};

enum smi_lvl {
	SMI_LVL_LOW = 0,
	SMI_LVL_HIGH = 1,
};

void hudson_configure_gevent_smi(uint8_t gevent, uint8_t mode, uint8_t level);
void hudson_disable_gevent_smi(uint8_t gevent);
void hudson_enable_acpi_cmd_smi(void);

void hudson_enable_smi_generation(void);

#endif /* _SOUTHBRIDGE_AMD_PI_HUDSON_SMI_H */
