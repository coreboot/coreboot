#include <cpu/cpufixup.h>
#include <printk.h>
#include <cpu/i786/thermal_monitoring.h>
#include <cpu/p6/msr.h>
#include <cpu/i786/cpufixup.h>
#include <pc80/mc146818rtc.h>


static int first_time = 0;

static void set_thermal_monitoring(int thermal_monitoring)
{
        int tm_high,tm_low;

        rdmsr(MISC_ENABLE,tm_low,tm_high);
        if(thermal_monitoring != THERMAL_MONITORING_OFF) {
                tm_low |= THERMAL_MONITORING_SET;
		printk_debug("Thermal Monitoring on\n");
        }
        else {
                tm_low &= ~THERMAL_MONITORING_SET;
		printk_debug("Thermal Monitoring off\n");
        }
        wrmsr(MISC_ENABLE,tm_low,tm_high);
        return;
}

static void set_level3_cache(int disabled)
{
        int tm_high,tm_low;

        rdmsr(MISC_ENABLE,tm_low,tm_high);
        if(disabled) {
                tm_low |= L3_CACHE_DISABLE;
                printk_debug("L3 cache disabled\n");
        }
        else {
                tm_low &= ~L3_CACHE_DISABLE;
                printk_debug("L3 cache enabled\n");
        }
        wrmsr(MISC_ENABLE,tm_low,tm_high);
        return;
}



void i786_cpufixup(struct mem_range *mem)
{
        int thermal_monitoring;
        
        printk_debug("Updating microcode\n");
        display_cpuid_update_microcode();

	thermal_monitoring=THERMAL_MONITORING_OFF;
        if(get_option(&thermal_monitoring, "thermal_monitoring")){
                thermal_monitoring = THERMAL_MONITORING_OFF;
	}
        set_thermal_monitoring(thermal_monitoring);

	set_level3_cache(L3_CACHE_DISABLE);	/*This disables L3 cache */


}
