/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>

#define BSP_CPU_SLOT 0

static efi_return_status_t mp_get_number_of_processors(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t *number_of_processors,
	efi_uintn_t *number_of_enabled_processors)
{
	if (number_of_processors == NULL || number_of_enabled_processors ==
			NULL)
		return FSP_INVALID_PARAMETER;

	*number_of_processors = get_cpu_count();
	*number_of_enabled_processors = get_cpu_count();

	return FSP_SUCCESS;
}

static efi_return_status_t mp_get_processor_info(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t processor_number,
	efi_processor_information *processor_info_buffer)
{
	if (cpu_index() < 0)
		return FSP_DEVICE_ERROR;

	if (processor_info_buffer == NULL)
		return FSP_INVALID_PARAMETER;

	if (processor_number >= get_cpu_count())
		return FSP_NOT_FOUND;

	processor_info_buffer->ProcessorId = lapicid();

	processor_info_buffer->StatusFlag = PROCESSOR_HEALTH_STATUS_BIT
			| PROCESSOR_ENABLED_BIT;

	if (processor_number == BSP_CPU_SLOT)
		processor_info_buffer->StatusFlag |= PROCESSOR_AS_BSP_BIT;

	/* TODO: Fill EFI_CPU_PHYSICAL_LOCATION structure information */
	return FSP_SUCCESS;
}

static efi_return_status_t mp_startup_all_aps(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_ap_procedure procedure, efi_boolean_t ignored3,
	efi_uintn_t timeout_usec, void *argument)
{
	if (cpu_index() < 0)
		return FSP_DEVICE_ERROR;

	if (procedure == NULL)
		return FSP_INVALID_PARAMETER;

	if (mp_run_on_aps((void *)procedure, argument,
			MP_RUN_ON_ALL_CPUS, timeout_usec)) {
		printk(BIOS_DEBUG, "%s: Exit with Failure\n", __func__);
		return FSP_NOT_STARTED;
	}

	return FSP_SUCCESS;
}

static efi_return_status_t mp_startup_this_ap(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_ap_procedure procedure, efi_uintn_t processor_number,
	efi_uintn_t timeout_usec, void *argument)
{
	if (cpu_index() < 0)
		return FSP_DEVICE_ERROR;

	if (processor_number > get_cpu_count())
		return FSP_NOT_FOUND;

	if (processor_number == BSP_CPU_SLOT)
		return FSP_INVALID_PARAMETER;

	if (procedure == NULL)
		return FSP_INVALID_PARAMETER;

	if (mp_run_on_aps((void *)procedure, argument,
			processor_number, timeout_usec)) {
		printk(BIOS_DEBUG, "%s: Exit with Failure\n", __func__);
		return FSP_NOT_STARTED;
	}

	return FSP_SUCCESS;
}

static efi_return_status_t mp_switch_bsp(const efi_pei_services **ignored1,
	efi_pei_mp_services_ppi *ignored2, efi_uintn_t ignored3,
	efi_boolean_t ignored4)
{
	/* FSP don't need this API hence return unsupported */
	return FSP_UNSUPPORTED;
}

static efi_return_status_t mp_enable_disable_ap(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t ignored3, efi_boolean_t ignored4, efi_uint32_t *ignored5)
{
	/* FSP don't need this API hence return unsupported */
	return FSP_UNSUPPORTED;
}

static efi_return_status_t mp_identify_processor(const
	efi_pei_services **ignored1, efi_pei_mp_services_ppi *ignored2,
	efi_uintn_t *processor_number)
{
	int index;

	if (processor_number == NULL)
		return FSP_INVALID_PARAMETER;

	index = cpu_index();

	if (index < 0)
		return FSP_DEVICE_ERROR;

	*processor_number = index;

	return FSP_SUCCESS;
}

/*
 * EDK2 UEFIPKG Open Source MP Service PPI to be installed
 */

static efi_pei_mp_services_ppi mp_service_ppi = {
	mp_get_number_of_processors,
	mp_get_processor_info,
	mp_startup_all_aps,
	mp_startup_this_ap,
	mp_switch_bsp,
	mp_enable_disable_ap,
	mp_identify_processor,
};

efi_pei_mp_services_ppi *mp_fill_ppi_services_data(void)
{
	return &mp_service_ppi;
}
