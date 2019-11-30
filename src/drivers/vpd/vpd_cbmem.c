/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <console/console.h>
#include <cbmem.h>
#include <fmap.h>
#include <string.h>
#include <timestamp.h>

#include "vpd_tables.h"
#include "vpd.h"

/* Currently we only support Google VPD 2.0, which has a fixed offset. */
enum {
	CROSVPD_CBMEM_MAGIC = 0x43524f53,
	CROSVPD_CBMEM_VERSION = 0x0001,
};

struct vpd_cbmem {
	uint32_t magic;
	uint32_t version;
	uint32_t ro_size;
	uint32_t rw_size;
	uint8_t blob[0];
	/* The blob contains both RO and RW data. It starts with RO (0 ..
	 * ro_size) and then RW (ro_size .. ro_size+rw_size).
	 */
};

static void cbmem_add_cros_vpd(int is_recovery)
{
	struct vpd_cbmem *cbmem;
	const struct vpd_blob *blob;

	timestamp_add_now(TS_START_COPYVPD);

	blob = vpd_load_blob();

	/* Return if no VPD at all */
	if (blob->ro_size == 0 && blob->rw_size == 0)
		return;

	cbmem = cbmem_add(CBMEM_ID_VPD, sizeof(*cbmem) + blob->ro_size +
		blob->rw_size);
	if (!cbmem) {
		printk(BIOS_ERR, "%s: Failed to allocate CBMEM (%u+%u).\n",
			__func__, blob->ro_size, blob->rw_size);
		return;
	}

	cbmem->magic = CROSVPD_CBMEM_MAGIC;
	cbmem->version = CROSVPD_CBMEM_VERSION;
	cbmem->ro_size = blob->ro_size;
	cbmem->rw_size = blob->rw_size;

	if (blob->ro_size) {
		memcpy(cbmem->blob, blob->ro_base, blob->ro_size);
		timestamp_add_now(TS_END_COPYVPD_RO);
	}

	if (blob->rw_size) {
		memcpy(cbmem->blob + blob->ro_size, blob->rw_base,
			blob->rw_size);
		timestamp_add_now(TS_END_COPYVPD_RW);
	}
}

void vpd_get_buffers(struct vpd_blob *blob)
{
	const struct vpd_cbmem *vpd;

	vpd = cbmem_find(CBMEM_ID_VPD);
	if (!vpd || !vpd->ro_size)
		return;

	blob->ro_base = (void *)vpd->blob;
	blob->ro_size = vpd->ro_size;
	blob->rw_base = (void *)vpd->blob + vpd->ro_size;
	blob->rw_size = vpd->rw_size;
}

RAMSTAGE_CBMEM_INIT_HOOK(cbmem_add_cros_vpd)
