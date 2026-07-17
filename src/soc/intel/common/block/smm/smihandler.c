/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hlt.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/em64t100_save_state.h>
#include <cpu/intel/em64t101_save_state.h>
#include <cpu/x86/save_state.h>
#include <cpu/intel/msr.h>
#include <delay.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <elog.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/oc_wdt.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/smihandler.h>
#include <intelblocks/tco.h>
#include <intelblocks/uart.h>
#include <intelblocks/wadt_wake.h>
#include <option.h>
#include <security/lockdown/lockdown.h>
#include <security/tcg/opal_s3_smm.h>
#include <smmstore.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/smbus.h>
#include <spi-generic.h>
#include <stdint.h>
#include <string.h>
#include <types.h>

/* SoC overrides. */

__weak const struct smm_save_state_ops *get_smm_save_state_ops(void)
{
	return &em64t101_smm_ops;
}

/* Specific SOC SMI handler during ramstage finalize phase */
__weak void smihandler_soc_at_finalize(void)
{
	/* no-op */
}

__weak int smihandler_soc_disable_busmaster(pci_devfn_t dev)
{
	return 1;
}

/* Mainboard overrides. */

__weak void mainboard_smi_gpi_handler(
	const struct gpi_status *sts)
{
	/* no-op */
}

__weak void mainboard_smi_espi_handler(void)
{
	/* no-op */
}

#define SMMSTORE_SYNC_SMI_SETTLE_USEC 50
#define SMMSTORE_SYNC_SMI_CLEAR_TRIES 8

static void smmstore_drain_sync_smi(void)
{
	int i;

	for (i = 0; i < SMMSTORE_SYNC_SMI_CLEAR_TRIES; i++) {
		if (!fast_spi_clear_sync_smi_status())
			return;
		udelay(SMMSTORE_SYNC_SMI_SETTLE_USEC);
	}
}

