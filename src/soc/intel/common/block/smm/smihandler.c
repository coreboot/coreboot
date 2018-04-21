/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/hlt.h>
#include <arch/io.h>
#include <compiler.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <device/pci_def.h>
#include <elog.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/smihandler.h>
#include <intelblocks/uart.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <spi-generic.h>
#include <stdint.h>
#include <stdlib.h>

/* GNVS needs to be set by coreboot initiating a software SMI. */
static struct global_nvs_t *gnvs;

/* SoC overrides. */

/* Specific SOC SMI handler during ramstage finalize phase */
__weak void smihandler_soc_at_finalize(void)
{
	return;
}

__weak int smihandler_soc_disable_busmaster(device_t dev)
{
	return 1;
}

/* SMI handlers that should be serviced in SCI mode too. */
__weak uint32_t smihandler_soc_get_sci_mask(void)
{
	return 0; /* No valid SCI mask for SMI handler */
}

/*
 * Needs to implement the mechanism to know if an illegal attempt
 * has been made to write to the BIOS area.
 */
__weak void smihandler_soc_check_illegal_access(
	uint32_t tco_sts)
{
	return;
}

/* Mainboard overrides. */

__weak void mainboard_smi_gpi_handler(
	const struct gpi_status *sts)
{
	return;
}

__weak void mainboard_smi_espi_handler(void)
{
	return;
}

/* Common Functions */

static void *find_save_state(const struct smm_save_state_ops *save_state_ops,
	int cmd)
{
	int node;
	void *state = NULL;
	uint32_t io_misc_info;
	uint8_t reg_al;

	/* Check all nodes looking for the one that issued the IO */
	for (node = 0; node < CONFIG_MAX_CPUS; node++) {
		state = smm_get_save_state(node);

		io_misc_info = save_state_ops->get_io_misc_info(state);

		/* Check for Synchronous IO (bit0==1) */
		if (!(io_misc_info & (1 << 0)))
			continue;
		/* Make sure it was a write (bit4==0) */
		if (io_misc_info & (1 << 4))
			continue;
		/* Check for APMC IO port */
		if (((io_misc_info >> 16) & 0xff) != APM_CNT)
			continue;
		/* Check AL against the requested command */
		reg_al = save_state_ops->get_reg(state, RAX);
		if (reg_al != cmd)
			continue;
		break;
	}
	return state;
}

/* Inherited from cpu/x86/smm.h resulting in a different signature */
void southbridge_smi_set_eos(void)
{
	pmc_enable_smi(EOS);
}

struct global_nvs_t *smm_get_gnvs(void)
{
	return gnvs;
}

static void busmaster_disable_on_bus(int bus)
{
	int slot, func;
	unsigned int val;
	unsigned char hdr;

	for (slot = 0; slot < 0x20; slot++) {
		for (func = 0; func < 8; func++) {
			u32 reg32;
			device_t dev = PCI_DEV(bus, slot, func);

			if (!smihandler_soc_disable_busmaster(dev))
				continue;
			val = pci_read_config32(dev, PCI_VENDOR_ID);

			if (val == 0xffffffff || val == 0x00000000 ||
			    val == 0x0000ffff || val == 0xffff0000)
				continue;

			/* Disable Bus Mastering for this one device */
			reg32 = pci_read_config32(dev, PCI_COMMAND);
			reg32 &= ~PCI_COMMAND_MASTER;
			pci_write_config32(dev, PCI_COMMAND, reg32);

			/* If it's not a bridge, move on. */
			hdr = pci_read_config8(dev, PCI_HEADER_TYPE);
			hdr &= 0x7f;
			if (hdr != PCI_HEADER_TYPE_BRIDGE &&
			    hdr != PCI_HEADER_TYPE_CARDBUS)
				continue;

			/*
			 * If secondary bus is equal to current bus bypass
			 * the bridge because it's likely unconfigured and
			 * would cause infinite recursion.
			 */
			int secbus = pci_read_config8(dev, PCI_SECONDARY_BUS);

			if (secbus == bus)
				continue;

			busmaster_disable_on_bus(secbus);
		}
	}
}


