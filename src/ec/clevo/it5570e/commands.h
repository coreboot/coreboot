/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_CLEVO_IT5570E_COMMANDS_H
#define EC_CLEVO_IT5570E_COMMANDS_H

#ifndef __ACPI__
#define ECRAM	CONFIG_EC_CLEVO_IT5570E_MEM_BASE

/* EC RAM fields and bits */
#define FANC				0x1c8
#define RINF				0x1db
#define   TP_TOGGLE_CTRLALTF9		(BIT(4) | BIT(2))
#define ECKS				0x1e2
#define   SWFN				3
//	####				0x1e6
#define   G3FG				2
#define   FOAC				6
#define KBBO				0x1e9
//	####				0x1eb
#define   DGPT				3
#define   APRD				7
#define PL2B				0x1f0
#define PL2T				0x1f2
#define TAUT				0x1f4
#define FCMD				0x1f8
#define FDAT				0x1f9
#define FBUF				0x1fa
#define FBF1				0x1fb
#define FBF2				0x1fc
#define FBF3				0x1fd
#endif // __ACPI__

/* EC commands */
#define ECCMD_NOP			0x00	/* dummy, triggers FCMDs */
#define ECCMD_ENABLE_ACPI_MODE		0x90
#define ECCMD_DISABLE_ACPI_MODE		0x91
#define ECCMD_READ_MODEL		0x92
#define ECCMD_READ_FW_VER		0x93
#define ECCMD_ENABLE_HOTKEYS		0x98
#define ECCMD_GET_DEVICES_STATE		0x9a
#define ECCMD_SET_INV_DEVICE_STATE	0x9c
#define   DEVICE_CAMERA			2
#define   DEVICE_STATE(state)		(!(state) << 7)
#define ECCMD_SET_BATLOW_ALARM		0x9d
#define ECCMD_SETUP_DEVICES		0xa8

/* FCMD commands */
#define FCMD_DEVICES			0xb8
#define	  FDAT_DEVICE_SET_INV_STATE	0xc2	/* inverted! en=0xc2|0, dis=0xc2|1 */
#define FCMD_KLED			0xca
#define   FDAT_KBLED_WHITE_SET_LEVEL	0x00
#define   FDAT_KBLED_WHITE_GET_LEVEL	0x01
#define FCMD_FLEXICHARGER		0xcb
#define FCMD_SET_KBLED_TIMEOUT		0xd4

#ifndef __ACPI__
enum camera_state {
	CAMERA_STATE_DISABLE,
	CAMERA_STATE_ENABLE,
	CAMERA_STATE_KEEP,
};

char *ec_read_model(void);
char *ec_read_fw_version(void);
void ec_set_acpi_mode(bool state);
void ec_set_aprd(void);
void ec_set_enter_g3_in_s4s5(bool state);
void ec_set_dgpu_present(bool state);
void ec_set_fn_win_swap(bool state);
void ec_set_ac_fan_always_on(bool state);
void ec_set_kbled_timeout(uint16_t timeout);
void ec_set_flexicharger(bool state, uint8_t start, uint8_t stop);
void ec_set_camera_boot_state(enum camera_state state);
void ec_set_tp_toggle_mode(uint8_t mode);
#endif // __ACPI__

#endif /* EC_CLEVO_IT5570E_COMMANDS_H */
