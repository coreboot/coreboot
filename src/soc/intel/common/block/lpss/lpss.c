/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/lpss.h>

/* Clock register */
#define LPSS_CLOCK_CTL_REG	0x200
#define LPSS_CNT_CLOCK_EN	1
#define LPSS_CNT_CLK_UPDATE	(1 << 31)
#define LPSS_CLOCK_DIV_N(n)	(((n) & 0x7fff) << 16)
#define LPSS_CLOCK_DIV_M(m)	(((m) & 0x7fff) << 1)

/* reset register  */
#define LPSS_RESET_CTL_REG	0x204

/*
 * Bit 1:0 controls LPSS controller reset.
 *
 * 00 ->LPSS Host Controller is in reset (Reset Asserted)
 * 01/10 ->Reserved
 * 11 ->LPSS Host Controller is NOT at reset (Reset Released)
 */

#define LPSS_CNT_RST_RELEASE	3

/* DMA Software Reset Control */
#define LPSS_DMA_RST_RELEASE	(1 << 2)

/* Power management control and status register */
#define PME_CTRL_STATUS	0x84
/* Bit 1:0 Powerstate, controls D0 and D3 state */
#define POWER_STATE_MASK	3

bool lpss_is_controller_in_reset(uintptr_t base)
{
	uint8_t *addr = (void *)base;
	uint32_t val = read32(addr + LPSS_RESET_CTL_REG);

	if (val == 0xFFFFFFFF)
		return true;

	return !(val & LPSS_CNT_RST_RELEASE);
}

void lpss_reset_release(uintptr_t base)
{
	uint8_t *addr = (void *)base;

	/* Take controller out of reset */
	write32(addr + LPSS_RESET_CTL_REG, LPSS_CNT_RST_RELEASE);
}

void lpss_clk_update(uintptr_t base, uint32_t clk_m_val, uint32_t clk_n_val)
{
	uint8_t *addr = (void *)base;
	uint32_t clk_sel;

	addr += LPSS_CLOCK_CTL_REG;
	clk_sel = LPSS_CLOCK_DIV_N(clk_n_val) | LPSS_CLOCK_DIV_M(clk_m_val);
	clk_sel |= LPSS_CNT_CLK_UPDATE | LPSS_CNT_CLOCK_EN;

	write32(addr, clk_sel);
}

/* Set controller power state to D0 or D3 */
void lpss_set_power_state(pci_devfn_t devfn, enum lpss_pwr_state state)
{
	uint8_t reg8 = pci_s_read_config8(devfn, PME_CTRL_STATUS);
	reg8 &= ~POWER_STATE_MASK;
	reg8 |= state;
	pci_s_write_config8(devfn, PME_CTRL_STATUS, reg8);
}

bool is_dev_lpss(const struct device *dev)
{
	static size_t size;
	static const pci_devfn_t *lpss_devices;

	if (dev->path.type != DEVICE_PATH_PCI)
		return false;

	if (!lpss_devices)
		lpss_devices = soc_lpss_controllers_list(&size);

	for (int i = 0; i < size; i++) {
		if (lpss_devices[i] == dev->path.pci.devfn)
			return true;
	}
	return false;
}
