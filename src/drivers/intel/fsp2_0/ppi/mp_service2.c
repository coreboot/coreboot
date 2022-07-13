/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <Ppi/MpServices2.h>

typedef EDKII_PEI_MP_SERVICES2_PPI efi_pei_mp_services_ppi;

static efi_return_status_t mps2_get_number_of_processors(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t *number_of_processors,
	efi_uintn_t *number_of_enabled_processors)
{
	return mp_get_number_of_processors(number_of_processors, number_of_enabled_processors);
}

static efi_return_status_t mps2_get_processor_info(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t processor_number,
	efi_processor_information *processor_info_buffer)
{
	return mp_get_processor_info(processor_number, processor_info_buffer);
}

static efi_return_status_t mps2_startup_all_aps(
	efi_pei_mp_services_ppi *ignored1,
	efi_ap_procedure procedure, efi_boolean_t run_serial,
	efi_uintn_t timeout_usec, void *argument)
{
	return mp_startup_all_aps(procedure, run_serial, timeout_usec, argument);
}

static efi_return_status_t mps2_startup_all_cpus(
	efi_pei_mp_services_ppi *ignored1,
	efi_ap_procedure procedure,
	efi_uintn_t timeout_usec, void *argument)
{
	return mp_startup_all_cpus(procedure, timeout_usec, argument);
}

static efi_return_status_t mps2_startup_this_ap(
	efi_pei_mp_services_ppi *ignored1,
	efi_ap_procedure procedure, efi_uintn_t processor_number,
	efi_uintn_t timeout_usec, void *argument)
{
	return mp_startup_this_ap(procedure, processor_number, timeout_usec, argument);
}

static efi_return_status_t mps2_switch_bsp(
	efi_pei_mp_services_ppi *ignored1, efi_uintn_t ignored2,
	efi_boolean_t ignored3)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_enable_disable_ap(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t ignored2, efi_boolean_t ignored3, efi_uint32_t *ignored4)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_identify_processor(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t *processor_number)
{
	return mp_identify_processor(processor_number);
}

/* edk2 UEFIPKG Open Source MP Services 2 PPI to be installed */

static efi_pei_mp_services_ppi mp_service2_ppi = {
	mps2_get_number_of_processors,
	mps2_get_processor_info,
	mps2_startup_all_aps,
	mps2_startup_this_ap,
	mps2_switch_bsp,
	mps2_enable_disable_ap,
	mps2_identify_processor,
	mps2_startup_all_cpus,
};

void *mp_fill_ppi_services_data(void)
{
	return (void *)&mp_service2_ppi;
}
