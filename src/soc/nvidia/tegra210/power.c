/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/pmc.h>
#include <soc/power.h>

static struct tegra_pmc_regs *const pmc = (void *)TEGRA_PMC_BASE;

enum {
	POWER_GATE = 0,
	POWER_UNGATE = 1,
};

static int partition_powered(int id)
{
	if (read32(&pmc->pwrgate_status) & (0x1 << id))
		return POWER_UNGATE;

	return POWER_GATE;
}

static const char *const power_gate_string[] = {
	[POWER_GATE] = "Gat",
	[POWER_UNGATE] = "Ungat",
};

static void power_gate_toggle_request(uint32_t id, int request)
{
	printk(BIOS_INFO, "%sing power partition %d.\n",
	       power_gate_string[request], id);

	int part_powered = partition_powered(id);

	if (request == part_powered) {
		printk(BIOS_INFO, "Partition %d already %sed.\n", id,
		       power_gate_string[request]);
		return;
	}

	uint32_t pwrgate_toggle = read32(&pmc->pwrgate_toggle);
	pwrgate_toggle &= ~(PMC_PWRGATE_TOGGLE_PARTID_MASK);
	pwrgate_toggle |= (id << PMC_PWRGATE_TOGGLE_PARTID_SHIFT);
	pwrgate_toggle |= PMC_PWRGATE_TOGGLE_START;
	write32(&pmc->pwrgate_toggle, pwrgate_toggle);

	// Wait for the request to be accepted.
	while (read32(&pmc->pwrgate_toggle) & PMC_PWRGATE_TOGGLE_START)
		;
	printk(BIOS_INFO, "Power gate toggle request accepted.\n");

	while (1) {
		part_powered = partition_powered(id);
		if (request == part_powered) {
			printk(BIOS_INFO, "Partition %d %sed.\n", id,
			       power_gate_string[request]);
			return;
		}
	}
}

void power_gate_partition(uint32_t id)
{
	power_gate_toggle_request(id, POWER_GATE);
}

void power_ungate_partition(uint32_t id)
{
	power_gate_toggle_request(id, POWER_UNGATE);
}

uint8_t pmc_rst_status(void)
{
	return read32(&pmc->rst_status) & PMC_RST_STATUS_SOURCE_MASK;
}

static const char *pmc_rst_status_str[PMC_RST_STATUS_NUM_SOURCES] = {
	[PMC_RST_STATUS_SOURCE_POR] = "POR",
	[PMC_RST_STATUS_SOURCE_WATCHDOG] = "Watchdog",
	[PMC_RST_STATUS_SOURCE_SENSOR] = "Sensor",
	[PMC_RST_STATUS_SOURCE_SW_MAIN] = "SW Main",
	[PMC_RST_STATUS_SOURCE_LP0] = "LP0",
};

void pmc_print_rst_status(void)
{
	uint8_t rst_status = pmc_rst_status();
	assert(rst_status < PMC_RST_STATUS_NUM_SOURCES);
	printk(BIOS_INFO, "PMC Reset Status: %s\n",
	       pmc_rst_status_str[rst_status]);
}

static int partition_clamp_on(int id)
{
	return read32(&pmc->clamp_status) & (1 << id);
}

void remove_clamps(int id)
{
	if (!partition_clamp_on(id))
		return;

	/* Remove clamp */
	write32(&pmc->remove_clamping_cmd, (1 << id));

	/* Wait for clamp off */
	while (partition_clamp_on(id))
		;
}

void pmc_override_pwr_det(uint32_t bits, uint32_t override)
{
	uint32_t val = read32(&pmc->pwr_det_val);
	val &= ~bits;
	val |= (override & bits);
	write32(&pmc->pwr_det_val, val);
}
