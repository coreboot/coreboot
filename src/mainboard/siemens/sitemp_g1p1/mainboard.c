/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <delay.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>
#include <southbridge/amd/sb600/sb600.h>
#include <southbridge/amd/rs690/chip.h>
#include <southbridge/amd/rs690/rs690.h>
#include <superio/ite/it8712f/it8712f.h>
#if IS_ENABLED(CONFIG_PCI_OPTION_ROM_RUN_YABEL)
#include <x86emu/x86emu.h>
#endif
#include "int15_func.h"
#include "mainboard.h"

// ****LCD panel ID support: *****
// Callback Sub-Function 00h - Get LCD Panel ID
#define PANEL_TABLE_ID_NO 	0 // no LCD
#define PANEL_TABLE_ID1 	1 // 1024x768_65MHz_Dual
#define PANEL_TABLE_ID2 	2 // 920x1200_162MHz
#define PANEL_TABLE_ID3 	3 // 600x1200_162MHz
#define PANEL_TABLE_ID4 	4 // 1024x768_65MHz
#define PANEL_TABLE_ID5 	5 // 1400x1050_108MHz
#define PANEL_TABLE_ID6 	6 // 1680x1050_119MHz
#define PANEL_TABLE_ID7 	7 // 2048x1536_164MHz
#define PANEL_TABLE_ID8 	8 // 1280x1024_108MHz
#define PANEL_TABLE_ID9 	9 // 1366x768_86MHz_chimei_V32B1L01

// Callback Sub-Function 05h - Select Boot-up TV Standard
#define TV_MODE_00	0x00	/* NTSC */
#define TV_MODE_01	0x01	/* PAL */
#define TV_MODE_02	0x02	/* PALM */
#define TV_MODE_03	0x03	/* PAL60 */
#define TV_MODE_04	0x04	/* NTSCJ */
#define TV_MODE_05	0x05	/* PALCN */
#define TV_MODE_06	0x06	/* PALN */
#define TV_MODE_09	0x09	/* SCART-RGB */
#define TV_MODE_NO	0xff	/* No TV Support */

#define PLX_VIDDID 0x861610b5

/* 7475 Common Registers */
#define REG_DEVREV2             0x12    /* ADT7490 only */
#define REG_VTT                 0x1E    /* ADT7490 only */
#define REG_EXTEND3             0x1F    /* ADT7490 only */
#define REG_VOLTAGE_BASE        0x20
#define REG_TEMP_BASE           0x25
#define REG_TACH_BASE           0x28
#define REG_PWM_BASE            0x30
#define REG_PWM_MAX_BASE        0x38
#define REG_DEVID               0x3D
#define REG_VENDID              0x3E
#define REG_DEVID2              0x3F
#define REG_STATUS1             0x41
#define REG_STATUS2             0x42
#define REG_VID                 0x43    /* ADT7476 only */
#define REG_VOLTAGE_MIN_BASE    0x44
#define REG_VOLTAGE_MAX_BASE    0x45
#define REG_TEMP_MIN_BASE       0x4E
#define REG_TEMP_MAX_BASE       0x4F
#define REG_TACH_MIN_BASE       0x54
#define REG_PWM_CONFIG_BASE     0x5C
#define REG_TEMP_TRANGE_BASE    0x5F
#define REG_PWM_MIN_BASE        0x64
#define REG_TEMP_TMIN_BASE      0x67
#define REG_TEMP_THERM_BASE     0x6A
#define REG_REMOTE1_HYSTERSIS   0x6D
#define REG_REMOTE2_HYSTERSIS   0x6E
#define REG_TEMP_OFFSET_BASE    0x70
#define REG_CONFIG2             0x73
#define REG_EXTEND1             0x76
#define REG_EXTEND2             0x77
#define REG_CONFIG1				0x40	// ADT7475
#define REG_CONFIG3             0x78
#define REG_CONFIG5             0x7C
#define REG_CONFIG6				0x10	// ADT7475
#define REG_CONFIG7				0x11	// ADT7475
#define REG_CONFIG4             0x7D
#define REG_STATUS4             0x81    /* ADT7490 only */
#define REG_VTT_MIN             0x84    /* ADT7490 only */
#define REG_VTT_MAX             0x86    /* ADT7490 only */

#define VID_VIDSEL              0x80    /* ADT7476 only */

