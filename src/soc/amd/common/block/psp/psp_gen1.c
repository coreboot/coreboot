/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/amd/msr.h>
#include <cpu/x86/msr.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <cbfs.h>
#include <region_file.h>
#include <timer.h>
#include <console/console.h>
#include <amdblocks/psp.h>
#include <soc/iomap.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include "psp_def.h"

#define PSP_MAILBOX_OFFSET		0x70

/* generic PSP interface status, v1 */
#define PSPV1_STATUS_INITIALIZED	BIT(0)
#define PSPV1_STATUS_ERROR		BIT(1)
#define PSPV1_STATUS_TERMINATED		BIT(2)
#define PSPV1_STATUS_HALT		BIT(3)
#define PSPV1_STATUS_RECOVERY		BIT(4)

/*
 * pspv1_mbox consists of hardware registers beginning at PSPx000070
 *   mbox_command: BIOS->PSP command, cleared by PSP when complete
 *   mbox_status:  BIOS->PSP interface status
 *   cmd_response: pointer to command/response buffer
 */
struct pspv1_mbox {
	u32 mbox_command;
	u32 mbox_status;
	u64 cmd_response; /* definition conflicts w/BKDG but matches agesa */
} __packed;

static void *soc_get_mbox_address(void)
{
	uintptr_t psp_mmio;

	/* Check for presence of the PSP */
	if (pci_read_config32(SOC_PSP_DEV, PCI_VENDOR_ID) == 0xffffffff) {
		printk(BIOS_WARNING, "PSP: No SOC_PSP_DEV found at D%xF%x\n",
			PSP_DEV, PSP_FUNC);
		return 0;
	}

	/* Determine if Bar3Hide has been set, and if hidden get the base from
	 * the MSR instead. */
	if (pci_read_config32(SOC_PSP_DEV, PSP_BAR_ENABLES) & BAR3HIDE) {
		psp_mmio = rdmsr(PSP_ADDR_MSR).lo;
		if (!psp_mmio) {
			printk(BIOS_WARNING, "PSP: BAR hidden, PSP_ADDR_MSR uninitialized\n");
			return 0;
		}
	} else {
		psp_mmio = pci_read_config32(SOC_PSP_DEV, PSP_MAILBOX_BAR) &
				~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	}

	return (void *)(psp_mmio + PSP_MAILBOX_OFFSET);
}

static u32 rd_mbox_sts(struct pspv1_mbox *mbox)
{
	return read32(&mbox->mbox_status);
}

static void wr_mbox_cmd(struct pspv1_mbox *mbox, u32 cmd)
{
	write32(&mbox->mbox_command, cmd);
}

static u32 rd_mbox_cmd(struct pspv1_mbox *mbox)
{
	return read32(&mbox->mbox_command);
}

static void wr_mbox_cmd_resp(struct pspv1_mbox *mbox, void *buffer)
{
	write64(&mbox->cmd_response, (uintptr_t)buffer);
}

static int wait_initialized(struct pspv1_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_INIT_TIMEOUT);

	do {
		if (rd_mbox_sts(mbox) & PSPV1_STATUS_INITIALIZED)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_INIT_TIMEOUT;
}

static int wait_command(struct pspv1_mbox *mbox)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PSP_CMD_TIMEOUT);

	do {
		if (!rd_mbox_cmd(mbox))
			return 0;
	} while (!stopwatch_expired(&sw));

	return -PSPSTS_CMD_TIMEOUT;
}

int send_psp_command(u32 command, void *buffer)
{
	struct pspv1_mbox *mbox = soc_get_mbox_address();
	if (!mbox)
		return -PSPSTS_NOBASE;

	/* check for PSP error conditions */
	if (rd_mbox_sts(mbox) & PSPV1_STATUS_HALT)
		return -PSPSTS_HALTED;

	if (rd_mbox_sts(mbox) & PSPV1_STATUS_RECOVERY)
		return -PSPSTS_RECOVERY;

	/* PSP must be finished with init and ready to accept a command */
	if (wait_initialized(mbox))
		return -PSPSTS_INIT_TIMEOUT;

	if (wait_command(mbox))
		return -PSPSTS_CMD_TIMEOUT;

	/* set address of command-response buffer and write command register */
	wr_mbox_cmd_resp(mbox, buffer);
	wr_mbox_cmd(mbox, command);

	/* PSP clears command register when complete */
	if (wait_command(mbox))
		return -PSPSTS_CMD_TIMEOUT;

	/* check delivery status */
	if (rd_mbox_sts(mbox) & (PSPV1_STATUS_ERROR | PSPV1_STATUS_TERMINATED))
		return -PSPSTS_SEND_ERROR;

	return 0;
}

/*
 * Tell the PSP to load a firmware blob from a location in the BIOS image.
 */
int psp_load_named_blob(enum psp_blob_type type, const char *name)
{
	int cmd_status;
	u32 command;
	void *blob;

	switch (type) {
	case BLOB_SMU_FW:
		command = MBOX_BIOS_CMD_SMU_FW;
		break;
	case BLOB_SMU_FW2:
		command = MBOX_BIOS_CMD_SMU_FW2;
		break;
	default:
		printk(BIOS_ERR, "BUG: Invalid PSP blob type %x\n", type);
		return -PSPSTS_INVALID_BLOB;
	}

	/* type can only be BLOB_SMU_FW or BLOB_SMU_FW2 here, so don't re-check for this */
	if (!CONFIG(SOC_AMD_PSP_SELECTABLE_SMU_FW)) {
		printk(BIOS_ERR, "BUG: Selectable firmware is not supported\n");
		return -PSPSTS_UNSUPPORTED;
	}

	blob = cbfs_map(name, NULL);
	if (!blob) {
		printk(BIOS_ERR, "BUG: Cannot map blob for PSP loading\n");
		return -PSPSTS_INVALID_NAME;
	}

	printk(BIOS_DEBUG, "PSP: Load blob type %x from @%p... ", type, blob);

	/* Blob commands use the buffer registers as data, not pointer to buf */
	cmd_status = send_psp_command(command, blob);
	psp_print_cmd_status(cmd_status, NULL);

	cbfs_unmap(blob);
	return cmd_status;
}

/*
 * Notify the PSP that DRAM is present.  Upon receiving this command, the PSP
 * will load its OS into fenced DRAM that is not accessible to the x86 cores.
 */
int psp_notify_dram(void)
{
	int cmd_status;
	struct mbox_default_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};

	printk(BIOS_DEBUG, "PSP: Notify that DRAM is available... ");

	cmd_status = send_psp_command(MBOX_BIOS_CMD_DRAM_INFO, &buffer);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &buffer.header);

	return cmd_status;
}
