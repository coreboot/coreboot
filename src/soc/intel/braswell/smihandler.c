/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hlt.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/em64t100_save_state.h>
#include <device/pci_def.h>
#include <elog.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <spi-generic.h>
#include <stdint.h>
#include <soc/gpio.h>
#include <smmstore.h>

void southbridge_smi_set_eos(void)
{
	enable_smi(EOS);
}

static void busmaster_disable_on_bus(int bus)
{
	int slot, func;
	unsigned int val;
	unsigned char hdr;

	for (slot = 0; slot < 0x20; slot++) {
		for (func = 0; func < 8; func++) {
			u16 reg16;
			pci_devfn_t dev = PCI_DEV(bus, slot, func);

			val = pci_read_config32(dev, PCI_VENDOR_ID);

			if (val == 0xffffffff || val == 0x00000000 ||
			    val == 0x0000ffff || val == 0xffff0000)
				continue;

			/* Disable Bus Mastering for this one device */
			reg16 = pci_read_config16(dev, PCI_COMMAND);
			reg16 &= ~PCI_COMMAND_MASTER;
			pci_write_config16(dev, PCI_COMMAND, reg16);

			/* If this is a bridge, then follow it. */
			hdr = pci_read_config8(dev, PCI_HEADER_TYPE);
			hdr &= 0x7f;
			if (hdr == PCI_HEADER_TYPE_BRIDGE || hdr == PCI_HEADER_TYPE_CARDBUS) {
				unsigned int buses;
				buses = pci_read_config32(dev, PCI_PRIMARY_BUS);
				busmaster_disable_on_bus((buses >> 8) & 0xff);
			}
		}
	}
}

static void tristate_gpios(uint32_t val)
{
	/* Tri-state eMMC */
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + SDMMC1_CMD_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + SDMMC1_D0_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + SDMMC1_D1_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + SDMMC1_D2_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + SDMMC1_D3_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + MMC1_D4_SD_WE_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + MMC1_D5_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + MMC1_D6_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + MMC1_D7_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHEAST_BASE + MMC1_RCLK_OFFSET, val);

	/* Tri-state HDMI */
	write32((void *)COMMUNITY_GPNORTH_BASE + HV_DDI2_DDC_SDA_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPNORTH_BASE + HV_DDI2_DDC_SCL_MMIO_OFFSET, val);

	/* Tri-state CFIO 139 and 140 */
	write32((void *)COMMUNITY_GPSOUTHWEST_BASE + CFIO_139_MMIO_OFFSET, val);
	write32((void *)COMMUNITY_GPSOUTHWEST_BASE + CFIO_140_MMIO_OFFSET, val);
}

static void southbridge_smi_sleep(void)
{
	uint32_t reg32;
	uint8_t slp_typ;
	uint16_t pmbase = get_pmbase();

	/* First, disable further SMIs */
	disable_smi(SLP_SMI_EN);

	/* Figure out SLP_TYP */
	reg32 = inl(pmbase + PM1_CNT);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%08x\n", reg32);
	slp_typ = acpi_sleep_from_pm1(reg32);

	/* Do any mainboard sleep handling */
	mainboard_smi_sleep(slp_typ);

	/* Log S3, S4, and S5 entry */
	if (slp_typ >= ACPI_S3)
		elog_gsmi_add_event_byte(ELOG_TYPE_ACPI_ENTER, slp_typ);

	/* Clear pending GPE events */
	clear_gpe_status();

	/* Next, do the deed. */
	switch (slp_typ) {
	case ACPI_S0:
		printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n");
		break;
	case ACPI_S1:
		printk(BIOS_DEBUG, "SMI#: Entering S1 (Assert STPCLK#)\n");
		break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");

		/* Invalidate the cache before going to S3 */
		wbinvd();
		break;
	case ACPI_S4:
		printk(BIOS_DEBUG, "SMI#: Entering S4 (Suspend-To-Disk)\n");
		break;
	case ACPI_S5:
		printk(BIOS_DEBUG, "SMI#: Entering S5 (Soft Power off)\n");

		/* Disable all GPE */
		disable_all_gpe();

		/* Also iterates over all bridges on bus 0 */
		busmaster_disable_on_bus(0);
		break;
	default:
		printk(BIOS_DEBUG, "SMI#: ERROR: SLP_TYP reserved\n");
		break;
	}

	/* Clear pending wake status bit to avoid immediate wake */
	write32((void *)(0xfed88000 + 0x0200), read32((void *)(0xfed88000 + 0x0200)));

	/* Tri-state specific GPIOS to avoid leakage during S3/S5 */
	if ((slp_typ == ACPI_S3) || (slp_typ == ACPI_S5))
		tristate_gpios(PAD_CONTROL_REG0_TRISTATE);

	/*
	 * Write back to the SLP register to cause the originally intended event again.
	 * We need to set BIT13 (SLP_EN) though to make the sleep happen.
	 */
	enable_pm1_control(SLP_EN);

	/* Make sure to stop executing code here for S3/S4/S5 */
	if (slp_typ >= ACPI_S3)
		hlt();

	/*
	 * In most sleep states, the code flow of this function ends at
	 * the line above. However, if we entered sleep state S1 and wake
	 * up again, we will continue to execute code in this function.
	 */
	reg32 = inl(pmbase + PM1_CNT);
	if (reg32 & SCI_EN) {
		/* The OS is not an ACPI OS, so we set the state to S0 */
		disable_pm1_control(SLP_EN | SLP_TYP);
	}
}

