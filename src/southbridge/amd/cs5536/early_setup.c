/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * cs5536_early_setup.c:	Early chipset initialization for CS5536 companion device
 *	This file implements the initialization sequence documented in section 4.2 of
 *	AMD Geode GX Processor CS5536 Companion Device GeodeROM Porting Guide.
 */

/**
 * @brief Setup PCI IDSEL for CS5536
 */
static void cs5536_setup_extmsr(void)
{
	msr_t msr;

	/* forward MSR access to CS5536_GLINK_PORT_NUM to CS5536_DEV_NUM */
	msr.hi = msr.lo = 0x00000000;
#if CS5536_GLINK_PORT_NUM <= 4
	msr.lo = CS5536_DEV_NUM << (unsigned char)((CS5536_GLINK_PORT_NUM - 1) * 8);
#else
	msr.hi = CS5536_DEV_NUM << (unsigned char)((CS5536_GLINK_PORT_NUM - 5) * 8);
#endif
	wrmsr(GLPCI_ExtMSR, msr);
}

static void cs5536_setup_idsel(void)
{
	/* write IDSEL to the write once register at address 0x0000 */
	outl(0x1 << (CS5536_DEV_NUM + 10), 0);
}

static void cs5536_usb_swapsif(void)
{
	msr_t msr;

	msr = rdmsr(USB1_SB_GLD_MSR_CAP + 0x5);
	//USB Serial short detect bit.
	if (msr.hi & 0x10) {
		/* We need to preserve bits 32,33,35 and not clear any BIST
		 * error, but clear the SERSHRT error bit */

		msr.hi &= 0xFFFFFFFB;
		wrmsr(USB1_SB_GLD_MSR_CAP + 0x5, msr);
	}
}

static void cs5536_setup_iobase(void)
{
	msr_t msr;
	/* setup LBAR for SMBus controller */
	msr.hi = 0x0000f001;
	msr.lo = SMBUS_IO_BASE;
	wrmsr(MDD_LBAR_SMB, msr);

	/* setup LBAR for GPIO */
	msr.hi = 0x0000f001;
	msr.lo = GPIO_IO_BASE;
	wrmsr(MDD_LBAR_GPIO, msr);

	/* setup LBAR for MFGPT */
	msr.hi = 0x0000f001;
	msr.lo = MFGPT_IO_BASE;
	wrmsr(MDD_LBAR_MFGPT, msr);

	/* setup LBAR for ACPI */
	msr.hi = 0x0000f001;
	msr.lo = ACPI_IO_BASE;
	wrmsr(MDD_LBAR_ACPI, msr);

	/* setup LBAR for PM Support */
	msr.hi = 0x0000f001;
	msr.lo = PMS_IO_BASE;
	wrmsr(MDD_LBAR_PMS, msr);
}

static void cs5536_setup_power_button(void)
{
#if CONFIG_ENABLE_POWER_BUTTON
	outl(0x40020000, PMS_IO_BASE + 0x40);
#endif

	/* setup WORK_AUX/GPIO24, it is the external signal for 5536
	 * vsb_work_aux controls all voltage rails except Vstandby & Vmem.
	 * We need to enable, OUT_AUX1 and OUTPUT_ENABLE in this order.
	 * If WORK_AUX/GPIO24 is not enabled then soft-off will not work.
	 */
	outl(GPIOH_24_SET, GPIO_IO_BASE + GPIOH_OUT_AUX1_SELECT);
	outl(GPIOH_24_SET, GPIO_IO_BASE + GPIOH_OUTPUT_ENABLE);

}

