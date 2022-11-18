/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/int15.h>
#include <ec/acpi/ec.h>
#include <halt.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pmclib.h>
#include <rtc.h>
#include <soc/nhlt.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include "include/ec.h"
#include "include/gpio.h"

static unsigned long mainboard_write_acpi_tables(
	const struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;

	start_addr = current;

	nhlt = nhlt_init();
	if (!nhlt) {
		return start_addr;
	}

	/* Override subsystem ID */
	nhlt->subsystem_id = 0x10251037;

	/* 1 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 1) != 0) {
		printk(BIOS_ERR, "Couldn't add 1CH DMIC array.\n");
	}

	/* 2 Channel DMIC array. */
	if (nhlt_soc_add_dmic_array(nhlt, 2) != 0) {
		printk(BIOS_ERR, "Couldn't add 2CH DMIC array.\n");
	}

	end_addr = nhlt_soc_serialize(nhlt, start_addr);

	if (end_addr != start_addr) {
		acpi_add_table(rsdp, (void *)start_addr);
	}

	return end_addr;
}

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);

	if (CONFIG(INCLUDE_NHLT_BLOBS)) {
		dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
	}
}

/* Update the EC's clock. */
static void ec_send_time(void)
{
	struct rtc_time time;
	uint8_t ec_time_byte;

	rtc_get(&time);

	/* RTC time could be negative (before 2016) */
	int32_t ec_time = ((time.year << 26) + (time.mon << 22) + (time.mday << 17)
			+ (time.hour << 12) + (time.min << 6) + (time.sec)
			/* 16 years */
			- 0x40000000);

	printk(BIOS_DEBUG, "EC: reporting present time 0x%x\n", ec_time);
	send_ec_command(0xE0);
	for (int i = 0; i < 4; i++) {
		/* Shift bytes */
		ec_time_byte = (uint8_t)(ec_time >> (i * 8));
		printk(BIOS_DEBUG, "EC: Sending 0x%x (iteration %d)\n", ec_time_byte, i);
		send_ec_data(ec_time_byte);
	}

	printk(BIOS_DEBUG, "EC: response 0x%x\n", recv_ec_data());
}

static void ec_requests_time(void)
{
	/* This is executed as protocol notify in vendor's RtKbcDriver
	   when *CommonService protocol is installed. Effectively,
	   this code could execute from the entrypoint */
	uint8_t dat = ec_cmd_90_read(0x79);
	if (dat & 1) {
		ec_send_time();
	}
}

/*
 * Init from vendor's PeiOemModule. KbcPeim does not appear to be used
 * (It implements commands also found in RtKbcDriver and SmmKbcDriver).
 *
 * Mostly, this puts the system back to sleep if the lid is closed during
 * an S3 resume.
 */
static void ec_init(void)
{
	/* This is called via a "$FNC" in a PeiOemModule pointer table,
	   with "$DPX" on SiInit */
	outb(0x5A, 0x6C);	// 6Ch is the EC sideband port
	if (acpi_is_wakeup_s3()) {
		/* "MLID" in LGMR-based memory map is equivalent to "ELID" in EC-based
		   memory map. Vendor firmware accesses through LGMR; remapped
		   - ec_cmd* function calls will not remapped */
		uint8_t power_state = ec_read(0x70);
		if (!(power_state & 2)) {	// Lid is closed
			uint8_t out_data = ec_cmd_90_read(0x0A);
			if (!(out_data & 2)) {
				ec_cmd_91_write(0x0A, out_data | 2);
			}

			/* Clear below events and go back to sleep */
			/* Clear ABase PM1_STS - RW/1C set bits */
			pmc_clear_pm1_status();
			/* Clear ABase GPE0_STS[127:96] - RW/1C set bits */
			uint32_t gpe_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
			outl(gpe_sts, ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
			/* Clear xHCI PM_CS[PME_Status] - RW/1C -
			   and disable xHCI PM_CS[PME_En] */
			pci_update_config16(PCH_DEV_XHCI, 0x74, ~0x100, 0x8000);

			/* Enter S3 sleep */
			pmc_enable_pm1_control(SLP_EN | (SLP_TYP_S3 << SLP_TYP_SHIFT));
			halt();
		}
	}
}

static void mainboard_init(void *chip_info)
{
	mainboard_config_stage_gpios();
	/* Notify EC */
	ec_init();
	/* Program the same 64K range of EC memory as vendor FW
	   - Open unconditionally, user can select whether ACPI uses LGMR */
	lpc_open_mmio_window(0xFE800000, 0x10000);
	/* EC is notified of platform resets with UEFI firmware, but coreboot
	   does not offer this service to boards */
	ec_requests_time();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.init = mainboard_init,
};
