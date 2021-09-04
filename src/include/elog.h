/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ELOG_H_
#define ELOG_H_

#include <commonlib/bsd/elog.h>
#include <stdint.h>

#if CONFIG(ELOG)
/* Eventlog backing storage must be initialized before calling elog_init(). */
extern int elog_init(void);
extern int elog_clear(void);
/* Event addition functions return < 0 on failure and 0 on success. */
extern int elog_add_event_raw(u8 event_type, void *data, u8 data_size);
extern int elog_add_event(u8 event_type);
extern int elog_add_event_byte(u8 event_type, u8 data);
extern int elog_add_event_word(u8 event_type, u16 data);
extern int elog_add_event_dword(u8 event_type, u32 data);
extern int elog_add_event_wake(u8 source, u32 instance);
extern int elog_smbios_write_type15(unsigned long *current, int handle);
extern int elog_add_extended_event(u8 type, u32 complement);
#else
/* Stubs to help avoid littering sources with #if CONFIG_ELOG */
static inline int elog_init(void) { return -1; }
static inline int elog_clear(void) { return -1; }
static inline int elog_add_event_raw(u8 event_type, void *data,
					u8 data_size) { return 0; }
static inline int elog_add_event(u8 event_type) { return 0; }
static inline int elog_add_event_byte(u8 event_type, u8 data) { return 0; }
static inline int elog_add_event_word(u8 event_type, u16 data) { return 0; }
static inline int elog_add_event_dword(u8 event_type, u32 data) { return 0; }
static inline int elog_add_event_wake(u8 source, u32 instance) { return 0; }
static inline int elog_smbios_write_type15(unsigned long *current,
						int handle) {
	return 0;
}
static inline int elog_add_extended_event(u8 type, u32 complement) { return 0; }
#endif

#if CONFIG(ELOG_GSMI)
#define elog_gsmi_add_event elog_add_event
#define elog_gsmi_add_event_byte elog_add_event_byte
#define elog_gsmi_add_event_word elog_add_event_word
#else
static inline int elog_gsmi_add_event(u8 event_type) { return 0; }
static inline int elog_gsmi_add_event_byte(u8 event_type, u8 data) { return 0; }
static inline int elog_gsmi_add_event_word(u8 event_type, u16 data) { return 0; }
#endif

extern u32 gsmi_exec(u8 command, u32 *param);

#if CONFIG(ELOG_BOOT_COUNT)
u32 boot_count_read(void);
#else
static inline u32 boot_count_read(void)
{
	return 0;
}
#endif
u32 boot_count_increment(void);

static inline void elog_boot_notify(int s3_resume)
{
	if (CONFIG(ELOG_BOOT_COUNT) && !s3_resume)
		boot_count_increment();
}

/*
 * Callback from GSMI handler to allow platform to log any wake source
 * information.
 */
void elog_gsmi_cb_platform_log_wake_source(void);

/*
 * Callback from GSMI handler to allow mainboard to log any wake source
 * information.
 */
void elog_gsmi_cb_mainboard_log_wake_source(void);

#endif /* ELOG_H_ */
