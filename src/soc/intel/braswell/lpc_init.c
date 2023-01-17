/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/iomap.h>
#include <soc/pm.h>

#define SUSPEND_CYCLE 1
#define RESUME_CYCLE 0
#define LPC_FAMILY_NUMBER(gpio_pad) (gpio_pad / MAX_FAMILY_PAD_GPIO_NO)
#define LPC_INTERNAL_PAD_NUM(gpio_pad) (gpio_pad %  MAX_FAMILY_PAD_GPIO_NO)
#define LPC_GPIO_OFFSET(gpio_pad)  (FAMILY_PAD_REGS_OFF \
		  + (FAMILY_PAD_REGS_SIZE * LPC_FAMILY_NUMBER(gpio_pad) \
		  + (GPIO_REGS_SIZE * LPC_INTERNAL_PAD_NUM(gpio_pad))))

#define LPC_AD2_MMIO_OFFSET	LPC_GPIO_OFFSET(45)
#define LPC_CLKRUN_MMIO_OFFSET	LPC_GPIO_OFFSET(46)
#define LPC_AD0_MMIO_OFFSET	LPC_GPIO_OFFSET(47)
#define LPC_FRAME_MMIO_OFFSET	LPC_GPIO_OFFSET(48)
#define LPC_AD3_MMIO_OFFSET	LPC_GPIO_OFFSET(50)
#define LPC_AD1_MMIO_OFFSET	LPC_GPIO_OFFSET(52)

/* Value written into pad control reg 0 in early init */
#define PAD_CFG0_NATIVE(mode, term, inv_rx_tx) (PAD_GPIO_DISABLE \
				| PAD_GPIOFG_HI_Z \
				| PAD_MODE_SELECTION(mode) | PAD_PULL(term))

#define PAD_CFG0_NATIVE_PU20K(mode) PAD_CFG0_NATIVE(mode, 9, 0) /* PU 20K */
#define PAD_CFG0_NATIVE_PD20K(mode) PAD_CFG0_NATIVE(mode, 1, 0) /* PD 20K */
#define PAD_CFG0_NATIVE_M1          PAD_CFG0_NATIVE(1, 0, 0)    /* no pull */

/*
 * Configure value in LPC GPIO PADCFG0 registers. This function would be called
 * to configure for low power/restore LPC GPIO lines
 */
static void lpc_gpio_config(u32 cycle)
{
	if (cycle == SUSPEND_CYCLE) { /* Suspend cycle */
		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_FRAME_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD0_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD1_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD2_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD3_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_CLKRUN_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PD20K(1));

	} else { /* Resume cycle */
		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_FRAME_MMIO_OFFSET),
				PAD_CFG0_NATIVE_M1);

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD0_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD1_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD2_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_AD3_MMIO_OFFSET),
				PAD_CFG0_NATIVE_PU20K(1));

		write32((void *)(COMMUNITY_GPSOUTHEAST_BASE + LPC_CLKRUN_MMIO_OFFSET),
				PAD_CFG0_NATIVE_M1);
	}
}

/*
 * Configure LPC GPIO lines for low power
 */
void lpc_set_low_power(void)
{
	lpc_gpio_config(SUSPEND_CYCLE);
}

/*
 * Configure GPIO lines early during romstage.
 */
void lpc_init(void)
{
	uint16_t pm1_sts;
	uint32_t pm1_cnt;
	int slp_type = 0;

	/*
	 * On S3 resume re-initialize GPIO lines which were
	 * configured for low power during S3 entry.
	 */
	pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);

	if (pm1_sts & WAK_STS)
		slp_type = acpi_sleep_from_pm1(pm1_cnt);

	if ((slp_type == ACPI_S3) || (slp_type == ACPI_S5))
		lpc_gpio_config(RESUME_CYCLE);
}
