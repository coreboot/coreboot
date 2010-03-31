/*
 *
 * cs5535_early_setup.c:	Early chipset initialization for CS5535 companion device
 *
 *
 * This file implements the initialization sequence documented in section 4.2 of
 * AMD Geode GX Processor CS5535 Companion Device GoedeROM Porting Guide.
 *
 */

#define CS5535_GLINK_PORT_NUM	0x02	/* the geode link port number to the CS5535 */       
#define CS5535_DEV_NUM 		0x0F	/* default PCI device number for CS5535 */

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
	if (CS5535_GLINK_PORT_NUM <= 4) {
		msr.lo = CS5535_DEV_NUM << ((CS5535_GLINK_PORT_NUM - 1) * 8);
	} else {
		msr.hi = CS5535_DEV_NUM << ((CS5535_GLINK_PORT_NUM - 5) * 8);
	}
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

static int cs5535_setup_iobase(void)
{
	msr_t msr;

	/* setup LBAR for SMBus controller */
	__builtin_wrmsr(0x5140000b, 0x00006000, 0x0000f001);
	/* setup LBAR for GPIO */
	__builtin_wrmsr(0x5140000c, 0x00006100, 0x0000f001);
	/* setup LBAR for MFGPT */
	__builtin_wrmsr(0x5140000d, 0x00006200, 0x0000f001);
	/* setup LBAR for ACPI */
	__builtin_wrmsr(0x5140000e, 0x00009c00, 0x0000f001);
	/* setup LBAR for PM Support */
	__builtin_wrmsr(0x5140000f, 0x00009d00, 0x0000f001);
}

static void cs5535_setup_power_bottun(void)
{
	/* not implemented yet */
#if 0
	pwrBtn_setup:
	;
	;	Power Button Setup
	;
	;mov	eax, 0C0020000h				; 4 seconds + lock
	mov	eax, 040020000h				; 4 seconds no lock
	mov	dx, PMLogic_BASE + 40h
	out	dx, eax

	; setup GPIO24, it is the external signal for 5535 vsb_work_aux
	; which controls all voltage rails except Vstandby & Vmem.
	; We need to enable, OUT_AUX1 and OUTPUT_ENABLE in this order.
	; If GPIO24 is not enabled then soft-off will not work.
	mov	dx, GPIOH_OUT_AUX1_SELECT
	mov	eax, GPIOH_24_SET
	out	dx, eax
	mov	dx, GPIOH_OUTPUT_ENABLE
	out	dx, eax

#endif
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

static void cs5535_disable_internal_uart(void)
{
	/* not implemented yet */
#if 0
	; The UARTs default to enabled.
	; Disable and reset them and configure them later. (SIO init)
	mov	ecx, MDD_UART1_CONF
	RDMSR
	mov	eax, 1h					; reset
	WRMSR
	mov	eax, 0h					; disabled
	WRMSR

	mov	ecx, MDD_UART2_CONF
	RDMSR
	mov	eax, 1h					; reset
	WRMSR
	mov	eax, 0h					; disabled
	WRMSR

#endif
}

static void cs5535_setup_cis_mode(void)
{
	msr_t msr;

	/* setup CPU interface serial to mode C on both sides */
	msr = __builtin_rdmsr(0x51000010);
	msr.lo &= ~0x18;
	msr.lo |= 0x10;
	__builtin_wrmsr(0x51000010, msr.lo, msr.hi);
	//Only do this if we are building for 5535
	__builtin_wrmsr(0x54002010, 0x00000002, 0x00000000);
}

static void dummy(void)
{
}

static int cs5535_early_setup(void)
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
