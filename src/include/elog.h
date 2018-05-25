/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef ELOG_H_
#define ELOG_H_

#include <compiler.h>

/* SMI command code for GSMI event logging */
#define ELOG_GSMI_APM_CNT                 0xEF

#define MAX_EVENT_SIZE                    0x7F

/* End of log */
#define ELOG_TYPE_EOL                     0xFF

/*
 * Standard SMBIOS event log types below 0x80
 */
#define ELOG_TYPE_UNDEFINED_EVENT         0x00
#define ELOG_TYPE_SINGLE_BIT_ECC_MEM_ERR  0x01
#define ELOG_TYPE_MULTI_BIT_ECC_MEM_ERR   0x02
#define ELOG_TYPE_MEM_PARITY_ERR          0x03
#define ELOG_TYPE_BUS_TIMEOUT             0x04
#define ELOG_TYPE_IO_CHECK                0x05
#define ELOG_TYPE_SW_NMI                  0x06
#define ELOG_TYPE_POST_MEM_RESIZE         0x07
#define ELOG_TYPE_POST_ERR                0x08
#define ELOG_TYPE_PCI_PERR                0x09
#define ELOG_TYPE_PCI_SERR                0x0A
#define ELOG_TYPE_CPU_FAIL                0x0B
#define ELOG_TYPE_EISA_TIMEOUT            0x0C
#define ELOG_TYPE_CORRECTABLE_MEMLOG_DIS  0x0D
#define ELOG_TYPE_LOG_DISABLED            0x0E
#define ELOG_TYPE_UNDEFINED_EVENT2        0x0F
#define ELOG_TYPE_SYS_LIMIT_EXCEED        0x10
#define ELOG_TYPE_ASYNC_HW_TIMER_EXPIRED  0x11
#define ELOG_TYPE_SYS_CONFIG_INFO         0x12
#define ELOG_TYPE_HDD_INFO                0x13
#define ELOG_TYPE_SYS_RECONFIG            0x14
#define ELOG_TYPE_CPU_ERROR               0x15
#define ELOG_TYPE_LOG_CLEAR               0x16
#define ELOG_TYPE_BOOT                    0x17

/*
 * Extended defined OEM event types start at 0x80
 */

/* OS/kernel events */
#define ELOG_TYPE_OS_EVENT                0x81

/* Last event from coreboot */
#define ELOG_TYPE_OS_BOOT                 0x90

/* Embedded controller event */
#define ELOG_TYPE_EC_EVENT                0x91
#define EC_EVENT_LID_CLOSED                  0x01
#define EC_EVENT_LID_OPEN                    0x02
#define EC_EVENT_POWER_BUTTON                0x03
#define EC_EVENT_AC_CONNECTED                0x04
#define EC_EVENT_AC_DISCONNECTED             0x05
#define EC_EVENT_BATTERY_LOW                 0x06
#define EC_EVENT_BATTERY_CRITICAL            0x07
#define EC_EVENT_BATTERY                     0x08
#define EC_EVENT_THERMAL_THRESHOLD           0x09
#define EC_EVENT_DEVICE_EVENT                0x0a
#define EC_EVENT_THERMAL                     0x0b
#define EC_EVENT_USB_CHARGER                 0x0c
#define EC_EVENT_KEY_PRESSED                 0x0d
#define EC_EVENT_INTERFACE_READY             0x0e
#define EC_EVENT_KEYBOARD_RECOVERY           0x0f
#define EC_EVENT_THERMAL_SHUTDOWN            0x10
#define EC_EVENT_BATTERY_SHUTDOWN            0x11
#define EC_EVENT_FAN_ERROR                   0x12
#define EC_EVENT_THROTTLE_STOP               0x13
#define EC_EVENT_HANG_DETECT                 0x14
#define EC_EVENT_HANG_REBOOT                 0x15
#define EC_EVENT_PD_MCU                      0x16
#define EC_EVENT_BATTERY_STATUS              0x17
#define EC_EVENT_PANIC                       0x18
#define EC_EVENT_KEYBOARD_FASTBOOT           0x19
#define EC_EVENT_RTC                         0x1a
#define EC_EVENT_MKBP                        0x1b
#define EC_EVENT_USB_MUX                     0x1c
#define EC_EVENT_MODE_CHANGE                 0x1d
#define EC_EVENT_KEYBOARD_RECOVERY_HWREINIT  0x1e
#define EC_EVENT_EXTENDED                    0x1f

/* Power */
#define ELOG_TYPE_POWER_FAIL              0x92
#define ELOG_TYPE_SUS_POWER_FAIL          0x93
#define ELOG_TYPE_PWROK_FAIL              0x94
#define ELOG_TYPE_SYS_PWROK_FAIL          0x95
#define ELOG_TYPE_POWER_ON                0x96
#define ELOG_TYPE_POWER_BUTTON            0x97
#define ELOG_TYPE_POWER_BUTTON_OVERRIDE   0x98

/* Reset */
#define ELOG_TYPE_RESET_BUTTON            0x99
#define ELOG_TYPE_SYSTEM_RESET            0x9a
#define ELOG_TYPE_RTC_RESET               0x9b
#define ELOG_TYPE_TCO_RESET               0x9c

