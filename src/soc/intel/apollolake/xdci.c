/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/xdci.h>
#include <soc/pci_devs.h>
#include <timer.h>

#define DUAL_ROLE_CFG0		0x80d8
# define DRD_CONFIG_MASK	(0x3 << 0)
# define DRD_CONFIG_DYNAMIC	(0x0 << 0)
# define DRD_CONFIG_HOST	(0x1 << 0)
# define DRD_CONFIG_DEVICE	(0x2 << 0)
# define SW_VBUS_VALID_MASK	(1 << 24)
# define SW_VBUS_DEASSERT_VALID	(0 << 24)
# define SW_VBUS_ASSERT_VALID	(1 << 24)
# define SW_IDPIN_EN_MASK	(1 << 21)
# define SW_IDPIN_DIS		(0 << 21)
# define SW_IDPIN_EN		(1 << 21)
# define SW_IDPIN_MASK		(1 << 20)
# define SW_IDPIN_HOST		(0 << 20)
# define SW_IDPIN_DEVICE	(1 << 20)
#define DUAL_ROLE_CFG1		0x80dc
# define DRD_MODE_MASK		(1 << 29)
# define DRD_MODE_DEVICE	(0 << 29)
# define DRD_MODE_HOST		(1 << 29)

static void configure_host_mode_port0(struct device *dev)
{
	uint32_t *cfg0;
	uint32_t *cfg1;
	const struct resource *res;
	uint32_t reg;
	struct stopwatch sw;

	/*
	 * Only default to host mode if the xdci device is present and
	 * enabled. If it's disabled assume the switch was already done
	 * in FSP.
	 */
	if (!dev->enabled)
		return;

	printk(BIOS_INFO, "Putting port 0 into host mode.\n");

	res = find_resource(dev, PCI_BASE_ADDRESS_0);

	cfg0 = (void *)(uintptr_t)(res->base + DUAL_ROLE_CFG0);
	cfg1 = (void *)(uintptr_t)(res->base + DUAL_ROLE_CFG1);

	reg = read32(cfg0);
	reg &= ~(DRD_CONFIG_MASK | SW_IDPIN_EN_MASK | SW_IDPIN_MASK);
	reg &= ~(SW_VBUS_VALID_MASK);
	reg |= DRD_CONFIG_DYNAMIC | SW_IDPIN_EN | SW_IDPIN_HOST;
	reg |= SW_VBUS_DEASSERT_VALID;
	write32(cfg0, reg);

	stopwatch_init_msecs_expire(&sw, 10);

	/* Wait for the host mode status bit. */
	while ((read32(cfg1) & DRD_MODE_MASK) != DRD_MODE_HOST) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_INFO, "Timed out waiting for host mode.\n");
			break;
		}
	}

	printk(BIOS_INFO, "XDCI port 0 host switch over took %lld ms\n",
		stopwatch_duration_msecs(&sw));
}

void soc_xdci_init(struct device *dev)
{
	configure_host_mode_port0(dev);
}
