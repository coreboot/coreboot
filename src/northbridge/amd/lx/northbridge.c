/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/amd/lxdef.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/vr.h>
#include <cpu/cpu.h>
#include "northbridge.h"
#include "../../../southbridge/amd/cs5536/cs5536.h"


/* here is programming for the various MSRs.*/
#define IM_QWAIT 0x100000

#define DMCF_WRITE_SERIALIZE_REQUEST (2<<12) /* 2 outstanding */	/* in high */
#define DMCF_SERIAL_LOAD_MISSES  (2)	/* enabled */

/* these are the 8-bit attributes for controlling RCONF registers */
#define CACHE_DISABLE (1<<0)
#define WRITE_ALLOCATE (1<<1)
#define WRITE_PROTECT (1<<2)
#define WRITE_THROUGH (1<<3)
#define WRITE_COMBINE (1<<4)
#define WRITE_SERIALIZE (1<<5)

/* RAM has none of this stuff */
#define RAM_PROPERTIES (0)
#define DEVICE_PROPERTIES (WRITE_SERIALIZE|CACHE_DISABLE)
#define ROM_PROPERTIES (WRITE_SERIALIZE|WRITE_PROTECT|CACHE_DISABLE)
#define MSR_WS_CD_DEFAULT (0x21212121)

/* 1810-1817 give you 8 registers with which to program protection regions */
/* the are region configuration range registers, or RRCF */
/* in msr terms, the are a straight base, top address assign, since they are 4k aligned. */
/* so no left-shift needed for top or base */
#define RRCF_LOW(base,properties) (base|(1<<8)|properties)
#define RRCF_LOW_CD(base)	RRCF_LOW(base, CACHE_DISABLE)

/* build initializer for P2D MSR */
#define P2D_BM(msr, pdid1, bizarro, pbase, pmask) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(pbase>>24), .lo=(pbase<<8)|pmask}}
#define P2D_BMO(msr, pdid1, bizarro, poffset, pbase, pmask) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(poffset<<8)|(pbase>>24), .lo=(pbase<<8)|pmask}}
#define P2D_R(msr, pdid1, bizarro, pmax, pmin) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(pmax>>12), .lo=(pmax<<20)|pmin}}
#define P2D_RO(msr, pdid1, bizarro, poffset, pmax, pmin) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(poffset<<8)|(pmax>>12), .lo=(pmax<<20)|pmin}}
#define P2D_SC(msr, pdid1, bizarro, wen, ren,pscbase) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(wen), .lo=(ren<<16)|(pscbase>>18)}}
#define IOD_BM(msr, pdid1, bizarro, ibase, imask) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(ibase>>12), .lo=(ibase<<20)|imask}}
#define IOD_SC(msr, pdid1, bizarro, en, wen, ren, ibase) {msr, {.hi=(pdid1<<29)|(bizarro<<28), .lo=(en<<24)|(wen<<21)|(ren<<20)|(ibase<<3)}}

void print_conf(void);
void graphics_init(void);
void do_vsmbios(void);