/* Sleep/Wake */
#define ELOG_TYPE_ACPI_ENTER              0x9d
/* Deep Sx wake variant is provided below - 0xad */
#define ELOG_TYPE_ACPI_WAKE               0x9e
#define ELOG_TYPE_WAKE_SOURCE             0x9f
#define  ELOG_WAKE_SOURCE_PCIE             0x00
#define  ELOG_WAKE_SOURCE_PME              0x01
#define  ELOG_WAKE_SOURCE_PME_INTERNAL     0x02
#define  ELOG_WAKE_SOURCE_RTC              0x03
#define  ELOG_WAKE_SOURCE_GPIO             0x04
#define  ELOG_WAKE_SOURCE_SMBUS            0x05
#define  ELOG_WAKE_SOURCE_PWRBTN           0x06
#define  ELOG_WAKE_SOURCE_PME_HDA          0x07
#define  ELOG_WAKE_SOURCE_PME_GBE          0x08
#define  ELOG_WAKE_SOURCE_PME_EMMC         0x09
#define  ELOG_WAKE_SOURCE_PME_SDCARD       0x0a
#define  ELOG_WAKE_SOURCE_PME_PCIE1        0x0b
#define  ELOG_WAKE_SOURCE_PME_PCIE2        0x0c
#define  ELOG_WAKE_SOURCE_PME_PCIE3        0x0d
#define  ELOG_WAKE_SOURCE_PME_PCIE4        0x0e
#define  ELOG_WAKE_SOURCE_PME_PCIE5        0x0f
#define  ELOG_WAKE_SOURCE_PME_PCIE6        0x10
#define  ELOG_WAKE_SOURCE_PME_PCIE7        0x11
#define  ELOG_WAKE_SOURCE_PME_PCIE8        0x12
#define  ELOG_WAKE_SOURCE_PME_PCIE9        0x13
#define  ELOG_WAKE_SOURCE_PME_PCIE10       0x14
#define  ELOG_WAKE_SOURCE_PME_PCIE11       0x15
#define  ELOG_WAKE_SOURCE_PME_PCIE12       0x16
#define  ELOG_WAKE_SOURCE_PME_SATA         0x17
#define  ELOG_WAKE_SOURCE_PME_CSE          0x18
#define  ELOG_WAKE_SOURCE_PME_CSE2         0x19
#define  ELOG_WAKE_SOURCE_PME_CSE3         0x1a
#define  ELOG_WAKE_SOURCE_PME_XHCI         0x1b
#define  ELOG_WAKE_SOURCE_PME_XDCI         0x1c
#define  ELOG_WAKE_SOURCE_PME_XHCI_USB_2   0x1d
#define  ELOG_WAKE_SOURCE_PME_XHCI_USB_3   0x1e
#define  ELOG_WAKE_SOURCE_PME_WIFI         0x1f

struct elog_event_data_wake {
	u8 source;
	u32 instance;
} __packed;

/* Chrome OS related events */
#define ELOG_TYPE_CROS_DEVELOPER_MODE     0xa0
#define ELOG_TYPE_CROS_RECOVERY_MODE      0xa1
#define  ELOG_CROS_RECOVERY_MODE_BUTTON    0x02

/* Management Engine Events */
#define ELOG_TYPE_MANAGEMENT_ENGINE       0xa2
#define ELOG_TYPE_MANAGEMENT_ENGINE_EXT   0xa4
struct elog_event_data_me_extended {
	u8 current_working_state;
	u8 operation_state;
	u8 operation_mode;
	u8 error_code;
	u8 progress_code;
	u8 current_pmevent;
	u8 current_state;
} __packed;

/* Last post code from previous boot */
#define ELOG_TYPE_LAST_POST_CODE          0xa3
#define ELOG_TYPE_POST_EXTRA              0xa6

/* EC Shutdown Reason */
#define ELOG_TYPE_EC_SHUTDOWN             0xa5

/* ARM/generic versions of sleep/wake - These came from another firmware
 * apparently, but not all the firmware sources were updated so that the
 * elog namespace was coherent. */
#define ELOG_TYPE_SLEEP                   0xa7
#define ELOG_TYPE_WAKE                    0xa8
#define ELOG_TYPE_FW_WAKE                 0xa9

/* Memory Cache Update */
#define ELOG_TYPE_MEM_CACHE_UPDATE        0xaa
#define  ELOG_MEM_CACHE_UPDATE_SLOT_NORMAL    0
#define  ELOG_MEM_CACHE_UPDATE_SLOT_RECOVERY  1
#define  ELOG_MEM_CACHE_UPDATE_SLOT_VARIABLE  2
#define  ELOG_MEM_CACHE_UPDATE_STATUS_SUCCESS 0
#define  ELOG_MEM_CACHE_UPDATE_STATUS_FAIL    1
struct elog_event_mem_cache_update {
	u8 slot;
	u8 status;
} __packed;

/* CPU Thermal Trip */
#define ELOG_TYPE_THERM_TRIP              0xab

/* Cr50 */
#define ELOG_TYPE_CR50_UPDATE             0xac

/* Deep Sx wake variant */
#define ELOG_TYPE_ACPI_DEEP_WAKE          0xad

/* EC Device Event */
#define ELOG_TYPE_EC_DEVICE_EVENT         0xae
#define ELOG_EC_DEVICE_EVENT_TRACKPAD       0x01
#define ELOG_EC_DEVICE_EVENT_DSP            0x02
#define ELOG_EC_DEVICE_EVENT_WIFI           0x03

/* S0ix sleep/wake */
#define ELOG_TYPE_S0IX_ENTER              0xaf
#define ELOG_TYPE_S0IX_EXIT               0xb0

#if IS_ENABLED(CONFIG_ELOG)
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
#endif

extern u32 gsmi_exec(u8 command, u32 *param);

#if IS_ENABLED(CONFIG_ELOG_BOOT_COUNT)
u32 boot_count_read(void);
#else
static inline u32 boot_count_read(void)
{
	return 0;
}
#endif
u32 boot_count_increment(void);

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