/* Inherited from cpu/x86/smm.h resulting in a different signature */
void southbridge_smi_set_eos(void)
{
	pmc_enable_smi(EOS);
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

			if (!smihandler_soc_disable_busmaster(dev))
				continue;
			val = pci_read_config32(dev, PCI_VENDOR_ID);

			if (val == 0xffffffff || val == 0x00000000 ||
			    val == 0x0000ffff || val == 0xffff0000)
				continue;

			/* Disable Bus Mastering for this one device */
			reg16 = pci_read_config16(dev, PCI_COMMAND);
			reg16 &= ~PCI_COMMAND_MASTER;
			pci_write_config16(dev, PCI_COMMAND, reg16);

			/* If it's not a bridge, move on. */
			hdr = pci_read_config8(dev, PCI_HEADER_TYPE);
			hdr &= 0x7f;
			if (hdr != PCI_HEADER_TYPE_BRIDGE &&
			    hdr != PCI_HEADER_TYPE_CARDBUS)
				continue;

			/*
			 * If secondary bus is lesser-equal to current bus bypass
			 * the bridge because it's likely unconfigured and
			 * would cause infinite recursion.
			 */
			int secbus = pci_read_config8(dev, PCI_SECONDARY_BUS);

			/*
			 * Note: Secondary bus numbers must be strictly greater than the
			 * parent bus number. This prevents infinite recursion from
			 * cycles programmed into bridge config space (e.g., 0 -> N -> 0)
			 * and bounds maximum stack depth to 256 levels.
			 */
			if (secbus <= bus)
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
	bool wadt_enabled = false;
	uint32_t gpe_preserve = 0;

	/* First, disable further SMIs */
	pmc_disable_smi(SLP_SMI_EN);
	/* Figure out SLP_TYP */
	reg32 = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	printk(BIOS_SPEW, "SMI#: SLP = 0x%08x\n", reg32);
	slp_typ = acpi_sleep_from_pm1(reg32);

	opal_s3_smi_sleep(slp_typ);

	/* Do any mainboard sleep handling */
	mainboard_smi_sleep(slp_typ);

	/* Log S3, S4, and S5 entry */
	if (slp_typ >= ACPI_S3)
		elog_gsmi_add_event_byte(ELOG_TYPE_ACPI_ENTER, slp_typ);

	if (wadt_wake_should_preserve(slp_typ)) {
		wadt_enabled = wadt_wake_is_enabled();
		gpe_preserve = wadt_wake_status_preserve_mask(wadt_enabled);
	}

	/* Clear pending GPE events, preserving an armed WADT if needed. */
	pmc_clear_gpe_status(gpe_preserve);

	/* Next, do the deed. */

	switch (slp_typ) {
	case ACPI_S0:
		printk(BIOS_DEBUG, "SMI#: Entering S0 (On)\n");
		break;
	case ACPI_S3:
		printk(BIOS_DEBUG, "SMI#: Entering S3 (Suspend-To-RAM)\n");
		if (CONFIG(SOC_INTEL_COMMON_BLOCK_UART))
			gnvs->uior = uart_is_controller_initialized();

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
		pmc_set_power_failure_state(false);
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

	opal_s3_smi_sleep_finalize(slp_typ);

	/* Allow mainboard to restore wake sources (e.g. for S5 WOL). */
	mainboard_smi_sleep_finalize(slp_typ);

	if (wadt_wake_should_preserve(slp_typ))
		wadt_wake_restore(wadt_enabled);

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
	int node;
	uint32_t eax_val, reg_ebx;

	node = save_state_ops->apmc_node(APM_CNT_ELOG_GSMI);
	if (node < 0)
		return;
	if (save_state_ops->get_reg(RAX, node, &eax_val, sizeof(eax_val)) != 0)
		return;
	/* Command and return value in EAX */
	sub_command = (eax_val >> 8) & 0xff;

	if (save_state_ops->get_reg(RBX, node, &reg_ebx, sizeof(reg_ebx)) != 0)
		return;

	/* drivers/elog/gsmi.c */
	ret = gsmi_exec(sub_command, &reg_ebx);
	save_state_ops->set_reg(RAX, node, &ret, sizeof(ret));
}

static void set_insmm_sts(const bool enable_writes)
{
	msr_t msr = {
		.lo = read32p(0xfed30880),
		.hi = 0,
	};
	if (enable_writes)
		msr.lo |= 1;
	else
		msr.lo &= ~1;

	wrmsr(MSR_SPCL_CHIPSET_USAGE, msr);
}

static void southbridge_smi_store(
	const struct smm_save_state_ops *save_state_ops)
{
	u8 sub_command, ret;
	int node;
	uint32_t eax_val, reg_ebx;

	node = save_state_ops->apmc_node(APM_CNT_SMMSTORE);
	if (node < 0)
		return;
	if (save_state_ops->get_reg(RAX, node, &eax_val, sizeof(eax_val)) != 0)
		return;
	/* Command and return value in EAX */
	sub_command = (eax_val >> 8) & 0xff;

	if (save_state_ops->get_reg(RBX, node, &reg_ebx, sizeof(reg_ebx)) != 0)
		return;

	const bool wp_enabled = !fast_spi_wpd_status();
	if (wp_enabled) {
		set_insmm_sts(true);
		/*
		 * As per BWG, clearing "SPI_BIOS_CONTROL_SYNC_SS"
		 * bit is a must prior setting SPI_BIOS_CONTROL_WPD" bit
		 * to avoid 3-strike error.
		 */
		smmstore_drain_sync_smi();
		fast_spi_disable_wp();
	}

	/* drivers/smmstore/smi.c */
	ret = smmstore_exec(sub_command, (void *)(uintptr_t)reg_ebx);
	smmstore_drain_sync_smi();
	save_state_ops->set_reg(RAX, node, &ret, sizeof(ret));

	if (wp_enabled) {
		fast_spi_enable_wp();
		set_insmm_sts(false);
	}
}

__weak const struct gpio_lock_config *soc_gpio_lock_config(size_t *num)
{
	*num = 0;
	return NULL;
}

static void soc_lock_gpios(void)
{
	const struct gpio_lock_config *soc_gpios;
	size_t soc_gpio_num;

	/* get list of gpios from SoC */
	soc_gpios = soc_gpio_lock_config(&soc_gpio_num);

	/* Lock any soc requested gpios */
	if (soc_gpio_num)
		gpio_lock_pads(soc_gpios, soc_gpio_num);
}

static void finalize(void)
{
	static int finalize_done;

	if (finalize_done) {
		printk(BIOS_DEBUG, "SMM already finalized.\n");
		return;
	}
	finalize_done = 1;

	if (CONFIG(SPI_FLASH_SMM))
		/* Re-init SPI driver to handle locked BAR */
		fast_spi_init();

	if (enable_smm_bios_protection()) {
		fast_spi_enable_wp();
		set_insmm_sts(false);
	}

	/*
	 * HECI is disabled in smihandler_soc_at_finalize() which also locks down the side band
	 * interface.  Some boards may require this interface in mainboard_smi_finalize(),
	 * therefore, this call must precede smihandler_soc_at_finalize().
	 */
	mainboard_smi_finalize();

	/* Lock down all GPIOs that may have been requested by the SoC and/or the mainboard. */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_LOCK_GPIO_PADS))
		soc_lock_gpios();

	/* Specific SOC SMI handler during ramstage finalize phase */
	smihandler_soc_at_finalize();
}

