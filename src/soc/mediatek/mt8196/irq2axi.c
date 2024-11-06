/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/irq2axi.h>

void irq2axi_disable(void)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	/* disable IRQ2AXI */
	write32p(IRQ2AXI_CFG1, 0x0);

	/* disable mcusys ack */
	clrbits32p(MCUSYS_ACK_REG, MCUSYS_ACK_CLR);

	/* switch to legacy channel */
	clrbits32p(CIRQ_AXI_MODE, CIRQ_AXI_MODE_LEGACY);
}
