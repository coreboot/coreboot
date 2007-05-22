/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include <device/pci_def.h>
#include "../../../southbridge/amd/cs5536/cs5536.h"
#include "chip.h"

/* Print the platform configuration - do before PCI init or it will not
 * work right.
 */
void print_conf(void)
{
#if DEFAULT_CONSOLE_LOGLEVEL >= BIOS_ERR
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

	printk_debug("---------- CPU ------------\n");

	for (i = 0; cpu_msr_defs[i] != GL_END; i++) {
		msr = rdmsr(cpu_msr_defs[i]);
		printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n",
			     cpu_msr_defs[i], msr.hi, msr.lo);
	}

	printk_debug("---------- GLIU 0 ------------\n");

	for (i = 0; gliu0_msr_defs[i] != GL_END; i++) {
		msr = rdmsr(gliu0_msr_defs[i]);
		printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n",
			     gliu0_msr_defs[i], msr.hi, msr.lo);
	}

	printk_debug("---------- GLIU 1 ------------\n");

	for (i = 0; gliu1_msr_defs[i] != GL_END; i++) {
		msr = rdmsr(gliu1_msr_defs[i]);
		printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n",
			     gliu1_msr_defs[i], msr.hi, msr.lo);
	}

	printk_debug("---------- RCONF ------------\n");

	for (i = 0; rconf_msr[i] != GL_END; i++) {
		msr = rdmsr(rconf_msr[i]);
		printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", rconf_msr[i],
			     msr.hi, msr.lo);
	}

	printk_debug("---------- VARIA ------------\n");
	msr = rdmsr(0x51300010);
	printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", 0x51300010, msr.hi,
		     msr.lo);

	msr = rdmsr(0x51400015);
	printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", 0x51400015, msr.hi,
		     msr.lo);

	printk_debug("---------- DIVIL IRQ ------------\n");
	msr = rdmsr(MDD_IRQM_YLOW);
	printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_YLOW, msr.hi,
		     msr.lo);
	msr = rdmsr(MDD_IRQM_YHIGH);
	printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_YHIGH,
		     msr.hi, msr.lo);
	msr = rdmsr(MDD_IRQM_ZLOW);
	printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_ZLOW, msr.hi,
		     msr.lo);
	msr = rdmsr(MDD_IRQM_ZHIGH);
	printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", MDD_IRQM_ZHIGH,
		     msr.hi, msr.lo);

	printk_debug("---------- PCI ------------\n");

	for (i = 0; pci_msr[i] != GL_END; i++) {
		msr = rdmsr(pci_msr[i]);
		printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", pci_msr[i],
			     msr.hi, msr.lo);
	}

	printk_debug("---------- LPC/UART DMA ------------\n");

	for (i = 0; dma_msr[i] != GL_END; i++) {
		msr = rdmsr(dma_msr[i]);
		printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", dma_msr[i],
			     msr.hi, msr.lo);
	}

	printk_debug("---------- CS5536 ------------\n");

	for (i = 0; cs5536_msr[i] != GL_END; i++) {
		msr = rdmsr(cs5536_msr[i]);
		printk_debug("MSR 0x%08X is now 0x%08X:0x%08X\n", cs5536_msr[i],
			     msr.hi, msr.lo);
	}

	iol = inl(GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
	printk_debug("IOR 0x%08X is now 0x%08X\n",
		     GPIO_IO_BASE + GPIOL_INPUT_ENABLE, iol);
	iol = inl(GPIOL_EVENTS_ENABLE);
	printk_debug("IOR 0x%08X is now 0x%08X\n",
		     GPIO_IO_BASE + GPIOL_EVENTS_ENABLE, iol);
	iol = inl(GPIOL_INPUT_INVERT_ENABLE);
	printk_debug("IOR 0x%08X is now 0x%08X\n",
		     GPIO_IO_BASE + GPIOL_INPUT_INVERT_ENABLE, iol);
	iol = inl(GPIO_MAPPER_X);
	printk_debug("IOR 0x%08X is now 0x%08X\n", GPIO_IO_BASE + GPIO_MAPPER_X,
		     iol);
#endif				//DEFAULT_CONSOLE_LOGLEVEL >= BIOS_ERR
}

static void init(struct device *dev)
{
	printk_debug("Norwich ENTER %s\n", __FUNCTION__);
	printk_debug("Norwich EXIT %s\n", __FUNCTION__);
}

static void enable_dev(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_amd_norwich_ops = {
	CHIP_NAME("AMD Norwich Mainboard")
	    .enable_dev = enable_dev,

};
