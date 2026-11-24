/* SPDX-License-Identifier: GPL-2.0-only */

#include "opal_nvme.h"

#include <console/console.h>
#include <commonlib/bsd/helpers.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <string.h>
#include <timer.h>

#define NVME_ADMIN_SECURITY_SEND_CMD	0x81
#define NVME_ADMIN_SECURITY_RECV_CMD	0x82

#define NVME_CC_EN		(1U << 0)
#define NVME_CC_CSS_NVM		(0U << 4)
#define NVME_CC_MPS_4K		(0U << 7)
#define NVME_CC_AMS_RR		(0U << 11)
#define NVME_CC_SHN_NONE	(0U << 14)
#define NVME_CC_IOSQES_64B	(6U << 16)
#define NVME_CC_IOCQES_16B	(4U << 20)

#define NVME_QUEUE_SIZE		2
#define NVME_PAGE_SIZE		4096U
#define NVME_SCRATCH_PAGES	3U
#define NVME_SCRATCH_MIN_BYTES	(NVME_SCRATCH_PAGES * NVME_PAGE_SIZE)

#define NVME_REGS_MIN_SIZE	0x2000U

struct nvme_sq_entry {
	u32 dw[16];
};

struct nvme_cq_entry {
	u32 dw[4];
};

/* Use the public definition from opal_nvme.h. */

static int nvme_wait_ready(u8 *regs, bool ready)
{
	const u64 cap = read64(regs + 0x0);
	const int timeout_units = (cap >> 24) & 0xff;
	int timeout_ms = MAX(timeout_units, 1) * 500;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);
	while (!stopwatch_expired(&sw)) {
		const u32 csts = read32(regs + 0x1c);
		const bool rdy = !!(csts & 0x1);
		const bool cfs = !!(csts & 0x2);

		if (cfs)
			return -1;
		if (rdy == ready)
			return 0;

		mdelay(10);
	}

	return -1;
}

