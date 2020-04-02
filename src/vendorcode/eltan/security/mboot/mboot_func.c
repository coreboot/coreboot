/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <mboot.h>

int log_efi_specid_event(void) {
	return TPM_SUCCESS;
}

int log_event_tcg_12_format(TCG_PCR_EVENT2_HDR *EventHdr, uint8_t *EventLog) {
	return TPM_SUCCESS;
}

int log_event_tcg_20_format(TCG_PCR_EVENT2_HDR *EventHdr, uint8_t *EventLog) {
	return TPM_SUCCESS;
}