void smihandler_southbridge_sleep(
	const struct smm_save_state_ops *save_state_ops)
{
	uint32_t reg32;
	uint8_t slp_typ;

	/* First, disable further SMIs */
	pmc_disable_smi(SLP_SMI_EN);
	/* Figure out SLP_TYP */
	reg32 = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%08x\n", reg32);
	slp_typ = acpi_sleep_from_pm1(reg32);

	/* Do any mainboard sleep handling */
	mainboard_smi_sleep(slp_typ);

	/* Log S3, S4, and S5 entry */
	if (slp_typ >= ACPI_S3 && IS_ENABLED(CONFIG_ELOG_GSMI))
		elog_add_event_byte(ELOG_TYPE_ACPI_ENTER, slp_typ);

	/* Clear pending GPE events */
	pmc_clear_all_gpe_status();

	/* Next, do the deed. */

	switch (slp_typ) {
	case ACPI_S0:
		printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n");
		break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");

		gnvs->uior = uart_debug_controller_is_initialized();

		/* Invalidate the cache before going to S3 */
		wbinvd();
		break;
	case ACPI_S4:
		printk(BIOS_DEBUG, "SMI#: Entering S4 (Suspend-To-Disk)\n");
		break;
	case ACPI_S5:
		printk(BIOS_DEBUG, "SMI#: Entering S5 (Soft Power off)\n");

		/* Disable all GPE */
		pmc_disable_all_gpe();
		/* Set which state system will be after power reapplied */
		pmc_soc_restore_power_failure();
		/* also iterates over all bridges on bus 0 */
		busmaster_disable_on_bus(0);

		/*
		 * Some platforms (e.g. Chromebooks) have observed race between
		 * SLP SMI and PWRBTN SMI because of the way these SMIs are
		 * triggered on power button press. Allow adding a delay before
		 * triggering sleep enable for S5, so that power button
		 * interrupt does not result into immediate wake.
		 */
		mdelay(CONFIG_SOC_INTEL_COMMON_BLOCK_SMM_S5_DELAY_MS);

		/*
		 * Ensure any pending power button status bits are cleared as
		 * the system is entering S5 and doesn't want to be woken up
		 * right away from older power button events.
		 */
		pmc_clear_pm1_status();

		break;
	default:
		printk(BIOS_DEBUG, "SMI#: ERROR: SLP_TYP reserved\n");
		break;
	}

	/* Tri-state specific GPIOS to avoid leakage during S3/S5 */

	/*
	 * Write back to the SLP register to cause the originally intended
	 * event again. We need to set BIT13 (SLP_EN) though to make the
	 * sleep happen.
	 */
	pmc_enable_pm1_control(SLP_EN);

	/* Make sure to stop executing code here for S3/S4/S5 */
	if (slp_typ >= ACPI_S3)
		hlt();

	/*
	 * In most sleep states, the code flow of this function ends at
	 * the line above. However, if we entered sleep state S1 and wake
	 * up again, we will continue to execute code in this function.
	 */
	if (pmc_read_pm1_control() & SCI_EN) {
		/* The OS is not an ACPI OS, so we set the state to S0 */
		pmc_disable_pm1_control(SLP_EN | SLP_TYP);
	}
}

static void southbridge_smi_gsmi(
	const struct smm_save_state_ops *save_state_ops)
{
	u8 sub_command, ret;
	void *io_smi = NULL;
	uint32_t reg_ebx;

	io_smi = find_save_state(save_state_ops, ELOG_GSMI_APM_CNT);
	if (!io_smi)
		return;
	/* Command and return value in EAX */
	sub_command = (save_state_ops->get_reg(io_smi, RAX) >> 8)
		& 0xff;

	/* Parameter buffer in EBX */
	reg_ebx = save_state_ops->get_reg(io_smi, RBX);

	/* drivers/elog/gsmi.c */
	ret = gsmi_exec(sub_command, &reg_ebx);
	save_state_ops->set_reg(io_smi, RAX, ret);
}

static void finalize(void)
{
	static int finalize_done;

	if (finalize_done) {
		printk(BIOS_DEBUG, "SMM already finalized.\n");
		return;
	}
	finalize_done = 1;

	if (IS_ENABLED(CONFIG_SPI_FLASH_SMM))
		/* Re-init SPI driver to handle locked BAR */
		fast_spi_init();

	/* Specific SOC SMI handler during ramstage finalize phase */
	smihandler_soc_at_finalize();
}