static void cs5536_setup_gpio(void)
{
	uint32_t val;

	/* setup GPIO pins 14/15 for SDA/SCL */
	val = GPIOL_15_SET | GPIOL_14_SET;
	/* Output Enable */
	outl(val, GPIO_IO_BASE + GPIOL_OUT_AUX1_SELECT);
	/* Output AUX1 */
	outl(val, GPIO_IO_BASE + GPIOL_OUTPUT_ENABLE);
	/* Input Enable */
	outl(val, GPIO_IO_BASE + GPIOL_IN_AUX1_SELECT);
	/* Input AUX1 */
	outl(val, GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
}

void cs5536_disable_internal_uart(void)
{
	msr_t msr;
	/* The UARTs default to enabled.
	 * Disable and reset them and configure them later. (SIO init)
	 */
	msr = rdmsr(MDD_UART1_CONF);
	msr.lo = 1;		// reset
	wrmsr(MDD_UART1_CONF, msr);
	msr.lo = 0;		// disabled
	wrmsr(MDD_UART1_CONF, msr);

	msr = rdmsr(MDD_UART2_CONF);
	msr.lo = 1;		// reset
	wrmsr(MDD_UART2_CONF, msr);
	msr.lo = 0;		// disabled
	wrmsr(MDD_UART2_CONF, msr);
}

static void cs5536_setup_cis_mode(void)
{
	msr_t msr;

	/* Setup CPU serial SouthBridge interface to mode C. */
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
static void cs5536_setup_onchipuart1(void)
{
	msr_t msr;

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

static void cs5536_setup_onchipuart2(void)
{
	msr_t msr;

	/* GPIO4 - UART2_TX */
	/* Set: Output Enable  (0x4) */
	outl(GPIOL_4_SET, GPIO_IO_BASE + GPIOL_OUTPUT_ENABLE);
	/* Set: OUTAUX1 Select (0x10) */
	outl(GPIOL_4_SET, GPIO_IO_BASE + GPIOL_OUT_AUX1_SELECT);
	/* GPIO4 - UART2_RX */
	/* Set: Input Enable   (0x20) */
	outl(GPIOL_3_SET, GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
	/* Set: INAUX1 Select  (0x34) */
	outl(GPIOL_3_SET, GPIO_IO_BASE + GPIOL_IN_AUX1_SELECT);

	/* Set: GPIO 3 + 3 Pull Up  (0x18) */
	outl(GPIOL_3_SET | GPIOL_4_SET,
	     GPIO_IO_BASE + GPIOL_PULLUP_ENABLE);

	/* set address to 2F8 */
	msr = rdmsr(MDD_LEG_IO);
	msr.lo |= 0x5 << 20;
	wrmsr(MDD_LEG_IO, msr);

	/* Bit 1 = DEVEN (device enable)
	 * Bit 4 = EN_BANKS (allow access to the upper banks
	 */
	msr.lo = (1 << 4) | (1 << 1);
	msr.hi = 0;

	/* enable COM2 */
	wrmsr(MDD_UART2_CONF, msr);
}

void cs5536_setup_onchipuart(int uart)
{
	switch (uart) {
	case 1:
		cs5536_setup_onchipuart1();
		break;
	case 2:
		cs5536_setup_onchipuart2();
		break;
	}
}


/* note: you can't do prints in here in most cases,
 * and we don't want to hang on serial, so they are
 * commented out
 */
static void cs5536_early_setup(void)
{
	msr_t msr;

	cs5536_setup_extmsr();
	cs5536_setup_cis_mode();

	msr = rdmsr(GLCP_SYS_RSTPLL);
	if (msr.lo & (0x3f << 26)) {
		/* PLL is already set and we are reboot from PLL reset */
		//print_debug("reboot from BIOS reset\n");
		return;
	}
	//print_debug("Setup idsel\n");
	cs5536_setup_idsel();
	//print_debug("Setup iobase\n");
	cs5536_usb_swapsif();
	cs5536_setup_iobase();
	//print_debug("Setup gpio\n");
	cs5536_setup_gpio();
	//print_debug("Setup smbus\n");
	cs5536_enable_smbus();
	//print_debug("Setup power button\n");
	cs5536_setup_power_button();
}
