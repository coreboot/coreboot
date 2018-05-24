/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 LiPPERT Embedded Computers GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Based on romstage.c from AMD's DB800 and DBM690T mainboards. */

#include <stdlib.h>
#include <stdint.h>
#include <spd.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include <southbridge/amd/cs5536/cs5536.h>
#include <southbridge/amd/cs5536/smbus.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8712f/it8712f.h>
#include <northbridge/amd/lx/raminit.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8712F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8712F_GPIO)

/* Bit0 enables Spread Spectrum, bit1 makes on-board SSD act as IDE slave. */
#if IS_ENABLED(CONFIG_ONBOARD_IDE_SLAVE)
	#define SMC_CONFIG	0x03
#else
	#define SMC_CONFIG	0x01
#endif

static const unsigned char spdbytes[] = {	// 4x Promos V58C2512164SA-J5I
	0xFF, 0xFF,				// only values used by Geode-LX raminit.c are set
	[SPD_MEMORY_TYPE]		= SPD_MEMORY_TYPE_SDRAM_DDR,	// (Fundamental) memory type
	[SPD_NUM_ROWS]			= 0x0D,	// Number of row address bits [13]
	[SPD_NUM_COLUMNS]		= 0x0A,	// Number of column address bits [10]
	[SPD_NUM_DIMM_BANKS]		= 1,	// Number of module rows (banks)
	0xFF, 0xFF, 0xFF,
	[SPD_MIN_CYCLE_TIME_AT_CAS_MAX]	= 0x50,	// SDRAM cycle time (highest CAS latency), RAS access time (tRAC) [5.0 ns in BCD]
	0xFF, 0xFF,
	[SPD_REFRESH]			= 0x82,	// Refresh rate/type [Self Refresh, 7.8 us]
	[SPD_PRIMARY_SDRAM_WIDTH]	= 64,	// SDRAM width (primary SDRAM) [64 bits]
	0xFF, 0xFF, 0xFF,
	[SPD_NUM_BANKS_PER_SDRAM]	= 4,	// SDRAM device attributes, number of banks on SDRAM device
	[SPD_ACCEPTABLE_CAS_LATENCIES]	= 0x1C,	// SDRAM device attributes, CAS latency [3, 2.5, 2]
	0xFF, 0xFF,
	[SPD_MODULE_ATTRIBUTES]		= 0x20,	// SDRAM module attributes [differential clk]
	[SPD_DEVICE_ATTRIBUTES_GENERAL]	= 0x40,	// SDRAM device attributes, general [Concurrent AP]
	[SPD_SDRAM_CYCLE_TIME_2ND]	= 0x60,	// SDRAM cycle time (2nd highest CAS latency) [6.0 ns in BCD]
	0xFF,
	[SPD_SDRAM_CYCLE_TIME_3RD]	= 0x75,	// SDRAM cycle time (3rd highest CAS latency) [7.5 ns in BCD]
	0xFF,
	[SPD_tRP]			= 60,	// Min. row precharge time [15 ns in units of 0.25 ns]
	[SPD_tRRD]			= 40,	// Min. row active to row active [10 ns in units of 0.25 ns]
	[SPD_tRCD]			= 60,	// Min. RAS to CAS delay [15 ns in units of 0.25 ns]
	[SPD_tRAS]			= 40,	// Min. RAS pulse width = active to precharge delay [40 ns]
	[SPD_BANK_DENSITY]		= 0x40,	// Density of each row on module [256 MB]
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	[SPD_tRFC]			= 70	// SDRAM Device Minimum Auto Refresh to Active/Auto Refresh [70 ns]
};

int spd_read_byte(unsigned int device, unsigned int address)
{
	if (device != DIMM0)
		return 0xFF;	/* No DIMM1, don't even try. */

#if IS_ENABLED(CONFIG_DEBUG_SMBUS)
	if (address >= sizeof(spdbytes) || spdbytes[address] == 0xFF) {
		printk(BIOS_ERR, "ERROR: spd_read_byte(DIMM0, 0x%02x) "
			"returns 0xff\n", address);
	}
#endif

	/* Fake SPD ROM value */
	return (address < sizeof(spdbytes)) ? spdbytes[address] : 0xFF;
}

/* Send config data to System Management Controller via SMB. */
static int smc_send_config(unsigned char config_data)
{
	if (smbus_check_stop_condition(SMBUS_IO_BASE))
		return 1;
	if (smbus_start_condition(SMBUS_IO_BASE))
		return 2;
	if (smbus_send_slave_address(SMBUS_IO_BASE, 0x50)) // SMC address
		return 3;
	if (smbus_send_command(SMBUS_IO_BASE, 0x28)) // set config data
		return 4;
	if (smbus_send_command(SMBUS_IO_BASE, 0x01)) // data length
		return 5;
	if (smbus_send_command(SMBUS_IO_BASE, config_data))
		return 6;
	smbus_stop_condition(SMBUS_IO_BASE);
	return 0;
}

#include "northbridge/amd/lx/pll_reset.c"
#include "cpu/amd/geode_lx/cpureginit.c"
#include "cpu/amd/geode_lx/syspreinit.c"
#include "cpu/amd/geode_lx/msrinit.c"

static const u16 sio_init_table[] = { // hi = data, lo = index
	0x072C,		// VIN6 enabled, FAN4/5 disabled, VIN7,VIN3 internal
	0x1423,		// don't delay PoWeROK1/2
	0x9072,		// watchdog triggers PWROK, counts seconds
#if !IS_ENABLED(CONFIG_USE_WATCHDOG_ON_BOOT)
	0x0073, 0x0074,	// disarm watchdog by changing 56 s timeout to 0
#endif
	0xBF25, 0x172A, 0xF326,	// select GPIO function for most pins
	0xFF27, 0xDF28, 0x2729,	// (GP45 = SUSB, GP23,22,16,15 = SPI, GP13 = PWROK1)
	0x66B8, 0x0CB9,	// enable pullups on SPI, RS485_EN
	0x07C0,		// enable Simple-I/O for GP12-10= RS485_EN2,1, LIVE_LED
	0x07C8,		// config GP12-10 as output
	0x2DF5,		// map Hw Monitor Thermal Output to GP55
	0x08F8,		// map GP LED Blinking 1 to GP10 = LIVE_LED (deactivate Simple I/O to use)
};

/* Early mainboard specific GPIO setup. */
static void mb_gpio_init(void)
{
	int i;

	/* Init Super I/O WDT, GPIOs. Done early, WDT init may trigger reset! */
	for (i = 0; i < ARRAY_SIZE(sio_init_table); i++) {
		u16 reg = sio_init_table[i];
		ite_reg_write(GPIO_DEV, (u8) reg, (reg >> 8));
	}
}

void asmlinkage mainboard_romstage_entry(unsigned long bist)
{
	int err;

	static const struct mem_controller memctrl[] = {
		{.channel0 = {DIMM0, DIMM1}}
	};

	SystemPreInit();
	msr_init();

	cs5536_early_setup();

	/*
	 * Note: Must do this AFTER the early_setup! It is counting on some
	 * early MSR setup for CS5536.
	 */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	mb_gpio_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset();

	cpuRegInit(0, DIMM0, DIMM1, DRAM_TERMINATED);

	/* bit1 = on-board IDE is slave, bit0 = Spread Spectrum */
	if ((err = smc_send_config(SMC_CONFIG))) {
		printk(BIOS_ERR, "ERROR %d sending config data to SMC\n", err);
	}

	sdram_initialize(1, memctrl);

	/* Memory is setup. Return to cache_as_ram.inc and continue to boot. */
	return;
}
