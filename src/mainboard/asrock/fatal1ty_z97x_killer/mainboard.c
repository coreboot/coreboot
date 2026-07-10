/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/haswell/haswell.h>
#include <superio/nuvoton/common/hwm.h>

/* HWM base address, set in devicetree.cb. */
#define HWM_IOBASE		0x290

/* Per-fan register banks. Only SYSFAN and CPUFAN are wired up. */
#define BANK_SYSFAN		1
#define BANK_CPUFAN		2

/* Fan profiles originally tuned for an i7-12700, does also seem to fit an i7-4790K */
static const struct nuvoton_fan_curve fans[] = {
	{
		.name			= "SYSFAN",
		.bank			= BANK_SYSFAN,
		.source			= NUVOTON_FAN_SOURCE_PECI0,
		.temp			= { 40, 60, 75, 90 },
		.duty			= {
			NUVOTON_PERCENT_TO_DUTY(20),
			NUVOTON_PERCENT_TO_DUTY(40),
			NUVOTON_PERCENT_TO_DUTY(70),
			NUVOTON_PERCENT_TO_DUTY(100),
		},
		.crit_temp		= 95,
		.crit_duty_en		= 1,
		.crit_duty		= 255,
		.crit_temp_tolerance	= 2,
	},
	{
		.name			= "CPUFAN",
		.bank			= BANK_CPUFAN,
		.source			= NUVOTON_FAN_SOURCE_PECI0,
		.temp			= { 40, 60, 75, 90 },
		.duty			= {
			NUVOTON_PERCENT_TO_DUTY(20),
			NUVOTON_PERCENT_TO_DUTY(40),
			NUVOTON_PERCENT_TO_DUTY(70),
			NUVOTON_PERCENT_TO_DUTY(100),
		},
		.crit_temp		= 95,
		.crit_duty_en		= 1,
		.crit_duty		= 255,
		.crit_temp_tolerance	= 2,
	},
};

static void hwm_init(void *arg)
{
	/* Tjmax differs between Haswell (100) and Broadwell (95) */
	const u8 tcc_temp = rdmsr(MSR_TEMPERATURE_TARGET).lo >> 16 & 0xff;

	/* Super I/O pin 120 must be set to PECI mode first */
	nuvoton_hwm_enable_peci(HWM_IOBASE, tcc_temp);

	for (size_t i = 0; i < ARRAY_SIZE(fans); i++)
		nuvoton_hwm_configure_fan(HWM_IOBASE, &fans[i]);
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, hwm_init, NULL);