/*
 * Look for Synchronous IO SMI and use save state from that core in case
 * we are not running on the same core that initiated the IO transaction.
 */
static em64t100_smm_state_save_area_t *smi_apmc_find_state_save(uint8_t cmd)
{
	em64t100_smm_state_save_area_t *state;
	int node;

	/* Check all nodes looking for the one that issued the IO */
	for (node = 0; node < CONFIG_MAX_CPUS; node++) {
		state = smm_get_save_state(node);

		/* Check for Synchronous IO (bit0==1) */
		if (!(state->io_misc_info & (1 << 0)))
			continue;

		/* Make sure it was a write (bit4==0) */
		if (state->io_misc_info & (1 << 4))
			continue;

		/* Check for APMC IO port */
		if (((state->io_misc_info >> 16) & 0xff) != APM_CNT)
			continue;

		/* Check AX against the requested command */
		if ((state->rax & 0xff) != cmd)
			continue;

		return state;
	}

	return NULL;
}

static void southbridge_smi_gsmi(void)
{
	u32 *ret, *param;
	uint8_t sub_command;
	em64t100_smm_state_save_area_t *io_smi = smi_apmc_find_state_save(APM_CNT_ELOG_GSMI);

	if (!io_smi)
		return;

	/* Command and return value in EAX */
	ret = (u32 *)&io_smi->rax;
	sub_command = (uint8_t)(*ret >> 8);

	/* Parameter buffer in EBX */
	param = (u32 *)&io_smi->rbx;

	/* drivers/elog/gsmi.c */
	*ret = gsmi_exec(sub_command, param);
}

static void southbridge_smi_store(void)
{
	u8 sub_command, ret;
	em64t100_smm_state_save_area_t *io_smi = smi_apmc_find_state_save(APM_CNT_SMMSTORE);
	uint32_t reg_ebx;

	if (!io_smi)
		return;
	/* Command and return value in EAX */
	sub_command = (io_smi->rax >> 8) & 0xff;

	/* Parameter buffer in EBX */
	reg_ebx = io_smi->rbx;

	/* drivers/smmstore/smi.c */
	ret = smmstore_exec(sub_command, (void *)reg_ebx);
	io_smi->rax = ret;
}

static void southbridge_smi_apmc(void)
{
	uint8_t reg8;

	reg8 = apm_get_apmc();
	switch (reg8) {
	case APM_CNT_ACPI_DISABLE:
		disable_pm1_control(SCI_EN);
		break;
	case APM_CNT_ACPI_ENABLE:
		enable_pm1_control(SCI_EN);
		break;
	case APM_CNT_ELOG_GSMI:
		if (CONFIG(ELOG_GSMI))
			southbridge_smi_gsmi();
		break;
	case APM_CNT_SMMSTORE:
		if (CONFIG(SMMSTORE))
			southbridge_smi_store();
		break;
	}

	mainboard_smi_apmc(reg8);
}