#define CONFIG2_ATTN            0x20
#define CONFIG3_SMBALERT        0x01
#define CONFIG3_THERM           0x02
#define CONFIG4_PINFUNC         0x03
#define CONFIG4_MAXDUTY         0x08
#define CONFIG4_ATTN_IN10       0x30
#define CONFIG4_ATTN_IN43       0xC0
#define CONFIG5_TWOSCOMP        0x01
#define CONFIG5_TEMPOFFSET      0x02
#define CONFIG5_VIDGPIO         0x10    /* ADT7476 only */
#define REMOTE1					0
#define LOCAL					1
#define REMOTE2					2

/* ADT7475 Settings */
#define ADT7475_VOLTAGE_COUNT   5       /* Not counting Vtt */
#define ADT7475_TEMP_COUNT      3
#define ADT7475_TACH_COUNT      4
#define ADT7475_PWM_COUNT       3

/* Macros to easily index the registers */
#define TACH_REG(idx) (REG_TACH_BASE + ((idx) * 2))
#define TACH_MIN_REG(idx) (REG_TACH_MIN_BASE + ((idx) * 2))

#define PWM_REG(idx) (REG_PWM_BASE + (idx))
#define PWM_MAX_REG(idx) (REG_PWM_MAX_BASE + (idx))
#define PWM_MIN_REG(idx) (REG_PWM_MIN_BASE + (idx))
#define PWM_CONFIG_REG(idx) (REG_PWM_CONFIG_BASE + (idx))

#define VOLTAGE_REG(idx) (REG_VOLTAGE_BASE + (idx))
#define VOLTAGE_MIN_REG(idx) (REG_VOLTAGE_MIN_BASE + ((idx) * 2))
#define VOLTAGE_MAX_REG(idx) (REG_VOLTAGE_MAX_BASE + ((idx) * 2))

#define TEMP_REG(idx) (REG_TEMP_BASE + (idx))
#define TEMP_MIN_REG(idx) (REG_TEMP_MIN_BASE + ((idx) * 2))
#define TEMP_MAX_REG(idx) (REG_TEMP_MAX_BASE + ((idx) * 2))
#define TEMP_TMIN_REG(idx) (REG_TEMP_TMIN_BASE + (idx))
#define TEMP_THERM_REG(idx) (REG_TEMP_THERM_BASE + (idx))
#define TEMP_OFFSET_REG(idx) (REG_TEMP_OFFSET_BASE + (idx))
#define TEMP_TRANGE_REG(idx) (REG_TEMP_TRANGE_BASE + (idx))

#define SMBUS_IO_BASE 0x1000
#define ADT7475_ADDRESS 0x2E

#define D_OPEN	(1 << 6)
#define D_CLS		(1 << 5)
#define D_LCK		(1 << 4)
#define G_SMRAME	(1 << 3)
#define A_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0)) // 0x2: SMM space at 640KB-768KB

extern int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address);
extern int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address, u8 val);

static u32 smbus_io_base = SMBUS_IO_BASE;
static u32 adt7475_address = ADT7475_ADDRESS;

/* Macro to read the registers */
#define adt7475_read_byte(reg) \
	do_smbus_read_byte(smbus_io_base, adt7475_address, reg)

#define adt7475_write_byte(reg, val) \
	do_smbus_write_byte(smbus_io_base, adt7475_address, reg, val)

#define TWOS_COMPL 1

struct __table__{
	const char *info;
	u8 val;
};

struct __table__ dutycycles[] = {
	{"25%", 0x3f},{"30%", 0x4c},{"35%", 0x59},{"40%", 0x66},{"45%", 0x73},
	{"50%", 0x80},{"55%", 0x8d},{"60%", 0x9a},{"65%", 0xa7},{"70%", 0xb4},
	{"75%", 0xc1},{"80%", 0xce},{"85%", 0xdb},{"90%", 0xe8},{"95%", 0xf5},
	{"100%", 0xff}
};
#define SIZEOF_DUTYCYCLES sizeof(dutycycles)/sizeof(struct __table__)
#define DUTYCYCLE(i,d) (i < SIZEOF_DUTYCYCLES) ? dutycycles[i].val : dutycycles[d].val // hopefully d is a correct value !!! fix
#define DUTYCYCLE_INFO(i) (i < SIZEOF_DUTYCYCLES) ? dutycycles[i].info : "out_of_range"
#if TWOS_COMPL == 0
struct __table__ temperatures[] = {
	{"30C", 0x5e},{"35C", 0x63},{"40C", 0x68},{"45C", 0x6d},{"50C", 0x72},
	{"55C", 0x77},{"60C", 0x7c},{"65C", 0x81},{"70C", 0x86},{"75C", 0x8b},
	{"80C", 0x90}
};
#else
struct __table__ temperatures[] = {
	{"30C", 30},{"35C", 35},{"40C", 40},{"45C", 45},{"50C", 50},
	{"55C", 55},{"60C", 60},{"65C", 65},{"70C", 70},{"75C", 75},
	{"80C", 80}
};
#endif
// FIXME: implicit conversion from 'double' to 'int'
// int trange[] = {2.0,2.5,3.33,4.0,5.0,6.67,8.0,10.0,13.33,16.0,20.0,26.67,32.0,40.0,53.33,80.0};
int trange[] = {2,2,3,4,5,6,8,10,13,16,20,26,32,40,53,80};