struct msr_defaults {
	int msr_no;
	msr_t msr;
} msr_defaults[] = {
	{
		0x1700, {
	.hi = 0,.lo = IM_QWAIT}}, {
		0x1800, {
	.hi = DMCF_WRITE_SERIALIZE_REQUEST,.lo =
			    DMCF_SERIAL_LOAD_MISSES}},
	    /* 1808 will be done down below, so we have to do 180a->1817 (well, 1813 really) */
	    /* for 180a, for now, we assume VSM will configure it */
	    /* 180b is left at reset value,a0000-bffff is non-cacheable */
	    /* 180c, c0000-dffff is set to write serialize and non-cachable */
	    /* oops, 180c will be set by CPU bug handling in cpubug.c */
	    //{0x180c, {.hi = MSR_WS_CD_DEFAULT, .lo = MSR_WS_CD_DEFAULT}},
	    /* 180d is left at default, e0000-fffff is non-cached */
	    /* we will assume 180e, the ssm region configuration, is left at default or set by VSM */
	    /* we will not set 0x180f, the DMM,yet */
	    //{0x1810, {.hi = 0xee7ff000, .lo = RRCF_LOW(0xee000000, WRITE_COMBINE|CACHE_DISABLE)}},
	    //{0x1811, {.hi = 0xefffb000, .lo = RRCF_LOW_CD(0xefff8000)}},
	    //{0x1812, {.hi = 0xefff7000, .lo = RRCF_LOW_CD(0xefff4000)}},
	    //{0x1813, {.hi = 0xefff3000, .lo = RRCF_LOW_CD(0xefff0000)}},
	    /* now for GLPCI routing */
	    /* GLIU0 */
	    P2D_BM(MSR_GLIU0_BASE1, 0x1, 0x0, 0x0, 0xfff80),
	    P2D_BM(MSR_GLIU0_BASE2, 0x1, 0x0, 0x80000, 0xfffe0),
	    P2D_SC(MSR_GLIU0_SHADOW, 0x1, 0x0, 0x0, 0xff03, 0xC0000),
	    /* GLIU1 */
	    P2D_BM(MSR_GLIU1_BASE1, 0x1, 0x0, 0x0, 0xfff80),
	    P2D_BM(MSR_GLIU1_BASE2, 0x1, 0x0, 0x80000, 0xfffe0),
	    P2D_SC(MSR_GLIU1_SHADOW, 0x1, 0x0, 0x0, 0xff03, 0xC0000), {
	0}
};

/* Print the platform configuration - do before PCI init or it will not
 * work right.
 */
