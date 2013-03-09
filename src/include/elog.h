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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef ELOG_H_
#define ELOG_H_

#if CONFIG_ELOG

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
#define EC_EVENT_LID_CLOSED                0x01
#define EC_EVENT_LID_OPEN                  0x02
#define EC_EVENT_POWER_BUTTON              0x03
#define EC_EVENT_AC_CONNECTED              0x04
#define EC_EVENT_AC_DISCONNECTED           0x05
#define EC_EVENT_BATTERY_LOW               0x06
#define EC_EVENT_BATTERY_CRITICAL          0x07
#define EC_EVENT_BATTERY                   0x08
#define EC_EVENT_THERMAL_THRESHOLD         0x09
#define EC_EVENT_THERMAL_OVERLOAD          0x0a
#define EC_EVENT_THERMAL                   0x0b
#define EC_EVENT_USB_CHARGER               0x0c
#define EC_EVENT_KEY_PRESSED               0x0d
#define EC_EVENT_INTERFACE_READY           0x0e
#define EC_EVENT_KEYBOARD_RECOVERY         0x0f
#define EC_EVENT_THERMAL_SHUTDOWN          0x10
#define EC_EVENT_BATTERY_SHUTDOWN          0x11

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
#define ELOG_TYPE_ACPI_WAKE               0x9e
#define ELOG_TYPE_WAKE_SOURCE             0x9f
#define  ELOG_WAKE_SOURCE_PCIE             0x00
#define  ELOG_WAKE_SOURCE_PME              0x01
#define  ELOG_WAKE_SOURCE_PME_INTERNAL     0x02
#define  ELOG_WAKE_SOURCE_RTC              0x03
#define  ELOG_WAKE_SOURCE_GPIO             0x04
#define  ELOG_WAKE_SOURCE_SMBUS            0x05
#define  ELOG_WAKE_SOURCE_PWRBTN           0x06
struct elog_event_data_wake {
	u8 source;
	u32 instance;
} __attribute__ ((packed));

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
} __attribute__ ((packed));

/* Last post code from previous boot */
#define ELOG_TYPE_LAST_POST_CODE          0xa3

extern int elog_init(void);
extern int elog_clear(void);
extern void elog_add_event_raw(u8 event_type, void *data, u8 data_size);
extern void elog_add_event(u8 event_type);
extern void elog_add_event_byte(u8 event_type, u8 data);
extern void elog_add_event_word(u8 event_type, u16 data);
extern void elog_add_event_dword(u8 event_type, u32 data);
extern void elog_add_event_wake(u8 source, u32 instance);
extern int elog_smbios_write_type15(unsigned long *current, int handle);

#if CONFIG_ELOG_GSMI
extern u32 gsmi_exec(u8 command, u32 *param);
#endif

#if CONFIG_ELOG_BOOT_COUNT
u32 boot_count_read(void);
u32 boot_count_increment(void);
#endif

#endif /* !CONFIG_ELOG */

#endif /* ELOG_H_ */
