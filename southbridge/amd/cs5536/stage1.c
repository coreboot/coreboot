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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <msr.h>
#include <lib.h>
#include <amd_geodelx.h>
#include "cs5536.h"

/*
 * Early chipset initialization for the AMD CS5536 Companion Device.
 *
 * This code is needed for setting up RAM, since we need SMBus working as
 * well as a serial port.
 *
 * This file implements the initialization sequence documented in section 4.2
 * of AMD Geode GX Processor CS5536 Companion Device GoedeROM Porting Guide.
 */

/**
 * Set up GLINK routing for this part.
 *
 * The routing is controlled by an MSR. This appears to be the same on
 * all boards.
 */
static void cs5536_setup_extmsr(void)
{
	struct msr msr;

	/* Forward MSR access to CS5536_GLINK_PORT_NUM to CS5536_DEV_NUM. */
	msr.hi = msr.lo = 0;

	/* TODO: unsigned char -> u8? */
#if CS5536_GLINK_PORT_NUM <= 4
	msr.lo = CS5536_DEV_NUM <<
	    (unsigned char)((CS5536_GLINK_PORT_NUM - 1) * 8);
#else
	msr.hi = CS5536_DEV_NUM <<
	    (unsigned char)((CS5536_GLINK_PORT_NUM - 5) * 8);
#endif

	wrmsr(GLPCI_ExtMSR, msr);
}

/**
 * Setup PCI IDSEL for CS5536. There is a Magic Register that must be
 * written so that the chip appears at the expected place in the PCI tree.
 */
static void cs5536_setup_idsel(void)
{
	/* Write IDSEL to the write once register at address 0x0000. */
	outl(0x1 << (CS5536_DEV_NUM + 10), 0);
}

/**
 * Magic Bits for undocumented register. You don't need to see those papers.
 * These are not the bits you're looking for. You can go about your business.
 * Move along, move along.
 */
static void cs5536_usb_swapsif(void)
{
	struct msr msr;

	msr = rdmsr(USB1_SB_GLD_MSR_CAP + 0x5);

	/* USB Serial short detect bit. */
	if (msr.hi & 0x10) {
		/* We need to preserve bits 32,33,35 and not clear any BIST
		 * error, but clear the SERSHRT error bit.
		 */
		msr.hi &= 0xFFFFFFFB;
		wrmsr(USB1_SB_GLD_MSR_CAP + 0x5, msr);
	}
}

static const struct msrinit msr_table[] = {
	{MDD_LBAR_SMB,   {.hi = 0x0000f001, .lo = SMBUS_IO_BASE}},
	{MDD_LBAR_GPIO,  {.hi = 0x0000f001, .lo = GPIO_IO_BASE}},
	{MDD_LBAR_MFGPT, {.hi = 0x0000f001, .lo = MFGPT_IO_BASE}},
	{MDD_LBAR_ACPI,  {.hi = 0x0000f001, .lo = ACPI_IO_BASE}},
	{MDD_LBAR_PMS,   {.hi = 0x0000f001, .lo = PMS_IO_BASE}},
};

/**
 * Set up I/O bases for SMBus, GPIO, MFGPT, ACPI, and PM.
 *
 * These can be changed by Linux later. We set some initial value so that
 * the resources are there as needed. The values are hardcoded because,
 * this early in the process, fancy allocation can do more harm than good.
 */
static void cs5536_setup_iobase(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(msr_table); i++)
		wrmsr(msr_table[i].msrnum, msr_table[i].msr);
}

/**
 * Power button setup.
 *
 * Setup GPIO24, it is the external signal for CS5536 vsb_work_aux which
 * controls all voltage rails except Vstandby & Vmem. We need to enable
 * OUT_AUX1 and OUTPUT_ENABLE in this order.
 *
 * If GPIO24 is not enabled then soft-off will not work.
 */
static void cs5536_setup_power_button(void)
{
	/* TODO: Should be a #define? */
	outl(0x40020000, PMS_IO_BASE + 0x40);
	outl(GPIOH_24_SET, GPIO_IO_BASE + GPIOH_OUT_AUX1_SELECT);
	outl(GPIOH_24_SET, GPIO_IO_BASE + GPIOH_OUTPUT_ENABLE);
}

/**
 * Set the various GPIOs.
 *
 * An unknown question at this point is how general this is to all mainboards.
 * At the same time, many boards seem to follow this particular reference spec.
 */
