/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BAYTRAIL_SMM_H_
#define _BAYTRAIL_SMM_H_

/* There is a bug in the order of Kconfig includes in that arch/x86/Kconfig
 * is included after chipset code. This causes the chipset's Kconfig to be
 * cloberred by the arch/x86/Kconfig if they have the same name. */
static inline int smm_region_size(void)
{
	/* Make it 8MiB by default. */
	if (CONFIG_SMM_TSEG_SIZE == 0)
		return (8 << 20);
	return CONFIG_SMM_TSEG_SIZE;
}

void *smm_region_start(void);

#if !defined(__PRE_RAM__) && !defined(__SMM___)
#include <stdint.h>
void southcluster_smm_clear_state(void);
void southcluster_smm_enable_smi(void);
void southcluster_smm_save_gpio_route(uint32_t route);
#endif

#endif /* _BAYTRAIL_SMM_H_ */