void smihandler_southbridge_apmc(
	const struct smm_save_state_ops *save_state_ops)
{
	uint8_t reg8;
	void *state = NULL;
	static int smm_initialized = 0;

	/* Emulate B2 register as the FADT / Linux expects it */

	reg8 = inb(APM_CNT);
	switch (reg8) {
	case APM_CNT_CST_CONTROL:
		/*
		 * Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		printk(BIOS_DEBUG, "C-state control\n");
		break;
	case APM_CNT_PST_CONTROL:
		/*
		 * Calling this function seems to cause
		 * some kind of race condition in Linux
		 * and causes a kernel oops
		 */
		printk(BIOS_DEBUG, "P-state control\n");
		break;
	case APM_CNT_ACPI_DISABLE:
		pmc_disable_pm1_control(SCI_EN);
		printk(BIOS_DEBUG, "SMI#: ACPI disabled.\n");
		break;
	case APM_CNT_ACPI_ENABLE:
		pmc_enable_pm1_control(SCI_EN);
		printk(BIOS_DEBUG, "SMI#: ACPI enabled.\n");
		break;
	case APM_CNT_GNVS_UPDATE:
		if (smm_initialized) {
			printk(BIOS_DEBUG,
			       "SMI#: SMM structures already initialized!\n");
			return;
		}
		state = find_save_state(save_state_ops, reg8);
		if (state) {
			/* EBX in the state save contains the GNVS pointer */
			uint32_t reg_ebx = save_state_ops->get_reg(state, RBX);
			gnvs = (struct global_nvs_t *)(uintptr_t)reg_ebx;
			smm_initialized = 1;
			printk(BIOS_DEBUG, "SMI#: Setting GNVS to %p\n", gnvs);
		}
		break;
	case ELOG_GSMI_APM_CNT:
		if (IS_ENABLED(CONFIG_ELOG_GSMI))
			southbridge_smi_gsmi(save_state_ops);
		break;
	case APM_CNT_FINALIZE:
		finalize();
		break;
	}

	mainboard_smi_apmc(reg8);
}

void smihandler_southbridge_pm1(
	const struct smm_save_state_ops *save_state_ops)
{
	uint16_t pm1_sts = pmc_clear_pm1_status();
	u16 pm1_en = pmc_read_pm1_enable();

	/*
	 * While OSPM is not active, poweroff immediately
	 * on a power button event.
	 */
	if ((pm1_sts & PWRBTN_STS) && (pm1_en & PWRBTN_EN)) {
		/* power button pressed */
		if (IS_ENABLED(CONFIG_ELOG_GSMI))
			elog_add_event(ELOG_TYPE_POWER_BUTTON);
		pmc_disable_pm1_control(-1UL);
		pmc_enable_pm1_control(SLP_EN | (SLP_TYP_S5 << SLP_TYP_SHIFT));
	}
}

void smihandler_southbridge_gpe0(
	const struct smm_save_state_ops *save_state_ops)
{
	pmc_clear_all_gpe_status();
}

void smihandler_southbridge_tco(
	const struct smm_save_state_ops *save_state_ops)
{
	uint32_t tco_sts = pmc_clear_tco_status();

	/* Any TCO event? */
	if (!tco_sts)
		return;

	smihandler_soc_check_illegal_access(tco_sts);

	if (tco_sts & TCO_TIMEOUT) { /* TIMEOUT */
		/* Handle TCO timeout */
		printk(BIOS_DEBUG, "TCO Timeout.\n");
	}
}

void smihandler_southbridge_periodic(
	const struct smm_save_state_ops *save_state_ops)
{
	uint32_t reg32;

	reg32 = pmc_get_smi_en();

	/* Are periodic SMIs enabled? */
	if ((reg32 & PERIODIC_EN) == 0)
		return;
	printk(BIOS_DEBUG, "Periodic SMI.\n");
}

void smihandler_southbridge_gpi(
	const struct smm_save_state_ops *save_state_ops)
{
	struct gpi_status smi_sts;

	gpi_clear_get_smi_status(&smi_sts);
	mainboard_smi_gpi_handler(&smi_sts);

	/* Clear again after mainboard handler */
	gpi_clear_get_smi_status(&smi_sts);
}

