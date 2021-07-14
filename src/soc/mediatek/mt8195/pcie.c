/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/stdlib.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/resource.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/gpio.h>
#include <soc/pcie.h>
#include <soc/pcie_common.h>
#include <stdlib.h>
#include <string.h>

#define PCIE_REG_BASE_PORT0	0x112f0000
#define PCIE_RST_CTRL_REG	(PCIE_REG_BASE_PORT0 + 0x148)
#define PCIE_MAC_RSTB		BIT(0)
#define PCIE_PHY_RSTB		BIT(1)
#define PCIE_BRG_RSTB		BIT(2)
#define PCIE_PE_RSTB		BIT(3)


/* MMIO range (64MB): 0x20000000 ~ 0x24000000 */
/* Some devices still need io ranges, reserve 16MB for compatibility */
static const struct mtk_pcie_mmio_res pcie_mmio_res_io = {
	.cpu_addr = 0x20000000,
	.pci_addr = 0x20000000,
	.size = 16 * MiB,
	.type = IORESOURCE_IO,
};

static const struct mtk_pcie_mmio_res pcie_mmio_res_mem = {
	.cpu_addr = 0x21000000,
	.pci_addr = 0x21000000,
	.size = 48 * MiB,
	.type = IORESOURCE_MEM,
};

struct pad_func {
	gpio_t gpio;
	u8 func;
};

#define PAD_FUNC(name, func) {GPIO(name), PAD_##name##_FUNC_##func}

static const struct pad_func pcie_pins[2][3] = {
	{
		PAD_FUNC(PCIE_WAKE_N, WAKEN),
		PAD_FUNC(PCIE_PERESET_N, PERSTN),
		PAD_FUNC(PCIE_CLKREQ_N, CLKREQN),
	},
	{
		PAD_FUNC(CMMCLK0, PERSTN_1),
		PAD_FUNC(CMMCLK1, CLKREQN_1),
		PAD_FUNC(CMMCLK2, WAKEN_1),
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

static void mtk_pcie_reset(uintptr_t reg, bool enable)
{
	uint32_t val;

	val = read32p(reg);

	if (enable)
		val |= PCIE_MAC_RSTB | PCIE_PHY_RSTB | PCIE_BRG_RSTB |
		       PCIE_PE_RSTB;
	else
		val &= ~(PCIE_MAC_RSTB | PCIE_PHY_RSTB | PCIE_BRG_RSTB |
			 PCIE_PE_RSTB);

	write32p(reg, val);
}

void mtk_pcie_pre_init(void)
{
	mtk_pcie_set_pinmux(0);

	/* Assert all reset signals at early stage */
	mtk_pcie_reset(PCIE_RST_CTRL_REG, true);
}

void mtk_pcie_get_hw_info(struct mtk_pcie_controller *ctrl)
{
	ctrl->base = PCIE_REG_BASE_PORT0;
	ctrl->mmio_res_io = &pcie_mmio_res_io;
	ctrl->mmio_res_mem = &pcie_mmio_res_mem;
	ctrl->reset = &mtk_pcie_reset;
}
