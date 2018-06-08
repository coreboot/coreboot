/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Utilities for SMI handlers and SMM setup
 */

#ifndef _SOUTHBRIDGE_AMD_PI_HUDSON_SMI_H
#define _SOUTHBRIDGE_AMD_PI_HUDSON_SMI_H

#include <arch/io.h>

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

static inline uint32_t smi_read32(uint8_t offset)
{
	return read32((void *)(SMI_BASE + offset));
}

static inline void smi_write32(uint8_t offset, uint32_t value)
{
	write32((void *)(SMI_BASE + offset), value);
}

static inline uint16_t smi_read16(uint8_t offset)
{
	return read16((void *)(SMI_BASE + offset));
}

static inline void smi_write16(uint8_t offset, uint16_t value)
{
	write16((void *)(SMI_BASE + offset), value);
}

void hudson_configure_gevent_smi(uint8_t gevent, uint8_t mode, uint8_t level);
void hudson_disable_gevent_smi(uint8_t gevent);
void hudson_enable_acpi_cmd_smi(void);

#ifndef __SMM__
void hudson_enable_smi_generation(void);
#endif

#endif /* _SOUTHBRIDGE_AMD_PI_HUDSON_SMI_H */