void smihandler_southbridge_espi(
	const struct smm_save_state_ops *save_state_ops)
{
	mainboard_smi_espi_handler();
}

void southbridge_smi_handler(void)
{
	int i;
	uint32_t smi_sts;
	const struct smm_save_state_ops *save_state_ops;

	/*
	 * We need to clear the SMI status registers, or we won't see what's
	 * happening in the following calls.
	 */
	smi_sts = pmc_clear_smi_status();

	/*
	 * In SCI mode, execute only those SMI handlers that have
	 * declared themselves as available for service in that mode
	 * using smihandler_soc_get_sci_mask.
	 */
	if (pmc_read_pm1_control() & SCI_EN)
		smi_sts &= smihandler_soc_get_sci_mask();

	if (!smi_sts)
		return;

	save_state_ops = get_smm_save_state_ops();

	/* Call SMI sub handler for each of the status bits */
	for (i = 0; i < ARRAY_SIZE(southbridge_smi); i++) {
		if (!(smi_sts & (1 << i)))
			continue;

		if (southbridge_smi[i] != NULL) {
			southbridge_smi[i](save_state_ops);
		} else {
			printk(BIOS_DEBUG,
			       "SMI_STS[%d] occurred, but no "
			       "handler available.\n", i);
		}
	}
}

static uint32_t em64t100_smm_save_state_get_io_misc_info(void *state)
{
	em64t100_smm_state_save_area_t *smm_state = state;
	return smm_state->io_misc_info;
}

static uint64_t em64t100_smm_save_state_get_reg(void *state, enum smm_reg reg)
{
	uintptr_t value = 0;
	em64t100_smm_state_save_area_t *smm_state = state;

	switch (reg) {
	case RAX:
		value = smm_state->rax;
		break;
	case RBX:
		value = smm_state->rbx;
		break;
	case RCX:
		value = smm_state->rcx;
		break;
	case RDX:
		value = smm_state->rdx;
		break;
	default:
		break;
	}
	return value;
}

static void em64t100_smm_save_state_set_reg(void *state, enum smm_reg reg,
	uint64_t val)
{
	em64t100_smm_state_save_area_t *smm_state = state;
	switch (reg) {
	case RAX:
		smm_state->rax = val;
		break;
	case RBX:
		smm_state->rbx = val;
		break;
	case RCX:
		smm_state->rcx = val;
		break;
	case RDX:
		smm_state->rdx = val;
		break;
	default:
		break;
	}
}

static uint32_t em64t101_smm_save_state_get_io_misc_info(void *state)
{
	em64t101_smm_state_save_area_t *smm_state = state;
	return smm_state->io_misc_info;
}

static uint64_t em64t101_smm_save_state_get_reg(void *state, enum smm_reg reg)
{
	uintptr_t value = 0;
	em64t101_smm_state_save_area_t *smm_state = state;

	switch (reg) {
	case RAX:
		value = smm_state->rax;
		break;
	case RBX:
		value = smm_state->rbx;
		break;
	case RCX:
		value = smm_state->rcx;
		break;
	case RDX:
		value = smm_state->rdx;
		break;
	default:
		break;
	}
	return value;
}

static void em64t101_smm_save_state_set_reg(void *state, enum smm_reg reg,
	uint64_t val)
{
	em64t101_smm_state_save_area_t *smm_state = state;
	switch (reg) {
	case RAX:
		smm_state->rax = val;
		break;
	case RBX:
		smm_state->rbx = val;
		break;
	case RCX:
		smm_state->rcx = val;
		break;
	case RDX:
		smm_state->rdx = val;
		break;
	default:
		break;
	}
}

const struct smm_save_state_ops em64t100_smm_ops = {
	.get_io_misc_info = em64t100_smm_save_state_get_io_misc_info,
	.get_reg = em64t100_smm_save_state_get_reg,
	.set_reg = em64t100_smm_save_state_set_reg,
};

const struct smm_save_state_ops em64t101_smm_ops = {
	.get_io_misc_info = em64t101_smm_save_state_get_io_misc_info,
	.get_reg = em64t101_smm_save_state_get_reg,
	.set_reg = em64t101_smm_save_state_set_reg,
};
