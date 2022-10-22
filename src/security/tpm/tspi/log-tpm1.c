/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Unlike log.c this implements TPM log according to TPM1.2 specification
 * rather than using coreboot-specific log format.
 */

#include <endian.h>
#include <console/console.h>
#include <security/tpm/tspi/logs.h>
#include <security/tpm/tspi.h>
#include <string.h>
#include <symbols.h>
#include <cbmem.h>
#include <vb2_sha.h>

void *tpm1_log_cbmem_init(void)
{
	static struct tpm_1_log_table *tclt;
	if (tclt)
		return tclt;

	if (cbmem_possibly_online()) {
		size_t tpm_log_len;
		struct spec_id_event_data *hdr;

		tclt = cbmem_find(CBMEM_ID_TCPA_TCG_LOG);
		if (tclt)
			return tclt;

		tpm_log_len = sizeof(*tclt) + MAX_TPM_LOG_ENTRIES * sizeof(tclt->entries[0]);
		tclt = cbmem_add(CBMEM_ID_TCPA_TCG_LOG, tpm_log_len);
		if (!tclt)
			return NULL;

		memset(tclt, 0, sizeof(*tclt));
		hdr = &tclt->spec_id;

		/* Fill in first "header" entry. */
		tclt->event_type = htole32(EV_NO_ACTION);
		tclt->spec_id_size = htole32(sizeof(tclt->spec_id) + sizeof(tclt->vendor));
		strcpy(hdr->signature, TCPA_SPEC_ID_EVENT_SIGNATURE);
		hdr->platform_class = htole32(0x00); // client platform
		hdr->spec_version_minor = 0x02;
		hdr->spec_version_major = 0x01;
		hdr->spec_errata = 0x01;
		hdr->vendor_info_size = sizeof(tclt->vendor);

		tclt->vendor.reserved = 0;
		tclt->vendor.version_major = TPM_1_LOG_VI_MAJOR;
		tclt->vendor.version_minor = TPM_1_LOG_VI_MINOR;
		tclt->vendor.magic = htole32(TPM_1_LOG_VI_MAGIC);
		tclt->vendor.max_entries = htole16(MAX_TPM_LOG_ENTRIES);
		tclt->vendor.num_entries = htole16(0);
		tclt->vendor.entry_size = htole32(sizeof(tclt->entries[0]));
	}

	return tclt;
}

void tpm1_log_dump(void)
{
	int i, j;
	struct tpm_1_log_table *tclt;

	tclt = tpm_log_init();
	if (!tclt)
		return;

	printk(BIOS_INFO, "coreboot TPM 1.2 measurements:\n\n");
	for (i = 0; i < le16toh(tclt->vendor.num_entries); i++) {
		struct tpm_1_log_entry *tce = &tclt->entries[i];

		printk(BIOS_INFO, " PCR-%u ", le32toh(tce->pcr));

		for (j = 0; j < TPM_1_LOG_DIGEST_MAX_LENGTH; j++)
			printk(BIOS_INFO, "%02x", tce->digest[j]);

		printk(BIOS_INFO, " %s [%s]\n", "SHA1", (char *)tce->data);
	}
	printk(BIOS_INFO, "\n");
}

void tpm1_log_add_table_entry(const char *name, const uint32_t pcr,
			      enum vb2_hash_algorithm digest_algo,
			      const uint8_t *digest,
			      const size_t digest_len)
{
	struct tpm_1_log_table *tclt;
	struct tpm_1_log_entry *tce;

	tclt = tpm_log_init();
	if (!tclt) {
		printk(BIOS_WARNING, "TPM LOG: non-existent!\n");
		return;
	}

	if (!name) {
		printk(BIOS_WARNING, "TPM LOG: entry name not set\n");
		return;
	}

	if (digest_algo != VB2_HASH_SHA1) {
		printk(BIOS_WARNING, "TPM LOG: unsupported hash algorithm\n");
		return;
	}

	if (le16toh(tclt->vendor.num_entries) >= le16toh(tclt->vendor.max_entries)) {
		printk(BIOS_WARNING, "TPM LOG: log table is full\n");
		return;
	}

	tce = &tclt->entries[le16toh(tclt->vendor.num_entries)];
	tclt->vendor.num_entries = htole16(le16toh(tclt->vendor.num_entries) + 1);

	tce->pcr = htole32(pcr);
	tce->event_type = htole32(EV_ACTION);

	memcpy(tce->digest, digest, digest_len);

	tce->data_length = htole32(TPM_1_LOG_DATA_MAX_LENGTH);
	strncpy((char *)tce->data, name, sizeof(tce->data) - 1);
	tce->data[sizeof(tce->data) - 1] = '\0';
}

void tpm1_preram_log_clear(void)
{
	printk(BIOS_INFO, "TPM LOG: clearing the log\n");
	/*
	 * Pre-RAM log is only for internal use and isn't exported anywhere, hence it's header
	 * is not initialized.
	 */
	struct tpm_1_log_table *tclt = (struct tpm_1_log_table *)_tpm_log;
	tclt->vendor.max_entries = htole16(MAX_TPM_LOG_ENTRIES);
	tclt->vendor.num_entries = htole16(0);
}

int tpm1_log_get(int entry_idx, int *pcr, const uint8_t **digest_data,
		 enum vb2_hash_algorithm *digest_algo, const char **event_name)
{
	struct tpm_1_log_table *tclt;
	struct tpm_1_log_entry *tce;

	tclt = tpm_log_init();
	if (!tclt)
		return 1;

	if (entry_idx < 0 || entry_idx >= le16toh(tclt->vendor.num_entries))
		return 1;

	tce = &tclt->entries[entry_idx];

	*pcr = le32toh(tce->pcr);
	*digest_data = tce->digest;
	*digest_algo = VB2_HASH_SHA1;
	*event_name = (char *)tce->data;
	return 0;
}

uint16_t tpm1_log_get_size(const void *log_table)
{
	const struct tpm_1_log_table *tclt = log_table;
	return le16toh(tclt->vendor.num_entries);
}

void tpm1_log_copy_entries(const void *from, void *to)
{
	const struct tpm_1_log_table *from_log = from;
	struct tpm_1_log_table *to_log = to;
	int i;

	for (i = 0; i < le16toh(from_log->vendor.num_entries); i++) {
		struct tpm_1_log_entry *tce =
			&to_log->entries[le16toh(to_log->vendor.num_entries)];
		memcpy(tce, &from_log->entries[i], sizeof(*tce));

		to_log->vendor.num_entries = htole16(le16toh(to_log->vendor.num_entries) + 1);
	}
}
