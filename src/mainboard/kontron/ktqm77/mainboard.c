/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <console/console.h>
#if CONFIG(VGA_ROM_RUN)
#include <x86emu/x86emu.h>
#endif
#include <option.h>
#include <arch/interrupt.h>
#include <southbridge/intel/bd82x6x/pch.h>

#if CONFIG(VGA_ROM_RUN)
static int int15_handler(void)
{
	int res = 0;

	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
			  __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_EAX & 0xffff) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video BIOS default
		 */
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x005f;
		X86_ECX &= 0xffffff00;
		X86_ECX |= 0x00;	/* Use video BIOS default */
		res = 1;
		break;
	case 0x5f35:
		/*
		 * Boot Display Device Hook:
		 *  bit 0 = CRT
		 *  bit 1 = TV (eDP)
		 *  bit 2 = EFP
		 *  bit 3 = LFP
		 *  bit 4 = CRT2
		 *  bit 5 = TV2 (eDP)
		 *  bit 6 = EFP2
		 *  bit 7 = LFP2
		 */
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x005f;
		X86_ECX &= 0xffff0000;
		X86_ECX |= 0x0000;	/* Use video BIOS default */
		res = 1;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SDVO-LVDS, LFP driven by SDVO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x005f;
		X86_ECX &= 0xffff0000;
		X86_ECX |= 0x0000; /* TODO: Make this configurable in NVRAM? */
		res = 1;
		break;
	case 0x5f40:
		/*
		 * Boot Panel Type Hook:
		 *  BL(in): 00h = LFP, 01h = LFP2
		 *  CL(out): panel type id in table: 1..16
		 */
		if (0 == (X86_EBX & 0xff)) {
			X86_EAX &= 0xffff0000;
			X86_EAX |= 0x015f;
			res = 1;
		} else if (1 == (X86_EBX & 0xff)) {
			X86_EAX &= 0xffff0000;
			X86_EAX |= 0x015f;
			res = 1;
		} else {
			printk(BIOS_DEBUG,
			       "Unknown panel index %u "
			       "in INT15 function %04x!\n",
			       X86_EBX & 0xff, X86_EAX & 0xffff);
		}
		break;
	case 0x5f52:
		/*
		 * Panel Color Depth:
		 *  00h = 18 bit
		 *  01h = 24 bit
		 */
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x005f;
		X86_ECX &= 0xffff0000;
		X86_ECX |= 0x0001;
		res = 1;
		break;
	case 0x5f14:
		if ((X86_EBX & 0xffff) == 0x78f) {
			/*
			 * Get Miscellaneous Status Hook:
			 *  bit 2: AC power active?
			 *  bit 1: lid closed?
			 *  bit 0: docked?
			 */
			X86_EAX &= 0xffff0000;
			X86_EAX |= 0x015f;
			res = 1;
		} else {
			printk(BIOS_DEBUG,
			       "Unknown BX 0x%04x in INT15 function %04x!\n",
			       X86_EBX & 0xffff, X86_EAX & 0xffff);
		}
		break;
	case 0x5f49:
		/*
		 * Get Inverter Type and Polarity:
		 *  EBX: backlight control brightness: 0..255
		 *  ECX:
		 *   0 = Enable PWM inverted, 2 = Enable PWM
		 *   1 = Enable I2C inverted, 3 = Enable I2C
		 */
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x015f;
		res = 1;
		break;
	default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
		       X86_EAX & 0xffff);
		break;
	}
	return res;
}
#endif



/* mainboard_enable is executed as first thing after */
/* enumerate_buses(). */

static void mainboard_enable(struct device *dev)
{
#if CONFIG(PCI_OPTION_ROM_RUN_YABEL) || \
	CONFIG(PCI_OPTION_ROM_RUN_REALMODE)
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif

	unsigned int disable = 0;
	if ((get_option(&disable, "ethernet1") == CB_SUCCESS) && disable) {
		struct device *nic = pcidev_on_root(0x1c, 2);
		if (nic) {
			printk(BIOS_DEBUG, "DISABLE FIRST NIC!\n");
			nic->enabled = 0;
		}
	}
	disable = 0;
	if ((get_option(&disable, "ethernet2") == CB_SUCCESS) && disable) {
		struct device *nic = pcidev_on_root(0x1c, 3);
		if (nic) {
			printk(BIOS_DEBUG, "DISABLE SECOND NIC!\n");
			nic->enabled = 0;
		}
	}
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
