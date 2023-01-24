/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <intelblocks/cse.h>
#include <soc/me.h>
#include <types.h>

static void dump_me_status(void *unused)
{
	union me_hfsts1 hfsts1;
	union me_hfsts2 hfsts2;
	union me_hfsts3 hfsts3;
	union me_hfsts4 hfsts4;
	union me_hfsts5 hfsts5;
	union me_hfsts6 hfsts6;

	if (!is_cse_enabled())
		return;

	hfsts1.data = me_read_config32(PCI_ME_HFSTS1);
	hfsts2.data = me_read_config32(PCI_ME_HFSTS2);
	hfsts3.data = me_read_config32(PCI_ME_HFSTS3);
	hfsts4.data = me_read_config32(PCI_ME_HFSTS4);
	hfsts5.data = me_read_config32(PCI_ME_HFSTS5);
	hfsts6.data = me_read_config32(PCI_ME_HFSTS6);

	printk(BIOS_DEBUG, "ME: HFSTS1                      : 0x%08X\n", hfsts1.data);
	printk(BIOS_DEBUG, "ME: HFSTS2                      : 0x%08X\n", hfsts2.data);
	printk(BIOS_DEBUG, "ME: HFSTS3                      : 0x%08X\n", hfsts3.data);
	printk(BIOS_DEBUG, "ME: HFSTS4                      : 0x%08X\n", hfsts4.data);
	printk(BIOS_DEBUG, "ME: HFSTS5                      : 0x%08X\n", hfsts5.data);
	printk(BIOS_DEBUG, "ME: HFSTS6                      : 0x%08X\n", hfsts6.data);

	/*
	 * Lock Descriptor, and Fuses must be programmed on a
	 * production system to indicate ME Manufacturing mode is disabled.
	 */
	printk(BIOS_DEBUG, "ME: Manufacturing Mode          : %s\n",
		((hfsts1.fields.mfg_mode == 0) &&
		(hfsts6.fields.fpf_soc_lock == 1)) ? "NO" : "YES");
	/*
	 * The SPI Protection Mode bit reflects SPI descriptor
	 * locked(0) or unlocked(1).
	 */
	printk(BIOS_DEBUG, "ME: SPI Protection Mode Enabled : %s\n",
		hfsts1.fields.mfg_mode ? "NO" : "YES");
	printk(BIOS_DEBUG, "ME: FW Partition Table          : %s\n",
		hfsts1.fields.fpt_bad ? "BAD" : "OK");
	printk(BIOS_DEBUG, "ME: Bringup Loader Failure      : %s\n",
		hfsts1.fields.ft_bup_ld_flr ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Firmware Init Complete      : %s\n",
		hfsts1.fields.fw_init_complete ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Boot Options Present        : %s\n",
		hfsts1.fields.boot_options_present ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Update In Progress          : %s\n",
		hfsts1.fields.update_in_progress ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: D0i3 Support                : %s\n",
		hfsts1.fields.d0i3_support_valid ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Low Power State Enabled     : %s\n",
		hfsts2.fields.low_power_state ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Replaced                : %s\n",
		hfsts2.fields.cpu_replaced  ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Replacement Valid       : %s\n",
		hfsts2.fields.cpu_replaced_valid ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Current Working State       : %u\n",
		hfsts1.fields.working_state);
	printk(BIOS_DEBUG, "ME: Current Operation State     : %u\n",
		hfsts1.fields.operation_state);
	printk(BIOS_DEBUG, "ME: Current Operation Mode      : %u\n",
		hfsts1.fields.operation_mode);
	printk(BIOS_DEBUG, "ME: Error Code                  : %u\n",
		hfsts1.fields.error_code);
	printk(BIOS_DEBUG, "ME: Enhanced Debug Mode         : %s\n",
		hfsts1.fields.invoke_enhance_dbg_mode ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Debug Disabled          : %s\n",
		hfsts6.fields.cpu_debug_disable ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: TXT Support                 : %s\n",
		hfsts6.fields.txt_support ? "YES" : "NO");
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_EXIT, print_me_fw_version, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME_CHECK, BS_ON_EXIT, dump_me_status, NULL);
