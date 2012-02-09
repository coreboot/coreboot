/*
 * This file is part of the coreboot project.
 *
 * (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <device/device.h>
#include <console/console.h>
#include <delay.h>
#include <stdlib.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>

#define MSR_IA32_PERF_STATUS	0x00000198
#define MSR_IA32_PERF_CTL	0x00000199
#define MSR_IA32_MISC_ENABLE	0x000001a0

static int c7a_speed_translation[] = {
//      LFM     HFM
	0x0409, 0x0f13,		// 400MHz, 844mV --> 1500MHz, 1.004V    C7-M
	0x0409, 0x1018,		// 400MHz, 844mV --> 1600MHz, 1.084V
	0x0409, 0x0c18,		// 533MHz, 844mV --> 1600MHz, 1.084V
	0x0409, 0x121c,		// 400MHz, 844mV --> 1800MHz, 1.148V
	0x0409, 0x0e1c,		// 533MHz, 844mV --> 1860MHz, 1.148V
	0x0409, 0x141f,		// 400MHz, 844mV --> 2000MHz, 1.196V
	0x0409, 0x0f1f,		// 533MHz, 844mV --> 2000MHz, 1.196V
	0x0406, 0x0a06,		// 400MHz, 796mV --> 1000MHz, 796mV     C7-M ULV
	0x0406, 0x0a09,		// 400MHz, 796mV --> 1000MHz, 844mV
	0x0406, 0x0c09,		// 400MHz, 796mV --> 1200MHz, 844mV
	0x0406, 0x0f10,		// 400MHz, 796mV --> 1500MHz, 956mV
};

static int c7d_speed_translation[] = {
//      LFM     HFM
	0x0409, 0x1018,		// 400MHz, 844mV --> 1600MHz, 1.084V    C7-M
	0x0409, 0x121f,		// 400MHz, 844mV --> 1800MHz, 1.196V
	0x0809, 0x121f,		// 800MHz, 844mV --> 1800MHz, 1.196V
	0x0409, 0x141f,		// 400MHz, 844mV --> 2000MHz, 1.196V
	0x0809, 0x141f,		// 800MHz, 844mV --> 2000MHz, 1.196V
	0x0406, 0x0806,		// 400MHz, 796mV --> 800MHz, 796mV      C7-M ULV
	0x0406, 0x0a06,		// 400MHz, 796mV --> 1000MHz, 796mV
	0x0406, 0x0c09,		// 400MHz, 796mV --> 1200MHz, 844mV
	0x0806, 0x0c09,		// 800MHz, 796mV --> 1200MHz, 844mV
	0x0406, 0x0f10,		// 400MHz, 796mV --> 1500MHz, 956mV
	0x0806, 0x1010,		// 800MHz, 796mV --> 1600MHz, 956mV
};

static void set_c7_speed(int model) {
	int cnt, current, new, i;
	msr_t msr;
	printk(BIOS_DEBUG, "Enabling improved C7 clock and voltage.\n");

	// Enable Speedstep
	msr = rdmsr(MSR_IA32_MISC_ENABLE);
	msr.lo |= (1 << 16);
	wrmsr(MSR_IA32_MISC_ENABLE, msr);

	msr = rdmsr(MSR_IA32_PERF_STATUS);

	printk(BIOS_INFO, "Voltage: %dmV (min %dmV; max %dmV)\n",
		    ((int)(msr.lo & 0xff) * 16 + 700),
		    ((int)((msr.hi >> 16) & 0xff) * 16 + 700),
		    ((int)(msr.hi & 0xff) * 16 + 700));

	printk(BIOS_INFO, "CPU multiplier: %dx (min %dx; max %dx)\n",
		    (int)((msr.lo >> 8) & 0xff),
		    (int)((msr.hi >> 24) & 0xff), (int)((msr.hi >> 8) & 0xff));

	printk(BIOS_DEBUG, " msr.lo = %x\n", msr.lo);

	/* Wait while CPU is busy */
	cnt = 0;
	while (msr.lo & ((1 << 16) | (1 << 17))) {
		udelay(16);
		msr = rdmsr(MSR_IA32_PERF_STATUS);
		cnt++;
		if (cnt > 128) {
			printk(BIOS_WARNING, "Could not update multiplier and voltage.\n");
			return;
		}
	}

	current = msr.lo & 0xffff;

	// Start out with no change.
	new = current;
	switch (model) {
	case 10:		// model A
		for (i = 0; i < ARRAY_SIZE(c7a_speed_translation); i += 2) {
			if ((c7a_speed_translation[i] == current) &&
			    ((c7a_speed_translation[i + 1] & 0xff00) ==
			     (msr.hi & 0xff00))) {
				new = c7a_speed_translation[i + 1];
			}
		}
		break;
	case 13:		// model D
		for (i = 0; i < ARRAY_SIZE(c7d_speed_translation); i += 2) {
			if ((c7d_speed_translation[i] == current) &&
			    ((c7d_speed_translation[i + 1] & 0xff00) ==
			     (msr.hi & 0xff00))) {
				new = c7d_speed_translation[i + 1];
			}
		}
		break;
	default:
		print_info("CPU type not known, multiplier unchanged.\n");
	}

	msr.lo = new;
	msr.hi = 0;
	printk(BIOS_DEBUG, " new msr.lo = %x\n", msr.lo);

	wrmsr(MSR_IA32_PERF_CTL, msr);

	/* Wait until the power transition ends */
	cnt = 0;
	do {
		udelay(16);
		msr = rdmsr(MSR_IA32_PERF_STATUS);
		cnt++;
		if (cnt > 128) {
			printk(BIOS_WARNING, "Error while updating multiplier and voltage\n");
			break;
		}
	} while (msr.lo & ((1 << 16) | (1 << 17)));

	printk(BIOS_INFO, "Current voltage: %dmV\n", ((int)(msr.lo & 0xff) * 16 + 700));
	printk(BIOS_INFO, "Current CPU multiplier: %dx\n", (int)((msr.lo >> 8) & 0xff));
}

