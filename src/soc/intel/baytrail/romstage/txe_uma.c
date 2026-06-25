/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * TXE UMA register handshake.
 *
 * The TXE ROM advertises a UMA size in TXE PCI config space (B0:D26:F0). Read
 * it before memory init, carve that much DRAM at the top, then bridge the
 * carved range back to the TXE with a DRAM Init Done.
 *
 * txe_uma_get_size_mb() reads the requested size and must run before memory
 * init. txe_uma_did() programs the SATT1 bridge to the carved base and sends
 * the DRAM Init Done. The carve itself is done by the memory init code, not
 * here.
 */

#include <cf9_reset.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <types.h>

#define TXE_PCI_DEV			PCI_DEV(0, TXE_DEV, TXE_FUNC)

#define TXE_FW_STS0			0x40		/* HFS0: DID ACK [28], BIOS action [27:25] */
# define TXE_FW_STS0_DID_ACK		BIT(28)
# define TXE_FW_STS0_BIOS_ACTION(x)	(((x) >> 25) & 0x7)

#define TXE_MEM_REQ			0x44		/* UMA request: valid/invalid + size (KB) */
# define TXE_MEM_REQ_VALID		BIT(31)
# define TXE_MEM_REQ_INVALID		BIT(30)
# define TXE_MEM_REQ_SIZE_MASK		0xffff		/* request field is in KB */

#define TXE_DID_MSG			0x60		/* DRAM Init Done message */
# define TXE_DID_MSG_IND		BIT(28)		/* DRAM Init Done indicator */

#define TXE_SATT1_CTRL			0xc0		/* SATT1: base MSB [11:8], ENTRY_VLD [0] */
# define TXE_SATT1_ENTRY_VLD		BIT(0)
#define TXE_SATT1_BRG_BA_LSB		0xc4		/* SATT1 base = UMA byte base [31:0] */
#define TXE_SATT1_SAP_SIZE		0xc8		/* SATT1 window size in bytes */


#define TXE_POLL_TIMEOUT_US		(5 * 1000 * 1000)

/* BIOS action the TXE requests in the DID ACK. */
enum txe_bios_action {
	TXE_ACK_NO_DID		= 0,
	TXE_ACK_RESET		= 1,	/* non-power cycle reset */
	TXE_ACK_PWR_CYCLE	= 2,
	TXE_ACK_S3		= 3,
	TXE_ACK_S4		= 4,
	TXE_ACK_S5		= 5,
	TXE_ACK_GBL_RESET	= 6,
	TXE_ACK_CONTINUE	= 7,
};

uint32_t txe_uma_get_size_mb(void)
{
	/*
	 * Skip the handshake entirely when the board turns the TXE off. The
	 * TXE locks up if it is hidden through the PMC function disable
	 * register after having been given a DRAM Init Done, which is what
	 * ramstage does for a disabled device.
	 */
	if (!is_devfn_enabled(PCI_DEVFN(TXE_DEV, TXE_FUNC))) {
		printk(BIOS_INFO, "TXE: disabled in devicetree, no UMA\n");
		return 0;
	}

	uint16_t id = pci_read_config16(TXE_PCI_DEV, PCI_DEVICE_ID);
	if ((id & 0xfff8) != TXE_DEVID) {
		printk(BIOS_INFO, "TXE: not present (DevID %04x), no UMA\n", id);
		return 0;
	}

	pci_write_config32(TXE_PCI_DEV, 0x80, 0x4803a001);

	/* The ROM guarantees one of these bits will set; bound the loop anyway. */
	uint32_t req = 0;
	for (long t = 0; t < TXE_POLL_TIMEOUT_US; t += 10) {
		req = pci_read_config32(TXE_PCI_DEV, TXE_MEM_REQ);
		if (req & (TXE_MEM_REQ_VALID | TXE_MEM_REQ_INVALID))
			break;
		udelay(10);
	}

	if (!(req & TXE_MEM_REQ_VALID)) {
		printk(BIOS_ERR, "TXE: UMA request not valid (MEM_REQ %08x)\n", req);
		return 0;
	}

	uint32_t mb = (req & TXE_MEM_REQ_SIZE_MASK) / KiB;	/* field is KB -> MB */
	if (mb >= 32)						/* vendor FW also clamps this */
		mb = 31;
	printk(BIOS_INFO, "TXE: requests %u MB UMA\n", mb);
	return mb;
}

/*
 * Arm or disarm a global reset, i.e. one that resets the TXE along with the
 * host, for the next write to the CF9 reset port. CF9LOCK is only set during
 * ramstage lockdown, so ETR is still writable here.
 */
static void txe_set_global_reset(bool enable)
{
	void *etr = (void *)(PMC_BASE_ADDRESS + ETR);
	uint32_t reg = read32(etr);

	/* Clear CF9 Without Resume Well Reset Enable */
	reg &= ~CWORWRE;

	if (enable)
		reg |= CF9GR;
	else
		reg &= ~CF9GR;

	write32(etr, reg);
}

void txe_uma_did(uint32_t base_mb, uint32_t size_mb)
{
	if (!size_mb)
		return;

	/* SATT1: linear bridge from the TXE IOSF view to host physical UMA. */
	pci_write_config32(TXE_PCI_DEV, TXE_SATT1_BRG_BA_LSB, base_mb << 20);
	pci_or_config32(TXE_PCI_DEV, TXE_SATT1_CTRL, ((base_mb >> 12) & 0xf) << 8);
	pci_write_config32(TXE_PCI_DEV, TXE_SATT1_SAP_SIZE, size_mb << 20);
	pci_or_config32(TXE_PCI_DEV, TXE_SATT1_CTRL, TXE_SATT1_ENTRY_VLD);

	/* Send DRAM Init Done with status 0 (success). */
	pci_or_config32(TXE_PCI_DEV, TXE_DID_MSG, TXE_DID_MSG_IND);
	printk(BIOS_INFO, "TXE: DID sent (UMA base %u MB, size %u MB)\n",
	       base_mb, size_mb);

	/* Poll for DID acknowledgment. */
	uint32_t fw_sts0 = 0;

	for (long t = 0; t < TXE_POLL_TIMEOUT_US; t += 1000) {
		fw_sts0 = pci_read_config32(TXE_PCI_DEV, TXE_FW_STS0);
		if (fw_sts0 & TXE_FW_STS0_DID_ACK)
			break;
		udelay(1000);
	}

	if (!(fw_sts0 & TXE_FW_STS0_DID_ACK)) {
		printk(BIOS_ERR, "TXE: DID ACK timeout (FWSTS0 %08x)\n", fw_sts0);
		return;
	}

	uint8_t action = TXE_FW_STS0_BIOS_ACTION(fw_sts0);
	printk(BIOS_INFO, "TXE: DID ACK received, BIOS action %u\n", action);

	switch (action) {
	case TXE_ACK_RESET:
		printk(BIOS_INFO, "TXE: requests a warm reset\n");
		txe_set_global_reset(false);
		system_reset();
	case TXE_ACK_PWR_CYCLE:
		printk(BIOS_INFO, "TXE: requests a power cycle reset\n");
		txe_set_global_reset(false);
		full_reset();
	case TXE_ACK_GBL_RESET:
		printk(BIOS_INFO, "TXE: requests a global reset\n");
		txe_set_global_reset(true);
		full_reset();
	default:
		/* NO_DID, CONTINUE and the S3/S4/S5 codes: carry on booting. */
		break;
	}
}
