/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef MBOOT_H
#define MBOOT_H

#include <arch/io.h>
#include <arch/acpi.h>
#include <string.h>
#include <console/console.h>
#include <cbfs.h>
#include <lib.h>
#include <boot/coreboot_tables.h>
#include <security/tpm/tss/tcg-2.0/tss_structures.h>
#include <security/tpm/tss.h>
#include <swab.h>

/* TPM2 interface */
#define EFI_TPM2_ACPI_TABLE_START_METHOD_TIS	6
#define TPM_SHA1_160_HASH_LEN	0x14

/* Part 2, section 5.4: TPM_DIGEST */

/* Index to a PCR register */
typedef uint32_t TPM_PCRINDEX;
typedef uint32_t TCG_EVENTTYPE;
typedef TPM_PCRINDEX TCG_PCRINDEX;
typedef int8_t TCG_DIGEST;

/* TCG_PCR_EVENT_HDR */
typedef struct {
	TCG_PCRINDEX pcrIndex;
	TCG_EVENTTYPE eventType;
	TCG_DIGEST digest[TPM_SHA1_160_HASH_LEN];
	uint32_t eventSize;
} __packed TCG_PCR_EVENT_HDR;

/* TCG_PCR_EVENT2_HDR */
typedef struct {
	TCG_PCRINDEX pcrIndex;
	TCG_EVENTTYPE eventType;
	TPML_DIGEST_VALUES digest;
	uint32_t eventSize;
} __packed TCG_PCR_EVENT2_HDR;

typedef uint32_t EFI_TCG2_EVENT_ALGORITHM_BITMAP;

#define EFI_TCG2_BOOT_HASH_ALG_SHA1	0x00000001
#define EFI_TCG2_BOOT_HASH_ALG_SHA256	0x00000002
#define EFI_TCG2_BOOT_HASH_ALG_SHA384	0x00000004
#define EFI_TCG2_BOOT_HASH_ALG_SHA512	0x00000008
#define EFI_TCG2_BOOT_HASH_ALG_SM3_256	0x00000010

/* Standard event types */
#define EV_POST_CODE		((TCG_EVENTTYPE) 0x00000001)
#define EV_NO_ACTION		((TCG_EVENTTYPE) 0x00000003)
#define EV_SEPARATOR		((TCG_EVENTTYPE) 0x00000004)
#define EV_S_CRTM_CONTENTS	((TCG_EVENTTYPE) 0x00000007)
#define EV_S_CRTM_VERSION	((TCG_EVENTTYPE) 0x00000008)
#define EV_CPU_MICROCODE	((TCG_EVENTTYPE) 0x00000009)
#define EV_TABLE_OF_DEVICES	((TCG_EVENTTYPE) 0x0000000B)

#define MBOOT_PCR_INDEX_0	0x0
#define MBOOT_PCR_INDEX_1	0x1
#define MBOOT_PCR_INDEX_2	0x2
#define MBOOT_PCR_INDEX_3	0x3
#define MBOOT_PCR_INDEX_4	0x4
#define MBOOT_PCR_INDEX_5	0x5
#define MBOOT_PCR_INDEX_6	0x6
#define MBOOT_PCR_INDEX_7	0x7

/*
 * used to indicate a hash is provide so there is no need to perform the
 * measurement
 */
#define MBOOT_HASH_PROVIDED (0x00000001)

int is_zero_buffer(void *buffer, unsigned int size);

int mboot_hash_extend_log(uint64_t flags, uint8_t *hashData, uint32_t hashDataLen,
			  TCG_PCR_EVENT2_HDR *newEventHdr, uint8_t *eventLog);

void mboot_print_buffer(uint8_t *buffer, uint32_t bufferSize);

int mb_crtm(void);

typedef struct {
	const char *cbfs_name;
	uint32_t cbfs_type;
	uint32_t pcr;
	TCG_EVENTTYPE eventType;
	const char *event_msg;
} mboot_measure_item_t;

int mb_measure_log_worker(const char *name, uint32_t type, uint32_t pcr,
			  TCG_EVENTTYPE eventType, const char *event_msg);

int mb_measure_log_start(void);
void invalidate_pcrs(void);

EFI_TCG2_EVENT_ALGORITHM_BITMAP tpm2_get_active_pcrs(void);

int tpm2_get_capability_pcrs(TPML_PCR_SELECTION *Pcrs);

int mb_measure(int wake_from_s3);
int mb_entry(int wake_from_s3);

int log_efi_specid_event(void);
int log_event_tcg_20_format(TCG_PCR_EVENT2_HDR *EventHdr, uint8_t *EventLog);
int log_event_tcg_12_format(TCG_PCR_EVENT2_HDR *EventHdr, uint8_t *EventLog);

int get_intel_me_hash(uint8_t *hash);

#endif /* MBOOT_H */