#define SIZEOF_TEMPERATURES sizeof(temperatures)/sizeof(struct __table__)
#define TEMPERATURE(i,d) (i < SIZEOF_TEMPERATURES) ? temperatures[i].val : temperatures[d].val // hopefully d is a correct value !!! fix
#define TEMPERATURE_INFO(i) (i < SIZEOF_TEMPERATURES) ? temperatures[i].info : "out of range"

struct fan_control {
		unsigned int enable : 1;
		u8 polarity;
		u8 t_min;
		u8 t_max;
		u8 pwm_min;
		u8 pwm_max;
		u8 t_range;
};
/* ############################################################################################# */
#if IS_ENABLED(CONFIG_PCI_OPTION_ROM_RUN_YABEL)
static int int15_handler(void)
{
#define BOOT_DISPLAY_DEFAULT	0
#define BOOT_DISPLAY_CRT	(1 << 0)
#define BOOT_DISPLAY_TV		(1 << 1)
#define BOOT_DISPLAY_EFP	(1 << 2)
#define BOOT_DISPLAY_LCD	(1 << 3)
#define BOOT_DISPLAY_CRT2	(1 << 4)
#define BOOT_DISPLAY_TV2	(1 << 5)
#define BOOT_DISPLAY_EFP2	(1 << 6)
#define BOOT_DISPLAY_LCD2	(1 << 7)

	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
			  __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_AX) {
	case 0x4e08: /* Boot Display */
	    switch (X86_BX) {
		case 0x80:
			X86_AX &= ~(0xff); // Success
			X86_BX &= ~(0xff);
			printk(BIOS_DEBUG, "Integrated System Information\n");
			break;
		case 0x00:
			X86_AX &= ~(0xff);
			X86_BX = 0x00;
			printk(BIOS_DEBUG, "Panel ID = 0\n");
			break;
		case 0x05:
			X86_AX &= ~(0xff);
			X86_BX = 0xff;
			printk(BIOS_DEBUG, "TV = off\n");
			break;
		default:
			return 0;
		}
		break;
	case 0x5f35: /* Boot Display */
		X86_AX = 0x005f; // Success
		X86_CL = BOOT_DISPLAY_DEFAULT;
		break;
	case 0x5f40: /* Boot Panel Type */
		// M.x86.R_AX = 0x015f; // Supported but failed
		X86_AX = 0x005f; // Success
		X86_CL = 3; // Display ID
		break;
	default:
		/* Interrupt was not handled */
		return 0;
	}

	/* Interrupt handled */
	return 1;
}
#endif
/* ############################################################################################# */

static u8 calc_trange(u8 t_min, u8 t_max) {

	u8 prev;
	int i;
	int diff = t_max - t_min;

	// walk through the trange table
	for(i = 0, prev = 0; i < sizeof(trange)/sizeof(int); i++) {
		if( trange[i] < diff ) {
			prev = i; // save last val
			continue;
		}
		if( diff == trange[i] ) return i;
		if( (diff - trange[prev]) < (trange[i] - diff) ) break; // return with last val index
		return i;
	}
	return prev;
}

/********************************************************
* sina uses SB600 GPIO9 to detect IDE_DMA66.
* IDE_DMA66 is routed to GPIO 9. So we read Gpio 9 to
* get the cable type, 40 pin or 80 pin?
********************************************************/
static void cable_detect(void)
{

	u8 byte;
	struct device *sm_dev;
	struct device *ide_dev;

	/* SMBus Module and ACPI Block (Device 20, Function 0) on SB600 */
	printk(BIOS_DEBUG, "%s.\n", __func__);
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	byte = pci_read_config8(sm_dev, 0xA9);
	byte |= (1 << 5);	/* Set Gpio9 as input */
	pci_write_config8(sm_dev, 0xA9, byte);

	/* IDE Controller (Device 20, Function 1) on SB600 */
	ide_dev = dev_find_slot(0, PCI_DEVFN(0x14, 1));

	byte = pci_read_config8(ide_dev, 9);
	printk(BIOS_INFO, "IDE controller in %s Mode\n", byte & (1 << 0) ? "Native" : "Compatibility");

	byte = pci_read_config8(ide_dev, 0x56);
	byte &= ~(7 << 0);
	if( pci_read_config8(sm_dev, 0xAA) & (1 << 5) )
		byte |= 2 << 0;	/* mode 2 */
	else
		byte |= 5 << 0;	/* mode 5 */
	printk(BIOS_INFO, "DMA mode %d selected\n", byte & (7 << 0));
	pci_write_config8(ide_dev, 0x56, byte);
}

