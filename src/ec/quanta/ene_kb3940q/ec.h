/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * EC communication interface for QUANTA EnE KB3940Q Embedded Controller.
 */

#ifndef _EC_QUANTA_ENE_KB3940Q_EC_H
#define _EC_QUANTA_ENE_KB3940Q_EC_H

#define EC_IO 0x380 /* Mainboard specific. Could be Kconfig option */
#define EC_IO_HIGH EC_IO + 1
#define EC_IO_LOW  EC_IO + 2
#define EC_IO_DATA EC_IO + 3

// 60h/64h Command Interface
#define KBD_DATA	0x60
#define KBD_COMMAND	0x64
#define KBD_STATUS	0x64
#define   KBD_IBF	(1 << 1) // 1: input buffer full (data ready for ec)
#define   KBD_OBF	(1 << 0) // 1: output buffer full (data ready for host)

// 62h/66h Command Interface
#define EC_DATA	0x62
#define EC_COMMAND	0x66
#define EC_SC	0x66

/* Wait 400ms for keyboard controller answers */
#define KBC_TIMEOUT_IN_MS 400

u8 ec_kbc_read_ob(void);
void ec_kbc_write_cmd(u8 cmd);
void ec_kbc_write_ib(u8 data);
u8 ec_read_ob(void);
void ec_write_cmd(u8 cmd);
void ec_write_ib(u8 data);

u8 ec_mem_read(u8 addr);
void ec_mem_write(u8 addr, u8 data);

/*****************************************************************************
 * EC Internal memory
 */

#define EC_BAT_DCAP_LO			0x40
#define EC_BAT_DCAP_HI			0x41
#define EC_BAT_DVOLT_LO			0x42
#define EC_BAT_DVOLT_HI			0x43
#define EC_BAT_FULL_CAP_LO		0x44
#define EC_BAT_FULL_CAP_HI		0x45
#define EC_BAT_RATE_LO			0x46
#define EC_BAT_RATE_HI			0x47
#define EC_BAT_RMC_LO			0x48
#define EC_BAT_RMC_HI			0x49
#define EC_BAT_VOLT_LO			0x4A
#define EC_BAT_VOLT_HI			0x4B
#define EC_BAT_CHRG_CURT_LO		0x4C
#define EC_BAT_CHRG_CURT_HI		0x4D
#define EC_BAT_CHRG_VOLT_LO		0x4E
#define EC_BAT_CHRG_VOLT_HI		0x4F
#define EC_BAT_TEMP_LO			0x50
#define EC_BAT_TEMP_HI			0x51
#define EC_BAT_SN_LO			0x52
#define EC_BAT_SN_HI			0x53
#define EC_BAT_RSOC_LO			0x54
#define EC_BAT_RSOC_HI			0x55
#define EC_BAT_STATUS_LO		0x56
#define EC_BAT_STATUS_HI		0x57
#define EC_BAT_CYCLE_COUNT_LO	0x58
#define EC_BAT_CYCLE_COUNT_HI	0x59
#define EC_BAT_MFG_DATE_LO		0x5A
#define EC_BAT_MFG_DATE_HI		0x5B
#define EC_BAT_CHEMISTRY0		0x60
#define EC_BAT_CHEMISTRY1		0x61
#define EC_BAT_CHEMISTRY2		0x62
#define EC_BAT_CHEMISTRY3		0x63
#define EC_BAT_DEVICE_NAME0		0x64
#define EC_BAT_DEVICE_NAME1		0x65
#define EC_BAT_DEVICE_NAME2		0x66
#define EC_BAT_DEVICE_NAME3		0x67
#define EC_BAT_DEVICE_NAME4		0x68
#define EC_BAT_DEVICE_NAME5		0x69
#define EC_BAT_DEVICE_NAME6		0x6A

#define EC_POWER_FLAG			0x70
#define   EC_PF_ADAPTER_IN		(1 << 0)
#define   EC_PF_ADAPTER_PIN		(1 << 1)
#define   EC_PF_BATT_IN			(1 << 2)
#define   EC_PF_BATT_DESTROY	(1 << 3)
#define   EC_PF_ACPI_MODE		(1 << 4)
#define   EC_PF_X86_BIOS		(1 << 6)
#define   EC_PF_COREBOOT		0
#define   EC_PF_PASSIVE_THERM	(1 << 7)

#define EC_CHARGER_STATUS		0x71
#define   EC_CHS_BAT_DISCHARGING	(1 << 0)
#define   EC_CHS_BAT_CHARGING		(1 << 1)
#define   EC_CHS_BAT_CRITICAL		(1 << 2)

