/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MP_SERVICE_PPI_H
#define MP_SERVICE_PPI_H

/*
 * This file to implement MP_SERVICES_PPI for Intel FSP to use.
 * More details about this PPI can be found here :
 * http://github.com/tianocore/edk2/blob/master/MdePkg/Include/Ppi/MpServices.h
 */
#include <efi/efi_datatype.h>
#include <fsp/soc_binding.h>

/* SOC must call this function to get required EFI_PEI_MP_SERVICES_PPI structure */
void *mp_fill_ppi_services_data(void);

/* get the number of logical processors in the platform */
efi_return_status_t mp_get_number_of_processors(efi_uintn_t *number_of_processors,
	efi_uintn_t *number_of_enabled_processors);

/* get processor info such as id, status */
efi_return_status_t mp_get_processor_info(efi_uintn_t processor_number,
	efi_processor_information *processor_info_buffer);

/* executes a caller provided function on all enabled APs */
efi_return_status_t mp_startup_all_aps(efi_ap_procedure procedure,
	efi_uintn_t timeout_usec, void *argument);

/* executes a caller provided function on all enabled APs + BSP */
efi_return_status_t mp_startup_all_cpus(efi_ap_procedure procedure,
	efi_uintn_t timeout_usec, void *argument);

/* executes a caller provided function on specific AP */
efi_return_status_t mp_startup_this_ap(efi_ap_procedure procedure,
	efi_uintn_t processor_number, efi_uintn_t timeout_usec, void *argument);

/* get the processor instance */
efi_return_status_t mp_identify_processor(efi_uintn_t *processor_number);

/* for the APIs that are not supported/required */
static inline efi_return_status_t mp_api_unsupported(void)
{
	return FSP_UNSUPPORTED;
}

#endif	/* MP_SERVICE_PPI_H */
