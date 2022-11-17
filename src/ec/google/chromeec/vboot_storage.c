/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <security/vboot/vboot_common.h>

#define VBOOT_HASH_VSLOT 0
#define VBOOT_HASH_VSLOT_MASK (1 << (VBOOT_HASH_VSLOT))

int vboot_save_hash(void *digest, size_t digest_size)
{
	const int slot = VBOOT_HASH_VSLOT;
	uint32_t lock_status;
	int num_slots;

	/* Ensure the digests being saved does not exceed the EC's slot size. */
	assert(digest_size > 0 && digest_size <= EC_VSTORE_SLOT_SIZE);

	if (google_chromeec_vstore_write(slot, digest, digest_size))
		return -1;

	/* Assert the slot is locked on successful write. */
	num_slots = google_chromeec_vstore_info(&lock_status);

	/* Normalize to be 0 based. If num_slots returned 0 then it'll be -1. */
	num_slots--;

	if (num_slots < slot) {
		printk(BIOS_ERR, "Not enough vstore slots for vboot hash: %d\n",
			num_slots + 1);
		return -1;
	}

	if ((lock_status & VBOOT_HASH_VSLOT_MASK) == 0) {
		printk(BIOS_ERR, "Vstore slot not locked after write.\n");
		return -1;
	}

	return 0;
}

int vboot_retrieve_hash(void *digest, size_t digest_size)
{
	/* Ensure the digests being saved match the EC's slot size. */
	assert(digest_size == EC_VSTORE_SLOT_SIZE);

	return google_chromeec_vstore_read(VBOOT_HASH_VSLOT, digest);
}
