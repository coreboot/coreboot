/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP2_0_DEBUG_H_
#define _FSP2_0_DEBUG_H_

#include <fsp/util.h>

enum fsp_log_level {
	FSP_LOG_LEVEL_DISABLE = 0,
	FSP_LOG_LEVEL_ERR,
	FSP_LOG_LEVEL_ERR_WARN,
	FSP_LOG_LEVEL_ERR_WARN_INFO,
	FSP_LOG_LEVEL_ERR_WARN_INFO_EVENT,
	FSP_LOG_LEVEL_VERBOSE
};

/* FSP debug API */
enum fsp_log_level fsp_map_console_log_level(void);
void fsp_debug_before_memory_init(fsp_memory_init_fn memory_init,
	const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd);
void fsp_debug_after_memory_init(efi_return_status_t status);
void fsp_debug_before_silicon_init(fsp_silicon_init_fn silicon_init,
	const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd);
void fsp_debug_after_silicon_init(efi_return_status_t status);
void fsp_before_debug_notify(fsp_notify_fn notify,
	const struct fsp_notify_params *notify_params);
void fsp_debug_after_notify(efi_return_status_t status);
void fspm_display_upd_values(const FSPM_UPD *old,
	const FSPM_UPD *new);
void fsp_display_hobs(void);
void fsp_verify_memory_init_hobs(void);
void fsp_print_header_info(const struct fsp_header *hdr);

/* Callbacks for displaying UPD parameters - place in a separate file
 * that is conditionally build with CONFIG(DISPLAY_UPD_DATA).
 */
void soc_display_fspm_upd_params(const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd);
void soc_display_fsps_upd_params(const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd);

/* Callbacks for displaying HOBs - place in a separate file that is
 * conditionally build with CONFIG(DISPLAY_HOBS).
 */
const char *soc_get_hob_type_name(const struct hob_header *hob);
const char *soc_get_guid_name(const uint8_t *guid);
void soc_display_hob(const struct hob_header *hob);

/* FSP debug utility functions */
void fsp_display_upd_value(const char *name, size_t size, uint64_t old,
	uint64_t new);
void fsp_print_guid(int level, const void *guid);
void fsp_print_memory_resource_hobs(void);
void fsp_print_resource_descriptor(const void *base);
const char *fsp_get_hob_type_name(const struct hob_header *hob);
const char *fsp_get_guid_name(const uint8_t *guid);
void fsp_print_guid_extension_hob(const struct hob_header *hob);

/* Callback to snapshot all GPIO configurations. */
void gpio_snapshot(void);
/* Callback to verify that current GPIO configuration matches the saved snapshot */
size_t gpio_verify_snapshot(void);

/*
 * Retrieve fsp_pcd_debug_level file from option backend (e.g. CBFS) to identify the log-level
 * used for outputting FSP debug messages.
 *
 * 1. Critical errors, need action etc., FSP_LOG_LEVEL_ERR aka value 1
 * 2. #1 including warnings, FSP_LOG_LEVEL_ERR_WARN aka value 2
 * 3. #2 including additional informational messages, FSP_LOG_LEVEL_ERR_WARN_INFO aka value 3
 *
 * The default log-level is setup in coreboot while stitching the CBFS option binaries
 * depending upon the coreboot log-level. One can override that using below example:
 *
 * Here is an example of adding fsp_pcd_debug_level option binary file into the RO-CBFS
 * to specify the FSP log-level:
 *  - cbfstool <AP FW image> add-int -i <log-level> -n option/fsp_pcd_debug_level
 *
 * If OPTION_BACKEND_NONE then the then, use log levels will be determined by
 * calling into fsp_map_console_log_level API.
 */
enum fsp_log_level fsp_get_pcd_debug_log_level(void);
/*
 * Retrieve fsp_mrc_debug_level file from option backend (e.g. CBFS) to identify the log-level
 * used for outputting FSP debug messages.
 *
 * 1. Critical errors, need action etc., FSP_LOG_LEVEL_ERR aka value 1
 * 2. #1 including warnings, FSP_LOG_LEVEL_ERR_WARN aka value 2
 * 3. #2 including additional informational messages, FSP_LOG_LEVEL_ERR_WARN_INFO aka value 3
 * 4. #3 including event logs, FSP_LOG_LEVEL_ERR_WARN_INFO_EVENT aka value 4
 * 5. Use FSP_LOG_LEVEL_VERBOSE aka 5 for all types of debug messages.
 *
 * The default log-level is setup in coreboot while stitching the CBFS option binaries
 * depending upon the coreboot log-level. One can override that using below example:
 *
 * Here is an example of adding fsp_mrc_debug_level option binary file into the RO-CBFS
 * to specify the FSP log-level:
 *  - cbfstool <AP FW image> add-int -i <log-level> -n option/fsp_mrc_debug_level
 *
 * If OPTION_BACKEND_NONE then the then, use log levels will be determined by
 * calling into fsp_map_console_log_level API.
 */
enum fsp_log_level fsp_get_mrc_debug_log_level(void);

#endif /* _FSP2_0_DEBUG_H_ */