/**
 * @brief Detect the ADT7475 device
 *
 */

static const char * adt7475_detect( void ) {

	int vendid, devid, devid2;
	const char *name = NULL;

	vendid = adt7475_read_byte(REG_VENDID);
	devid2 = adt7475_read_byte(REG_DEVID2);
	if (vendid != 0x41 || (devid2 & 0xf8) != 0x68) /* Analog Devices */
		return name;

	devid = adt7475_read_byte(REG_DEVID);
	if (devid == 0x73)
		name = "adt7473";
	else if (devid == 0x75 && adt7475_address == 0x2e)
		name = "adt7475";
	else if (devid == 0x76)
		name = "adt7476";
	else if ((devid2 & 0xfc) == 0x6c)
		name = "adt7490";

	return name;
}

// thermal control defaults
const struct fan_control cpu_fan_control_defaults = {
	.enable = 0, // disable by default
	.polarity = 0, // high by default
	.t_min = 3, // default = 45°C
	.t_max = 7, // 65°C
	.pwm_min = 1, // default dutycycle = 30%
	.pwm_max = 13, // 90%
};
const struct fan_control case_fan_control_defaults = {
	.enable = 0, // disable by default
	.polarity = 0, // high by default
	.t_min = 2, // default = 40°C
	.t_max = 8, // 70°C
	.pwm_min = 0, // default dutycycle = 25%
	.pwm_max = 13, // 90%
};

static void pm_init( void )
{
	u16 word;
	u8 byte;
	struct device *sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	/* set SB600 GPIO 64 to GPIO with pull-up */
	byte = pm2_ioread(0x42);
	byte &= 0x3f;
	pm2_iowrite(0x42, byte);

	/* set GPIO 64 to tristate */
	word = pci_read_config16(sm_dev, 0x56);
	word |= 1 << 7;
	pci_write_config16(sm_dev, 0x56, word);

	/* set GPIO 64 internal pull-up */
	byte = pm2_ioread(0xf0);
	byte &= 0xee;
	pm2_iowrite(0xf0, byte);

	/* set Talert to be active low */
	byte = pm_ioread(0x67);
	byte &= ~(1 << 5);
	pm_iowrite(0x67, byte);

	/* set Talert to generate ACPI event */
	byte = pm_ioread(0x3c);
	byte &= 0xf3;
	pm_iowrite(0x3c, byte);

	/* set GPM5 to not wake from s5 */
	byte = pm_ioread(0x77);
	byte &= ~(1 << 5);
	pm_iowrite(0x77, byte);
}

 /**
 * @brief Setup thermal config on SINA Mainboard
 *
 */