static int nvme_read_bar0(pci_devfn_t dev, u64 *bar_out)
{
	const u32 bar_low = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	const bool is_mem = ((bar_low & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY);
	const bool is_64 = ((bar_low & PCI_BASE_ADDRESS_MEM_LIMIT_MASK) ==
				    PCI_BASE_ADDRESS_MEM_LIMIT_64);
	u64 bar;

	/*
	 * Callers must ensure memory decoding is enabled before attempting to
	 * access the BAR.
	 */
	if (!(pci_read_config16(dev, PCI_COMMAND) & PCI_COMMAND_MEMORY))
		return -1;

	if (bar_low == 0xffffffff)
		return -1;

	if (!is_mem)
		return -1;

	bar = bar_low & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	if (is_64) {
		const u32 bar_high = pci_read_config32(dev, PCI_BASE_ADDRESS_0 + 4);
		bar |= ((u64)bar_high) << 32;

		if (!ENV_X86_64 && bar_high)
			return -1;
	}

	if (!bar)
		return -1;

	if (bar > (u64)(uintptr_t)~0)
		return -1;

	if (smm_points_to_smram((void *)(uintptr_t)bar, NVME_REGS_MIN_SIZE))
		return -1;

	*bar_out = bar;
	return 0;
}

static int nvme_admin_cmd(struct opal_nvme *nvme, const struct nvme_sq_entry *cmd)
{
	struct nvme_sq_entry *s = &nvme->sq[nvme->sq_tail];
	struct nvme_cq_entry *c = &nvme->cq[nvme->cq_head];
	struct stopwatch sw;
	bool timed_out = false;

	memcpy(s, cmd, sizeof(*cmd));
	nvme->sq_tail = (nvme->sq_tail + 1) & (NVME_QUEUE_SIZE - 1);
	write32(nvme->sq_db, nvme->sq_tail);

	/* Wait for phase toggle indicating a new completion entry. */
	stopwatch_init_msecs_expire(&sw, 1000);
	while ((((read32(&c->dw[3]) >> 16) & 0x1) == nvme->cq_phase)) {
		if (stopwatch_expired(&sw)) {
			timed_out = true;
			break;
		}
		mdelay(1);
	}

	if (timed_out) {
		printk(BIOS_ERR, "OPAL NVMe: admin cmd timeout\n");
		return -1;
	}

	nvme->cq_head = (nvme->cq_head + 1) & (NVME_QUEUE_SIZE - 1);
	write32(nvme->cq_db, nvme->cq_head);
	if (nvme->cq_head == 0)
		nvme->cq_phase ^= 1;

	/* DW3 bits 31:17 are Status Field (SC + SCT + etc). */
	return (int)(c->dw[3] >> 17);
}

int opal_nvme_init(struct opal_nvme *nvme, pci_devfn_t dev, void *scratch, size_t scratch_size)
{
	u64 regs;
	u16 pmcap;
	u8 *p = scratch;
	u32 cc;
	u32 csts;

	memset(nvme, 0, sizeof(*nvme));
	nvme->dev = dev;

	if (pci_read_config16(dev, PCI_VENDOR_ID) == 0xffff)
		return -1;

	if (scratch_size < NVME_SCRATCH_MIN_BYTES)
		return -1;

	p = (u8 *)ALIGN_UP((uintptr_t)p, NVME_PAGE_SIZE);
	if ((uintptr_t)p + NVME_SCRATCH_MIN_BYTES > (uintptr_t)scratch + scratch_size)
		return -1;

	nvme->sq = (struct nvme_sq_entry *)p;
	p += NVME_PAGE_SIZE;
	nvme->cq = (struct nvme_cq_entry *)p;
	p += NVME_PAGE_SIZE;

	/* Bring device to D0 if it supports PM capability. */
	pmcap = pci_s_find_capability(dev, PCI_CAP_ID_PM);
	if (pmcap) {
		u16 pmcsr = pci_read_config16(dev, pmcap + PCI_PM_CTRL);
		if ((pmcsr & PCI_PM_CTRL_STATE_MASK) != PCI_PM_CTRL_POWER_STATE_D0) {
			pmcsr &= ~PCI_PM_CTRL_STATE_MASK;
			pci_write_config16(dev, pmcap + PCI_PM_CTRL, pmcsr);
			udelay(1000);
		}
	}

	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MEMORY);

	if (nvme_read_bar0(dev, &regs)) {
		printk(BIOS_ERR, "OPAL NVMe: invalid BAR0\n");
		pci_and_config16(dev, PCI_COMMAND, ~(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));
		return -1;
	}

	nvme->regs = (u8 *)(uintptr_t)regs;
	nvme->cap = read64(nvme->regs + 0x0);
	nvme->dstrd_bytes = 4U << ((nvme->cap >> 32) & 0xf);
	/* The completion doorbell can lie beyond the initial register range. */
	const size_t register_size = MAX(NVME_REGS_MIN_SIZE,
					 0x1000U + nvme->dstrd_bytes + sizeof(u32));
	if (smm_points_to_smram(nvme->regs, register_size)) {
		printk(BIOS_ERR, "OPAL NVMe: doorbell overlaps SMRAM or wraps\n");
		pci_and_config16(dev, PCI_COMMAND, ~(PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER));
		return -1;
	}
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);
	nvme->sq_db = (u32 *)(nvme->regs + 0x1000 + (0 * nvme->dstrd_bytes));
	nvme->cq_db = (u32 *)(nvme->regs + 0x1000 + (1 * nvme->dstrd_bytes));

	/*
	 * If controller is already running, avoid disturbing it when it is
	 * already configured for our private scratch queues and appears idle.
	 * Otherwise, reinitialize to private queues to avoid interacting with
	 * OS-owned admin queues.
	 */
	cc = read32(nvme->regs + 0x14);
	csts = read32(nvme->regs + 0x1c);

	if ((cc & NVME_CC_EN) && (csts & 0x1)) {
		const u64 asq = read64(nvme->regs + 0x28);
		const u64 acq = read64(nvme->regs + 0x30);
		const u32 aqa = read32(nvme->regs + 0x24);
		const u32 expected_aqa = ((NVME_QUEUE_SIZE - 1) << 16) | (NVME_QUEUE_SIZE - 1);
		const u32 sq_tail = read32(nvme->sq_db) & 0xffff;
		const u32 cq_head = read32(nvme->cq_db) & 0xffff;

		if (asq == (u64)(uintptr_t)nvme->sq && acq == (u64)(uintptr_t)nvme->cq &&
		    aqa == expected_aqa && sq_tail == 0 && cq_head == 0) {
			nvme->sq_tail = 0;
			nvme->cq_head = 0;
			nvme->cq_phase = (read32(&nvme->cq[0].dw[3]) >> 16) & 0x1;
			return 0;
		}

		printk(BIOS_DEBUG, "OPAL NVMe: controller running, reinitializing\n");
	}

	/* Disable controller and wait for RDY=0. */
	write32(nvme->regs + 0x14, 0);
	if (nvme_wait_ready(nvme->regs, false)) {
		printk(BIOS_ERR, "OPAL NVMe: failed to disable controller\n");
		return -1;
	}

	/* Now that the controller is disabled, it is safe to clear our queues. */
	memset(nvme->sq, 0, NVME_PAGE_SIZE);
	memset(nvme->cq, 0, NVME_PAGE_SIZE);

	/* Admin queue attributes: (CQ size-1)<<16 | (SQ size-1). */
	write32(nvme->regs + 0x24, ((NVME_QUEUE_SIZE - 1) << 16) | (NVME_QUEUE_SIZE - 1));
	write64(nvme->regs + 0x28, (u64)(uintptr_t)nvme->sq);
	write64(nvme->regs + 0x30, (u64)(uintptr_t)nvme->cq);

	/* Enable controller. */
	write32(nvme->regs + 0x14, NVME_CC_EN | NVME_CC_CSS_NVM | NVME_CC_MPS_4K |
				 NVME_CC_AMS_RR | NVME_CC_SHN_NONE |
				 NVME_CC_IOSQES_64B | NVME_CC_IOCQES_16B);
	if (nvme_wait_ready(nvme->regs, true)) {
		printk(BIOS_ERR, "OPAL NVMe: controller not ready\n");
		return -1;
	}
	nvme->sq_tail = 0;
	nvme->cq_head = 0;
	nvme->cq_phase = 0;

	return 0;
}

