/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2006 MSI
 * (Written by Bingxun Shi <bingxunshi@gmail.com> for MSI)
 * Copyright (C) 2008 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

unsigned int get_sbdn(unsigned bus);

#if CONFIG_K8_REV_F_SUPPORT
#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0
#endif

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/ite/it8712f/early_serial.c"
#include "southbridge/via/vt8237r/early_smbus.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"
#include <spd.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8712F_SP1)
#define WATCHDOG_DEV PNP_DEV(0x2e, IT8712F_GPIO)

#define IT8712F_GPIO_BASE		0x0a20

static void memreset(int controllers, const struct mem_controller *ctrl) { }
static void activate_spd_rom(const struct mem_controller *ctrl) { }

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "southbridge/via/k8t890/early_car.c"
#include "northbridge/amd/amdk8/amdk8.h"
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "northbridge/amd/amdk8/raminit_f.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/dualcore/dualcore.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#include "cpu/amd/model_fxx/fidvid.c"
#include "northbridge/amd/amdk8/resourcemap.c"

void soft_reset(void)
{
	uint8_t tmp;

	set_bios_reset();
	print_debug("soft reset\n");

	/* PCI reset */
	tmp = pci_read_config8(PCI_DEV(0, 0x11, 0), 0x4f);
	tmp |= 0x01;
	/* FIXME from S3 set bit1 to disable USB reset VT8237A/S */
	pci_write_config8(PCI_DEV(0, 0x11, 0), 0x4f, tmp);

	while (1) {
		/* daisy daisy ... */
		hlt();
	}
}

unsigned int get_sbdn(unsigned bus)
{
	device_t dev;

	dev = pci_locate_device_on_bus(PCI_ID(PCI_VENDOR_ID_VIA,
					PCI_DEVICE_ID_VIA_VT8237R_LPC), bus);
	return (dev >> 15) & 0x1f;
}

struct gpio_init_val {
	u8 addr;
	u8 val;
};

static const struct gpio_init_val gpio_init_data[] = {
	/* multi-function pin selection */
	{ 0x25, 0x00 },
	{ 0x28, 0x00 }, /* gp46 is infrared receive input */
	{ 0x29, 0x40 }, /* reserved value?!? */
	{ 0x2a, 0x00 },
	{ 0x2c, 0x1d }, /* pin91 is VIN7 instead of PCIRSTIN# */
	/* gpio i/o port base */
	{ 0x62, IT8712F_GPIO_BASE >> 8 },
	{ 0x63, IT8712F_GPIO_BASE & 0xff },
	/* 0xb8 - 0xbc: gpio pull-up enable */
	{ 0xb8, 0x00 },
	/* 0xc0 - 0xc4: gpio alternate function select */
	{ 0xc0, 0x00 },
	{ 0xc3, 0x00 },
	{ 0xc4, 0xc0 },
	/* 0xc8 - 0xcc: gpio output enable */
	{ 0xc8, 0x00 },
	{ 0xcb, 0x00 },
	{ 0xcc, 0xc0 },
	/* end of list */
	{ 0, 0 },
};

static void m2v_it8712f_gpio_init(void)
{
	const struct gpio_init_val *giv;

	printk(BIOS_SPEW, "it8712f gpio init...\n");

	/*
	 * it8712f gpio config
	 *
	 * Most importantly this switches pin 91 from
	 * PCIRSTIN# to VIN7.
	 * Note that only PCIRST3# and PCIRST5# are affected
	 * by PCIRSTIN#, the PCIRST1#, PCIRST2#, PCIRST4# are always
	 * direct buffers of #LRESET (low pin count bus reset).
	 * If this is not done All PCIRST are in reset state and the
	 * pcie slots don't initialize.
	 *
	 * pci reset handling:
	 * pin 91: VIN7 (alternate PCIRSTIN#)
	 * pin 48: PCIRST5# / gpio port 5 bit 0
	 * pin 84: PCIRST4# / gpio port 1 bit 0
	 * pin 31: PCIRST1# / gpio port 1 bit 4
	 * pin 33: PCIRST2# / gpio port 1 bit 2
	 * pin 34: PCIRST3# / gpio port 1 bit 1
	 *
	 * PCIRST[0-5]# are connected as follows:
	 * pcirst1# -> pci bus
	 * pcirst2# -> ide bus
	 * pcirst3# -> pcie devices
	 * pcirst4# -> pcie graphics
	 * pcirst5# -> maybe n/c (untested)
	 *
	 * For software control of PCIRST[1-5]#:
	 * 0x2a=0x17 (deselect pcirst# hardwiring, enable 0x25 control)
	 * 0x25=0x17 (select gpio function)
	 * 0xc0=0x17, 0xc8=0x17 gpio port 1 select & output enable
	 * 0xc4=0xc1, 0xcc=0xc1 gpio port 5 select & output enable
	 */
	it8712f_enter_conf();
	giv = gpio_init_data;
	while (giv->addr) {
		printk(BIOS_SPEW, "it8712f gpio: %02x=%02x\n",
				giv->addr, giv->val);
		it8712f_sio_write(IT8712F_GPIO, giv->addr, giv->val);
		giv++;
	}
	it8712f_exit_conf();

	printk(BIOS_INFO, "it8712f gpio: Setting DDR2 voltage to 1.80V\n");
	/*
	 * upper two bits of gpio_base+4 control ddr2 voltage:
	 * 11: 1.80V
	 * 01: 1.85V
	 * 10: 1.90V
	 * 00: 1.95V
	 *
	 * The lower six bits are inputs and normally read back as 1
	 * (except bit 2, which seems rather random).
	 */
	outb(0xff, IT8712F_GPIO_BASE+4);
}

