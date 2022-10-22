/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LOGS_H_
#define LOGS_H_

#include <stdint.h>
#include <vb2_api.h>

/* coreboot-specific TPM log format */

void *tpm_cb_log_init(void);
void *tpm_cb_log_cbmem_init(void);
void tpm_cb_preram_log_clear(void);
uint16_t tpm_cb_log_get_size(const void *log_table);
void tpm_cb_log_copy_entries(const void *from, void *to);
int tpm_cb_log_get(int entry_idx, int *pcr, const uint8_t **digest_data,
		   enum vb2_hash_algorithm *digest_algo, const char **event_name);
void tpm_cb_log_add_table_entry(const char *name, const uint32_t pcr,
				enum vb2_hash_algorithm digest_algo,
				const uint8_t *digest,
				const size_t digest_len);
void tpm_cb_log_dump(void);

/* TPM 1.2 log format */

void *tpm1_log_init(void);
void *tpm1_log_cbmem_init(void);
void tpm1_preram_log_clear(void);
uint16_t tpm1_log_get_size(const void *log_table);
void tpm1_log_copy_entries(const void *from, void *to);
int tpm1_log_get(int entry_idx, int *pcr, const uint8_t **digest_data,
		 enum vb2_hash_algorithm *digest_algo, const char **event_name);
void tpm1_log_add_table_entry(const char *name, const uint32_t pcr,
			      enum vb2_hash_algorithm digest_algo,
			      const uint8_t *digest,
			      const size_t digest_len);
void tpm1_log_dump(void);

/* TPM 2.0 log format */

void *tpm2_log_init(void);
void *tpm2_log_cbmem_init(void);
void tpm2_preram_log_clear(void);
uint16_t tpm2_log_get_size(const void *log_table);
void tpm2_log_copy_entries(const void *from, void *to);
int tpm2_log_get(int entry_idx, int *pcr, const uint8_t **digest_data,
		 enum vb2_hash_algorithm *digest_algo, const char **event_name);
void tpm2_log_add_table_entry(const char *name, const uint32_t pcr,
			      enum vb2_hash_algorithm digest_algo,
			      const uint8_t *digest,
			      const size_t digest_len);
void tpm2_log_dump(void);

#endif /* LOGS_H_ */