void opal_nvme_deinit(struct opal_nvme *nvme)
{
	/* Keep controller state as-is; OS will reinitialize if needed. */
	(void)nvme;
}

static int nvme_security_cmd(struct opal_nvme *nvme, u8 opcode, u8 protocol, u16 sp_specific,
			     const void *buf, size_t buf_size)
{
	struct nvme_sq_entry cmd = { 0 };
	const u64 prp1 = (u64)(uintptr_t)buf;

	cmd.dw[0] = opcode; /* CID is left as 0 */
	cmd.dw[1] = 0;      /* NSID = 0 (controller) */
	cmd.dw[6] = (u32)prp1;
	cmd.dw[7] = (u32)(prp1 >> 32);
	/*
	 * NVMe command dwords are little-endian on x86. The Security Protocol
	 * Specific field is a numeric value in the command; do not byte-swap it.
	 */
	cmd.dw[10] = ((u32)protocol << 24) | ((u32)sp_specific << 8);
	cmd.dw[11] = (u32)buf_size;

	return nvme_admin_cmd(nvme, &cmd);
}

int opal_nvme_security_send(struct opal_nvme *nvme, u8 protocol, u16 sp_specific,
			    const void *buf, size_t buf_size)
{
	return nvme_security_cmd(nvme, NVME_ADMIN_SECURITY_SEND_CMD, protocol, sp_specific,
				 buf, buf_size);
}

int opal_nvme_security_recv(struct opal_nvme *nvme, u8 protocol, u16 sp_specific,
			    void *buf, size_t buf_size, size_t *xfer_out)
{
	int ret;

	if (xfer_out)
		*xfer_out = 0;

	ret = nvme_security_cmd(nvme, NVME_ADMIN_SECURITY_RECV_CMD, protocol, sp_specific,
				buf, buf_size);
	if (ret)
		return ret;

	if (xfer_out)
		*xfer_out = buf_size;

	return 0;
}