static void c7_init(device_t dev)
{
	u8 brand;
	struct cpuinfo_x86 c;
	msr_t msr;

	get_fms(&c, dev->device);

	printk(BIOS_INFO, "Detected VIA ");

	switch (c.x86_model) {
	case 10:
		msr = rdmsr(0x1153);
		brand = (((msr.lo >> 2) ^ msr.lo) >> 18) & 3;
		printk(BIOS_INFO, "Model A ");
		break;
	case 13:
		msr = rdmsr(0x1154);
		brand = (((msr.lo >> 4) ^ (msr.lo >> 2))) & 0x000000ff;
		printk(BIOS_INFO, "Model D ");
		break;
	default:
		printk(BIOS_INFO, "Model Unknown ");
		brand = 0xff;
	}

	switch (brand) {
	case 0:
		printk(BIOS_INFO, "C7-M\n");
		break;
	case 1:
		printk(BIOS_INFO, "C7\n");
		break;
	case 2:
		printk(BIOS_INFO, "Eden\n");
		break;
	case 3:
		printk(BIOS_INFO, "C7-D\n");
		break;
	default:
		printk(BIOS_INFO, "%02x (please report)\n", brand);
	}

	/* Gear up */
	set_c7_speed(c.x86_model);

	/* Enable APIC */
	msr = rdmsr(0x1107);
	msr.lo |= 1<<24;
	wrmsr(0x1107, msr);

	/* Turn on cache */
	x86_enable_cache();

	/* Set up Memory Type Range Registers */
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init = c7_init,
};

/* Look in arch/x86/lib/cpu.c:cpu_initialize. If there is no CPU with an exact
 * ID, the cpu mask (stepping) is masked out and the check is repeated. This
 * allows us to keep the table significantly smaller.
 */

static struct cpu_device_id cpu_table[] = {
	{X86_VENDOR_CENTAUR, 0x06A0},	// VIA C7 Esther
	{X86_VENDOR_CENTAUR, 0x06A9},	// VIA C7 Esther
	{X86_VENDOR_CENTAUR, 0x06D0},	// VIA C7-M
	{0, 0},
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};
