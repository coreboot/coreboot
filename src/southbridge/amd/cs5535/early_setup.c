/*
 *
 * cs5535_early_setup.c:	Early chipset initialization for CS5535 companion device
 *
 *
 * This file implements the initialization sequence documented in section 4.2 of
 * AMD Geode GX Processor CS5535 Companion Device GoedeROM Porting Guide.
 *
 */

/**
 * @brief Setup PCI IDSEL for CS5535
 *
 *
 */

static void cs5535_setup_extmsr(void)
{
	msr_t msr;

	/* forward MSR access to CS5535_GLINK_PORT_NUM to CS5535_DEV_NUM */
	msr.hi = msr.lo = 0x00000000;
#if CS5535_GLINK_PORT_NUM <= 4
	msr.lo = CS5535_DEV_NUM << ((CS5535_GLINK_PORT_NUM - 1) * 8);
#else
	msr.hi = CS5535_DEV_NUM << ((CS5535_GLINK_PORT_NUM - 5) * 8);
#endif
	wrmsr(0x5000201e, msr);
}

static void cs5535_setup_idsel(void)
{
	/* write IDSEL to the write once register at address 0x0000 */
	outl(0x1 << (CS5535_DEV_NUM + 10), 0);
}

static void cs5535_usb_swapsif(void)
{
	msr_t msr;

	msr = rdmsr(0x51600005);
	//USB Serial short detect bit.
	if (msr.hi & 0x10) {
		/* We need to preserve bits 32,33,35 and not clear any BIST error, but clear the
		 * SERSHRT error bit */
		msr.hi &= 0xFFFFFFFB;
		wrmsr(0x51600005, msr);
	}
}

static void cs5535_setup_iobase(void)
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

static void cs5535_setup_gpio(void)
{
	uint32_t val;

	/* setup GPIO pins 14/15 for SDA/SCL */
	val = (1<<14 | 1<<15);
	/* Output Enable */
	outl(0x3fffc000, 0x6100 + 0x04);
	//outl(val, 0x6100 + 0x04);
	/* Output AUX1 */
	outl(0x3fffc000, 0x6100 + 0x10);
	//outl(val, 0x6100 + 0x10);
	/* Input Enable */
	//outl(0x0f5af0a5, 0x6100 + 0x20);
	outl(0x3fffc000, 0x6100 + 0x20);
	//outl(val, 0x6100 + 0x20);
	/* Input AUX1 */
	//outl(0x3ffbc004, 0x6100 + 0x34);
	outl(0x3fffc000, 0x6100 + 0x34);
	//outl(val, 0x6100 + 0x34);
}

void cs5535_disable_internal_uart(void)
{
}

static void cs5535_setup_cis_mode(void)
{
	msr_t msr;

	/* Setup CPU serial SouthBridge interface to mode C. */
	msr = rdmsr(GLPCI_SB_CTRL);
	msr.lo &= ~0x18;
	msr.lo |= 0x10;
	wrmsr(GLPCI_SB_CTRL, msr);
}

static void dummy(void)
{
}

static void cs5535_early_setup(void)
{
	msr_t msr;

	cs5535_setup_extmsr();

	msr = rdmsr(GLCP_SYS_RSTPLL);
	if (msr.lo & (0x3f << 26)) {
		/* PLL is already set and we are reboot from PLL reset */
		print_debug("reboot from BIOS reset\n");
		return;
	}
	print_debug("Setup idsel\n");
	cs5535_setup_idsel();
	print_debug("Setup iobase\n");
	cs5535_usb_swapsif();
	cs5535_setup_iobase();
	print_debug("Setup gpio\n");
	cs5535_setup_gpio();
	print_debug("Setup cis_mode\n");
	cs5535_setup_cis_mode();
	print_debug("Setup smbus\n");
	cs5535_enable_smbus();
	dummy();
}
