/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef FSP_DEBUG_EVENT_H
#define FSP_DEBUG_EVENT_H

/*
 * This file to implement FSP_EVENT_HANDLER for Intel FSP to use.
 * More details about this structure can be found here :
 * http://github.com/tianocore/edk2/blob/master/IntelFsp2Pkg/Include/FspEas/FspApi.h
 */
#include <efi/efi_datatype.h>
#include <fsp/soc_binding.h>

/* fsp debug event handler */
efi_return_status_t fsp_debug_event_handler(efi_status_code_type_t ignored1,
	efi_status_code_value_t ignored2, efi_uint32_t ignored3, efi_guid_t *ignored4,
	efi_status_code_data_t *data);

#endif	/* FSP_DEBUG_EVENT_H */
