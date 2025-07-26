/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/acpi/ec.h>
#include <gpio.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <soc/nvs.h>
#include <southbridge/intel/common/gpio.h>
#include "ec_oem.c"

#define MAX_LCD_BRIGHTNESS 0xd8

int mainboard_io_trap_handler(int smif)
{
	u8 reg8;

	switch (smif) {
	case 0x2b:
		printk(BIOS_DEBUG, "CPU power state switch\n");
		// TODO, move to CPU handler?
		break;
	case 0x3d:
		printk(BIOS_DEBUG, "Enable C-State SMM coordination\n");
		// TODO, move to CPU handler?
		break;
	case 0x46:
		printk(BIOS_DEBUG, "S3 DTS SMI (completely re-enable DTS)\n");
		// TODO, move to CPU handler?
		break;
	case 0x47:
		printk(BIOS_DEBUG, "S4 DTS SMI (Update NVS DTS temperature)\n");
		// TODO, move to CPU handler?
		break;
	case 0xc0:
		printk(BIOS_DEBUG, "Disable RF\n");
		// TODO
		break;
	case 0xd0:
		printk(BIOS_DEBUG, "ACBS LAN Power on\n");
		// TODO
		break;
	case 0xd1:
		printk(BIOS_DEBUG, "ACBS LAN Power off\n");
		// TODO
		break;
	case 0xd2:
		printk(BIOS_DEBUG, "Check AC status\n");
		// TODO
		break;
	case 0xd3:
		printk(BIOS_DEBUG, "Enable Bluetooth\n");
		// TODO
		break;
	case 0xd4:
		printk(BIOS_DEBUG, "Disable Bluetooth\n");
		// TODO
		break;
	case 0xd5:
		printk(BIOS_DEBUG, "Set Brightness\n");
		reg8 = gnvs->brtl;
		printk(BIOS_DEBUG, "brtl: %x\n", reg8);
		ec_write(0x17, reg8);
		break;
	case 0xd6:
		printk(BIOS_DEBUG, "Get Brightness\n");
		reg8 = ec_read(0x17);
		printk(BIOS_DEBUG, "brtl: %x\n", reg8);
		gnvs->brtl = reg8;
		break;
	case 0xd7:
		printk(BIOS_DEBUG, "Get ECO mode status\n");
		// TODO
		break;
	case 0xd8:
		printk(BIOS_DEBUG, "Get sunlight readable status\n");
		// TODO
		break;
	case 0xd9:
		printk(BIOS_DEBUG, "Get docking connection\n");
		// TODO
		break;
	case 0xda:
		printk(BIOS_DEBUG, "Power off docking\n");
		// TODO
		break;
	case 0xdc:
		printk(BIOS_DEBUG, "EC: Turn on LED on ECO enable\n");
		// TODO
		break;
	case 0xdd:
		printk(BIOS_DEBUG, "EC: Turn off LED on ECO disable\n");
		// TODO
		break;
	case 0xde:
		printk(BIOS_DEBUG, "LAN power off\n");
		gpio_set(24, 0); // Disable LAN Power
		break;
	case 0xdf:
		printk(BIOS_DEBUG, "RF enable\n");
		// TODO
		break;
	case 0xe0:
		printk(BIOS_DEBUG, "Get RTC wake flag\n");
		// TODO
		break;
	case 0xe1:
		printk(BIOS_DEBUG, "Hotkey function\n");
		// TODO
		break;
	case 0xe3:
		printk(BIOS_DEBUG, "ECO disable\n");
		// TODO
		break;
	default:
		return 0;
	}

	/* gnvs->smif:
	 *   On success, the IO Trap Handler returns 0
	 *   On failure, the IO Trap Handler returns a value != 0
	 */
	gnvs->smif = 0;
	return 1;
}

static void mainboard_smi_hotkey(u8 hotkey)
{
	u8 reg8;

	switch (hotkey) {
	case 0x3b: break; // Fn+F1
	case 0x3c: break; // Fn+F2
	case 0x3d: break; // Fn+F3
	case 0x3e: break; // Fn+F4
	case 0x3f: break; // Fn+F5
	case 0x40:        // Fn+F6 (Decrease Display Brightness)
		   reg8 = ec_read(0x17);
		   reg8 = (reg8 > 8) ? (reg8 - 8) : 0;
		   ec_write(0x17, reg8);
		   return;
	case 0x41:        // Fn+F7 (Increase Display Brightness)
		   reg8 = ec_read(0x17);
		   reg8 += 8;
		   reg8 = (reg8 >= MAX_LCD_BRIGHTNESS) ? MAX_LCD_BRIGHTNESS : reg8;
		   ec_write(0x17, reg8);
		   return;
	case 0x42: break; // Fn+F8
	case 0x43: break; // Fn+F9
	case 0x44: break; // Fn+F10
	case 0x57: break; // Fn+F11
	case 0x58: break; // Fn+F12
	}
	printk(BIOS_DEBUG, "EC hotkey: %02x\n", hotkey);
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	u8 source, hotkey;
	send_ec_oem_command(0x5c);
	source = recv_ec_oem_data();

	switch (source) {
	case 0:
		// Some kind of ACK?
		break;
	case 1:
		send_ec_oem_command(0x59);
		hotkey = recv_ec_oem_data();
		mainboard_smi_hotkey(hotkey);
		break;
	default:
		printk(BIOS_DEBUG, "EC SMI source: %02x\n", source);
	}
}