static void cs5536_setup_smbus_gpio(void)
{
	u32 val;

	/* Setup GPIO pins 14/15 for SDA/SCL. */
	val = GPIOL_15_SET | GPIOL_14_SET;

	/* Output AUX1 + enable */
	outl(val, GPIO_IO_BASE + GPIOL_OUT_AUX1_SELECT);
	outl(val, GPIO_IO_BASE + GPIOL_OUTPUT_ENABLE);

	/* Input AUX1 + enable */
	outl(val, GPIO_IO_BASE + GPIOL_IN_AUX1_SELECT);
	outl(val, GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
}

/**
 * Disable the internal UART.
 *
 * Different boards have different UARTs for COM1.
 */
void cs5536_disable_internal_uart(void)
{
	struct msr msr;

	/* The UARTs default to enabled.
	 * Disable and reset them and configure them later (SIO init).
	 */
	msr = rdmsr(MDD_UART1_CONF);
	msr.lo = 1;			/* Reset */
	wrmsr(MDD_UART1_CONF, msr);
	msr.lo = 0;			/* Disable */
	wrmsr(MDD_UART1_CONF, msr);

	msr = rdmsr(MDD_UART2_CONF);
	msr.lo = 1;			/* Reset */
	wrmsr(MDD_UART2_CONF, msr);
	msr.lo = 0;			/* Disable */
	wrmsr(MDD_UART2_CONF, msr);
}

/**
 * Set up the CS5536 CIS interface to CPU interface to match modes.
 *
 * The CIS is related to the interrupt system. It is important to match
 * the southbridge and the CPU chips. At the same time, they always seem
 * to use mode B.
 */
static void cs5536_setup_cis_mode(void)
{
	struct msr msr;

	/* Setup CPU interface serial to mode B to match CPU. */
	msr = rdmsr(GLPCI_SB_CTRL);
	msr.lo &= ~0x18;
	msr.lo |= 0x10;
	wrmsr(GLPCI_SB_CTRL, msr);
}

/**
 * Enable the on-chip UART.
 *
 * See page 412 of the AMD Geode CS5536 Companion Device data book.
 */
void cs5536_setup_onchipuart(void)
{
	struct msr msr;

	/* Setup early for polling only mode.
	 * 1. Enable GPIO 8 to OUT_AUX1, 9 to IN_AUX1.
	 *        GPIO LBAR + 0x04, LBAR + 0x10, LBAR + 0x20, LBAR + 34
	 * 2. Enable UART I/O space in MDD.
	 *        MSR 0x51400014 bit 18:16
	 * 3. Enable UART controller.
	 *        MSR 0x5140003A bit 0, 1
	 */

	/* GPIO8 - UART1_TX */
	/* Set: Output Enable (0x4) */
	outl(GPIOL_8_SET, GPIO_IO_BASE + GPIOL_OUTPUT_ENABLE);
	/* Set: OUTAUX1 Select (0x10) */
	outl(GPIOL_8_SET, GPIO_IO_BASE + GPIOL_OUT_AUX1_SELECT);

	/* GPIO9 - UART1_RX */
	/* Set: Input Enable (0x20) */
	outl(GPIOL_9_SET, GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
	/* Set: INAUX1 Select (0x34) */
	outl(GPIOL_9_SET, GPIO_IO_BASE + GPIOL_IN_AUX1_SELECT);

	/* Set address to 0x3F8. */
	msr = rdmsr(MDD_LEG_IO);
	msr.lo |= 0x7 << 16;
	wrmsr(MDD_LEG_IO, msr);

	/* Bit 1 = DEVEN (device enable)
	 * Bit 4 = EN_BANKS (allow access to the upper banks)
	 */
	msr.lo = (1 << 4) | (1 << 1);
	msr.hi = 0;

	/* Enable COM1. */
	wrmsr(MDD_UART1_CONF, msr);
}

/**
 * Board setup.
 *
 * Known to work on the AMD Norwich and Advanced Digital Logic boards.
 *
 * The extmsr and cis_mode are common for sure. The RSTPLL check is mandatory.
 * IDSEL of course is required, so the chip appears in PCI config space,
 * and the swapsif covers a necessary chip fix.
 *
 * Finally, the iobase is needed for DRAM, the GPIOs are likely common to all
 * boards, and the power button seems to be the same on all. At the same time,
 * we may need to move GPIO and power button out as developments demand.
 *
 * Note we do NOT do any UART setup here -- this is done later by the
 * mainboard setup, since UART usage is not universal.
 *
 * See also a comment from Marc Jones:
 * http://www.linuxbios.org/pipermail/linuxbios/2007-June/021958.html
 */
void cs5536_stage1(void)
{
	struct msr msr;

	/* Note: you can't do prints in here in most cases, and we don't want
	 * to hang on serial, so they are commented out.
	 */
	cs5536_setup_extmsr();
	cs5536_setup_cis_mode();

	msr = rdmsr(GLCP_SYS_RSTPLL);
	if (msr.lo & (0x3f << 26)) {
		/* PLL is already set and we are reboot from PLL reset. */
		return;
	}

	cs5536_setup_idsel();
	cs5536_usb_swapsif();
	cs5536_setup_iobase();
	cs5536_setup_smbus_gpio();
	/* cs5536_enable_smbus(); -- Leave this out for now. */
	cs5536_setup_power_button();
}
