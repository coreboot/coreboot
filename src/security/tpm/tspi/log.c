/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <security/tpm/tspi.h>
#include <region_file.h>
#include <string.h>
#include <symbols.h>
#include <cbmem.h>
#include <vb2_sha.h>

void *tpm_log_cbmem_init(void)
{
	static struct tpm_cb_log_table *tclt;
	if (tclt)
		return tclt;

	if (cbmem_possibly_online()) {
		tclt = cbmem_find(CBMEM_ID_TPM_CB_LOG);
		if (!tclt) {
			size_t tpm_log_len = sizeof(struct tpm_cb_log_table) +
				MAX_TPM_LOG_ENTRIES * sizeof(struct tpm_cb_log_entry);
			tclt = cbmem_add(CBMEM_ID_TPM_CB_LOG, tpm_log_len);
			if (tclt) {
				tclt->max_entries = MAX_TPM_LOG_ENTRIES;
				tclt->num_entries = 0;
			}
		}
	}
	return tclt;
}

void tpm_log_dump(void *unused)
{
	int i, j;
	struct tpm_cb_log_table *tclt;

	tclt = tpm_log_init();
	if (!tclt)
		return;

	printk(BIOS_INFO, "coreboot TPM log measurements:\n\n");
	for (i = 0; i < tclt->num_entries; i++) {
		struct tpm_cb_log_entry *tce = &tclt->entries[i];
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

void tpm_log_add_table_entry(const char *name, const uint32_t pcr,
			     enum vb2_hash_algorithm digest_algo,
			     const uint8_t *digest,
			     const size_t digest_len)
{
	struct tpm_cb_log_table *tclt = tpm_log_init();
	if (!tclt) {
		printk(BIOS_WARNING, "TPM LOG: Log non-existent!\n");
		return;
	}

	if (tclt->num_entries >= tclt->max_entries) {
		printk(BIOS_WARNING, "TPM LOG: log table is full\n");
		return;
	}

	if (!name) {
		printk(BIOS_WARNING, "TPM LOG: entry name not set\n");
		return;
	}

	struct tpm_cb_log_entry *tce = &tclt->entries[tclt->num_entries++];
	strncpy(tce->name, name, TPM_CB_LOG_PCR_HASH_NAME - 1);
	tce->pcr = pcr;

	if (digest_len > TPM_CB_LOG_DIGEST_MAX_LENGTH) {
		printk(BIOS_WARNING, "TPM LOG: PCR digest too long for log entry\n");
		return;
	}

	strncpy(tce->digest_type,
		vb2_get_hash_algorithm_name(digest_algo),
		TPM_CB_LOG_PCR_HASH_LEN - 1);
	tce->digest_length = digest_len;
	memcpy(tce->digest, digest, tce->digest_length);
}

void tpm_preram_log_clear(void)
{
	printk(BIOS_INFO, "TPM LOG: clearing preram log\n");
	struct tpm_cb_log_table *tclt = (struct tpm_cb_log_table *)_tpm_log;
	tclt->max_entries = MAX_TPM_LOG_ENTRIES;
	tclt->num_entries = 0;
}

int tpm_log_get(int entry_idx, int *pcr, const uint8_t **digest_data,
		enum vb2_hash_algorithm *digest_algo, const char **event_name)
{
	struct tpm_cb_log_table *tclt;
	struct tpm_cb_log_entry *tce;
	enum vb2_hash_algorithm algo;

	tclt = tpm_log_init();
	if (!tclt)
		return 1;

	if (entry_idx < 0 || entry_idx >= tclt->num_entries)
		return 1;

	tce = &tclt->entries[entry_idx];

	*pcr = tce->pcr;
	*digest_data = tce->digest;
	*event_name = tce->name;

	*digest_algo = VB2_HASH_INVALID;
	for (algo = VB2_HASH_INVALID; algo != VB2_HASH_ALG_COUNT; ++algo) {
		if (strcmp(tce->digest_type, vb2_hash_names[algo]) == 0) {
			*digest_algo = algo;
			break;
		}
	}
	return 0;
}

uint16_t tpm_log_get_size(const void *log_table)
{
	const struct tpm_cb_log_table *tclt = log_table;
	return tclt->num_entries;
}

void tpm_log_copy_entries(const void *from, void *to)
{
	const struct tpm_cb_log_table *from_log = from;
	struct tpm_cb_log_table *to_log = to;
	int i;

	for (i = 0; i < from_log->num_entries; i++) {
		struct tpm_cb_log_entry *tce = &to_log->entries[to_log->num_entries++];
		strncpy(tce->name, from_log->entries[i].name, TPM_CB_LOG_PCR_HASH_NAME - 1);
		tce->pcr = from_log->entries[i].pcr;

		if (from_log->entries[i].digest_length > TPM_CB_LOG_DIGEST_MAX_LENGTH) {
			printk(BIOS_WARNING, "TPM LOG: PCR digest too long for log entry\n");
			return;
		}

		strncpy(tce->digest_type, from_log->entries[i].digest_type,
			TPM_CB_LOG_PCR_HASH_LEN - 1);
		tce->digest_length = MIN(from_log->entries[i].digest_length,
					 TPM_CB_LOG_DIGEST_MAX_LENGTH);
		memcpy(tce->digest, from_log->entries[i].digest, tce->digest_length);
	}
}
