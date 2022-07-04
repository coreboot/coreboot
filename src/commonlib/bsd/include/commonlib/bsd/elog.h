/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef _COMMONLIB_BSD_ELOG_H_
#define _COMMONLIB_BSD_ELOG_H_

#include <stdint.h>

#include <commonlib/bsd/cb_err.h>

/* ELOG header */
struct elog_header {
	uint32_t magic;
	uint8_t version;
	uint8_t header_size;
	uint8_t reserved[2];
} __packed;

/* ELOG related constants */
#define ELOG_SIGNATURE			0x474f4c45  /* 'ELOG' */
#define ELOG_VERSION			1
#define ELOG_RW_REGION_NAME		"RW_ELOG"

/* SMBIOS event log header */
struct event_header {
	uint8_t type;
	uint8_t length;
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} __packed;

/* SMBIOS Type 15 related constants */
#define ELOG_HEADER_TYPE_OEM		0x88

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
#define ELOG_OS_EVENT_CLEAN              0  /* Clean Shutdown */
#define ELOG_OS_EVENT_NMIWDT             1  /* NMI Watchdog */
#define ELOG_OS_EVENT_PANIC              2  /* Panic */
#define ELOG_OS_EVENT_OOPS               3  /* Oops */
#define ELOG_OS_EVENT_DIE                4  /* Die */
#define ELOG_OS_EVENT_MCE                5  /* MCE */
#define ELOG_OS_EVENT_SOFTWDT            6  /* Software Watchdog */
#define ELOG_OS_EVENT_MBE                7  /* MBE */
#define ELOG_OS_EVENT_TRIPLE             8  /* Triple Fault */
#define ELOG_OS_EVENT_THERMAL            9  /* Critical Thermal Threshold */

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
#define EC_EVENT_THROTTLE_START              0x12
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
struct elog_ec_event {
	uint8_t event;
	uint8_t checksum;
} __packed;

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
/*
 * Deep Sx wake variant is provided below - 0xad
 * Sleep/"wake pending" event log provided below - 0xb1 - 0x01/0x02
 */

#define ELOG_TYPE_ACPI_WAKE               0x9e
#define ELOG_TYPE_WAKE_SOURCE             0x9f
#define  ELOG_WAKE_SOURCE_PCIE             0x00
#define  ELOG_WAKE_SOURCE_PME              0x01
#define  ELOG_WAKE_SOURCE_PME_INTERNAL     0x02
#define  ELOG_WAKE_SOURCE_RTC              0x03
#define  ELOG_WAKE_SOURCE_GPE              0x04
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
#define  ELOG_WAKE_SOURCE_PME_PCIE13       0x20
#define  ELOG_WAKE_SOURCE_PME_PCIE14       0x21
#define  ELOG_WAKE_SOURCE_PME_PCIE15       0x22
#define  ELOG_WAKE_SOURCE_PME_PCIE16       0x23
#define  ELOG_WAKE_SOURCE_PME_PCIE17       0x24
#define  ELOG_WAKE_SOURCE_PME_PCIE18       0x25
#define  ELOG_WAKE_SOURCE_PME_PCIE19       0x26
#define  ELOG_WAKE_SOURCE_PME_PCIE20       0x27
#define  ELOG_WAKE_SOURCE_PME_PCIE21       0x28
#define  ELOG_WAKE_SOURCE_PME_PCIE22       0x29
#define  ELOG_WAKE_SOURCE_PME_PCIE23       0x2a
#define  ELOG_WAKE_SOURCE_PME_PCIE24       0x2b
#define  ELOG_WAKE_SOURCE_GPIO             0x2c
#define  ELOG_WAKE_SOURCE_PME_TBT          0x2d
#define  ELOG_WAKE_SOURCE_PME_TCSS_XHCI    0x2e
#define  ELOG_WAKE_SOURCE_PME_TCSS_XDCI    0x2f
#define  ELOG_WAKE_SOURCE_PME_TCSS_DMA     0x30

struct elog_event_data_wake {
	uint8_t source;
	uint32_t instance;
} __packed;

/* ChromeOS related events */
#define ELOG_TYPE_CROS_DEVELOPER_MODE     0xa0
#define ELOG_TYPE_CROS_RECOVERY_MODE      0xa1
#define  ELOG_CROS_RECOVERY_MODE_BUTTON    0x02

