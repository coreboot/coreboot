/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <fsp/api.h>
#include <program_loading.h>

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
}

asmlinkage void car_stage_entry(void)
{
	post_code(0x40);
	console_init();

	post_code(0x41);

	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	fsp_memory_init(false); /* no S3 resume yet */

	run_ramstage();
}
