/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <romstage_handoff.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <arch/romstage.h>
#include <northbridge/intel/gm45/gm45.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmclib.h>
#include <southbridge/intel/common/pmutil.h>
#include <string.h>

#define LPC_DEV PCI_DEV(0, 0x1f, 0)
#define MCH_DEV PCI_DEV(0, 0, 0)

void __weak mb_setup_superio(void)
{
}

void __weak mb_pre_raminit_setup(sysinfo_t *sysinfo)
{
}

void __weak mb_post_raminit_setup(void)
{
}

/* Platform has no romstage entry point under mainboard directory,
 * so this one is named with prefix mainboard.
 */
void mainboard_romstage_entry(void)
{
	sysinfo_t sysinfo;
	int s3resume = 0;
	int cbmem_initted;
	u16 reg16;

	/* basic northbridge setup, including MMCONF BAR */
	gm45_early_init();

	/* First, run everything needed for console output. */
	i82801ix_early_init();
	setup_pch_gpios(&mainboard_gpio_map);

	reg16 = pci_read_config16(LPC_DEV, D31F0_GEN_PMCON_3);
	pci_write_config16(LPC_DEV, D31F0_GEN_PMCON_3, reg16);
	if ((mchbar_read16(SSKPD_MCHBAR) == 0xcafe) && !(reg16 & (1 << 9))) {
		printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
		gm45_early_reset();
	}

	/* ASPM related setting, set early by original BIOS. */
	dmibar_clrbits16(0x204, 3 << 10);

	/* Check for S3 resume. */
	s3resume = southbridge_detect_s3_resume();

	/* RAM initialization */
	enter_raminit_or_reset();
	memset(&sysinfo, 0, sizeof(sysinfo));
	get_mb_spd_addrmap(sysinfo.spd_map);
	const struct device *dev;
	dev = pcidev_on_root(2, 0);
	if (dev)
		sysinfo.enable_igd = dev->enabled;
	dev = pcidev_on_root(1, 0);
	if (dev)
		sysinfo.enable_peg = dev->enabled;
	get_gmch_info(&sysinfo);

	mb_pre_raminit_setup(&sysinfo);

	raminit(&sysinfo, s3resume);

	mb_post_raminit_setup();

	/* Disable D4F0 (unknown signal controller). */
	pci_and_config32(MCH_DEV, D0F0_DEVEN, ~0x4000);

	init_pm(&sysinfo, 0);

	i82801ix_dmi_setup();
	gm45_late_init(sysinfo.stepping);
	i82801ix_dmi_poll_vc1();

	mchbar_write16(SSKPD_MCHBAR, 0xcafe);

	init_iommu();

	cbmem_initted = !cbmem_recovery(s3resume);

	romstage_handoff_init(cbmem_initted && s3resume);

	printk(BIOS_SPEW, "exit main()\n");
}