void print_conf(void)
{
#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_ERR
	int i;
	unsigned long iol;
	msr_t msr;

	int cpu_msr_defs[] = { CPU_BC_L2_CONF, CPU_IM_CONFIG, CPU_DM_CONFIG0,
		CPU_RCONF_DEFAULT, CPU_RCONF_BYPASS, CPU_RCONF_A0_BF,
		CPU_RCONF_C0_DF, CPU_RCONF_E0_FF, CPU_RCONF_SMM, CPU_RCONF_DMM,
		GLCP_DELAY_CONTROLS, GL_END
	};

	int gliu0_msr_defs[] = { MSR_GLIU0_BASE1, MSR_GLIU0_BASE2,
		MSR_GLIU0_BASE4, MSR_GLIU0_BASE5, MSR_GLIU0_BASE6,
		GLIU0_P2D_BMO_0, GLIU0_P2D_BMO_1, MSR_GLIU0_SYSMEM,
		GLIU0_P2D_RO_0, GLIU0_P2D_RO_1, GLIU0_P2D_RO_2,
		MSR_GLIU0_SHADOW, GLIU0_IOD_BM_0, GLIU0_IOD_BM_1,
		GLIU0_IOD_BM_2, GLIU0_IOD_SC_0, GLIU0_IOD_SC_1, GLIU0_IOD_SC_2,
		GLIU0_IOD_SC_3, GLIU0_IOD_SC_4, GLIU0_IOD_SC_5,
		GLIU0_GLD_MSR_COH, GL_END
	};

	int gliu1_msr_defs[] = { MSR_GLIU1_BASE1, MSR_GLIU1_BASE2,
		MSR_GLIU1_BASE3, MSR_GLIU1_BASE4, MSR_GLIU1_BASE5,
		MSR_GLIU1_BASE6, MSR_GLIU1_BASE7, MSR_GLIU1_BASE8,
		MSR_GLIU1_BASE9, MSR_GLIU1_BASE10, GLIU1_P2D_R_0,
		GLIU1_P2D_R_1, GLIU1_P2D_R_2, GLIU1_P2D_R_3, MSR_GLIU1_SHADOW,
		GLIU1_IOD_BM_0, GLIU1_IOD_BM_1, GLIU1_IOD_BM_2, GLIU1_IOD_SC_0,
		GLIU1_IOD_SC_1, GLIU1_IOD_SC_2, GLIU1_IOD_SC_3,
		GLIU1_GLD_MSR_COH, GL_END
	};

	int rconf_msr[] = { CPU_RCONF0, CPU_RCONF1, CPU_RCONF2, CPU_RCONF3,
		CPU_RCONF4, CPU_RCONF5, CPU_RCONF6, CPU_RCONF7, GL_END
	};

	int cs5536_msr[] = { MDD_LBAR_GPIO, MDD_LBAR_FLSH0, MDD_LBAR_FLSH1,
		MDD_LEG_IO, MDD_PIN_OPT, MDD_IRQM_ZLOW, MDD_IRQM_ZHIGH,
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
	msr = rdmsr(0x51300010);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", 0x51300010, msr.hi,
		     msr.lo);

	msr = rdmsr(0x51400015);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", 0x51400015, msr.hi,
		     msr.lo);

	printk(BIOS_DEBUG, "---------- DIVIL IRQ ------------\n");
	msr = rdmsr(MDD_IRQM_YLOW);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_YLOW, msr.hi,
		     msr.lo);
	msr = rdmsr(MDD_IRQM_YHIGH);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_YHIGH,
		     msr.hi, msr.lo);
	msr = rdmsr(MDD_IRQM_ZLOW);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_ZLOW, msr.hi,
		     msr.lo);
	msr = rdmsr(MDD_IRQM_ZHIGH);
	printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_ZHIGH,
		     msr.hi, msr.lo);

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

	printk(BIOS_DEBUG, "---------- CS5536 ------------\n");

	for (i = 0; cs5536_msr[i] != GL_END; i++) {
		msr = rdmsr(cs5536_msr[i]);
		printk(BIOS_DEBUG, "MSR 0x%08X is now 0x%08X:0x%08X\n", cs5536_msr[i],
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
#endif				//CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_ERR
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
	if ((dimm & 7) != 7) {
		sizem = 4 << ((dimm >> 12) & 0x0F); /* 1:8MB, 2:16MB, 3:32MB, 4:64MB, ... 7:512MB, 8:1GB */
	}

	/* dimm 1 */
	dimm = msr.hi >> 16;
	/* installed? */
	if ((dimm & 7) != 7) {
		sizem += 4 << ((dimm >> 12) & 0x0F); /* 1:8MB, 2:16MB, 3:32MB, 4:64MB, ... 7:512MB, 8:1GB */
	}

	printk(BIOS_DEBUG, "sizeram: sizem 0x%xMB\n", sizem);
	return sizem;
}

static void enable_shadow(struct device *dev)
{
}

static void northbridge_init(struct device *dev)
{

	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __func__);

	enable_shadow(dev);

}

static void northbridge_set_resources(struct device *dev)
{
	uint8_t line;

	struct bus *bus;
	for (bus = dev->link_list; bus; bus = bus->next) {
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
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_LXBRIDGE,
};

#include <cbmem.h>

static void pci_domain_set_resources(struct device *dev)
{
	int idx;
	u32 tomk;
	struct device *mc_dev;

	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __func__);

	mc_dev = dev->link_list->children;
	if (mc_dev) {
		tomk = restore_top_of_low_cacheable() / 1024;
		/* Report the memory regions
		   All memory up to systop except 0xa0000-0xbffff */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 768, tomk - 768);	// Systop - 0xc0000 -> KB

		set_late_cbmem_top(tomk * 1024);
	}

	assign_resources(dev->link_list);
}

static void pci_domain_enable(struct device *dev)
{
	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __func__);

	// do this here for now -- this chip really breaks our device model
	northbridge_init_early();
	cpubug();
	chipsetinit();

	do_vsmbios();		// do the magic stuff here, so prepare your tambourine;)

	graphics_init();
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.enable_resources = NULL,
	.scan_bus = pci_domain_scan_bus,
	.enable = pci_domain_enable,
};

static void cpu_bus_init(struct device *dev)
{
	printk(BIOS_SPEW, ">> Entering northbridge.c: %s\n", __func__);

	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
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

struct chip_operations northbridge_amd_lx_ops = {
	CHIP_NAME("AMD LX Northbridge")
	    .enable_dev = enable_dev,
};
