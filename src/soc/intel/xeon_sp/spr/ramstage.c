/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <drivers/ocp/include/vpd.h>
#include <soc/ramstage.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <soc/msr.h>

unsigned int smbios_cpu_get_voltage(void)
{
	return 16; /* Per SMBIOS spec, voltage times 10 */
}

unsigned int smbios_cpu_get_current_speed_mhz(void)
{
	msr_t msr;
	msr = rdmsr(MSR_PLATFORM_INFO);
	return ((msr.lo >> 8) & 0xff) * CONFIG_CPU_BCLK_MHZ;
}

__weak void mainboard_override_fsp_gpio(void)
{
	/* Default weak implementation */
}

__weak void mainboard_override_usb_oc(void)
{
	/* Default weak implementation */
}
