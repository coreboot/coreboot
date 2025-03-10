/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/azalia_device.h>
#include <gpio.h>

const u32 cim_verb_data[] = {
	/* Realtek, ALC245 */
	0x10ec0245, /* Vendor ID */
	0x1558a741, /* Subsystem ID */
	30, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x1558a741),
	AZALIA_RESET(1),

	0x1271C30,     0x1271D01,     0x1271EA6,     0x1271F90,
	0x1371C00,     0x1371D00,     0x1371E00,     0x1371F40,
	0x1871CF0,     0x1871D11,     0x1871E11,     0x1871F41,
	0x1971CF0,     0x1971D11,     0x1971E11,     0x1971F41,
	0x1A71CF0,     0x1A71D11,     0x1A71E11,     0x1A71F41,
	0x1B71CF0,     0x1B71D11,     0x1B71E11,     0x1B71F41,
	0x1D71C2D,     0x1D71D9B,     0x1D71E68,     0x1D71F40,
	0x1E71CF0,     0x1E71D11,     0x1E71E11,     0x1E71F41,
	0x2171C20,     0x2171D10,     0x2171E21,     0x2171F04,
	0x5B50006,     0x5B40011,     0x205001A,     0x204810B,
	0x205004A,     0x2042010,     0x2050038,     0x2044909,
	0x5C50000,     0x5C43D82,     0x5C50000,     0x5C43D82,
	0x5350000,     0x534201A,     0x5350000,     0x534201A,
	0x535001D,     0x5340800,     0x535001E,     0x5340800,
	0x5350003,     0x5341EC4,     0x5350004,     0x5340000,
	0x5450000,     0x5442000,     0x545001D,     0x5440800,
	0x545001E,     0x5440800,     0x5450003,     0x5441EC4,
	0x5450004,     0x5440000,     0x5350000,     0x534A01A,
	0x205003C,     0x204F175,     0x205003C,     0x204F135,
	0x2050040,     0x2048800,     0x5A50001,     0x5A4001F,
	0x2050010,     0x2040020,     0x2050010,     0x2040020,
	0x170500,      0x170500,      0x5A50004,     0x5A40113,
	0x2050008,     0x2046A8C,     0x2050076,     0x204F000,
	0x205000E,     0x20465C0,     0x2050033,     0x2048580,
	0x2050069,     0x204FDA8,     0x2050068,     0x2040000,
	0x2050003,     0x2040002,     0x2050069,     0x2040000,
	0x2050068,     0x2040001,     0x205002E,     0x204290E,
	0x2050010,     0x2040020,     0x2050010,     0x2040020,

	/* Intel Meteor Lake HDMI */
	0x8086281d, /* Vendor ID */
	0x80860101, /* Subsystem ID */
	10, /* Number of entries */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;

static const u32 smartamp_verbs[] = {
	0x1471CF0,     0x1471D11,     0x1471E11,     0x1471F41,
	0x1771C10,     0x1771D01,     0x1771E17,     0x1771F90,
};

static const u32 no_smartamp_verbs[] = {
	0x1471C10,     0x1471D01,     0x1471E17,     0x1471F90,
	0x1771C11,     0x1771D01,     0x1771E17,     0x1771F90,
	0x205006B,     0x204A390,     0x205006B,     0x204A390,
	0x205006C,     0x2040C9E,     0x205006D,     0x2040C00,
};

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	if (gpio_get(GPP_E00)) {
		printk(BIOS_INFO, "Normal amp detected\n");
		azalia_program_verb_table(base, no_smartamp_verbs,
			ARRAY_SIZE(no_smartamp_verbs));
	} else {
		printk(BIOS_INFO, "Smart amp detected\n");
		azalia_program_verb_table(base, smartamp_verbs,
			ARRAY_SIZE(smartamp_verbs));
	}
}
