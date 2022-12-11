/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <Ppi/MpServices2.h>

typedef EDKII_PEI_MP_SERVICES2_PPI efi_pei_mp_services_ppi;

static efi_return_status_t mps2_noop_get_number_of_processors(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t *number_of_processors,
	efi_uintn_t *number_of_enabled_processors)
{
	*number_of_processors = 1; /* BSP alone */
	*number_of_enabled_processors = 1; /* BSP alone */

	return FSP_SUCCESS;
}

static efi_return_status_t mps2_noop_get_processor_info(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t ignored2,
	efi_processor_information *ignored3)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_noop_startup_all_aps(
	efi_pei_mp_services_ppi *ignored1,
	efi_ap_procedure ignored2, efi_boolean_t ignored3,
	efi_uintn_t ignored4, void *ignored5)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_noop_startup_all_cpus(
	efi_pei_mp_services_ppi *ignored1,
	efi_ap_procedure ignored2,
	efi_uintn_t ignored3, void *ignored4)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_noop_startup_this_ap(
	efi_pei_mp_services_ppi *ignored1,
	efi_ap_procedure ignored2, efi_uintn_t ignored3,
	efi_uintn_t ignored4, void *ignored5)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_noop_switch_bsp(
	efi_pei_mp_services_ppi *ignored1, efi_uintn_t ignored2,
	efi_boolean_t ignored3)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_noop_enable_disable_ap(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t ignored2, efi_boolean_t ignored3, efi_uint32_t *ignored4)
{
	return mp_api_unsupported();
}

static efi_return_status_t mps2_noop_identify_processor(
	efi_pei_mp_services_ppi *ignored1,
	efi_uintn_t *ignored2)
{
	return mp_api_unsupported();
}

/*
 * Default MP Services data structure adhering to the EDK2 UEFIPKG Open
 * Source version 2 specification
 */

static efi_pei_mp_services_ppi mp_service2_noop_ppi = {
	mps2_noop_get_number_of_processors,
	mps2_noop_get_processor_info,
	mps2_noop_startup_all_aps,
	mps2_noop_startup_this_ap,
	mps2_noop_switch_bsp,
	mps2_noop_enable_disable_ap,
	mps2_noop_identify_processor,
	mps2_noop_startup_all_cpus,
};

void *mp_fill_ppi_services_data(void)
{
	return (void *)&mp_service2_noop_ppi;
}