/* Management Engine Events */
#define ELOG_TYPE_MANAGEMENT_ENGINE      0xa2
#define  ELOG_ME_PATH_NORMAL              0x00
#define  ELOG_ME_PATH_S3WAKE              0x01
#define  ELOG_ME_PATH_ERROR               0x02
#define  ELOG_ME_PATH_RECOVERY            0x03
#define  ELOG_ME_PATH_DISABLED            0x04
#define  ELOG_ME_PATH_FW_UPDATE           0x05

#define ELOG_TYPE_MANAGEMENT_ENGINE_EXT  0xa4
#define  ELOG_ME_PHASE_ROM                0
#define  ELOG_ME_PHASE_BRINGUP            1
#define  ELOG_ME_PHASE_UKERNEL            2
#define  ELOG_ME_PHASE_POLICY             3
#define  ELOG_ME_PHASE_MODULE             4
#define  ELOG_ME_PHASE_UNKNOWN            5
#define  ELOG_ME_PHASE_HOST               6
struct elog_event_data_me_extended {
	uint8_t current_working_state;
	uint8_t operation_state;
	uint8_t operation_mode;
	uint8_t error_code;
	uint8_t progress_code;
	uint8_t current_pmevent;
	uint8_t current_state;
} __packed;

/* Last post code from previous boot */
#define ELOG_TYPE_LAST_POST_CODE          0xa3
#define ELOG_TYPE_POST_EXTRA              0xa6
#define  ELOG_TYPE_POST_EXTRA_PATH       0x01
#define   ELOG_DEV_PATH_TYPE_NONE          0
#define   ELOG_DEV_PATH_TYPE_ROOT          1
#define   ELOG_DEV_PATH_TYPE_PCI           2
#define   ELOG_DEV_PATH_TYPE_PNP           3
#define   ELOG_DEV_PATH_TYPE_I2C           4
#define   ELOG_DEV_PATH_TYPE_APIC          5
#define   ELOG_DEV_PATH_TYPE_DOMAIN        6
#define   ELOG_DEV_PATH_TYPE_CPU_CLUSTER   7
#define   ELOG_DEV_PATH_TYPE_CPU           8
#define   ELOG_DEV_PATH_TYPE_CPU_BUS       9
#define   ELOG_DEV_PATH_TYPE_IOAPIC        10

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
	uint8_t slot;
	uint8_t status;
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

/* Extended events */
#define ELOG_TYPE_EXTENDED_EVENT          0xb1
#define  ELOG_SLEEP_PENDING_PM1_WAKE       0x01
#define  ELOG_SLEEP_PENDING_GPE0_WAKE      0x02

/* Cr50 reset to enable TPM */
#define ELOG_TYPE_CR50_NEED_RESET         0xb2

/* CSME-Initiated Host Reset */
#define ELOG_TYPE_MI_HRPD                 0xb3
#define ELOG_TYPE_MI_HRPC                 0xb4
#define ELOG_TYPE_MI_HR                   0xb5

/* ChromeOS diagnostics-related events */
#define ELOG_TYPE_CROS_DIAGNOSTICS        0xb6
#define  ELOG_CROS_LAUNCH_DIAGNOSTICS      0x01

struct elog_event_extended_event {
	uint8_t event_type;
	uint32_t event_complement;
} __packed;

/*
 * Firmware boot related information retrieved from vboot and store as
 * per `union vb2_fw_boot_info` data structure.
 */
#define ELOG_TYPE_FW_VBOOT_INFO        0xb7

/* Only the 7-LSB are used for size */
#define ELOG_MAX_EVENT_SIZE                    0x7F

enum cb_err elog_verify_header(const struct elog_header *header);
const struct event_header *elog_get_next_event(const struct event_header *event);
const void *event_get_data(const struct event_header *event);
void elog_fill_timestamp(struct event_header *event, uint8_t sec, uint8_t min,
			 uint8_t hour, uint8_t mday, uint8_t mon, uint16_t year);
/* Update the checksum at the last byte. */
void elog_update_checksum(struct event_header *event, uint8_t checksum);
/* Simple byte checksum for events. */
uint8_t elog_checksum_event(const struct event_header *event);

#endif  /* _COMMONLIB_BSD_ELOG_H_ */
