/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 ChromeOS Authors
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef ROMSTAGE_HANDOFF_H
#define ROMSTAGE_HANDOFF_H

#include <stdint.h>
#include <string.h>
#include <cbmem.h>

/* It is the chipset's responsibility for maintaining the integrity of this
 * structure in CBMEM. For instance, if chipset code adds this structure
 * using the CBMEM_ID_ROMSTAGE_INFO id it needs to ensure it doesn't clobber
 * fields it doesn't own. */
struct romstage_handoff {
	/* Indicate if the current boot is an S3 resume. If
	 * CONFIG_RELOCTABLE_RAMSTAGE is enabled the chipset code is
	 * responsible for initializing this variable. Otherwise, ramstage
	 * will be re-loaded from cbfs (which can be slower since it lives
	 * in flash). */
	uint32_t s3_resume;
	/* The ramstage_entry_point is cached in the stag loading path. This
	 * cached value can only be utilized when the chipset code properly
	 * fills in the s3_resume field above. */
	uint32_t ramstage_entry_point;
};

#if defined(__PRE_RAM__)
/* The romstage_handoff_find_or_add() function provides the necessary logic
 * for initializing the romstage_handoff structure in cbmem. Different components
 * of the romstage may be responsible for setting up different fields. Therefore
 * that same logic flow should be used for allocating and initializing the
 * structure. A newly allocated structure will be memset to 0. */
static inline struct romstage_handoff *romstage_handoff_find_or_add(void)
{
	struct romstage_handoff *handoff;

	/* cbmem_add() first does a find and uses the old location before the
	 * real add. However, it is important to know when the structure is not
	 * found so it can be initialized to 0. */
	handoff = cbmem_find(CBMEM_ID_ROMSTAGE_INFO);

	if (handoff == NULL) {
		handoff = cbmem_add(CBMEM_ID_ROMSTAGE_INFO, sizeof(*handoff));
		if (handoff != NULL)
			memset(handoff, 0, sizeof(*handoff));
	}

	return handoff;
}
#endif

#endif /* ROMSTAGE_HANDOFF_H */

