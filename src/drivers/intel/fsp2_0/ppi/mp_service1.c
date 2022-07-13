/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <Ppi/MpServices.h>

typedef EFI_PEI_MP_SERVICES_PPI efi_pei_mp_services_ppi;

static efi_return_status_t mps1_get_number_of_processors(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t *number_of_processors, efi_uintn_t *number_of_enabled_processors)
{
	return mp_get_number_of_processors(number_of_processors, number_of_enabled_processors);
}

static efi_return_status_t mps1_get_processor_info(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t processor_number,
	efi_processor_information *processor_info_buffer)
{
	return mp_get_processor_info(processor_number, processor_info_buffer);
}

static efi_return_status_t mps1_startup_all_aps(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_ap_procedure procedure, efi_boolean_t run_serial,
	efi_uintn_t timeout_usec, void *argument)
{
	return mp_startup_all_aps(procedure, run_serial, timeout_usec, argument);
}

static efi_return_status_t mps1_startup_this_ap(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_ap_procedure procedure, efi_uintn_t processor_number,
	efi_uintn_t timeout_usec, void *argument)
{
	return mp_startup_this_ap(procedure, processor_number, timeout_usec, argument);
}

static efi_return_status_t mps1_switch_bsp(const efi_pei_services **ignored1,
	efi_pei_mp_services_ppi *ignored2, efi_uintn_t ignored3,
	efi_boolean_t ignored4)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps1_enable_disable_ap(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t ignored3, efi_boolean_t ignored4, efi_uint32_t *ignored5)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps1_identify_processor(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t *processor_number)
{
	return mp_identify_processor(processor_number);
}

/* edk2 UEFIPKG Open Source MP Service PPI to be installed */

static efi_pei_mp_services_ppi mp_service1_ppi = {
	mps1_get_number_of_processors,
	mps1_get_processor_info,
	mps1_startup_all_aps,
	mps1_startup_this_ap,
	mps1_switch_bsp,
	mps1_enable_disable_ap,
	mps1_identify_processor,
};

void *mp_fill_ppi_services_data(void)
{
	return (void *)&mp_service1_ppi;
}
