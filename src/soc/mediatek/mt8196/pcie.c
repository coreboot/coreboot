/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/early_init.h>
#include <soc/pcie.h>

#define PCIE_REG_BASE_PORT0	0x16940000
#define PCIE_RST_CTRL_REG	(PCIE_REG_BASE_PORT0 + 0x148)

#define PEXTP_CFG_BASE		0x169e0000
#define PEXTP_SW_RST_REG	(PEXTP_CFG_BASE + 0x4)
#define PEXTP_SW_RST_MAC_P2	BIT(8)
#define PEXTP_SW_RST_PHY_P2	BIT(9)
#define PEXTP_REQ_CTRL_0_REG	(PEXTP_CFG_BASE + 0x7c)
#define PEXTP_PCIE26M_BYPASS	BIT(4)

static const struct pad_func pcie_pins[2][3] = {
	{
		PAD_FUNC_UP(PCIE0_PERSTN, PCIE_PERSTN),
		PAD_FUNC_UP(PCIE0_WAKEN, PCIE_WAKEN),
		PAD_FUNC_UP(PCIE0_CLKREQN, PCIE_CLKREQN),
	},
	{
		PAD_FUNC_UP(BPI_D_BUS0, PCIE_WAKEN_1P),
		PAD_FUNC_UP(BPI_D_BUS1, PCIE_PERSTN_1P),
		PAD_FUNC_UP(BPI_D_BUS2, PCIE_CLKREQN_1P),
	},
};

static void mtk_pcie_set_pinmux(uint8_t port)
{
	const struct pad_func *pins = pcie_pins[port];
	size_t i;

	for (i = 0; i < ARRAY_SIZE(pcie_pins[port]); i++) {
		gpio_set_mode(pins[i].gpio, pins[i].func);
		gpio_set_pull(pins[i].gpio, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	}
}

void mtk_pcie_pre_init(void)
{
	mtk_pcie_set_pinmux(1);

	/* PCIe 2 is not used, assert it's reset for power saving */
	clrsetbits32p(PEXTP_SW_RST_REG, GENMASK(9, 8),
		      PEXTP_SW_RST_MAC_P2 | PEXTP_SW_RST_PHY_P2);

	/* PCIe 1 and 2 need to bypass PMRC signal */
	setbits32p(PEXTP_REQ_CTRL_0_REG, PEXTP_PCIE26M_BYPASS);

	/* Assert all reset signals at early stage */
	mtk_pcie_reset(PCIE_REG_BASE_PORT0, true);

	early_init_save_time(EARLY_INIT_PCIE);
}
