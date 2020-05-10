/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <assert.h>
#include <string.h>

#include <drivers/vpd/vpd.h>

void lb_table_add_serialno_from_vpd(struct lb_header *header)
{
	struct lb_string *serialno_rec = NULL;
	const char serialno_key[] = "serial_number";
	char serialno[32];
	size_t len;

	if (!vpd_gets(serialno_key, serialno,
				sizeof(serialno), VPD_ANY)) {
		printk(BIOS_ERR, "no serial number in vpd\n");
		return;
	}
	printk(BIOS_DEBUG, "serial number is %s\n", serialno);
	len = strlen(serialno) + 1;
	ASSERT(len <= 32);

	serialno_rec = (struct lb_string *)lb_new_record(header);
	serialno_rec->tag = LB_TAG_SERIALNO;

	serialno_rec->size = ALIGN_UP(sizeof(*serialno_rec) + len, 8);
	memcpy(&serialno_rec->string, serialno, len);
}
