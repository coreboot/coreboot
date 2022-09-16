/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/irq.h>

Method(_PRT)
{
	Return(Package() {

		Package(){0x0000FFFF, 0, 0, NPK_INT},
		Package(){0x0000FFFF, 1, 0, PUNIT_INT},
		Package(){0x0002FFFF, 0, 0, GEN_INT},
		Package(){0x0003FFFF, 0, 0, IUNIT_INT},
		Package(){0x000DFFFF, 1, 0, PMC_INT},
		Package(){0x000EFFFF, 0, 0, AUDIO_INT},
		Package(){0x000FFFFF, 0, 0, CSE_INT},
		Package(){0x0011FFFF, 0, 0, ISH_INT},
		Package(){0x0012FFFF, 0, 0, SATA_INT},
#if CONFIG(SOC_INTEL_GEMINILAKE)
		Package(){0x000CFFFF, 0, 0, CNVI_INT},
		Package(){0x0013FFFF, 0, 0, PIRQF_INT},
		Package(){0x0013FFFF, 1, 0, PIRQF_INT},
		Package(){0x0013FFFF, 2, 0, PIRQF_INT},
		Package(){0x0013FFFF, 3, 0, PIRQF_INT},
		Package(){0x0014FFFF, 0, 0, PIRQG_INT},
		Package(){0x0014FFFF, 1, 0, PIRQG_INT},
#else
		Package(){0x0013FFFF, 0, 0, PIRQA_INT},
		Package(){0x0013FFFF, 1, 0, PIRQB_INT},
		Package(){0x0013FFFF, 2, 0, PIRQC_INT},
		Package(){0x0013FFFF, 3, 0, PIRQD_INT},
		Package(){0x0014FFFF, 0, 0, PIRQB_INT},
		Package(){0x0014FFFF, 1, 0, PIRQC_INT},
		Package(){0x0014FFFF, 2, 0, PIRQD_INT},
		Package(){0x0014FFFF, 3, 0, PIRQA_INT},
#endif
		Package(){0x0015FFFF, 0, 0, XHCI_INT},
		Package(){0x0015FFFF, 1, 0, XDCI_INT},
		Package(){0x0016FFFF, 0, 0, I2C0_INT},
		Package(){0x0016FFFF, 1, 0, I2C1_INT},
		Package(){0x0016FFFF, 2, 0, I2C2_INT},
		Package(){0x0016FFFF, 3, 0, I2C3_INT},
		Package(){0x0017FFFF, 0, 0, I2C4_INT},
		Package(){0x0017FFFF, 1, 0, I2C5_INT},
		Package(){0x0017FFFF, 2, 0, I2C6_INT},
		Package(){0x0017FFFF, 3, 0, I2C7_INT},
		Package(){0x0018FFFF, 0, 0, LPSS_UART0_IRQ},
		Package(){0x0018FFFF, 1, 0, LPSS_UART1_IRQ},
		Package(){0x0018FFFF, 2, 0, LPSS_UART2_IRQ},
		Package(){0x0018FFFF, 3, 0, LPSS_UART3_IRQ},
		Package(){0x0019FFFF, 0, 0, SPI0_INT},
		Package(){0x0019FFFF, 1, 0, SPI1_INT},
		Package(){0x0019FFFF, 2, 0, SPI2_INT},
#if !CONFIG(SOC_INTEL_GEMINILAKE)
		Package(){0x001BFFFF, 0, 0, SDCARD_INT},
#endif
		Package(){0x001CFFFF, 0, 0, EMMC_INT},
#if CONFIG(SOC_INTEL_GEMINILAKE)
		Package(){0x001DFFFF, 0, 0, UFS_INT},
#endif
		Package(){0x001EFFFF, 0, 0, SDIO_INT},
		Package(){0x001FFFFF, 0, 0, SMBUS_INT},
	})
}
