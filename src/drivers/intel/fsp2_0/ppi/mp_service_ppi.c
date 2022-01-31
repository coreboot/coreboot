/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <fsp/api.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <types.h>

#define BSP_CPU_SLOT 0

efi_return_status_t mp_get_number_of_processors(efi_uintn_t *number_of_processors,
	efi_uintn_t *number_of_enabled_processors)
{
	if (number_of_processors == NULL || number_of_enabled_processors ==
			NULL)
		return FSP_INVALID_PARAMETER;

	*number_of_processors = get_cpu_count();
	*number_of_enabled_processors = get_cpu_count();

	return FSP_SUCCESS;
}

efi_return_status_t mp_get_processor_info(efi_uintn_t processor_number,
	efi_processor_information *processor_info_buffer)
{
	int apicid;
	uint8_t package, core, thread;

	if (cpu_index() < 0)
		return FSP_DEVICE_ERROR;

	if (processor_info_buffer == NULL)
		return FSP_INVALID_PARAMETER;

	if (processor_number >= get_cpu_count())
		return FSP_NOT_FOUND;

	apicid = cpu_get_apic_id(processor_number);

	if (apicid < 0)
		return FSP_DEVICE_ERROR;

	processor_info_buffer->ProcessorId = apicid;

	processor_info_buffer->StatusFlag = PROCESSOR_HEALTH_STATUS_BIT
			| PROCESSOR_ENABLED_BIT;

	if (processor_number == BSP_CPU_SLOT)
		processor_info_buffer->StatusFlag |= PROCESSOR_AS_BSP_BIT;

	/* Fill EFI_CPU_PHYSICAL_LOCATION structure information */
	get_cpu_topology_from_apicid(apicid, &package, &core, &thread);

	processor_info_buffer->Location.Package = package;
	processor_info_buffer->Location.Core = core;
	processor_info_buffer->Location.Thread = thread;

	return FSP_SUCCESS;
}

efi_return_status_t mp_startup_all_aps(efi_ap_procedure procedure,
	bool run_serial, efi_uintn_t timeout_usec, void *argument)
{
	if (cpu_index() < 0)
		return FSP_DEVICE_ERROR;

	if (procedure == NULL)
		return FSP_INVALID_PARAMETER;

	if (mp_run_on_all_aps((void *)procedure, argument, timeout_usec, !run_serial) !=
				CB_SUCCESS) {
		printk(BIOS_DEBUG, "%s: Exit with Failure\n", __func__);
		return FSP_NOT_STARTED;
	}

	return FSP_SUCCESS;
}

efi_return_status_t mp_startup_all_cpus(efi_ap_procedure procedure,
	efi_uintn_t timeout_usec, void *argument)
{
	if (cpu_index() < 0)
		return FSP_DEVICE_ERROR;

	if (procedure == NULL)
		return FSP_INVALID_PARAMETER;

	/* Run on BSP */
	procedure(argument);

	/* Run on APs */
	if (mp_run_on_aps((void *)procedure, argument,
			MP_RUN_ON_ALL_CPUS, timeout_usec) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s: Exit with Failure\n", __func__);
		return FSP_NOT_STARTED;
	}

	return FSP_SUCCESS;
}

efi_return_status_t mp_startup_this_ap(efi_ap_procedure procedure,
	efi_uintn_t processor_number, efi_uintn_t timeout_usec, void *argument)
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
			processor_number, timeout_usec) != CB_SUCCESS) {
		printk(BIOS_DEBUG, "%s: Exit with Failure\n", __func__);
		return FSP_NOT_STARTED;
	}

	return FSP_SUCCESS;
}

efi_return_status_t mp_identify_processor(efi_uintn_t *processor_number)
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