#define EC_HW_GPI_STATUS			0x72
#define   EC_GPI_LID_STAT_BIT		0
#define   EC_GPI_RECOVERY_MODE_BIT	1
#define   EC_GPI_LID_OPEN			(1 << EC_GPI_LID_STAT_BIT)
#define   EC_GPI_RECOVERY_STATUS	(1 << EC_GPI_RECOVERY_MODE_BIT)

#define EC_GPIO_STATUS				0x73
#define   EC_GPIO_TP_LED_ENABLE		(1 << 0)
#define   EC_GPIO_TP_LED_STATUS		(1 << 1)

#define EC_CPU_TMP					0x78
#define EC_GPU_TMP					0x79
#define EC_LOCAL_TMP1				0x7A
#define EC_LOCAL_TMP2				0x7B
#define EC_FAN_TACH_LO				0x7C
#define EC_FAN_TACH_HI				0x7D
#define EC_FAN_DBG_RPM_LO			0x7E
#define EC_FAN_DBG_RPM_HI			0x7F

#define EC_KBID_REG			0x80
#define   EC_KBD_EN			0
#define   EC_KBD_JP			(1 << 1)
#define EC_CURR_PS			0x81
#define EC_MAX_PS			0x82

#define EC_EC_PSW			0x83
#define   EC_PSW_IKB		(1 << 0)
#define   EC_PSW_TP			(1 << 1)
#define   EC_PSW_LAN		(1 << 3)
#define   EC_PSW_RTC		(1 << 4)
#define   EC_PSW_USB		(1 << 5)

#define EC_WAKE_EVEN_TID	0x84
#define   EC_WID_IKB		(1 << 0)
#define   EC_WID_TP			(1 << 1)
#define   EC_WID_LID		(1 << 2)
#define   EC_WID_PWRSW		(1 << 7)

#define EC_CODE_STATE				0x85
#define   EC_COS_INITIAL_STAGE		0xBB
#define   EC_COS_EC_RO				0xC0
#define   EC_COS_EC_RW				0xC1

#define EC_FW_REASON_ID				0x86
#define   EC_FWR_NOT_RO				0x00
#define   EC_FWR_GPI_ASSERTED		0x01
#define   EC_FWR_HOTKEY_PRESSED		0x02
#define   EC_FWR_FIRMWARE_CORRUPT	0x03

#define EC_SHUTDOWN_REASON		0xB9
#define EC_FW_VER0			0xBA
#define EC_FW_VER1			0xBB
#define EC_FW_VER2			0xBC
#define EC_FW_VER3			0xBD
#define EC_FW_VER4			0xBE
#define EC_FW_VER5			0xBF
#define EC_SMBPTCL			0xC0
#define EC_SMBSTA			0xC1
#define EC_SMBADDR			0xC2
#define EC_SMBCMD			0xC3
#define EC_SMBDATA			0xC4
#define EC_SMBBCNT			0xE4

/*****************************************************************************
 * SMI / SCI event status
 */
#define Q_EVENT_LID_STATUS			0x06
#define Q_EVENT_WIFI_BUTTON			0x06
#define Q_EVENT_THERM_EVENT			0x08
#define Q_EVENT_PSTATE_DOWN			0x0E
#define Q_EVENT_PSTATE_UP			0x0F
#define Q_EVENT_AC_PLUGGED			0x10
#define Q_EVENT_AC_UNPLUGGED		0x11
#define Q_EVENT_BATTERY_PLUGGED		0x12
#define Q_EVENT_BATTERY_UNPLUGGED	0x13
#define Q_EVENT_BATTERY_STATUS		0x14

/*****************************************************************************
 * EC Commands
 */
#define EC_CMD_ENABLE_ACPI_MODE		0x71
#define EC_CMD_DISABLE_ACPI_MODE	0x72
#define EC_CMD_DISABLE_SMBUS_EVENT	0x73
#define EC_CMD_ENABLE_SMBUS_EVENT	0x74
#define EC_CMD_SYSTEM_RESET			0x78
#define EC_CMD_SYSTEM_SHUTDOWN		0x79
#define EC_CMD_RESET_FOR_FW_UPDATE	0x7D
#define EC_CMD_IDLE_FOR SPI_UPDATE	0x7E
#define EC_CMD_READ_RAM				0x80
#define EC_CMD_WRITE_RAM			0x81
#define EC_CMD_BURST_ENABLE			0x82
#define EC_CMD_BURST_DISABLE		0x83
#define EC_CMD_QUERY_EVENT			0x84

#endif /* _EC_QUANTA_ENE_KB3940Q_EC_H */
