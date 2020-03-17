/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
