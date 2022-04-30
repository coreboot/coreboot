/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <superio/fintek/common/fan_control.h>
#include <amdblocks/lpc.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>

#define CPU_FAN		1
#define SYSTEM_FAN	2

/* Boundaries in celsius, sections in percent */
static u8 cpu_boudaries[FINTEK_BOUNDARIES_SIZE] = {
	80,
	65,
	50,
	35
};

static u8 system_boudaries[FINTEK_BOUNDARIES_SIZE] = {
	70,
	55,
	40,
	25
};

static u8 cpu_section[FINTEK_SECTIONS_SIZE] = {
	100,
	85,
	70,
	55,
	40
};

static u8 system_section[FINTEK_SECTIONS_SIZE] = {
	100,
	85,
	70,
	55,
	40
};

struct fintek_fan cpu_fan = {
	CPU_FAN,
	IGNORE_SENSOR,
	TEMP_SENSOR_DEFAULT,
	FAN_TEMP_TSI,
	FAN_TYPE_PWM_PUSH_PULL,
	FAN_MODE_DEFAULT,
	FAN_PWM_FREQ_23500,
	FAN_UP_RATE_10HZ,
	FAN_DOWN_RATE_10HZ,
	FAN_FOLLOW_INTERPOLATION,
	cpu_boudaries,
	cpu_section
};

struct fintek_fan system_fan = {
	SYSTEM_FAN,
	EXTERNAL_SENSOR2,
	TEMP_SENSOR_BJT,
	FAN_TEMP_EXTERNAL_2,
	FAN_TYPE_DAC_POWER,
	FAN_MODE_DEFAULT,
	FAN_PWM_FREQ_23500,
	FAN_UP_RATE_10HZ,
	FAN_DOWN_RATE_10HZ,
	FAN_FOLLOW_INTERPOLATION,
	system_boudaries,
	system_section
};

static void init_fan_control(void *unused)
{
	/* Open a LPC IO access to 0x0220-0x0227 */
	pci_or_config32(SOC_LPC_DEV, LPC_IO_PORT_DECODE_ENABLE, DECODE_ENABLE_SERIAL_PORT2);

	set_fan(&cpu_fan);
	set_fan(&system_fan);
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY, init_fan_control, NULL);