static void set_thermal_config(void)
{
	u8 byte, byte2;
	u8 cpu_pwm_conf, case_pwm_conf;
	struct device *sm_dev;
	struct fan_control cpu_fan_control, case_fan_control;
	const char *name = NULL;


	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	smbus_io_base = pci_read_config32(sm_dev, 0x10) & ~(0xf); // get BAR0-Address which holds the SMBUS_IO_BASE

	if( (name = adt7475_detect()) == NULL ) {
		printk(BIOS_NOTICE, "Couldn't detect an ADT7473/75/76/90 part at %x:%x\n", smbus_io_base, adt7475_address);
		return;
	}
	printk(BIOS_DEBUG, "Found %s part at %x:%x\n", name, smbus_io_base, adt7475_address);

	cpu_fan_control = cpu_fan_control_defaults;
	case_fan_control = case_fan_control_defaults;

	if (get_option(&byte, "cpu_fan_control") == CB_CMOS_CHECKSUM_INVALID) {
		printk(BIOS_WARNING, "%s: CMOS checksum invalid, keeping default values\n",__func__);
	} else {
		// get all the options needed
		if( get_option(&byte, "cpu_fan_control") == CB_SUCCESS )
			cpu_fan_control.enable = byte ? 1 : 0;

		get_option(&cpu_fan_control.polarity, "cpu_fan_polarity");
		get_option(&cpu_fan_control.t_min, "cpu_t_min");
		get_option(&cpu_fan_control.t_max, "cpu_t_max");
		get_option(&cpu_fan_control.pwm_min, "cpu_dutycycle_min");
		get_option(&cpu_fan_control.pwm_max, "cpu_dutycycle_max");

		if( get_option(&byte, "chassis_fan_control") == CB_SUCCESS)
			case_fan_control.enable = byte ? 1 : 0;
		get_option(&case_fan_control.polarity, "chassis_fan_polarity");
		get_option(&case_fan_control.t_min, "chassis_t_min");
		get_option(&case_fan_control.t_max, "chassis_t_max");
		get_option(&case_fan_control.pwm_min, "chassis_dutycycle_min");
		get_option(&case_fan_control.pwm_max, "chassis_dutycycle_max");

	}

	printk(BIOS_DEBUG, "cpu_fan_control:%s", cpu_fan_control.enable ? "enable" : "disable");
	printk(BIOS_DEBUG, " cpu_fan_polarity:%s", cpu_fan_control.polarity ? "low" : "high");

	printk(BIOS_DEBUG, " cpu_t_min:%s", TEMPERATURE_INFO(cpu_fan_control.t_min));
	cpu_fan_control.t_min = TEMPERATURE(cpu_fan_control.t_min, cpu_fan_control_defaults.t_min);

	printk(BIOS_DEBUG, " cpu_t_max:%s", TEMPERATURE_INFO(cpu_fan_control.t_max));
	cpu_fan_control.t_max = TEMPERATURE(cpu_fan_control.t_max, cpu_fan_control_defaults.t_max);

	printk(BIOS_DEBUG, " cpu_pwm_min:%s", DUTYCYCLE_INFO(cpu_fan_control.pwm_min));
	cpu_fan_control.pwm_min = DUTYCYCLE(cpu_fan_control.pwm_min, cpu_fan_control_defaults.pwm_min);

	printk(BIOS_DEBUG, " cpu_pwm_max:%s", DUTYCYCLE_INFO(cpu_fan_control.pwm_max));
	cpu_fan_control.pwm_max = DUTYCYCLE(cpu_fan_control.pwm_max, cpu_fan_control_defaults.pwm_max);

	cpu_fan_control.t_range = calc_trange(cpu_fan_control.t_min, cpu_fan_control.t_max);
	printk(BIOS_DEBUG, " cpu_t_range:0x%02x\n", cpu_fan_control.t_range);
	cpu_fan_control.t_range <<= 4;
	cpu_fan_control.t_range |= (4 << 0); // 35.3Hz

	printk(BIOS_DEBUG, "chassis_fan_control:%s", case_fan_control.enable ? "enable" : "disable");
	printk(BIOS_DEBUG, " chassis_fan_polarity:%s", case_fan_control.polarity ? "low" : "high");

	printk(BIOS_DEBUG, " chassis_t_min:%s", TEMPERATURE_INFO(case_fan_control.t_min));
	case_fan_control.t_min = TEMPERATURE(case_fan_control.t_min, case_fan_control_defaults.t_min);

	printk(BIOS_DEBUG, " chassis_t_max:%s", TEMPERATURE_INFO(case_fan_control.t_max));
	case_fan_control.t_max = TEMPERATURE(case_fan_control.t_max, case_fan_control_defaults.t_max);

	printk(BIOS_DEBUG, " chassis_pwm_min:%s", DUTYCYCLE_INFO(case_fan_control.pwm_min));
	case_fan_control.pwm_min = DUTYCYCLE(case_fan_control.pwm_min, case_fan_control_defaults.pwm_min);

	printk(BIOS_DEBUG, " chassis_pwm_max:%s", DUTYCYCLE_INFO(case_fan_control.pwm_max));
	case_fan_control.pwm_max = DUTYCYCLE(case_fan_control.pwm_max, case_fan_control_defaults.pwm_max);

	case_fan_control.t_range = calc_trange(case_fan_control.t_min, case_fan_control.t_max);
	printk(BIOS_DEBUG, " case_t_range:0x%02x\n", case_fan_control.t_range);
	case_fan_control.t_range <<= 4;
	case_fan_control.t_range |= (4 << 0); // 35.3Hz

	cpu_pwm_conf = (((cpu_fan_control.polarity & 0x1) << 4) | 0x2); // bit 4 control polarity of PWMx output
	case_pwm_conf = (((case_fan_control.polarity & 0x1) << 4) | 0x2); // bit 4 control polarity of PWMx output
	cpu_pwm_conf |= cpu_fan_control.enable ? (0 << 5) : (7 << 5);  // manual control
	case_pwm_conf |= case_fan_control.enable ? (1 << 5) : (7 << 5); // local temp

	/* set adt7475 */

	adt7475_write_byte(REG_CONFIG1, 0x04);  // clear register, bit 2 is read only

	/* Config Register 6:  */
	adt7475_write_byte(REG_CONFIG6, 0x00);
	/* Config Register 7 */
	adt7475_write_byte(REG_CONFIG7, 0x00);

	/* Config Register 5: */
	/* set Offset 64 format, enable THERM on Remote 1& Local */
	adt7475_write_byte(REG_CONFIG5, TWOS_COMPL ? 0x61 : 0x60);
	/* No offset for remote 1 */
	adt7475_write_byte(TEMP_OFFSET_REG(0), 0x00);
	/* No offset for local */
	adt7475_write_byte(TEMP_OFFSET_REG(1), 0x00);
	/* No offset for remote 2 */
	adt7475_write_byte(TEMP_OFFSET_REG(2), 0x00);

	/* remote 1 low temp limit */
	adt7475_write_byte(TEMP_MIN_REG(0), 0x00);
	/* remote 1 High temp limit    (90C) */
	adt7475_write_byte(TEMP_MAX_REG(0), 0x9a);

	/* local Low Temp Limit */
	adt7475_write_byte(TEMP_MIN_REG(1), 0x00);
	/* local High Limit    (90C) */
	adt7475_write_byte(TEMP_MAX_REG(1), 0x9a);

	/*  remote 1 therm temp limit    (95C) */
	adt7475_write_byte(TEMP_THERM_REG(0), 0x9f);
	/* local therm temp limit    (95C) */
	adt7475_write_byte(TEMP_THERM_REG(1), 0x9f);

	/* PWM 1 configuration register    CPU fan controlled by CPU Thermal Diode */
	adt7475_write_byte(PWM_CONFIG_REG(0), cpu_pwm_conf);
	/* PWM 3 configuration register    Case fan controlled by ADTxxxx temp */
	adt7475_write_byte(PWM_CONFIG_REG(2), case_pwm_conf);

	if( cpu_fan_control.enable ) {
		/* PWM 1 minimum duty cycle     (37%) */
		adt7475_write_byte(PWM_MIN_REG(0), cpu_fan_control.pwm_min);
		/* PWM 1 Maximum duty cycle    (100%) */
		adt7475_write_byte(PWM_MAX_REG(0), cpu_fan_control.pwm_max);
		/*  Remote 1 temperature Tmin     (32C) */
		adt7475_write_byte(TEMP_TMIN_REG(0), cpu_fan_control.t_min);
		/* remote 1 Trange            (53C ramp range) */
		adt7475_write_byte(TEMP_TRANGE_REG(0), cpu_fan_control.t_range);
	} else {
		adt7475_write_byte(PWM_REG(0), cpu_fan_control.pwm_max);
	}

	if( case_fan_control.enable ) {
		/* PWM 2 minimum duty cycle     (37%) */
		adt7475_write_byte(PWM_MIN_REG(2), case_fan_control.pwm_min);
		/* PWM 2 Maximum Duty Cycle    (100%) */
		adt7475_write_byte(PWM_MAX_REG(2), case_fan_control.pwm_max);
		/* local temperature Tmin     (32C) */
		adt7475_write_byte(TEMP_TMIN_REG(1), case_fan_control.t_min);
		/* local Trange            (53C ramp range) */
		adt7475_write_byte(TEMP_TRANGE_REG(1), case_fan_control.t_range); // Local TRange
		adt7475_write_byte(TEMP_TRANGE_REG(2), case_fan_control.t_range); // PWM2 Freq
	} else {
		adt7475_write_byte(PWM_REG(2), case_fan_control.pwm_max);
	}

	/* Config Register 3 - enable smbalert & therm */
	adt7475_write_byte(0x78, 0x03);
	/* Config Register 4 - enable therm output */
	adt7475_write_byte(0x7d, 0x09);
	/* Interrupt Mask Register 2 - Mask SMB alert for Therm Conditions, Fan 3 fault, SmbAlert Fan for Therm Timer event */
	adt7475_write_byte(0x75, 0x2e);

	/* Config Register 1 Set Start bit */
	adt7475_write_byte(0x40, 0x05);

	/* Read status register to clear any old errors */
	byte2 = adt7475_read_byte(0x42);
	byte = adt7475_read_byte(0x41);

	printk(BIOS_DEBUG, "Init 'Thermal Monitor' end , status 0x42 = 0x%02x, status 0x41 = 0x%02x\n",
		    byte2, byte);

}

