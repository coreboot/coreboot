/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <console/streams.h>
#include <cpu/x86/mtrr.h>
#include <fsp/debug.h>
#include <fsp/util.h>
#include <option.h>

enum fsp_call_phase {
	BEFORE_FSP_CALL,
	AFTER_FSP_CALL,
};

static void fsp_gpio_config_check(enum fsp_call_phase phase, const char *call_str)
{
	switch (phase) {
	case BEFORE_FSP_CALL:
		printk(BIOS_SPEW, "Snapshot all GPIOs before %s.\n", call_str);
		gpio_snapshot();
		break;
	case AFTER_FSP_CALL:
		printk(BIOS_SPEW, "Verify GPIO snapshot after %s...", call_str);
		printk(BIOS_SPEW, "%zd changes detected!\n", gpio_verify_snapshot());
		break;
	default:
		break;
	}
}

enum fsp_log_level fsp_map_console_log_level(void)
{
	enum fsp_log_level fsp_debug_level;

	switch (get_log_level()) {
	case BIOS_EMERG:
	case BIOS_ALERT:
	case BIOS_CRIT:
	case BIOS_ERR:
		fsp_debug_level = FSP_LOG_LEVEL_ERR;
		break;
	case BIOS_WARNING:
		fsp_debug_level = FSP_LOG_LEVEL_ERR_WARN;
		break;
	case BIOS_NOTICE:
		fsp_debug_level = FSP_LOG_LEVEL_ERR_WARN_INFO;
		break;
	case BIOS_INFO:
		fsp_debug_level = FSP_LOG_LEVEL_ERR_WARN_INFO_EVENT;
		break;
	case BIOS_DEBUG:
	case BIOS_SPEW:
		fsp_debug_level = FSP_LOG_LEVEL_VERBOSE;
		break;
	default:
		fsp_debug_level = FSP_LOG_LEVEL_DISABLE;
		break;
	}

	if (!CONFIG(DEBUG_RAM_SETUP))
		fsp_debug_level = MIN(fsp_debug_level, FSP_LOG_LEVEL_ERR_WARN_INFO);

	return fsp_debug_level;
}

/*-----------
 * MemoryInit
 *-----------
 */
void fsp_debug_before_memory_init(fsp_memory_init_fn memory_init,
	const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	display_mtrrs();

	/* Display the UPD values */
	if (CONFIG(DISPLAY_UPD_DATA))
		fspm_display_upd_values(fspm_old_upd, fspm_new_upd);

	/* Display the call entry point and parameters */
	if (!CONFIG(DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "Calling FspMemoryInit: %p\n", memory_init);
	printk(BIOS_SPEW, "\t%p: raminit_upd\n", fspm_new_upd);
	printk(BIOS_SPEW, "\t%p: &hob_list_ptr\n", fsp_get_hob_list_ptr());
}

void fsp_debug_after_memory_init(efi_return_status_t status)
{
	if (CONFIG(DISPLAY_FSP_CALLS_AND_STATUS))
		fsp_printk(status, BIOS_SPEW, "FspMemoryInit");

	if (status != FSP_SUCCESS)
		return;

	/* Verify that the HOB list pointer was set */
	if (fsp_get_hob_list() == NULL)
		die("ERROR - HOB list pointer was not returned!\n");

	/* Display and verify the HOBs */
	if (CONFIG(DISPLAY_HOBS))
		fsp_display_hobs();
	if (CONFIG(VERIFY_HOBS))
		fsp_verify_memory_init_hobs();

	display_mtrrs();
}

/*-----------
 * SiliconInit
 *-----------
 */
void fsp_debug_before_silicon_init(fsp_silicon_init_fn silicon_init,
	const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd)
{
	if (CONFIG(CHECK_GPIO_CONFIG_CHANGES))
		fsp_gpio_config_check(BEFORE_FSP_CALL, "FSP Silicon Init");

	display_mtrrs();

	/* Display the UPD values */
	if (CONFIG(DISPLAY_UPD_DATA))
		soc_display_fsps_upd_params(fsps_old_upd, fsps_new_upd);

	/* Display the call to FSP SiliconInit */
	if (!CONFIG(DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "Calling FspSiliconInit: %p\n", silicon_init);
	printk(BIOS_SPEW, "\t%p: upd\n", fsps_new_upd);
}

void fsp_debug_after_silicon_init(efi_return_status_t status)
{
	if (CONFIG(CHECK_GPIO_CONFIG_CHANGES))
		fsp_gpio_config_check(AFTER_FSP_CALL, "FSP Silicon Init");

	if (CONFIG(DISPLAY_FSP_CALLS_AND_STATUS))
		fsp_printk(status, BIOS_SPEW, "FspSiliconInit");

	/* Display the HOBs */
	if (CONFIG(DISPLAY_HOBS))
		fsp_display_hobs();

	display_mtrrs();
}

/*-----------
 * FspNotify
 *-----------
 */
void fsp_before_debug_notify(fsp_notify_fn notify,
	const struct fsp_notify_params *notify_params)
{
	if (CONFIG(CHECK_GPIO_CONFIG_CHANGES))
		fsp_gpio_config_check(BEFORE_FSP_CALL, "FSP Notify");

	/* Display the call to FspNotify */
	if (!CONFIG(DISPLAY_FSP_CALLS_AND_STATUS))
		return;
	printk(BIOS_SPEW, "0x%08x: notify_params->phase\n",
		notify_params->phase);
	printk(BIOS_SPEW, "Calling FspNotify: %p\n", notify);
	printk(BIOS_SPEW, "\t%p: notify_params\n", notify_params);
}

void fsp_debug_after_notify(efi_return_status_t status)
{
	if (CONFIG(CHECK_GPIO_CONFIG_CHANGES))
		fsp_gpio_config_check(AFTER_FSP_CALL, "FSP Notify");

	if (CONFIG(DISPLAY_FSP_CALLS_AND_STATUS))
		fsp_printk(status, BIOS_SPEW, "FspNotify");

	/* Display the HOBs */
	if (CONFIG(DISPLAY_HOBS))
		fsp_display_hobs();

	display_mtrrs();
}

enum fsp_log_level fsp_get_pcd_debug_log_level(void)
{
	return get_uint_option("fsp_pcd_debug_level", fsp_map_console_log_level());
}

enum fsp_log_level fsp_get_mrc_debug_log_level(void)
{
	return get_uint_option("fsp_mrc_debug_level", fsp_map_console_log_level());
}
