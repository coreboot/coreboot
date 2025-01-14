/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/pmif_spi.h>
#include <soc/pmif_spmi.h>
#include <soc/pmif_sw.h>
#include <soc/spmi.h>

const struct pmif pmif_spmi_arb[] = {
	{
		.mtk_pmif = (struct mtk_pmif_regs *)PMIF_SPMI_BASE,
		.ch = (struct chan_regs *)PMIF_SPMI_AP_CHAN,
		.mstid = SPMI_MASTER_0,
		.pmifid = PMIF_SPMI,
		.write = pmif_spmi_write,
		.read = pmif_spmi_read,
		.write_field = pmif_spmi_write_field,
		.read_field = pmif_spmi_read_field,
		.check_init_done = pmif_check_init_done,
	},
};

const size_t pmif_spmi_arb_count = ARRAY_SIZE(pmif_spmi_arb);

const struct pmif pmif_spi_arb[] = {
	{
		.mtk_pmif = (struct mtk_pmif_regs *)PMIF_SPI_BASE,
		.ch = (struct chan_regs *)PMIF_SPI_AP_CHAN,
		.pmifid = PMIF_SPI,
		.write = pmif_spi_write,
		.read = pmif_spi_read,
		.write_field = pmif_spi_write_field,
		.read_field = pmif_spi_read_field,
		.check_init_done = pmif_check_init_done,
	},
};

const size_t pmif_spi_arb_count = ARRAY_SIZE(pmif_spi_arb);

int mtk_pmif_init(void)
{
	int ret;

	ret = pmif_clk_init();
	if (!ret)
		ret = pmif_spmi_init(get_pmif_controller(PMIF_SPMI, SPMI_MASTER_0));
	if (!ret && !CONFIG(PWRAP_WITH_PMIF_SPMI))
		ret = pmif_spi_init(get_pmif_controller(PMIF_SPI, 0));

	return ret;
}
