/* SPDX-License-Identifier: BSD-3-Clause */

#include "eventlog.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include <commonlib/console/post_codes.h>
#include <commonlib/bsd/elog.h>
#include <vb2_api.h>

#include "common.h"
#include "valstr.h"

#define PATH_PCI_BUS_SHIFT		8
#define PATH_PCI_BUS_MASK		0xff
#define PATH_PCI_DEV_SHIFT		3
#define PATH_PCI_DEV_MASK		0x1f
#define PATH_PCI_FN_SHIFT		0
#define PATH_PCI_FN_MASK		0x03
#define PATH_I2C_MODE10BIT_SHIFT	8
#define PATH_I2C_MODE10BIT_MASK		0xff
#define PATH_I2C_ADDRESS_MASK		0xff

/* When true, then the separator is not printed */
static int eventlog_printf_ignore_separator_once = 1;

static void eventlog_printf(const char *format, ...)
{
	va_list args;

	// Separator for each field
	if (eventlog_printf_ignore_separator_once)
		eventlog_printf_ignore_separator_once = 0;
	else
		fprintf(stdout, " | ");

	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

/*
 * eventlog_print_timestamp - forms the key-value pair for event timestamp
 *
 * @entry:  the smbios log entry to get the data information
 *
 * Forms the key-value description pair for the event timestamp.
 */
static void eventlog_print_timestamp(const struct event_header *event)
{
	const char *tm_format = "%y-%m-%d%t%H:%M:%S";
	char tm_string[40];
	struct tm tm;
	time_t time;

	memset(&tm, 0, sizeof(tm));

	/* Time is in "hexa". Convert it to decimal, and then convert it to "tm" struct */
	snprintf(tm_string, sizeof(tm_string), "%02x-%02x-%02x %02x:%02x:%02x", event->year,
		 event->month, event->day, event->hour, event->minute, event->second);

	if (strptime(tm_string, tm_format, &tm) == NULL) {
		/* Backup in case string could not be parsed. Timezone not included */
		eventlog_printf("%02d%02x-%02x-%02x %02x:%02x:%02x",
				(event->year > 0x80 && event->year < 0x99) ? 19 : 20,
				event->year, event->month, event->day, event->hour,
				event->minute, event->second);
		return;
	}

	/* Set DST flag to -1 to indicate "not available" and let
	 * system determine if DST is on based on date */
	tm.tm_isdst = -1;

	time = mktime(&tm);
	time += tm.tm_gmtoff; /* force adjust for timezone */

	strftime(tm_string, sizeof(tm_string), "%Y-%m-%d %H:%M:%S", localtime(&time));

	eventlog_printf("%s", tm_string);
}


/*
 * eventlog_print_type - print the type of the entry
 *
 * @entry:  the smbios log entry to get type information
 *
 */
static void eventlog_print_type(const struct event_header *event)
{
	const char *type;
	static const struct valstr elog_event_types[] = {
		/* SMBIOS Event Log types, SMBIOSv2.4 section 3.3.16.1 */
		{ELOG_TYPE_UNDEFINED_EVENT, "Reserved"},
		{ELOG_TYPE_SINGLE_BIT_ECC_MEM_ERR, "Single-bit ECC memory error"},
		{ELOG_TYPE_MULTI_BIT_ECC_MEM_ERR, "Multi-bit ECC memory error"},
		{ELOG_TYPE_MEM_PARITY_ERR, "Parity memory error"},
		{ELOG_TYPE_BUS_TIMEOUT, "Bus timeout"},
		{ELOG_TYPE_IO_CHECK, "I/O channel check"},
		{ELOG_TYPE_SW_NMI, "Software NMI"},
		{ELOG_TYPE_POST_MEM_RESIZE, "POST memory resize"},
		{ELOG_TYPE_POST_ERR, "POST error"},
		{ELOG_TYPE_PCI_PERR, "PCI parity error"},
		{ELOG_TYPE_PCI_SERR, "PCI system error"},
		{ELOG_TYPE_CPU_FAIL, "CPU failure"},
		{ELOG_TYPE_EISA_TIMEOUT, "EISA failsafe timer timeout"},
		{ELOG_TYPE_CORRECTABLE_MEMLOG_DIS, "Correctable memory log disabled"},
		{ELOG_TYPE_LOG_DISABLED, "Logging disabled, too many errors"},
		{ELOG_TYPE_UNDEFINED_EVENT2, "Reserved"},
		{ELOG_TYPE_SYS_LIMIT_EXCEED, "System limit exceeded"},
		{ELOG_TYPE_ASYNC_HW_TIMER_EXPIRED, "Hardware watchdog reset"},
		{ELOG_TYPE_SYS_CONFIG_INFO, "System configuration information"},
		{ELOG_TYPE_HDD_INFO, "Hard-disk information"},
		{ELOG_TYPE_SYS_RECONFIG, "System reconfigured"},
		{ELOG_TYPE_CPU_ERROR, "Uncorrectable CPU-complex error"},
		{ELOG_TYPE_LOG_CLEAR, "Log area cleared"},
		{ELOG_TYPE_BOOT, "System boot"},

		/* Extended events defined by OEMs */
		{ELOG_TYPE_OS_EVENT, "Kernel Event"},
		{ELOG_TYPE_OS_BOOT, "OS Boot"},
		{ELOG_TYPE_EC_EVENT, "EC Event"},
		{ELOG_TYPE_POWER_FAIL, "Power Fail"},
		{ELOG_TYPE_SUS_POWER_FAIL, "SUS Power Fail"},
		{ELOG_TYPE_PWROK_FAIL, "PWROK Fail"},
		{ELOG_TYPE_SYS_PWROK_FAIL, "SYS PWROK Fail"},
		{ELOG_TYPE_POWER_ON, "Power On"},
		{ELOG_TYPE_POWER_BUTTON, "Power Button"},
		{ELOG_TYPE_POWER_BUTTON_OVERRIDE, "Power Button Override"},
		{ELOG_TYPE_RESET_BUTTON, "Reset Button"},
		{ELOG_TYPE_SYSTEM_RESET, "System Reset"},
		{ELOG_TYPE_RTC_RESET, "RTC Reset"},
		{ELOG_TYPE_TCO_RESET, "TCO Reset"},
		{ELOG_TYPE_ACPI_ENTER, "ACPI Enter"},
		{ELOG_TYPE_ACPI_WAKE, "ACPI Wake"},
		{ELOG_TYPE_ACPI_DEEP_WAKE, "ACPI Wake"},
		{ELOG_TYPE_S0IX_ENTER, "S0ix Enter"},
		{ELOG_TYPE_S0IX_EXIT, "S0ix Exit"},
		{ELOG_TYPE_WAKE_SOURCE, "Wake Source"},
		{ELOG_TYPE_CROS_DEVELOPER_MODE, "ChromeOS Developer Mode"},
		{ELOG_TYPE_CROS_RECOVERY_MODE, "ChromeOS Recovery Mode"},
		{ELOG_TYPE_MANAGEMENT_ENGINE, "Management Engine"},
		{ELOG_TYPE_MANAGEMENT_ENGINE_EXT, "Management Engine Extra"},
		{ELOG_TYPE_LAST_POST_CODE, "Last post code in previous boot"},
		{ELOG_TYPE_POST_EXTRA, "Extra info from previous boot"},
		{ELOG_TYPE_EC_SHUTDOWN, "EC Shutdown"},
		{ELOG_TYPE_SLEEP, "Sleep"},
		{ELOG_TYPE_WAKE, "Wake"},
		{ELOG_TYPE_FW_WAKE, "FW Wake"},
		{ELOG_TYPE_MEM_CACHE_UPDATE, "Memory Cache Update"},
		{ELOG_TYPE_THERM_TRIP, "CPU Thermal Trip"},
		{ELOG_TYPE_CR50_UPDATE, "cr50 Update Reset"},
		{ELOG_TYPE_CR50_NEED_RESET, "cr50 Reset Required"},
		{ELOG_TYPE_EC_DEVICE_EVENT, "EC Device"},
		{ELOG_TYPE_EXTENDED_EVENT, "Extended Event"},
		{ELOG_TYPE_CROS_DIAGNOSTICS, "Diagnostics Mode"},
		{ELOG_TYPE_FW_VBOOT_INFO, "Firmware vboot info"},

		{ELOG_TYPE_EOL, "End of log"},
	};

	/* Passing NULL as default, because we want to print the event->type if it fails */
	type = val2str_default(event->type, elog_event_types, NULL);

	if (type == NULL) {
		/* Indicate unknown type in value pair */
		eventlog_printf("Unknown");
		eventlog_printf("0x%02x", event->type);
		return;
	}

	eventlog_printf("%s", type);
}

/*
 * CMOS Extra log format:
 * [31:24] = Extra Log Type
 * [23:0]  = Extra Log Data
 *
 * If Extra Log Type is 0x01 then Data is Device Path
 * [23:16] = Device Type
 * [15:0]  = Encoded Device Path
 */
static int eventlog_print_post_extra(uint32_t extra)
{
	static const struct valstr path_type_values[] = {
		{ELOG_DEV_PATH_TYPE_NONE, "None"},
		{ELOG_DEV_PATH_TYPE_ROOT, "Root"},
		{ELOG_DEV_PATH_TYPE_PCI, "PCI"},
		{ELOG_DEV_PATH_TYPE_PNP, "PNP"},
		{ELOG_DEV_PATH_TYPE_I2C, "I2C"},
		{ELOG_DEV_PATH_TYPE_APIC, "APIC"},
		{ELOG_DEV_PATH_TYPE_DOMAIN, "DOMAIN"},
		{ELOG_DEV_PATH_TYPE_CPU_CLUSTER, "CPU Cluster"},
		{ELOG_DEV_PATH_TYPE_CPU, "CPU"},
		{ELOG_DEV_PATH_TYPE_CPU_BUS, "CPU Bus"},
		{ELOG_DEV_PATH_TYPE_IOAPIC, "IO-APIC"},
		{0, NULL},
	};
	const uint8_t type = (extra >> 16) & 0xff;

	/* Currently only know how to print device path */
	if ((extra >> 24) != ELOG_TYPE_POST_EXTRA_PATH) {
		eventlog_printf("0x%08x", extra);
		return 0;
	}

	eventlog_printf("%s", val2str(type, path_type_values));

	/* Handle different device path types */
	switch (type) {
	case ELOG_DEV_PATH_TYPE_PCI:
		eventlog_printf("%02x:%02x.%1x",
				(extra >> PATH_PCI_BUS_SHIFT) & PATH_PCI_BUS_MASK,
				(extra >> PATH_PCI_DEV_SHIFT) & PATH_PCI_DEV_MASK,
				(extra >> PATH_PCI_FN_SHIFT) & PATH_PCI_FN_MASK);
		break;
	case ELOG_DEV_PATH_TYPE_PNP:
	case ELOG_DEV_PATH_TYPE_I2C:
		eventlog_printf("%02x:%02x",
				(extra >> PATH_I2C_MODE10BIT_SHIFT) & PATH_I2C_MODE10BIT_MASK,
				extra & PATH_I2C_ADDRESS_MASK);
		break;
	case ELOG_DEV_PATH_TYPE_APIC:
	case ELOG_DEV_PATH_TYPE_DOMAIN:
	case ELOG_DEV_PATH_TYPE_CPU_CLUSTER:
	case ELOG_DEV_PATH_TYPE_CPU:
	case ELOG_DEV_PATH_TYPE_CPU_BUS:
	case ELOG_DEV_PATH_TYPE_IOAPIC:
		eventlog_printf("0x%04x", extra & 0xffff);
		break;
	}

	return 0;
}

/*
 * eventlog_print_data - print the data associated with the entry
 *
 * @event:  the smbios log entry to get the data information
 *
 * Returns 0 on failure, 1 on success.
 */
static int eventlog_print_data(const struct event_header *event)
{
	static const struct valstr os_events[] = {
		{ELOG_OS_EVENT_CLEAN, "Clean Shutdown"},
		{ELOG_OS_EVENT_NMIWDT, "NMI Watchdog"},
		{ELOG_OS_EVENT_PANIC, "Panic"},
		{ELOG_OS_EVENT_OOPS, "Oops"},
		{ELOG_OS_EVENT_DIE, "Die"},
		{ELOG_OS_EVENT_MCE, "MCE"},
		{ELOG_OS_EVENT_SOFTWDT, "Software Watchdog"},
		{ELOG_OS_EVENT_MBE, "Multi-bit Error"},
		{ELOG_OS_EVENT_TRIPLE, "Triple Fault"},
		{ELOG_OS_EVENT_THERMAL, "Critical Thermal Threshold"},
		{0, NULL},
	};
	static const struct valstr wake_source_types[] = {
		{ELOG_WAKE_SOURCE_PCIE, "PCI Express"},
		{ELOG_WAKE_SOURCE_PME, "PCI PME"},
		{ELOG_WAKE_SOURCE_PME_INTERNAL, "Internal PME"},
		{ELOG_WAKE_SOURCE_RTC, "RTC Alarm"},
		{ELOG_WAKE_SOURCE_GPE, "GPE #"},
		{ELOG_WAKE_SOURCE_SMBUS, "SMBALERT"},
		{ELOG_WAKE_SOURCE_PWRBTN, "Power Button"},
		{ELOG_WAKE_SOURCE_PME_HDA, "PME - HDA"},
		{ELOG_WAKE_SOURCE_PME_GBE, "PME - GBE"},
		{ELOG_WAKE_SOURCE_PME_EMMC, "PME - EMMC"},
		{ELOG_WAKE_SOURCE_PME_SDCARD, "PME - SDCARD"},
		{ELOG_WAKE_SOURCE_PME_PCIE1, "PME - PCIE1"},
		{ELOG_WAKE_SOURCE_PME_PCIE2, "PME - PCIE2"},
		{ELOG_WAKE_SOURCE_PME_PCIE3, "PME - PCIE3"},
		{ELOG_WAKE_SOURCE_PME_PCIE4, "PME - PCIE4"},
		{ELOG_WAKE_SOURCE_PME_PCIE5, "PME - PCIE5"},
		{ELOG_WAKE_SOURCE_PME_PCIE6, "PME - PCIE6"},
		{ELOG_WAKE_SOURCE_PME_PCIE7, "PME - PCIE7"},
		{ELOG_WAKE_SOURCE_PME_PCIE8, "PME - PCIE8"},
		{ELOG_WAKE_SOURCE_PME_PCIE9, "PME - PCIE9"},
		{ELOG_WAKE_SOURCE_PME_PCIE10, "PME - PCIE10"},
		{ELOG_WAKE_SOURCE_PME_PCIE11, "PME - PCIE11"},
		{ELOG_WAKE_SOURCE_PME_PCIE12, "PME - PCIE12"},
		{ELOG_WAKE_SOURCE_PME_SATA, "PME - SATA"},
		{ELOG_WAKE_SOURCE_PME_CSE, "PME - CSE"},
		{ELOG_WAKE_SOURCE_PME_CSE2, "PME - CSE2"},
		{ELOG_WAKE_SOURCE_PME_CSE3, "PME - CSE"},
		{ELOG_WAKE_SOURCE_PME_XHCI, "PME - XHCI"},
		{ELOG_WAKE_SOURCE_PME_XDCI, "PME - XDCI"},
		{ELOG_WAKE_SOURCE_PME_XHCI_USB_2, "PME - XHCI (USB 2.0 port)"},
		{ELOG_WAKE_SOURCE_PME_XHCI_USB_3, "PME - XHCI (USB 3.0 port)"},
		{ELOG_WAKE_SOURCE_PME_WIFI, "PME - WIFI"},
		{ELOG_WAKE_SOURCE_PME_PCIE13, "PME - PCIE13"},
		{ELOG_WAKE_SOURCE_PME_PCIE14, "PME - PCIE14"},
		{ELOG_WAKE_SOURCE_PME_PCIE15, "PME - PCIE15"},
		{ELOG_WAKE_SOURCE_PME_PCIE16, "PME - PCIE16"},
		{ELOG_WAKE_SOURCE_PME_PCIE17, "PME - PCIE17"},
		{ELOG_WAKE_SOURCE_PME_PCIE18, "PME - PCIE18"},
		{ELOG_WAKE_SOURCE_PME_PCIE19, "PME - PCIE19"},
		{ELOG_WAKE_SOURCE_PME_PCIE20, "PME - PCIE20"},
		{ELOG_WAKE_SOURCE_PME_PCIE21, "PME - PCIE21"},
		{ELOG_WAKE_SOURCE_PME_PCIE22, "PME - PCIE22"},
		{ELOG_WAKE_SOURCE_PME_PCIE23, "PME - PCIE23"},
		{ELOG_WAKE_SOURCE_PME_PCIE24, "PME - PCIE24"},
		{ELOG_WAKE_SOURCE_GPIO, " GPIO #"},
		{ELOG_WAKE_SOURCE_PME_TBT, "PME - Thunderbolt"},
		{ELOG_WAKE_SOURCE_PME_TCSS_XHCI, "PME - TCSS XHCI"},
		{ELOG_WAKE_SOURCE_PME_TCSS_XHCI, "PME - TCSS XDCI"},
		{ELOG_WAKE_SOURCE_PME_TCSS_XHCI, "PME - TCSS DMA"},
		{0, NULL},
	};
	static const struct valstr ec_event_types[] = {
		{EC_EVENT_LID_CLOSED, "Lid Closed"},
		{EC_EVENT_LID_OPEN, "Lid Open"},
		{EC_EVENT_POWER_BUTTON, "Power Button"},
		{EC_EVENT_AC_CONNECTED, "AC Connected"},
		{EC_EVENT_AC_DISCONNECTED, "AC Disconnected"},
		{EC_EVENT_BATTERY_LOW, "Battery Low"},
		{EC_EVENT_BATTERY_CRITICAL, "Battery Critical"},
		{EC_EVENT_BATTERY, "Battery"},
		{EC_EVENT_THERMAL_THRESHOLD, "Thermal Threshold"},
		{EC_EVENT_DEVICE_EVENT, "Device Event"},
		{EC_EVENT_THERMAL, "Thermal"},
		{EC_EVENT_USB_CHARGER, "USB Charger"},
		{EC_EVENT_KEY_PRESSED, "Key Pressed"},
		{EC_EVENT_INTERFACE_READY, "Host Interface Ready"},
		{EC_EVENT_KEYBOARD_RECOVERY, "Keyboard Recovery"},
		{EC_EVENT_THERMAL_SHUTDOWN, "Thermal Shutdown in previous boot"},
		{EC_EVENT_BATTERY_SHUTDOWN, "Battery Shutdown in previous boot"},
		{EC_EVENT_THROTTLE_START, "Throttle Requested"},
		{EC_EVENT_THROTTLE_STOP, "Throttle Request Removed"},
		{EC_EVENT_HANG_DETECT, "Host Event Hang"},
		{EC_EVENT_HANG_REBOOT, "Host Event Hang Reboot"},
		{EC_EVENT_PD_MCU, "PD MCU Request"},
		{EC_EVENT_BATTERY_STATUS, "Battery Status Request"},
		{EC_EVENT_PANIC, "Panic Reset in previous boot"},
		{EC_EVENT_KEYBOARD_FASTBOOT, "Keyboard Fastboot Recovery"},
		{EC_EVENT_RTC, "RTC"},
		{EC_EVENT_MKBP, "MKBP"},
		{EC_EVENT_USB_MUX, "USB MUX change"},
		{EC_EVENT_MODE_CHANGE, "Mode change"},
		{EC_EVENT_KEYBOARD_RECOVERY_HWREINIT,
		 "Keyboard Recovery Forced Hardware Reinit"},
		{EC_EVENT_EXTENDED, "Extended EC events"},
		{0, NULL},
	};
	static const struct valstr ec_device_event_types[] = {
		{ELOG_EC_DEVICE_EVENT_TRACKPAD, "Trackpad"},
		{ELOG_EC_DEVICE_EVENT_DSP, "DSP"},
		{ELOG_EC_DEVICE_EVENT_WIFI, "WiFi"},
		{0, NULL},
	};

	static const struct valstr cros_recovery_reasons[] = {
		{VB2_RECOVERY_LEGACY, "Legacy Utility"},
		{VB2_RECOVERY_RO_MANUAL, "Recovery Button Pressed"},
		{VB2_RECOVERY_RO_INVALID_RW, "RW Failed Signature Check"},
		{VB2_RECOVERY_DEPRECATED_RO_S3_RESUME, "S3 Resume Failed"},
		{VB2_RECOVERY_DEPRECATED_RO_TPM_ERROR, "TPM Error in RO Firmware"},
		{VB2_RECOVERY_RO_SHARED_DATA, "Shared Data Error in RO Firmware"},
		{VB2_RECOVERY_DEPRECATED_RO_TEST_S3, "Test Error from S3 Resume()"},
		{VB2_RECOVERY_DEPRECATED_RO_TEST_LF, "Test Error from LoadFirmware()"},
		{VB2_RECOVERY_DEPRECATED_RO_TEST_LFS, "Test Error from LoadFirmwareSetup()"},
		{VB2_RECOVERY_DEPRECATED_RW_NOT_DONE, "RW firmware check not done"},
		{VB2_RECOVERY_DEPRECATED_RW_DEV_FLAG_MISMATCH,
		 "RW firmware developer flag mismatch"},
		{VB2_RECOVERY_DEPRECATED_RW_REC_FLAG_MISMATCH,
		 "RW firmware recovery flash mismatch"},
		{VB2_RECOVERY_FW_KEYBLOCK, "RW firmware unable to verify keyblock"},
		{VB2_RECOVERY_FW_KEY_ROLLBACK, "RW firmware key version rollback detected"},
		{VB2_RECOVERY_DEPRECATED_RW_DATA_KEY_PARSE,
		 "RW firmware unable to parse data key"},
		{VB2_RECOVERY_FW_PREAMBLE, "RW firmware unable to verify preamble"},
		{VB2_RECOVERY_FW_ROLLBACK, "RW firmware version rollback detected"},
		{VB2_RECOVERY_DEPRECATED_FW_HEADER_VALID, "RW firmware header is valid"},
		{VB2_RECOVERY_DEPRECATED_FW_GET_FW_BODY,
		 "RW firmware unable to get firmware body"},
		{VB2_RECOVERY_DEPRECATED_FW_HASH_WRONG_SIZE, "RW firmware hash is wrong size"},
		{VB2_RECOVERY_FW_BODY, "RW firmware unable to verify firmware body"},
		{VB2_RECOVERY_DEPRECATED_FW_VALID, "RW firmware is valid"},
		{VB2_RECOVERY_DEPRECATED_FW_NO_RO_NORMAL,
		 "RW firmware read-only normal path is not supported"},
		{VB2_RECOVERY_RO_FIRMWARE, "Firmware Boot Failure"},
		{VB2_RECOVERY_RO_TPM_REBOOT, "Recovery Mode TPM Reboot"},
		{VB2_RECOVERY_EC_SOFTWARE_SYNC, "EC Software Sync Error"},
		{VB2_RECOVERY_EC_UNKNOWN_IMAGE, "Unable to determine active EC image"},
		{VB2_RECOVERY_DEPRECATED_EC_HASH,
		 "EC software sync error obtaining EC image hash"},
		{VB2_RECOVERY_DEPRECATED_EC_EXPECTED_IMAGE,
		 "EC software sync error obtaining expected EC image from BIOS"},
		{VB2_RECOVERY_EC_UPDATE, "EC software sync error updating EC"},
		{VB2_RECOVERY_EC_JUMP_RW, "EC software sync unable to jump to EC-RW"},
		{VB2_RECOVERY_EC_PROTECT, "EC software sync protection error"},
		{VB2_RECOVERY_EC_EXPECTED_HASH,
		 "EC software sync error obtaining expected EC hash from BIOS"},
		{VB2_RECOVERY_DEPRECATED_EC_HASH_MISMATCH,
		 "EC software sync error comparing expected EC hash and image"},
		{VB2_RECOVERY_SECDATA_FIRMWARE_INIT, "Secure NVRAM (TPM) initialization error"},
		{VB2_RECOVERY_GBB_HEADER, "Error parsing GBB header"},
		{VB2_RECOVERY_TPM_CLEAR_OWNER, "Error trying to clear TPM owner"},
		{VB2_RECOVERY_DEV_SWITCH, "Error reading or updating developer switch"},
		{VB2_RECOVERY_FW_SLOT, "Error selecting RW firmware slot"},
		{VB2_RECOVERY_AUXFW_UPDATE, "Error updating AUX firmware"},
		{VB2_RECOVERY_INTEL_CSE_LITE_SKU, "Intel CSE Lite SKU fimrware failure"},
		{VB2_RECOVERY_RO_UNSPECIFIED, "Unknown Error in RO Firmware"},
		{VB2_RECOVERY_DEPRECATED_RW_DEV_SCREEN, "User Requested from Developer Screen"},
		{VB2_RECOVERY_DEPRECATED_RW_NO_OS, "No OS Kernel Detected"},
		{VB2_RECOVERY_RW_INVALID_OS, "OS kernel or rootfs failed signature check"},
		{VB2_RECOVERY_DEPRECATED_RW_TPM_ERROR, "TPM Error in RW Firmware"},
		{VB2_RECOVERY_DEPRECATED_RW_DEV_MISMATCH, "RW Dev Firmware but not Dev Mode"},
		{VB2_RECOVERY_RW_SHARED_DATA, "Shared Data Error in RW Firmware"},
		{VB2_RECOVERY_DEPRECATED_RW_TEST_LK, "Test Error from LoadKernel()"},
		{VB2_RECOVERY_DEPRECATED_RW_NO_DISK, "No Bootable Disk Found"},
		{VB2_RECOVERY_TPM_E_FAIL, "TPM_E_FAIL or TPM_E_FAILEDSELFTEST"},
		{VB2_RECOVERY_RO_TPM_S_ERROR, "TPM setup error in read-only firmware"},
		{VB2_RECOVERY_RO_TPM_W_ERROR, "TPM write error in read-only firmware"},
		{VB2_RECOVERY_RO_TPM_L_ERROR, "TPM lock error in read-only firmware"},
		{VB2_RECOVERY_RO_TPM_U_ERROR, "TPM update error in read-only firmware"},
		{VB2_RECOVERY_RW_TPM_R_ERROR, "TPM read error in rewritable firmware"},
		{VB2_RECOVERY_RW_TPM_W_ERROR, "TPM write error in rewritable firmware"},
		{VB2_RECOVERY_RW_TPM_L_ERROR, "TPM lock error in rewritable firmware"},
		{VB2_RECOVERY_EC_HASH_FAILED, "EC software sync unable to get EC image hash"},
		{VB2_RECOVERY_EC_HASH_SIZE, "EC software sync invalid image hash size"},
		{VB2_RECOVERY_LK_UNSPECIFIED, "Unspecified error while trying to load kernel"},
		{VB2_RECOVERY_RW_NO_DISK, "No bootable storage device in system"},
		{VB2_RECOVERY_RW_NO_KERNEL, "No bootable kernel found on disk"},
		{VB2_RECOVERY_DEPRECATED_RW_BCB_ERROR, "BCB partition error on disk"},
		{VB2_RECOVERY_SECDATA_KERNEL_INIT,
		 "Kernel secure NVRAM (TPM) initialization error"},
		{VB2_RECOVERY_DEPRECATED_FW_FASTBOOT, "Fastboot-mode requested in firmware"},
		{VB2_RECOVERY_RO_TPM_REC_HASH_L_ERROR,
		 "Recovery hash space lock error in RO firmware"},
		{VB2_RECOVERY_TPM_DISABLE_FAILED,
		 "Failed to disable TPM before running untrusted code"},
		{VB2_RECOVERY_ALTFW_HASH_MISMATCH,
		 "Verification of alternative firmware payload failed"},
		{VB2_RECOVERY_SECDATA_FWMP_INIT, "FWMP secure data initialization failed"},
		{VB2_RECOVERY_CR50_BOOT_MODE, "Failed to get boot mode from TPM/Cr50"},
		{VB2_RECOVERY_ESCAPE_NO_BOOT,
		 "Attempt to escape from NO_BOOT mode was detected"},
		{VB2_RECOVERY_RW_UNSPECIFIED, "Unspecified/unknown error in RW firmware"},
		{VB2_RECOVERY_DEPRECATED_KE_DM_VERITY, "DM-verity error"},
		{VB2_RECOVERY_DEPRECATED_KE_UNSPECIFIED, "Unspecified/unknown error in kernel"},
		{VB2_RECOVERY_US_TEST, "Recovery mode test from user-mode"},
		{VB2_RECOVERY_DEPRECATED_BCB_USER_MODE, "User-mode requested recovery via BCB"},
		{VB2_RECOVERY_DEPRECATED_US_FASTBOOT, "User-mode requested fastboot mode"},
		{VB2_RECOVERY_TRAIN_AND_REBOOT,
		 "User requested recovery for training memory and rebooting"},
		{VB2_RECOVERY_US_UNSPECIFIED, "Unknown Error in User Mode"},
		{0, NULL},
	};
	static const struct valstr me_path_types[] = {
		{ELOG_ME_PATH_NORMAL, "Normal"},
		{ELOG_ME_PATH_NORMAL, "S3 Wake"},
		{ELOG_ME_PATH_ERROR, "Error"},
		{ELOG_ME_PATH_RECOVERY, "Recovery"},
		{ELOG_ME_PATH_DISABLED, "Disabled"},
		{ELOG_ME_PATH_FW_UPDATE, "Firmware Update"},
		{0, NULL},
	};
	static const struct valstr coreboot_post_codes[] = {
		{POST_RESET_VECTOR_CORRECT, "Reset Vector Correct"},
		{POST_ENTER_PROTECTED_MODE, "Enter Protected Mode"},
		{POST_PREPARE_RAMSTAGE, "Prepare RAM stage"},
		{POST_ENTRY_C_START, "RAM stage Start"},
		{POST_MEM_PREINIT_PREP_START, "Preparing memory init params"},
		{POST_MEM_PREINIT_PREP_END, "Memory init param preparation complete"},
		{POST_CONSOLE_READY, "Console is ready"},
		{POST_CONSOLE_BOOT_MSG, "Console Boot Message"},
		{POST_ENABLING_CACHE, "Before Enabling Cache"},
		{POST_PRE_HARDWAREMAIN, "Before Hardware Main"},
		{POST_ENTRY_HARDWAREMAIN, "First call in Hardware Main"},
		{POST_BS_PRE_DEVICE, "Before Device Probe"},
		{POST_BS_DEV_INIT_CHIPS, "Initialize Chips"},
		{POST_BS_DEV_ENUMERATE, "Device Enumerate"},
		{POST_BS_DEV_RESOURCES, "Device Resource Allocation"},
		{POST_BS_DEV_ENABLE, "Device Enable"},
		{POST_BS_DEV_INIT, "Device Initialize"},
		{POST_BS_POST_DEVICE, "After Device Probe"},
		{POST_BS_OS_RESUME_CHECK, "OS Resume Check"},
		{POST_BS_OS_RESUME, "OS Resume"},
		{POST_BS_WRITE_TABLES, "Write Tables"},
		{POST_BS_PAYLOAD_LOAD, "Load Payload"},
		{POST_BS_PAYLOAD_BOOT, "Boot Payload"},
		{POST_FSP_NOTIFY_BEFORE_END_OF_FIRMWARE, "FSP Notify Before End of Firmware"},
		{POST_FSP_NOTIFY_AFTER_END_OF_FIRMWARE, "FSP Notify After End of Firmware"},
		{POST_FSP_TEMP_RAM_INIT, "FSP-T Enter"},
		{POST_FSP_TEMP_RAM_EXIT, "FSP-T Exit"},
		{POST_FSP_MEMORY_INIT, "FSP-M Enter"},
		{POST_FSP_SILICON_INIT, "FSP-S Enter"},
		{POST_FSP_NOTIFY_BEFORE_ENUMERATE, "FSP Notify Before Enumerate"},
		{POST_FSP_NOTIFY_BEFORE_FINALIZE, "FSP Notify Before Finalize"},
		{POST_OS_ENTER_PTS, "ACPI _PTS Method"},
		{POST_OS_ENTER_WAKE, "ACPI _WAK Method"},
		{POST_FSP_MEMORY_EXIT, "FSP-M Exit"},
		{POST_FSP_SILICON_EXIT, "FSP-S Exit"},
		{POST_FSP_MULTI_PHASE_SI_INIT_ENTRY, "FSP-S Init Enter"},
		{POST_FSP_MULTI_PHASE_SI_INIT_EXIT, "FPS-S Init Exit"},
		{POST_FSP_NOTIFY_AFTER_ENUMERATE, "FSP Notify After Enumerate"},
		{POST_FSP_NOTIFY_AFTER_FINALIZE, "FSP Notify After Finalize"},
		{POST_INVALID_ROM, "Invalid ROM"},
		{POST_INVALID_CBFS, "Invalid CBFS"},
		{POST_INVALID_VENDOR_BINARY, "Invalid Vendor Binary"},
		{POST_RAM_FAILURE, "RAM Failure"},
		{POST_HW_INIT_FAILURE, "Hardware Init Failure"},
		{POST_VIDEO_FAILURE, "Video Failure"},
		{POST_TPM_FAILURE, "TPM Failure"},
		{POST_DEAD_CODE, "Dead Code"},
		{POST_RESUME_FAILURE, "Resume Failure"},
		{POST_JUMPING_TO_PAYLOAD, "Before Jump to Payload"},
		{POST_ENTER_ELF_BOOT, "Before ELF Boot"},
		{POST_OS_RESUME, "Before OS Resume"},
		{POST_OS_BOOT, "Before OS Boot"},
		{POST_DIE, "coreboot Dead"},
		{0, NULL},
	};
	static const struct valstr mem_cache_slots[] = {
		{ELOG_MEM_CACHE_UPDATE_SLOT_NORMAL, "Normal"},
		{ELOG_MEM_CACHE_UPDATE_SLOT_RECOVERY, "Recovery"},
		{ELOG_MEM_CACHE_UPDATE_SLOT_VARIABLE, "Variable"},
		{0, NULL},
	};
	static const struct valstr mem_cache_statuses[] = {
		{ELOG_MEM_CACHE_UPDATE_STATUS_SUCCESS, "Success"},
		{ELOG_MEM_CACHE_UPDATE_STATUS_FAIL, "Fail"},
		{0, NULL},
	};

	static const struct valstr extended_event_subtypes[] = {
		{ELOG_SLEEP_PENDING_PM1_WAKE, "S3 failed due to pending wake event, PM1"},
		{ELOG_SLEEP_PENDING_GPE0_WAKE, "S3 failed due to pending wake event, GPE0"},
		{0, NULL},
	};

	static const struct valstr cros_diagnostics_types[] = {
		{ELOG_CROS_LAUNCH_DIAGNOSTICS, "Launch Diagnostics"},
		{0, NULL},
	};

	switch (event->type) {
	case ELOG_TYPE_LOG_CLEAR: {
		const uint16_t *bytes = event_get_data(event);
		eventlog_printf("%u", *bytes);
		break;
	}

	case ELOG_TYPE_BOOT: {
		const uint32_t *count = event_get_data(event);
		eventlog_printf("%u", *count);
		break;
	}
	case ELOG_TYPE_LAST_POST_CODE: {
		const uint16_t *code = event_get_data(event);
		eventlog_printf("0x%02x", *code);
		eventlog_printf("%s", val2str(*code, coreboot_post_codes));
		break;
	}
	case ELOG_TYPE_POST_EXTRA: {
		const uint32_t *extra = event_get_data(event);
		eventlog_print_post_extra(*extra);
		break;
	}
	case ELOG_TYPE_OS_EVENT: {
		const uint32_t *osevent = event_get_data(event);
		eventlog_printf("%s", val2str(*osevent, os_events));
		break;
	}
	case ELOG_TYPE_ACPI_ENTER:
	case ELOG_TYPE_ACPI_WAKE: {
		const uint8_t *state = event_get_data(event);
		eventlog_printf("S%u", *state);
		break;
	}
	case ELOG_TYPE_ACPI_DEEP_WAKE: {
		const uint8_t *state = event_get_data(event);
		eventlog_printf("Deep S%u", *state);
		break;
	}
	case ELOG_TYPE_WAKE_SOURCE: {
		const struct elog_event_data_wake *wake_source;
		wake_source = event_get_data(event);
		eventlog_printf("%s", val2str(wake_source->source, wake_source_types));
		eventlog_printf("%u", wake_source->instance);
		break;
	}
	case ELOG_TYPE_EC_EVENT: {
		const uint8_t *ec_event = event_get_data(event);
		eventlog_printf("%s", val2str(*ec_event, ec_event_types));
		break;
	}
	case ELOG_TYPE_EC_DEVICE_EVENT: {
		const uint8_t *dev_event = event_get_data(event);
		eventlog_printf("%s", val2str(*dev_event, ec_device_event_types));
		break;
	}
	case ELOG_TYPE_CROS_RECOVERY_MODE: {
		const uint8_t *reason = event_get_data(event);
		eventlog_printf("%s", val2str(*reason, cros_recovery_reasons));
		eventlog_printf("0x%02x", *reason);
		break;
	}
	case ELOG_TYPE_MANAGEMENT_ENGINE: {
		const uint8_t *path = event_get_data(event);
		eventlog_printf("%s", val2str(*path, me_path_types));
		break;
	}
	case ELOG_TYPE_MEM_CACHE_UPDATE: {
		const struct elog_event_mem_cache_update *update;
		update = event_get_data(event);
		eventlog_printf("%s", val2str(update->slot, mem_cache_slots));
		eventlog_printf("%s", val2str(update->status, mem_cache_statuses));
		break;
	}
	case ELOG_TYPE_EXTENDED_EVENT: {
		const struct elog_event_extended_event *ext_event;
		ext_event = event_get_data(event);
		eventlog_printf("%s", val2str(ext_event->event_type, extended_event_subtypes));
		eventlog_printf("0x%X", ext_event->event_complement);
		break;
	}
	case ELOG_TYPE_CROS_DIAGNOSTICS: {
		const uint8_t *type = event_get_data(event);
		eventlog_printf("%s", val2str(*type, cros_diagnostics_types));
		break;
	}
	case ELOG_TYPE_FW_VBOOT_INFO: {
		const union vb2_fw_boot_info *info = event_get_data(event);

		eventlog_printf("boot_mode=%s", vb2_boot_mode_string(info->boot_mode));

		if (info->boot_mode == VB2_BOOT_MODE_BROKEN_SCREEN ||
		    info->boot_mode == VB2_BOOT_MODE_MANUAL_RECOVERY)
			eventlog_printf("recovery_reason=%#x/%#x (%s)",
				  info->recovery_reason, info->recovery_subcode,
				  vb2_get_recovery_reason_string(info->recovery_reason));

		eventlog_printf("fw_tried=%s", vb2_slot_string(info->slot));
		eventlog_printf("fw_try_count=%d", info->tries);
		eventlog_printf("fw_prev_tried=%s", vb2_slot_string(info->prev_slot));
		eventlog_printf("fw_prev_result=%s", vb2_result_string(info->prev_result));
		break;
	}
	default:
		break;
	}

	return 0;
}

void eventlog_print_event(const struct event_header *event, int count)
{
	/* Ignore the printf separator at the beginning and end of each line */
	eventlog_printf_ignore_separator_once = 1;

	eventlog_printf("%d", count);
	eventlog_print_timestamp(event);
	eventlog_print_type(event);
	eventlog_print_data(event);

	/* End of line, after printing each event */
	eventlog_printf_ignore_separator_once = 1;
	eventlog_printf("\n");
}

/*
 * Initializes the eventlog header with the given type and data,
 * and calculates the checksum.
 * buffer_get() points to the event to be initialized.
 * On success it returns 1, otherwise 0.
 */
int eventlog_init_event(const struct buffer *buf, uint8_t type,
			const void *data, int data_size)
{
	struct event_header *event;
	time_t secs = time(NULL);
	struct tm tm;

	/* Must have at least size for data + checksum byte */
	if (buffer_size(buf) < (size_t)data_size + 1)
		return 0;

	event = buffer_get(buf);

	event->type = type;
	gmtime_r(&secs, &tm);
	/* Month should be +1, since gmtime uses 0 as first month */
	elog_fill_timestamp(event, tm.tm_sec, tm.tm_min, tm.tm_hour,
			    tm.tm_mday, tm.tm_mon + 1, tm.tm_year);

	if (data && data_size) {
		uint32_t *ptr = (uint32_t *)&event[1];
		memcpy(ptr, data, data_size);
	}

	/* Header + data + checksum */
	event->length = sizeof(*event) + data_size + 1;

	/* Zero the checksum byte and then compute checksum */
	elog_update_checksum(event, 0);
	elog_update_checksum(event, -(elog_checksum_event(event)));

	return 1;
}