static void southbridge_smi_pm1(void)
{
	uint16_t pm1_sts = clear_pm1_status();

	/* While OSPM is not active, poweroff immediately on a power button event */
	if (pm1_sts & PWRBTN_STS) {
		/* Power button pressed */
		elog_gsmi_add_event(ELOG_TYPE_POWER_BUTTON);
		disable_pm1_control(-1UL);
		enable_pm1_control(SLP_EN | (SLP_TYP_S5 << SLP_TYP_SHIFT));
	}
}

static void southbridge_smi_gpe0(void)
{
	clear_gpe_status();
}

static void southbridge_smi_tco(void)
{
	uint32_t tco_sts = clear_tco_status();

	/* Any TCO event? */
	if (!tco_sts)
		return;

	if (tco_sts & TCO_TIMEOUT) { /* TIMEOUT */
		/* Handle TCO timeout */
		printk(BIOS_DEBUG, "TCO Timeout.\n");
	}
}

static void southbridge_smi_periodic(void)
{
	uint32_t reg32;

	reg32 = inl(get_pmbase() + SMI_EN);

	/* Are periodic SMIs enabled? */
	if ((reg32 & PERIODIC_EN) == 0)
		return;

	printk(BIOS_DEBUG, "Periodic SMI.\n");
}

typedef void (*smi_handler_t)(void);

static const smi_handler_t southbridge_smi[32] = {
	NULL,			  /*  [0] reserved */
	NULL,			  /*  [1] reserved */
	NULL,			  /*  [2] BIOS_STS */
	NULL,			  /*  [3] LEGACY_USB_STS */
	southbridge_smi_sleep,	  /*  [4] SLP_SMI_STS */
	southbridge_smi_apmc,	  /*  [5] APM_STS */
	NULL,			  /*  [6] SWSMI_TMR_STS */
	NULL,			  /*  [7] reserved */
	southbridge_smi_pm1,	  /*  [8] PM1_STS */
	southbridge_smi_gpe0,	  /*  [9] GPE0_STS */
	NULL,			  /* [10] reserved */
	NULL,			  /* [11] reserved */
	NULL,			  /* [12] reserved */
	southbridge_smi_tco,	  /* [13] TCO_STS */
	southbridge_smi_periodic, /* [14] PERIODIC_STS */
	NULL,			  /* [15] SERIRQ_SMI_STS */
	NULL,			  /* [16] SMBUS_SMI_STS */
	NULL,			  /* [17] LEGACY_USB2_STS */
	NULL,			  /* [18] INTEL_USB2_STS */
	NULL,			  /* [19] reserved */
	NULL,			  /* [20] PCI_EXP_SMI_STS */
	NULL,			  /* [21] reserved */
	NULL,			  /* [22] reserved */
	NULL,			  /* [23] reserved */
	NULL,			  /* [24] reserved */
	NULL,			  /* [25] reserved */
	NULL,			  /* [26] SPI_STS */
	NULL,			  /* [27] reserved */
	NULL,			  /* [28] PUNIT */
	NULL,			  /* [29] GUNIT */
	NULL,			  /* [30] reserved */
	NULL			  /* [31] reserved */
};

void southbridge_smi_handler(void)
{
	int i;
	uint32_t smi_sts;

	/*
	 * We need to clear the SMI status registers, or we won't see what's
	 * happening in the following calls.
	 */
	smi_sts = clear_smi_status();

	/* Call SMI sub handler for each of the status bits */
	for (i = 0; i < ARRAY_SIZE(southbridge_smi); i++) {
		if (!(smi_sts & (1 << i)))
			continue;

		if (southbridge_smi[i] != NULL) {
			southbridge_smi[i]();
		} else {
			printk(BIOS_DEBUG,
			       "SMI_STS[%d] occurred, but no handler available.\n", i);
		}
	}

	/*
	 * The GPIO SMI events do not have a status bit in SMI_STS. Therefore,
	 * these events need to be cleared and checked unconditionally.
	 */
	mainboard_smi_gpi(clear_alt_status());
}
