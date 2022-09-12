/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/clk_rst.h>
#include <soc/ccplex.h>
#include <soc/cpu.h>
#include <soc/flow.h>
#include <soc/mc.h>
#include <soc/pmc.h>
#include <soc/power.h>
#include <soc/romstage.h>
#include <timer.h>

#define PMC_REGS (void *)(uintptr_t)(TEGRA_PMC_BASE)

static void enable_cpu_clocks(void)
{
	clock_enable(CLK_ENB_CPU, 0, 0, SET_CLK_ENB_CPUG_ENABLE |
		     SET_CLK_ENB_CPULP_ENABLE, 0, 0, 0);
}

static void enable_cpu_power_partitions(void)
{
	/* Bring up fast cluster, non-CPU, CPU0, CPU1, CPU2 and CPU3 parts. */
	power_ungate_partition(POWER_PARTID_CRAIL);
	power_ungate_partition(POWER_PARTID_C0NC);
	power_ungate_partition(POWER_PARTID_CE0);

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE)) {
		/*
		 * Deassert reset signal of all the secondary CPUs.
		 * PMC and flow controller will take over the power sequence
		 * controller in the ATF.
		 */
		uint32_t reg = CRC_RST_CPUG_CLR_CPU1 | CRC_RST_CPUG_CLR_DBG1 |
			       CRC_RST_CPUG_CLR_CORE1 | CRC_RST_CPUG_CLR_CX1 |
			       CRC_RST_CPUG_CLR_CPU2 | CRC_RST_CPUG_CLR_DBG2 |
			       CRC_RST_CPUG_CLR_CORE2 | CRC_RST_CPUG_CLR_CX2 |
			       CRC_RST_CPUG_CLR_CPU3 | CRC_RST_CPUG_CLR_DBG3 |
			       CRC_RST_CPUG_CLR_CORE3 | CRC_RST_CPUG_CLR_CX3;
		write32(CLK_RST_REG(rst_cpug_cmplx_clr), reg);
	}
}

static void request_ram_repair(void)
{
	struct flow_ctlr * const flow = (void *)(uintptr_t)TEGRA_FLOW_BASE;
	const uint32_t req = 1 << 0;
	const uint32_t sts = 1 << 1;
	uint32_t reg;
	struct stopwatch sw;

	printk(BIOS_DEBUG, "Requesting RAM repair.\n");

	stopwatch_init(&sw);

	/* Perform RAM repair */
	reg = read32(&flow->ram_repair);
	reg |= req;
	write32(&flow->ram_repair, reg);
	while ((read32(&flow->ram_repair) & sts) != sts)
		;

	printk(BIOS_DEBUG, "RAM repair complete in %lld usecs.\n",
		stopwatch_duration_usecs(&sw));
}

static void set_cpu_ack_width(uint32_t val)
{
	uint32_t reg;

	reg = read32(CLK_RST_REG(cpu_softrst_ctrl2));
	reg &= ~CAR2PMC_CPU_ACK_WIDTH_MASK;
	reg |= val;
	write32(CLK_RST_REG(cpu_softrst_ctrl2), reg);
}

void ccplex_cpu_prepare(void)
{
	enable_cpu_clocks();

	/*
	 * The POR value of CAR2PMC_CPU_ACK_WIDTH is 0x200.
	 * The recommended value is 0.
	 */
	set_cpu_ack_width(0);

	enable_cpu_power_partitions();

	mainboard_configure_pmc();
	mainboard_enable_vdd_cpu();

	request_ram_repair();
}

static void start_common_clocks(void)
{
	/* Clear fast CPU partition reset. */
	write32(CLK_RST_REG(rst_cpug_cmplx_clr), CRC_RST_CPUG_CLR_NONCPU);

	/* Clear reset of L2 and CoreSight components. */
	write32(CLK_RST_REG(rst_cpug_cmplx_clr),
		CRC_RST_CPUG_CLR_L2 | CRC_RST_CPUG_CLR_PDBG);
}

void ccplex_cpu_start(void *entry_addr)
{
	/* Enable common clocks for the shared resources between the cores. */
	start_common_clocks();

	start_cpu(0, entry_addr);
}
