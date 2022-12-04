/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/intel/em64t101_save_state.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <cpu/x86/smm.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <soc/nvs.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/common/finalize.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmutil.h>
#include <types.h>

#include "pch.h"

int southbridge_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x32:
		printk(BIOS_DEBUG, "OS Init\n");
		/* gnvs->smif:
		 *  On success, the IO Trap Handler returns 0
		 *  On failure, the IO Trap Handler returns a value != 0
		 */
		gnvs->smif = 0;
		return 1; /* IO trap handled */
	}

	/* Not handled */
	return 0;
}

static void southbridge_gate_memory_reset_real(int offset,
					       u16 use, u16 io, u16 lvl)
{
	u32 reg32;

	/* Make sure it is set as GPIO */
	reg32 = inl(use);
	if (!(reg32 & (1 << offset))) {
		reg32 |= (1 << offset);
		outl(reg32, use);
	}

	/* Make sure it is set as output */
	reg32 = inl(io);
	if (reg32 & (1 << offset)) {
		reg32 &= ~(1 << offset);
		outl(reg32, io);
	}

	/* Drive the output low */
	reg32 = inl(lvl);
	reg32 &= ~(1 << offset);
	outl(reg32, lvl);
}

/*
 * Drive GPIO 60 low to gate memory reset in S3.
 *
 * Intel reference designs all use GPIO 60 but it is
 * not a requirement and boards could use a different pin.
 */
void southbridge_gate_memory_reset(void)
{
	u16 gpiobase;

	gpiobase = pci_read_config16(PCH_LPC_DEV, GPIOBASE) & 0xfffc;
	if (!gpiobase)
		return;

	if (CONFIG_DRAM_RESET_GATE_GPIO >= 32)
		southbridge_gate_memory_reset_real(CONFIG_DRAM_RESET_GATE_GPIO - 32,
						   gpiobase + GPIO_USE_SEL2,
						   gpiobase + GP_IO_SEL2,
						   gpiobase + GP_LVL2);
	else
		southbridge_gate_memory_reset_real(CONFIG_DRAM_RESET_GATE_GPIO,
						   gpiobase + GPIO_USE_SEL,
						   gpiobase + GP_IO_SEL,
						   gpiobase + GP_LVL);
}

void southbridge_smi_monitor(void)
{
#define IOTRAP(x) (trap_sts & (1 << x))
	u32 trap_sts, trap_cycle;
	u32 data, mask = 0;
	int i;

	trap_sts = RCBA32(0x1e00); // TRSR - Trap Status Register
	RCBA32(0x1e00) = trap_sts; // Clear trap(s) in TRSR

	trap_cycle = RCBA32(0x1e10);
	for (i=16; i<20; i++) {
		if (trap_cycle & (1 << i))
			mask |= (0xff << ((i - 16) << 2));
	}

	/* IOTRAP(3) SMI function call */
	if (IOTRAP(3)) {
		if (gnvs && gnvs->smif)
			io_trap_handler(gnvs->smif); // call function smif
		return;
	}

	/* IOTRAP(2) currently unused
	 * IOTRAP(1) currently unused */

	/* IOTRAP(0) SMIC */
	if (IOTRAP(0)) {
		if (!(trap_cycle & (1 << 24))) { // It's a write
			printk(BIOS_DEBUG, "SMI1 command\n");
			data = RCBA32(0x1e18);
			data &= mask;
			// if (smi1)
			//	southbridge_smi_command(data);
			// return;
		}
		// Fall through to debug
	}

	printk(BIOS_DEBUG, "  trapped io address = 0x%x\n", trap_cycle & 0xfffc);
	for (i=0; i < 4; i++) if (IOTRAP(i)) printk(BIOS_DEBUG, "  TRAP = %d\n", i);
	printk(BIOS_DEBUG, "  AHBE = %x\n", (trap_cycle >> 16) & 0xf);
	printk(BIOS_DEBUG, "  MASK = 0x%08x\n", mask);
	printk(BIOS_DEBUG, "  read/write: %s\n", (trap_cycle & (1 << 24)) ? "read" : "write");

	if (!(trap_cycle & (1 << 24))) {
		/* Write Cycle */
		data = RCBA32(0x1e18);
		printk(BIOS_DEBUG, "  iotrap written data = 0x%08x\n", data);
	}
#undef IOTRAP
}

/*
 * PCH BIOS Spec Rev 0.7.0, Section 13.5
 * Additional xHCI Controller Configurations Prior to Entering S3/S4
 */
static void xhci_a0_suspend_smm_workaround(void)
{
	/* Workaround only applies to Panther Point stepping A0 */
	if (pch_silicon_revision() != PCH_STEP_A0)
		return;

	/* The BAR is 64-bit, account for it being above 4 GiB */
	if (pci_read_config32(PCH_XHCI_DEV, PCI_BASE_ADDRESS_0 + 4))
		return;

	/* PCH datasheet indicates that only the upper 16 bits are valid */
	uintptr_t xhci_bar = pci_read_config32(PCH_XHCI_DEV, PCI_BASE_ADDRESS_0) &
			~PCI_BASE_ADDRESS_MEM_ATTR_MASK;

	if (smm_points_to_smram((void *)xhci_bar, 64 * KiB))
		return;

	/* Step 1: Set power state to D0 */
	pci_and_config16(PCH_XHCI_DEV, XHCI_PWR_CNTL_STS, ~(3 << 0));

	/* Step 2 */
	pci_or_config16(PCH_XHCI_DEV, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* Steps 3 to 6: If USB3 PORTSC current connect status (bit 0) is set, do IOBP magic */
	for (unsigned int port = 0; port < 4; port++) {
		if (read32p((xhci_bar + XHCI_PORTSC_x_USB3(port))) & (1 << 0))
			pch_iobp_update(0xec000082 + 0x100 * port, ~0, 3 << 2);
	}

	/* Step 7 */
	pci_and_config16(PCH_XHCI_DEV, PCI_COMMAND, ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY));

	/* Step 8: Set power state to D3 */
	pci_or_config16(PCH_XHCI_DEV, XHCI_PWR_CNTL_STS, 3 << 0);
}

void southbridge_smm_xhci_sleep(u8 slp_type)
{
	/* Only Panther Point has xHCI */
	if (pch_silicon_type() != PCH_TYPE_PPT)
		return;

	/* Verify that RCBA is still valid */
	if (pci_read_config32(PCH_LPC_DEV, RCBA) != (CONFIG_FIXED_RCBA_MMIO_BASE | RCBA_ENABLE))
		return;

	if (RCBA32(FD) & PCH_DISABLE_XHCI)
		return;

	switch (slp_type) {
	case ACPI_S3:
	case ACPI_S4:
		xhci_a0_suspend_smm_workaround();
		break;

	case ACPI_S5:
		/*
		 * PCH BIOS Spec Rev 0.7.0, Section 13.5
		 * Additional xHCI Controller Configurations Prior to Entering S5
		 *
		 * For all steppings:
		 *   Step 1: Set power state to D3 (bits 1:0)
		 *   Step 2: Set PME# enable bit (bit 8)
		 */
		pci_or_config16(PCH_XHCI_DEV, XHCI_PWR_CNTL_STS, 1 << 8 | 3 << 0);
		break;
	}
}

void southbridge_finalize_all(void)
{
	intel_me_finalize_smm();
	intel_pch_finalize_smm();
	intel_sandybridge_finalize_smm();
	intel_model_206ax_finalize_smm();
}
