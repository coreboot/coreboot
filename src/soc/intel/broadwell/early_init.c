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
	pci_write_config32(HOST_BRIDGE, MCHBAR, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, DMIBAR, CONFIG_FIXED_DMIBAR_MMIO_BASE | 1);
	pci_write_config32(HOST_BRIDGE, EPBAR,  CONFIG_FIXED_EPBAR_MMIO_BASE  | 1);

	mchbar_write32(EDRAMBAR, EDRAM_BASE_ADDRESS | 1);
	mchbar_write32(GDXCBAR, GDXC_BASE_ADDRESS | 1);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(HOST_BRIDGE, PAM0, 0x30);
	pci_write_config8(HOST_BRIDGE, PAM1, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM2, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM3, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM4, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM5, 0x33);
	pci_write_config8(HOST_BRIDGE, PAM6, 0x33);
}

void systemagent_early_init(void)
{
	const bool vtd_capable =
		!(pci_read_config32(HOST_BRIDGE, CAPID0_A) & VTD_DISABLE);

	broadwell_setup_bars();

	/* Device enable: IGD and Mini-HD */
	pci_write_config32(HOST_BRIDGE, DEVEN, DEVEN_D0EN | DEVEN_D2EN | DEVEN_D3EN);

	if (vtd_capable) {
		/* setup BARs: zeroize top 32 bits; set enable bit */
		mchbar_write32(GFXVTBAR + 4, GFXVT_BASE_ADDRESS >> 32);
		mchbar_write32(GFXVTBAR + 0, GFXVT_BASE_ADDRESS | 1);
		mchbar_write32(VTVC0BAR + 4, VTVC0_BASE_ADDRESS >> 32);
		mchbar_write32(VTVC0BAR + 0, VTVC0_BASE_ADDRESS | 1);

		/* set PRSCAPDIS, lock GFXVTBAR policy cfg registers */
		u32 reg32;
		reg32 = read32p(GFXVT_BASE_ADDRESS + ARCHDIS);
		write32p(GFXVT_BASE_ADDRESS + ARCHDIS,
				reg32 | DMAR_LCKDN | PRSCAPDIS);
		/* lock VTVC0BAR policy cfg registers */
		reg32 = read32p(VTVC0_BASE_ADDRESS + ARCHDIS);
		write32p(VTVC0_BASE_ADDRESS + ARCHDIS,
				reg32 | DMAR_LCKDN);
	}
}