static void patch_mmio_nonposted( void )
{
	unsigned reg, index;
	resource_t rbase, rend;
	u32 base, limit;
	struct resource *resource;
	struct device *dev;
	struct device *k8_f1 = dev_find_slot(0, PCI_DEVFN(0x18,1));

	printk(BIOS_DEBUG,"%s ...\n", __func__);

	dev = dev_find_slot(1, PCI_DEVFN(5,0));
	// the uma frame buffer
	index = 0x10;
	resource = probe_resource(dev, index);
	if( resource ) {
		// fixup resource nonposted in k8 mmio
		/* Get the base address */
		rbase = (resource->base >> 8) & ~(0xff);
		/* Get the limit (rounded up) */
		rend  = (resource_end(resource) >> 8) & ~(0xff);

		printk(BIOS_DEBUG,"%s %x base = %0llx limit = %0llx\n", dev_path(dev), index, rbase, rend);

		for( reg = 0xb8; reg >= 0x80; reg -= 8 ) {
			base = pci_read_config32(k8_f1,reg);
			limit = pci_read_config32(k8_f1,reg+4);
			printk(BIOS_DEBUG," %02x[%08x] %02x[%08x]", reg, base, reg+4, limit);
			if( ((base & ~(0xff)) == rbase) && ((limit & ~(0xff)) == rend) ) {
				limit |= (1 << 7);
				printk(BIOS_DEBUG, "\nPatching %s %x <- %08x", dev_path(k8_f1), reg, limit);
				pci_write_config32(k8_f1, reg+4, limit);
				break;
			}
		}
		printk(BIOS_DEBUG, "\n");
	}
}

