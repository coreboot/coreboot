#include <arch/io.h>
#include <part/mainboard.h>
#include <printk.h>
#include <pci.h>
#include <pci_ids.h>
#include <southbridge/intel/82801.h>
#include <arch/smp/mpspec.h>
#include <pc80/isa_dma.h>
#include <cpu/i786/multiplier.h>
#include <cpu/i786/thermal_monitoring.h>
#include <cpu/p6/msr.h>
#include <superio/w83627hf.h>
#include <superio/generic.h>
#include <subr.h>
#include <smbus.h>
#include <ramtest.h>
#include <northbridge/intel/82860/rdram.h>
#include <pc80/mc146818rtc.h>


unsigned long initial_apicid[MAX_CPUS] =
{
	0, 6, 1, 7
};

#ifndef CPU_CLOCK_MULTIPLIER
#define CPU_CLOCK_MULTIPLIER XEON_X17
#endif

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

static void set_power_on_after_power_fail(int setting)
{
	switch(setting) {
	case MAINBOARD_POWER_ON:
	default:
		ich3_power_after_power_fail(1);
		w83627hf_power_after_power_fail(POWER_ON);
		break;
	case MAINBOARD_POWER_OFF:
		ich3_power_after_power_fail(0);
		w83627hf_power_after_power_fail(POWER_OFF);
		break;

	}
}

static void set_thermal_monitoring(int thermal_monitoring)
{
	int tm_high,tm_low;
	
	rdmsr(MISC_ENABLE,tm_low,tm_high);
	if(thermal_monitoring != THERMAL_MONITORING_OFF) {
		tm_low |= THERMAL_MONITORING_SET;
	}
	else {
		tm_low &= ~THERMAL_MONITORING_SET;
	}
	wrmsr(MISC_ENABLE,tm_low,tm_high);
	return;
}

void mainboard_fixup(void)
{
	int cpu_clock_multiplier;
	int power_on_after_power_fail;
	int thermal_monitoring;

	w83627hf_power_led(LED_ON);
	ich3_enable_ioapic();
	p64h2_enable_ioapic();
	p64h2_setup_pcibridge();
	ich3_enable_serial_irqs();
	ich3_enable_ide(1,1);
	ich3_rtc_init();
	ich3_lpc_route_dma(0xff);
	isa_dma_init();
	ich3_1e0_misc();
	ich3_1f0_misc();
	e7500_pci_parity_enable();
	p64h2_pci_parity_enable();
	ich3_pci_parity_enable();

#if 0   /* CPU clock option is not presently used */
	cpu_clock_multiplier = CPU_CLOCK_MULTIPLIER;
	if(get_option(&cpu_clock_multiplier, "CPU_clock_speed"))
		cpu_clock_multiplier = CPU_CLOCK_MULTIPLIER;
	ich3_set_cpu_multiplier(cpu_clock_multiplier);
#endif

	power_on_after_power_fail = MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	if(get_option(&power_on_after_power_fail, "power_on_after_fail"))
		power_on_after_power_fail = MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	set_power_on_after_power_fail(power_on_after_power_fail);

	thermal_monitoring = THERMAL_MONITORING_OFF;
	if(get_option(&thermal_monitoring, "thermal_monitoring"))
		thermal_monitoring = THERMAL_MONITORING_OFF;
	set_thermal_monitoring(thermal_monitoring);

	return;
}

void hard_reset(void)
{
	ich3_hard_reset();
}