static void m2v_bus_init(void)
{
	device_t dev;

	printk(BIOS_SPEW, "m2v_bus_init\n");

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_K8T890CF_0), 0);
	pci_write_config8(dev, K8T890_MULTIPLE_FN_EN, 0x01);

	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_K8T890CF_5), 0);
	/*
	 * bit | meaning
	 * 6   | 0: hide scratch register function 0:0.6 (we don't use it)
	 * 5   | 1: enable pcie bridge 0:2.0
	 * 4   | 0: hide   pcie bridge 0:3.3 (not connected)
	 * 3   | 1: enable pcie bridge 0:3.2
	 * 2   | 1: enable pcie bridge 0:3.1
	 * 1   | 1: enable pcie bridge 0:3.0
	 */
	pci_write_config8(dev, 0xf0, 0x2e);
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr[] = {
		// Node 0
		DIMM0, DIMM2, 0, 0,
		DIMM1, DIMM3, 0, 0,
		// Node 1
		DIMM4, DIMM6, 0, 0,
		DIMM5, DIMM7, 0, 0,
	};
	unsigned bsp_apicid = 0;
	int needs_reset = 0;
	struct sys_info *sysinfo = &sysinfo_car;

	it8712f_24mhz_clkin();
	it8712f_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	it8712f_kill_watchdog();
	console_init();
	enable_rom_decode();
	m2v_bus_init();
	m2v_it8712f_gpio_init();
	it8712f_enable_3vsbsw();

	printk(BIOS_INFO, "now booting... \n");

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx, sysinfo);

	/* Halt if there was a built in self test failure. */
	report_bist_failure(bist);
	setup_default_resource_map();
	setup_coherent_ht_domain();
	wait_all_core0_started();

	printk(BIOS_INFO, "now booting... All core 0 started\n");

#if CONFIG_LOGICAL_CPUS
	/* It is said that we should start core1 after all core0 launched. */
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);
#endif
	init_timer();
	ht_setup_chains_x(sysinfo); /* Init sblnk and sbbusn, nodes, sbdn. */

	needs_reset = optimize_link_coherent_ht();
	print_debug_hex8(needs_reset);
	needs_reset |= optimize_link_incoherent_ht(sysinfo);
	print_debug_hex8(needs_reset);
	needs_reset |= k8t890_early_setup_ht();
	print_debug_hex8(needs_reset);

	if (needs_reset) {
		printk(BIOS_DEBUG, "ht reset -\n");
		soft_reset();
		printk(BIOS_DEBUG, "FAILED!\n");
	}

	/* the HT settings needs to be OK, because link freq chnage may cause HT disconnect */
	/* allow LDT STOP asserts */
	vt8237_sb_enable_fid_vid();

	enable_fid_change();
	print_debug("after enable_fid_change\n");

	init_fidvid_bsp(bsp_apicid);

	/* Stop the APs so we can start them later in init. */
	allow_all_aps_stop(bsp_apicid);

	/* It's the time to set ctrl now. */
	fill_mem_ctrl(sysinfo->nodes, sysinfo->ctrl, spd_addr);
	enable_smbus();
	sdram_initialize(sysinfo->nodes, sysinfo->ctrl, sysinfo);
	post_cache_as_ram();
}
