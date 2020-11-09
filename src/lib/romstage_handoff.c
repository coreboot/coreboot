/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <romstage_handoff.h>

struct romstage_handoff {
	/* Indicate if the current boot is an S3 resume. If
	 * CONFIG_RELOCATABLE_RAMSTAGE is enabled the chipset code is
	 * responsible for initializing this variable. Otherwise, ramstage
	 * will be re-loaded from cbfs (which can be slower since it lives
	 * in flash). */
	uint8_t s3_resume;
	uint8_t reboot_required;
	uint8_t reserved[2];
};

static struct romstage_handoff *romstage_handoff_find_or_add(void)
{
	struct romstage_handoff *handoff;

	/* cbmem_add() first does a find and uses the old location before the
	 * real add. However, it is important to know when the structure is not
	 * found so it can be initialized to 0. */
	handoff = cbmem_find(CBMEM_ID_ROMSTAGE_INFO);

	if (handoff)
		return handoff;

	handoff = cbmem_add(CBMEM_ID_ROMSTAGE_INFO, sizeof(*handoff));

	if (handoff != NULL)
		memset(handoff, 0, sizeof(*handoff));
	else
		printk(BIOS_DEBUG, "Romstage handoff structure not added!\n");

	return handoff;
}

int romstage_handoff_init(int is_s3_resume)
{
	struct romstage_handoff *handoff;

	handoff = romstage_handoff_find_or_add();

	if (handoff == NULL)
		return -1;

	handoff->s3_resume = is_s3_resume;

	return 0;
}

int romstage_handoff_is_resume(void)
{
	struct romstage_handoff *handoff;

	handoff = cbmem_find(CBMEM_ID_ROMSTAGE_INFO);

	if (handoff == NULL)
		return 0;

	return handoff->s3_resume;
}
