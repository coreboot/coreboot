/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>

static void broadwell_setup_bars(void)
{
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(SA_DEV_ROOT, MCHBAR, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);
	pci_write_config32(SA_DEV_ROOT, DMIBAR, CONFIG_FIXED_DMIBAR_MMIO_BASE | 1);
	pci_write_config32(SA_DEV_ROOT, EPBAR,  CONFIG_FIXED_EPBAR_MMIO_BASE  | 1);

	MCHBAR32(EDRAMBAR) = EDRAM_BASE_ADDRESS | 1;
	MCHBAR32(GDXCBAR)  =  GDXC_BASE_ADDRESS | 1;

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(SA_DEV_ROOT, PAM0, 0x30);
	pci_write_config8(SA_DEV_ROOT, PAM1, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM2, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM3, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM4, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM5, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM6, 0x33);
}

void systemagent_early_init(void)
{
	const bool vtd_capable =
		!(pci_read_config32(SA_DEV_ROOT, CAPID0_A) & VTD_DISABLE);

	broadwell_setup_bars();

	/* Device enable: IGD and Mini-HD */
	pci_write_config32(SA_DEV_ROOT, DEVEN, DEVEN_D0EN | DEVEN_D2EN | DEVEN_D3EN);

	if (vtd_capable) {
		/* setup BARs: zeroize top 32 bits; set enable bit */
		MCHBAR32(GFXVTBAR + 4) = GFXVT_BASE_ADDRESS >> 32;
		MCHBAR32(GFXVTBAR) = GFXVT_BASE_ADDRESS | 1;
		MCHBAR32(VTVC0BAR + 4) = VTVC0_BASE_ADDRESS >> 32;
		MCHBAR32(VTVC0BAR) = VTVC0_BASE_ADDRESS | 1;

		/* set PRSCAPDIS, lock GFXVTBAR policy cfg registers */
		u32 reg32;
		reg32 = read32((void *)(GFXVT_BASE_ADDRESS + ARCHDIS));
		write32((void *)(GFXVT_BASE_ADDRESS + ARCHDIS),
				reg32 | DMAR_LCKDN | PRSCAPDIS);
		/* lock VTVC0BAR policy cfg registers */
		reg32 = read32((void *)(VTVC0_BASE_ADDRESS + ARCHDIS));
		write32((void *)(VTVC0_BASE_ADDRESS + ARCHDIS),
				reg32 | DMAR_LCKDN);
	}
}
