// SPDX-License-Identifier: GPL-2.0-or-later

#include <arch/mmio.h>
#include <commonlib/bsd/stdlib.h>
#include <soc/addressmap.h>
#include <soc/qcom_spmi.h>
#include <timer.h>

#define PPID_MASK		(0xfffU << 8)

/* These are opcodes specific to this SPMI arbitrator, *not* SPMI commands. */
#define OPC_EXT_WRITEL		0
#define OPC_EXT_READL		1

#define ARB_STATUS_DONE		BIT(0)
#define ARB_STATUS_FAILURE	BIT(1)
#define ARB_STATUS_DENIED	BIT(2)
#define ARB_STATUS_DROPPED	BIT(3)

#define ERROR_APID_NOT_FOUND	(-(int)BIT(8))
#define ERROR_TIMEOUT		(-(int)BIT(9))

#define ARB_COMMAND_TIMEOUT_MS	100

// Individual register block per APID
struct qcom_spmi_regs {
	uint32_t cmd;
	uint32_t config;
	uint32_t status;
	uint32_t _reserved0;
	uint32_t wdata0;
	uint32_t wdata1;
	uint32_t rdata0;
	uint32_t rdata1;
	uint8_t _reserved_empty_until_next_apid[SPMI_PMIC_ARB_CHANNEL_SIZE - 0x20];
};
check_member(qcom_spmi_regs, rdata1, 0x1c);
_Static_assert(sizeof(struct qcom_spmi_regs) == SPMI_PMIC_ARB_CHANNEL_SIZE,
	       "struct qcom_spmi_regs must be " STRINGIFY(SPMI_PMIC_ARB_CHANNEL_SIZE) " bytes per APID");

struct qcom_spmi {
	struct qcom_spmi_regs *regs_per_apid;	// indexed by APID
	uint32_t *apid_map;
	size_t num_apid;
};

struct qcom_spmi qcom_spmi = {
	(void *) SPMI_PMIC_ARB_CHANNEL_BASE,
	(void *) SPMI_PMIC_ARB_APID_MAP_BASE,
	SPMI_PMIC_ARB_APID_COUNT
};

static struct qcom_spmi_regs *find_apid(uint32_t addr)
{
	size_t i;

	for (i = 0U; i < qcom_spmi.num_apid; i++) {
		uint32_t reg = read32(&qcom_spmi.apid_map[i]);
		if ((reg != 0U) && ((addr & PPID_MASK) == (reg & PPID_MASK)))
			return &qcom_spmi.regs_per_apid[i];
	}

	return NULL;
}

static int wait_for_done(struct qcom_spmi_regs *regs)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, ARB_COMMAND_TIMEOUT_MS);
	while (!stopwatch_expired(&sw)) {
		uint32_t status = read32(&regs->status);
		if ((status & ARB_STATUS_DONE) != 0U) {
			if ((status & ARB_STATUS_FAILURE) != 0U ||
			    (status & ARB_STATUS_DENIED) != 0U ||
			    (status & ARB_STATUS_DROPPED) != 0U)
				return -(int)(status & 0xff);
			return 0;
		}
	}
	printk(BIOS_ERR, "ERROR: SPMI_ARB timeout!\n");
	return ERROR_TIMEOUT;
}

static void arb_command(struct qcom_spmi_regs *regs, uint8_t opcode, uint32_t addr,
			uint8_t bytes)
{
	write32(&regs->cmd, (uint32_t)opcode << 27 |
			    (addr & 0xff) << 4 | (bytes - 1));
}

int spmi_read8(uint32_t addr)
{
	struct qcom_spmi_regs *regs = find_apid(addr);

	if (!regs)
		return ERROR_APID_NOT_FOUND;

	arb_command(regs, OPC_EXT_READL, addr, 1);

	int ret = wait_for_done(regs);
	if (ret != 0) {
		printk(BIOS_ERR, "ERROR: SPMI_ARB read error [0x%x]: 0x%x\n", addr, ret);
		return ret;
	}

	return read32(&regs->rdata0) & 0xff;
}

int spmi_write8(uint32_t addr, uint8_t data)
{
	struct qcom_spmi_regs *regs = find_apid(addr);

	if (!regs)
		return ERROR_APID_NOT_FOUND;

	write32(&regs->wdata0, data);
	arb_command(regs, OPC_EXT_WRITEL, addr, 1);

	int ret = wait_for_done(regs);
	if (ret != 0) {
		printk(BIOS_ERR, "ERROR: SPMI_ARB write error [0x%x] = 0x%x: 0x%x\n",
			addr, data, ret);
	}

	return ret;
}
