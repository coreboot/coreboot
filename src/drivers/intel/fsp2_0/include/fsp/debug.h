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
void fsp_debug_after_memory_init(uint32_t status);
void fsp_debug_before_silicon_init(fsp_silicon_init_fn silicon_init,
	const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd);
void fsp_debug_after_silicon_init(uint32_t status);
void fsp_before_debug_notify(fsp_notify_fn notify,
	const struct fsp_notify_params *notify_params);
void fsp_debug_after_notify(uint32_t status);
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

/*
 * Writes number_of_bytes data bytes from buffer to the console.
 * The number of bytes actually written to the console is returned.
 *
 * If number_of_bytes is zero, don't output any data but instead wait until
 * the console has output all data, then return 0.
 */
asmlinkage size_t fsp_write_line(uint8_t *buffer, size_t number_of_bytes);

/* Callback to snapshot all GPIO configurations. */
void gpio_snapshot(void);
/* Callback to verify that current GPIO configuration matches the saved snapshot */
size_t gpio_verify_snapshot(void);

#endif /* _FSP2_0_DEBUG_H_ */
