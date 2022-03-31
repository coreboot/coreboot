/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <security/tpm/tspi.h>
#include <region_file.h>
#include <string.h>
#include <symbols.h>
#include <cbmem.h>
#include <bootstate.h>
#include <vb2_sha.h>

static struct tcpa_table *tcpa_cbmem_init(void)
{
	static struct tcpa_table *tclt;
	if (tclt)
		return tclt;

	if (cbmem_possibly_online()) {
		tclt = cbmem_find(CBMEM_ID_TCPA_LOG);
		if (!tclt) {
			size_t tcpa_log_len = sizeof(struct tcpa_table) +
			MAX_TCPA_LOG_ENTRIES * sizeof(struct tcpa_entry);
			tclt = cbmem_add(CBMEM_ID_TCPA_LOG, tcpa_log_len);
			if (tclt) {
				tclt->max_entries = MAX_TCPA_LOG_ENTRIES;
				tclt->num_entries = 0;
			}
		}
	}
	return tclt;
}

struct tcpa_table *tcpa_log_init(void)
{
	static struct tcpa_table *tclt;

	/* We are dealing here with pre CBMEM environment.
	 * If cbmem isn't available use CAR or SRAM */
	if (!cbmem_possibly_online() &&
		!CONFIG(VBOOT_RETURN_FROM_VERSTAGE))
		return (struct tcpa_table *)_tpm_tcpa_log;
	else if (ENV_CREATES_CBMEM
		 && !CONFIG(VBOOT_RETURN_FROM_VERSTAGE)) {
		tclt = tcpa_cbmem_init();
		if (!tclt)
			return (struct tcpa_table *)_tpm_tcpa_log;
	} else {
		tclt = tcpa_cbmem_init();
	}

	return tclt;
}

void tcpa_log_dump(void *unused)
{
	int i, j;
	struct tcpa_table *tclt;

	tclt = tcpa_log_init();
	if (!tclt)
		return;

	printk(BIOS_INFO, "coreboot TCPA measurements:\n\n");
	for (i = 0; i < tclt->num_entries; i++) {
		struct tcpa_entry *tce = &tclt->entries[i];
		if (tce) {
			printk(BIOS_INFO, " PCR-%u ", tce->pcr);

			for (j = 0; j < tce->digest_length; j++)
				printk(BIOS_INFO, "%02x", tce->digest[j]);

			printk(BIOS_INFO, " %s [%s]\n",
				   tce->digest_type, tce->name);
		}
	}
	printk(BIOS_INFO, "\n");
}

void tcpa_log_add_table_entry(const char *name, const uint32_t pcr,
			enum vb2_hash_algorithm digest_algo,
			const uint8_t *digest,
			const size_t digest_len)
{
	struct tcpa_table *tclt = tcpa_log_init();
	if (!tclt) {
		printk(BIOS_WARNING, "TCPA: Log non-existent!\n");
		return;
	}

	if (tclt->num_entries >= tclt->max_entries) {
		printk(BIOS_WARNING, "TCPA: TCPA log table is full\n");
		return;
	}

	if (!name) {
		printk(BIOS_WARNING, "TCPA: TCPA entry name not set\n");
		return;
	}

	struct tcpa_entry *tce = &tclt->entries[tclt->num_entries++];
	strncpy(tce->name, name, TCPA_PCR_HASH_NAME - 1);
	tce->pcr = pcr;

	if (digest_len > TCPA_DIGEST_MAX_LENGTH) {
		printk(BIOS_WARNING, "TCPA: PCR digest too long for TCPA log entry\n");
		return;
	}

	strncpy(tce->digest_type,
			vb2_get_hash_algorithm_name(digest_algo),
			TCPA_PCR_HASH_LEN - 1);
	tce->digest_length = digest_len;
	memcpy(tce->digest, digest, tce->digest_length);
}

void tcpa_preram_log_clear(void)
{
	printk(BIOS_INFO, "TCPA: Clearing coreboot TCPA log\n");
	struct tcpa_table *tclt = (struct tcpa_table *)_tpm_tcpa_log;
	tclt->max_entries = MAX_TCPA_LOG_ENTRIES;
	tclt->num_entries = 0;
}

#if !CONFIG(VBOOT_RETURN_FROM_VERSTAGE)
static void recover_tcpa_log(int is_recovery)
{
	struct tcpa_table *preram_log = (struct tcpa_table *)_tpm_tcpa_log;
	struct tcpa_table *ram_log = NULL;
	int i;

	if (preram_log->num_entries > MAX_PRERAM_TCPA_LOG_ENTRIES) {
		printk(BIOS_WARNING, "TCPA: Pre-RAM TCPA log is too full, possible corruption\n");
		return;
	}

	ram_log = tcpa_cbmem_init();
	if (!ram_log) {
		printk(BIOS_WARNING, "TCPA: CBMEM not available something went wrong\n");
		return;
	}

	for (i = 0; i < preram_log->num_entries; i++) {
		struct tcpa_entry *tce = &ram_log->entries[ram_log->num_entries++];
		strncpy(tce->name, preram_log->entries[i].name, TCPA_PCR_HASH_NAME - 1);
		tce->pcr = preram_log->entries[i].pcr;

		if (preram_log->entries[i].digest_length > TCPA_DIGEST_MAX_LENGTH) {
			printk(BIOS_WARNING, "TCPA: PCR digest too long for TCPA log entry\n");
			return;
		}

		strncpy(tce->digest_type, preram_log->entries[i].digest_type, TCPA_PCR_HASH_LEN - 1);
		tce->digest_length = MIN(preram_log->entries[i].digest_length, TCPA_DIGEST_MAX_LENGTH);
		memcpy(tce->digest, preram_log->entries[i].digest, tce->digest_length);
	}
}
CBMEM_CREATION_HOOK(recover_tcpa_log);
#endif

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, tcpa_log_dump, NULL);