void smihandler_southbridge_apmc(
	const struct smm_save_state_ops *save_state_ops)
{
	uint8_t reg8;

	reg8 = apm_get_apmc();
	switch (reg8) {
	case APM_CNT_ACPI_DISABLE:
		pmc_disable_pm1_control(SCI_EN);
		break;
	case APM_CNT_ACPI_ENABLE:
		pmc_enable_pm1_control(SCI_EN);
		break;
	case APM_CNT_ELOG_GSMI:
		if (CONFIG(ELOG_GSMI))
			southbridge_smi_gsmi(save_state_ops);
		break;
	case APM_CNT_SMMSTORE:
		if (CONFIG(SMMSTORE))
			southbridge_smi_store(save_state_ops);
		break;
	case APM_CNT_FINALIZE:
		finalize();
		break;
	}

	if (opal_s3_smi_apmc(reg8))
		return;

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
		elog_gsmi_add_event(ELOG_TYPE_POWER_BUTTON);
		pmc_disable_pm1_control(~0);
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

	/*
	 * SPI synchronous SMIs are TCO SMIs, but they do not have a status
	 * bit in the TCO_STS register. Furthermore, the TCO_STS bit in the
	 * SMI_STS register is continually set until the SMI handler clears
	 * the SPI synchronous SMI status bit in the SPI controller. To not
	 * risk missing any other TCO SMIs, do not clear the TCO_STS bit in
	 * this SMI handler invocation. If the TCO_STS bit remains set when
	 * returning from SMM, another SMI immediately happens which clears
	 * the TCO_STS bit and handles any pending events.
	 */
	fast_spi_clear_sync_smi_status();

	/* If enabled, enforce SMM BIOS write protection */
	if (enable_smm_bios_protection() && fast_spi_wpd_status()) {
		/*
		 * BWE is RW, so the SMI was caused by a
		 * write to BWE, not by a write to the BIOS
		 *
		 * This is the place where we notice someone
		 * is trying to tinker with the BIOS. We are
		 * trying to be nice and just ignore it. A more
		 * resolute answer would be to power down the
		 * box.
		 */
		printk(BIOS_DEBUG, "Switching SPI back to RO\n");
		fast_spi_enable_wp();
		set_insmm_sts(false);
	}

	/* Any TCO event? */
	if (!tco_sts)
		return;

	if (tco_sts & TCO1_STS_TIMEOUT) { /* TIMEOUT */
		/* Handle TCO timeout */
		printk(BIOS_DEBUG, "TCO Timeout.\n");
	}

	if (tco_sts & (TCO2_INTRD_DET << 16)) { /* INTRUDER# assertion */
		/*
		 * Handle intrusion event
		 * If we ever get here, probably the case has been opened.
		 */
		printk(BIOS_CRIT, "Case intrusion detected.\n");
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

	if (CONFIG(SOC_INTEL_COMMON_OC_WDT_RELOAD_IN_PERIODIC_SMI))
		oc_wdt_reload();
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
	 * When the SCI_EN bit is set, PM1 and GPE0 events will trigger a SCI
	 * instead of a SMI#. However, SMI_STS bits PM1_STS and GPE0_STS can
	 * still be set. Therefore, when SCI_EN is set, ignore PM1 and GPE0
	 * events in the SMI# handler, as these events have triggered a SCI.
	 * Do not ignore any other SMI# types, since they cannot cause a SCI.
	 */
	if (pmc_read_pm1_control() & SCI_EN)
		smi_sts &= ~(1 << PM1_STS_BIT | 1 << GPE0_STS_BIT);

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

static int em64t100_get_reg(const enum cpu_reg reg, const int node, void *out, const uint8_t length)
{
	em64t100_smm_state_save_area_t *state;
	u64 value;

	if (!out || length > sizeof(value))
		return -1;

	state = smm_get_save_state(node);
	if (!state)
		return -1;

	switch (reg) {
	case RAX:
		value = state->rax;
		break;
	case RBX:
		value = state->rbx;
		break;
	case RCX:
		value = state->rcx;
		break;
	case RDX:
		value = state->rdx;
		break;
	default:
		return -1;
	}

	memcpy(out, &value, length);
	return 0;
}

static int em64t100_set_reg(const enum cpu_reg reg, const int node, void *in, const uint8_t length)
{
	em64t100_smm_state_save_area_t *state;
	u64 value;

	if (!in || length > sizeof(value))
		return -1;

	state = smm_get_save_state(node);
	if (!state)
		return -1;

	value = 0;
	memcpy(&value, in, length);

	switch (reg) {
	case RAX:
		state->rax = value;
		break;
	case RBX:
		state->rbx = value;
		break;
	case RCX:
		state->rcx = value;
		break;
	case RDX:
		state->rdx = value;
		break;
	default:
		return -1;
	}

	return 0;
}

static int em64t100_apmc_node(u8 cmd)
{
	int node;

	for (node = 0; node < CONFIG_MAX_CPUS; node++) {
		em64t100_smm_state_save_area_t *state;
		u32 io_misc_info;
		u8 reg_al;

		state = smm_get_save_state(node);
		if (!state)
			continue;

		io_misc_info = state->io_misc_info;

		/* Synchronous I/O (bit0 == 1). */
		if (!(io_misc_info & (1 << 0)))
			continue;

		/* Write (bit4 == 0). */
		if (io_misc_info & (1 << 4))
			continue;

		/* APMC port. */
		if (((io_misc_info >> 16) & 0xff) != APM_CNT)
			continue;

		reg_al = (u8)state->rax;
		if (reg_al != cmd)
			continue;

		return node;
	}

	return -1;
}

static int em64t101_get_reg(const enum cpu_reg reg, const int node, void *out, const uint8_t length)
{
	em64t101_smm_state_save_area_t *state;
	u64 value;

	if (!out || length > sizeof(value))
		return -1;

	state = smm_get_save_state(node);
	if (!state)
		return -1;

	switch (reg) {
	case RAX:
		value = state->rax;
		break;
	case RBX:
		value = state->rbx;
		break;
	case RCX:
		value = state->rcx;
		break;
	case RDX:
		value = state->rdx;
		break;
	default:
		return -1;
	}

	memcpy(out, &value, length);
	return 0;
}

static int em64t101_set_reg(const enum cpu_reg reg, const int node, void *in, const uint8_t length)
{
	em64t101_smm_state_save_area_t *state;
	u64 value;

	if (!in || length > sizeof(value))
		return -1;

	state = smm_get_save_state(node);
	if (!state)
		return -1;

	value = 0;
	memcpy(&value, in, length);

	switch (reg) {
	case RAX:
		state->rax = value;
		break;
	case RBX:
		state->rbx = value;
		break;
	case RCX:
		state->rcx = value;
		break;
	case RDX:
		state->rdx = value;
		break;
	default:
		return -1;
	}

	return 0;
}

static int em64t101_apmc_node(u8 cmd)
{
	int node;

	for (node = 0; node < CONFIG_MAX_CPUS; node++) {
		em64t101_smm_state_save_area_t *state;
		u32 io_misc_info;
		u8 reg_al;

		state = smm_get_save_state(node);
		if (!state)
			continue;

		io_misc_info = state->io_misc_info;

		/* Synchronous I/O (bit0 == 1). */
		if (!(io_misc_info & (1 << 0)))
			continue;

		/* Write (bit4 == 0). */
		if (io_misc_info & (1 << 4))
			continue;

		/* APMC port. */
		if (((io_misc_info >> 16) & 0xff) != APM_CNT)
			continue;

		reg_al = (u8)state->rax;
		if (reg_al != cmd)
			continue;

		return node;
	}

	return -1;
}

static const uint32_t em64t100_revision_table[] = {
	0x30100,
	SMM_REV_INVALID,
};

static const uint32_t em64t101_revision_table[] = {
	0x30101,
	SMM_REV_INVALID,
};

const struct smm_save_state_ops em64t100_smm_ops = {
	.revision_table = em64t100_revision_table,
	.get_reg = em64t100_get_reg,
	.set_reg = em64t100_set_reg,
	.apmc_node = em64t100_apmc_node,
};


const struct smm_save_state_ops em64t101_smm_ops = {
	.revision_table = em64t101_revision_table,
	.get_reg = em64t101_get_reg,
	.set_reg = em64t101_set_reg,
	.apmc_node = em64t101_apmc_node,
};

const struct smm_save_state_ops *em64t100_ops = &em64t100_smm_ops;
const struct smm_save_state_ops *em64t101_ops = &em64t101_smm_ops;
