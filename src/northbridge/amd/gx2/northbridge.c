/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Nils Jacobs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include "northbridge.h"
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/vr.h>
#include <cpu/cpu.h>
#include "../../../southbridge/amd/cs5536/cs5536.h"

void print_conf(void);

/* Print the platform configuration - do before PCI init or it will not
 * work right.
 */
void print_conf(void)
{
#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_ERR
	int i;
	unsigned long iol;
	msr_t msr;

	int cpu_msr_defs[] = { CPU_IM_CONFIG, CPU_DM_CONFIG0,
		CPU_RCONF_DEFAULT, CPU_RCONF_BYPASS, CPU_RCONF_A0_BF,
		CPU_RCONF_C0_DF, CPU_RCONF_E0_FF, CPU_RCONF_SMM, CPU_RCONF_DMM,
		GLCP_DELAY_CONTROLS, GL_END
	};

	int gliu0_msr_defs[] = { GLIU0_P2D_BM_0, GLIU0_P2D_BM_1,
		GLIU0_P2D_BM_2, GLIU0_P2D_BM_3, GLIU0_P2D_BM_4,
		GLIU0_P2D_BM_5, GLIU0_P2D_BMO_0, GLIU0_P2D_BMO_1,
		GLIU0_P2D_R_0, GLIU0_P2D_RO_0, GLIU0_P2D_RO_1,
		GLIU0_P2D_RO_2, GLIU0_P2D_SC_0, GLIU0_IOD_BM_0, GLIU0_IOD_BM_1,
		GLIU0_IOD_BM_2, GLIU0_IOD_SC_0, GLIU0_IOD_SC_1, GLIU0_IOD_SC_2,
		GLIU0_IOD_SC_3, GLIU0_IOD_SC_4, GLIU0_IOD_SC_5,
		GLIU0_GLD_MSR_COH, GL_END
	};

	int gliu1_msr_defs[] = { GLIU1_P2D_BM_0, GLIU1_P2D_BM_1,
		GLIU1_P2D_BM_2, GLIU1_P2D_BM_3, GLIU1_P2D_BM_4,
		GLIU1_P2D_BM_5, GLIU1_P2D_BM_6, GLIU1_P2D_BM_7,
		GLIU1_P2D_BM_8, GLIU1_P2D_R_0, GLIU1_P2D_R_1,
		GLIU1_P2D_R_2, GLIU1_P2D_R_3, GLIU1_P2D_SC_0,
		GLIU1_IOD_BM_0, GLIU1_IOD_BM_1, GLIU1_IOD_BM_2, GLIU1_IOD_SC_0,
		GLIU1_IOD_SC_1, GLIU1_IOD_SC_2, GLIU1_IOD_SC_3, GLIU1_IOD_SC_4,
		GLIU1_IOD_SC_5, GLIU1_GLD_MSR_COH, GL_END
	};

	int rconf_msr[] = { CPU_RCONF0, CPU_RCONF1, CPU_RCONF2, CPU_RCONF3,
		CPU_RCONF4, CPU_RCONF5, CPU_RCONF6, CPU_RCONF7, GL_END
	};

	int lbar_msr[] = { MDD_LBAR_GPIO, MDD_LBAR_FLSH0, MDD_LBAR_FLSH1, GL_END
	};

	int irq_msr[] = { MDD_IRQM_YLOW, MDD_IRQM_YHIGH, MDD_IRQM_ZLOW, MDD_IRQM_ZHIGH,
		MDD_IRQM_PRIM, GL_END
	};

	int pci_msr[] = { GLPCI_CTRL, GLPCI_ARB, GLPCI_REN, GLPCI_A0_BF,
		GLPCI_C0_DF, GLPCI_E0_FF, GLPCI_RC0, GLPCI_RC1, GLPCI_RC2,
		GLPCI_RC3, GLPCI_ExtMSR, GLPCI_SPARE, GL_END
	};

	int dma_msr[] = { MDD_DMA_MAP, MDD_DMA_SHAD1, MDD_DMA_SHAD2,
		MDD_DMA_SHAD3, MDD_DMA_SHAD4, MDD_DMA_SHAD5, MDD_DMA_SHAD6,
		MDD_DMA_SHAD7, MDD_DMA_SHAD8, MDD_DMA_SHAD9, GL_END
	};

	printk(BIOS_DEBUG, "---------- CPU ------------\n");

	for (i = 0; cpu_msr_defs[i] != GL_END; i++) {
		msr = rdmsr(cpu_msr_defs[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n",
			     cpu_msr_defs[i], msr.hi, msr.lo);
	}

	printk(BIOS_DEBUG, "---------- GLIU 0 ------------\n");

	for (i = 0; gliu0_msr_defs[i] != GL_END; i++) {
		msr = rdmsr(gliu0_msr_defs[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n",
			     gliu0_msr_defs[i], msr.hi, msr.lo);
	}

	printk(BIOS_DEBUG, "---------- GLIU 1 ------------\n");

	for (i = 0; gliu1_msr_defs[i] != GL_END; i++) {
		msr = rdmsr(gliu1_msr_defs[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n",
			     gliu1_msr_defs[i], msr.hi, msr.lo);
	}

	printk(BIOS_DEBUG, "---------- RCONF ------------\n");

	for (i = 0; rconf_msr[i] != GL_END; i++) {
		msr = rdmsr(rconf_msr[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", rconf_msr[i],
			     msr.hi, msr.lo);
	}

	printk(BIOS_DEBUG, "---------- VARIA ------------\n");
	msr = rdmsr(ATA_SB_IDE_CFG);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", ATA_SB_IDE_CFG, msr.hi,
		     msr.lo);

	msr = rdmsr(MDD_LEG_IO);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_LEG_IO, msr.hi,
		     msr.lo);

	msr = rdmsr(MDD_PIN_OPT);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_PIN_OPT, msr.hi,
		     msr.lo);

	printk(BIOS_DEBUG, "---------- PCI ------------\n");

	for (i = 0; pci_msr[i] != GL_END; i++) {
		msr = rdmsr(pci_msr[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", pci_msr[i],
			     msr.hi, msr.lo);
	}

	printk(BIOS_DEBUG, "---------- LPC/UART DMA ------------\n");

	for (i = 0; dma_msr[i] != GL_END; i++) {
		msr = rdmsr(dma_msr[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", dma_msr[i],
			     msr.hi, msr.lo);
	}

	printk(BIOS_DEBUG, "---------- DIVIL IRQ ------------\n");

	for (i = 0; irq_msr[i] != GL_END; i++) {
		msr = rdmsr(irq_msr[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", irq_msr[i],
			     msr.hi, msr.lo);
	}

	printk(BIOS_DEBUG, "---------- DIVIL LBAR -----------\n");

	for (i = 0; lbar_msr[i] != GL_END; i++) {
		msr = rdmsr(lbar_msr[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", lbar_msr[i],
			     msr.hi, msr.lo);
	}

	iol = inl(GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
	printk(BIOS_DEBUG, "IOR 0x%08X is now 0x%08lX\n",
		     GPIO_IO_BASE + GPIOL_INPUT_ENABLE, iol);
	iol = inl(GPIOL_EVENTS_ENABLE);
	printk(BIOS_DEBUG, "IOR 0x%08X is now 0x%08lX\n",
		     GPIO_IO_BASE + GPIOL_EVENTS_ENABLE, iol);
	iol = inl(GPIOL_INPUT_INVERT_ENABLE);
	printk(BIOS_DEBUG, "IOR 0x%08X is now 0x%08lX\n",
		     GPIO_IO_BASE + GPIOL_INPUT_INVERT_ENABLE, iol);
	iol = inl(GPIO_MAPPER_X);
	printk(BIOS_DEBUG, "IOR 0x%08X is now 0x%08lX\n", GPIO_IO_BASE + GPIO_MAPPER_X,
		     iol);
#endif				/* CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_ERR */
}

/* todo: add a resource record. We don't do this here because this may be called when
 * very little of the platform is actually working.
 */
int sizeram(void)
{
	msr_t msr;
	int sizem = 0;
	unsigned short dimm;

	/* Get the RAM size from the memory controller as calculated and set by auto_size_dimm() */
	msr = rdmsr(MC_CF07_DATA);
	printk(BIOS_DEBUG, "sizeram: _MSR MC_CF07_DATA: %08x:%08x\n", msr.hi, msr.lo);

	/* dimm 0 */
	dimm = msr.hi;
	/* installed? */
	if ((dimm & 7) != 7)
		sizem = (1 << ((dimm >> 12)-1)) * 8;

	/* dimm 1 */
	dimm = msr.hi >> 16;
	/* installed? */
	if ((dimm & 7) != 7)
		sizem += (1 << ((dimm >> 12)-1)) * 8;

	printk(BIOS_DEBUG, "sizeram: sizem 0x%x\n", sizem);
	return sizem;
}

static void enable_shadow(device_t dev)
{

}

static void northbridge_init(device_t dev)
{
	printk(BIOS_SPEW, ">> Entering northbridge: %s()\n", __func__);

	enable_shadow(dev);
}

static void northbridge_set_resources(struct device *dev)
{
	uint8_t line;

	struct bus *bus;

	for(bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children) {
			printk(BIOS_DEBUG, "my_dev_set_resources: assign_resources %d\n",
			     bus->secondary);
			assign_resources(bus);
		}
	}

	/* set a default latency timer */
	pci_write_config8(dev, PCI_LATENCY_TIMER, 0x40);

	/* set a default secondary latency timer */
	if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE) {
		pci_write_config8(dev, PCI_SEC_LATENCY_TIMER, 0x40);
	}

	/* zero the irq settings */
	line = pci_read_config8(dev, PCI_INTERRUPT_PIN);
	if (line) {
		pci_write_config8(dev, PCI_INTERRUPT_LINE, 0);
	}
	/* set the cache line size, so far 64 bytes is good for everyone */
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, 64 >> 2);
}

static struct device_operations northbridge_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = northbridge_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = northbridge_init,
	.enable = 0,
	.ops_pci = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_NS,
	.device = PCI_DEVICE_ID_NS_GX2,
};

#include <cbmem.h>

static void pci_domain_set_resources(device_t dev)
{
	int idx;
	u32 tomk;
	device_t mc_dev;

	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __func__);

	mc_dev = dev->link_list->children;
	if (mc_dev) {
		tomk = get_systop() / 1024;
		/* Report the memory regions
		   All memory up to systop except 0xa0000-0xbffff */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 768, tomk - 768);	/* Systop - 0xc0000 -> KB */

		set_top_of_ram(tomk * 1024);
	}

	assign_resources(dev->link_list);
}

static void pci_domain_enable(device_t dev)
{
	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __func__);

	/* do this here for now -- this chip really breaks our device model */
	northbridge_init_early();
	cpubug();
	chipsetinit();
	print_conf();
	do_vsmbios();
	graphics_init();
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.enable_resources = NULL,
	.scan_bus = pci_domain_scan_bus,
	.enable = pci_domain_enable,
	.ops_pci_bus = pci_bus_default_ops,
};

static void cpu_bus_init(device_t dev)
{
	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __func__);

	initialize_cpus(dev->link_list);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources = cpu_bus_noop,
	.set_resources = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init = cpu_bus_init,
	.scan_bus = 0,
};

static void enable_dev(struct device *dev)
{
	printk(BIOS_SPEW, ">> Entering northbridge.c: %s with path %d\n",
		    __func__, dev->path.type);

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

struct chip_operations northbridge_amd_gx2_ops = {
	CHIP_NAME("AMD GX (previously GX2) Northbridge")
	.enable_dev = enable_dev,
};
