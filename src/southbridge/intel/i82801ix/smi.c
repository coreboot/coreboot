/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <acpi/acpi.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/smi_deprecated.h>
#include <string.h>
#include <southbridge/intel/common/pmutil.h>
#include "i82801ix.h"

/* I945/GM45 */
#define SMRAM		0x9d
#define   D_OPEN	(1 << 6)
#define   D_CLS		(1 << 5)
#define   D_LCK		(1 << 4)
#define   G_SMRAME	(1 << 3)
#define   C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

/* While we read PMBASE dynamically in case it changed, let's
 * initialize it with a sane value
 */
static u16 pmbase = DEFAULT_PMBASE;

extern uint8_t smm_relocation_start, smm_relocation_end;
static void *default_smm_area = NULL;

static void aseg_smm_relocate(void)
{
	u32 smi_en;
	u16 pm1_en;

	printk(BIOS_DEBUG, "Initializing SMM handler...");

	pmbase = pci_read_config16(pcidev_on_root(0x1f, 0), D31F0_PMBASE) &
									0xfffc;
	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", pmbase);

	smi_en = inl(pmbase + SMI_EN);
	if (smi_en & GBL_SMI_EN) {
		printk(BIOS_INFO, "SMI# handler already enabled?\n");
		return;
	}

	default_smm_area = backup_default_smm_area();

	/* copy the SMM relocation code */
	memcpy((void *)0x38000, &smm_relocation_start,
			&smm_relocation_end - &smm_relocation_start);
	wbinvd();

	printk(BIOS_DEBUG, "\n");
	dump_smi_status(reset_smi_status());
	dump_pm1_status(reset_pm1_status());
	dump_gpe0_status(reset_gpe0_status());
	dump_alt_gp_smi_status(reset_alt_gp_smi_status());
	dump_tco_status(reset_tco_status());

	/* Enable SMI generation:
	 *  - on TCO events
	 *  - on APMC writes (io 0xb2)
	 *  - on writes to GBL_RLS (bios commands)
	 * No SMIs:
	 *  - on microcontroller writes (io 0x62/0x66)
	 */

	smi_en = 0; /* reset SMI enables */
	smi_en |= TCO_EN;
	smi_en |= APMC_EN;
	if (CONFIG(DEBUG_PERIODIC_SMI))
		smi_en |= PERIODIC_EN;
	smi_en |= BIOS_EN;

	/* The following need to be on for SMIs to happen */
	smi_en |= EOS | GBL_SMI_EN;

	outl(smi_en, pmbase + SMI_EN);

	pm1_en = 0;
	pm1_en |= PWRBTN_EN;
	pm1_en |= GBL_EN;
	outw(pm1_en, pmbase + PM1_EN);

	/**
	 * There are several methods of raising a controlled SMI# via
	 * software, among them:
	 *  - Writes to io 0xb2 (APMC)
	 *  - Writes to the Local Apic ICR with Delivery mode SMI.
	 *
	 * Using the local APIC is a bit more tricky. According to
	 * AMD Family 11 Processor BKDG no destination shorthand must be
	 * used.
	 * The whole SMM initialization is quite a bit hardware specific, so
	 * I'm not too worried about the better of the methods at the moment
	 */

	/* raise an SMI interrupt */
	printk(BIOS_SPEW, "  ... raise SMI#\n");
	apm_control(APM_CNT_NOOP_SMI);
}

static int smm_handler_copied = 0;

static void aseg_smm_install(void)
{
	/* The first CPU running this gets to copy the SMM handler. But not all
	 * of them.
	 */
	if (smm_handler_copied)
		return;
	smm_handler_copied = 1;

	/* if we're resuming from S3, the SMM code is already in place,
	 * so don't copy it again to keep the current SMM state */

	if (!acpi_is_wakeup_s3()) {
		/* enable the SMM memory window */
		pci_write_config8(pcidev_on_root(0, 0), SMRAM,
					D_OPEN | G_SMRAME | C_BASE_SEG);

		/* copy the real SMM handler */
		memcpy((void *)0xa0000, _binary_smm_start,
			_binary_smm_end - _binary_smm_start);
		wbinvd();
	}

	/* close the SMM memory window and enable normal SMM */
	pci_write_config8(pcidev_on_root(0, 0), SMRAM,
			G_SMRAME | C_BASE_SEG);
}

void smm_init(void)
{
	/* Put SMM code to 0xa0000 */
	aseg_smm_install();

	/* Put relocation code to 0x38000 and relocate SMBASE */
	aseg_smm_relocate();

	/* We're done. Make sure SMIs can happen! */
	smi_set_eos();
}

void smm_init_completion(void)
{
	restore_default_smm_area(default_smm_area);
}

void aseg_smm_lock(void)
{
	/* LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");
	pci_write_config8(pcidev_on_root(0, 0), SMRAM,
			D_LCK | G_SMRAME | C_BASE_SEG);
}