struct {
	unsigned int bus;
	unsigned int devfn;
} slot[] = {
	{0, PCI_DEVFN(0,0)},
	{0, PCI_DEVFN(18,0)},
	{0, PCI_DEVFN(19,0)},{0, PCI_DEVFN(19,1)},{0, PCI_DEVFN(19,2)},{0, PCI_DEVFN(19,3)},{0, PCI_DEVFN(19,4)},{0, PCI_DEVFN(19,5)},
	{0, PCI_DEVFN(20,0)},{0, PCI_DEVFN(20,1)},{0, PCI_DEVFN(20,2)},{0, PCI_DEVFN(20,3)},{0, PCI_DEVFN(20,5)},{0, PCI_DEVFN(20,6)},
	{0, PCI_DEVFN(5,0)},{0, PCI_DEVFN(5,2)},
	{255,0},
};


unsigned int plx_present = 0;

static void update_subsystemid( struct device *dev )
{
	int i;

	dev->subsystem_vendor = 0x110a;
	if( plx_present ){
		dev->subsystem_device = 0x4076; // U1P1 = 0x4076, U1P0 = 0x4077
	} else {
		dev->subsystem_device = 0x4077; // U1P0 = 0x4077
	}
	printk(BIOS_INFO, "%s [%x/%x]\n", dev_name(dev), dev->subsystem_vendor, dev->subsystem_device );
	for( i = 0; slot[i].bus < 255; i++) {
		struct device *d;
		d = dev_find_slot(slot[i].bus,slot[i].devfn);
		if( d ) {
			printk(BIOS_DEBUG,"%s subsystem <- %x/%x\n", dev_path(d), dev->subsystem_vendor, dev->subsystem_device);
			d->subsystem_device = dev->subsystem_device;
		}
	}
}

static void detect_hw_variant( struct device *dev )
{

	struct device *nb_dev = NULL, *dev2 = NULL;
	struct southbridge_amd_rs690_config *cfg;
	u32 lc_state, id = 0;

	printk(BIOS_INFO, "Scan for PLX device ...\n");
	nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!nb_dev) {
		die("CAN NOT FIND RS690 DEVICE, HALT!\n");
		/* NOT REACHED */
	}

	dev2 = dev_find_slot(0, PCI_DEVFN(2, 0));
	if (!dev2) {
		die("CAN NOT FIND GFX DEVICE 2, HALT!\n");
		/* NOT REACHED */
	}
	PcieReleasePortTraining(nb_dev, dev2, 2); // we assume PLX is connected to port 2

	mdelay(40);
	lc_state = nbpcie_p_read_index(dev2, 0xa5);	/* lc_state */
	printk(BIOS_DEBUG, "lc current state=%x\n", lc_state);
	/* LC_CURRENT_STATE = bit0-5 */
	switch( lc_state & 0x3f ){
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
		printk(BIOS_NOTICE, "No device present, skipping PLX scan ..\n");
		break;
	case 0x07:
	case 0x10:
	{
		struct device dummy;
		u32 pci_primary_bus, buses;
		u16 secondary, subordinate;

		printk(BIOS_DEBUG, "Scan for PLX bridge behind %s[%x]\n", dev_path(dev2), pci_read_config32(dev2, PCI_VENDOR_ID));
		// save the existing primary/secondary/subordinate bus number configuration.
		secondary = dev2->bus->secondary;
		subordinate = dev2->bus->subordinate;
		buses = pci_primary_bus = pci_read_config32(dev2, PCI_PRIMARY_BUS);

		// Configure the bus numbers for this bridge
		// bus number 1 is for internal gfx device, so we start with busnumber 2

		buses &= 0xff000000;
		buses |= ((2 << 8) | (0xff << 16));
		// setup the buses in device 2
		pci_write_config32(dev2,PCI_PRIMARY_BUS, buses);

		// fake a device descriptor for a device behind device 2
		dummy.bus = dev2->bus;
		dummy.bus->secondary = (buses >> 8) & 0xff;
		dummy.bus->subordinate = (buses >> 16) & 0xff;
		dummy.path.type = DEVICE_PATH_PCI;
		dummy.path.pci.devfn = PCI_DEVFN(0,0); // PLX: device number 0, function 0

		id = pci_read_config32(&dummy, PCI_VENDOR_ID);
		/* Have we found something?
		 * Some broken boards return 0 if a slot is empty, but
		 * the expected answer is 0xffffffff
		 */
		if ((id == 0xffffffff) || (id == 0x00000000) || (id == 0x0000ffff) || (id == 0xffff0000)) {
			printk(BIOS_DEBUG, "%s, bad id 0x%x\n", dev_path(&dummy), id);
		} else {
			printk(BIOS_DEBUG, "found device [%x]\n", id);
		}
		// restore changes made for device 2
		dev2->bus->secondary = secondary;
		dev2->bus->subordinate = subordinate;
		pci_write_config32(dev2, PCI_PRIMARY_BUS, pci_primary_bus);
	}
		break;
	default:
		break;
	}

	plx_present = 0;
	if( id == PLX_VIDDID ){
		printk(BIOS_INFO, "found PLX device\n");
		plx_present = 1;
		cfg = (struct southbridge_amd_rs690_config *)dev2->chip_info;
		if( cfg->gfx_tmds ) {
			printk(BIOS_INFO, "Disable 'gfx_tmds' support\n");
			cfg->gfx_tmds = 0;
			cfg->gfx_link_width = 4;
		}
		return;
	}
}

void smm_lock(void)
{
	/* LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");
	pci_write_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), 0x69,
			D_LCK | G_SMRAME | A_BASE_SEG);
}

 /**
 * @brief Init
 *
 * @param dev - the root device
 */

static void mainboard_init(struct device *dev)
{
#if IS_ENABLED(CONFIG_PCI_OPTION_ROM_RUN_REALMODE)
	INT15_function_extensions int15_func;
#endif

	printk(BIOS_DEBUG, "%s %s[%x/%x] %s\n",
		dev_name(dev), dev_path(dev), dev->subsystem_vendor, dev->subsystem_device, __func__);

#if IS_ENABLED(CONFIG_PCI_OPTION_ROM_RUN_REALMODE)
	if (get_option(&int15_func.regs.func00_LCD_panel_id, "lcd_panel_id") != CB_SUCCESS)
		int15_func.regs.func00_LCD_panel_id = PANEL_TABLE_ID_NO;
	int15_func.regs.func05_TV_standard = TV_MODE_NO;
	install_INT15_function_extensions(&int15_func);
#endif
	set_thermal_config();
	pm_init();
	cable_detect();
	patch_mmio_nonposted();
	smm_lock();
}

/*************************************************
* enable the dedicated function in sina board.
* This function called early than rs690_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{

	printk(BIOS_INFO, "%s %s[%x/%x] %s\n",
		dev_name(dev), dev_path(dev), dev->subsystem_vendor, dev->subsystem_device, __func__);
#if IS_ENABLED(CONFIG_PCI_OPTION_ROM_RUN_YABEL)
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif

	detect_hw_variant(dev);
	update_subsystemid(dev);

	dev->ops->init = mainboard_init;  // rest of mainboard init later
	dev->ops->acpi_inject_dsdt_generator = mainboard_inject_dsdt;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
