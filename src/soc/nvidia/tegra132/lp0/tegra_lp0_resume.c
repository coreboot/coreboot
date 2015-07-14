/*
 * Copyright 2014 Google Inc.
 * Copyright 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>

/* Function unit addresses. */
enum {
	UP_TAG_BASE = 0X60000000,
	TIMER_BASE = 0X60005000,
	CLK_RST_BASE = 0X60006000,
	FLOW_CTLR_BASE = 0X60007000,
	TEGRA_EVP_BASE = 0x6000f000,
	APB_MISC_BASE = 0x70000000,
	PMC_CTLR_BASE = 0X7000e400,
	MC_CTLR_BASE = 0X70019000,
};



/* UP tag registers. */
static uint32_t *up_tag_ptr = (void *)(UP_TAG_BASE + 0x0);
enum {
	UP_TAG_AVP = 0xaaaaaaaa
};


/*  APB Misc JTAG Configuration Register */
static uint32_t *misc_pp_config_ctl_ptr = (void *)(APB_MISC_BASE + 0x24);
enum {
	PP_CONFIG_CTL_JTAG = 0x1 << 6
};


/* Timer registers. */
static uint32_t *timer_us_ptr = (void *)(TIMER_BASE + 0x10);



/* Clock and reset controller registers. */
static uint32_t *clk_rst_rst_devices_l_ptr = (void *)(CLK_RST_BASE + 0x4);
enum {
	SWR_TRIG_SYS_RST = 0x1 << 2
};

static uint32_t *clk_rst_cclk_burst_policy_ptr = (void *)(CLK_RST_BASE + 0x20);
enum {
	CCLK_PLLP_BURST_POLICY = 0x20004444
};

static uint32_t *clk_rst_super_cclk_div_ptr = (void *)(CLK_RST_BASE + 0x24);
enum {
	SUPER_CDIV_ENB = 0x1 << 31
};

static uint32_t *clk_rst_osc_ctrl_ptr = (void *)(CLK_RST_BASE + 0x50);
enum {
	OSC_XOE = 0x1 << 0,
	OSC_XOFS_SHIFT = 4,
	OSC_XOFS_MASK = 0x3f << OSC_XOFS_SHIFT,
	OSC_FREQ_SHIFT = 28,
	OSC_FREQ_MASK = 0xf << OSC_FREQ_SHIFT
};
enum {
	OSC_FREQ_13 = 0,
	OSC_FREQ_16P8 = 1,
	OSC_FREQ_19P2 = 4,
	OSC_FREQ_38P4 = 5,
	OSC_FREQ_12 = 8,
	OSC_FREQ_48 = 9,
	OSC_FREQ_26 = 12
};

static uint32_t *clk_rst_pllu_base_ptr = (void *)(CLK_RST_BASE + 0xc0);
enum {
	PLLU_DIVM_SHIFT = 0,
	PLLU_DIVN_SHIFT = 8,
	PLLU_OVERRIDE = 0x1 << 24,
	PLLU_ENABLE = 0x1 << 30,
	PLLU_BYPASS = 0x1 << 31
};

static uint32_t *clk_rst_pllu_misc_ptr = (void *)(CLK_RST_BASE + 0xcc);
enum {
	PLLU_LFCON_SHIFT = 4,
	PLLU_CPCON_SHIFT = 8,
	PLLU_LOCK_ENABLE = 22
};

static uint32_t *clk_rst_pllx_base_ptr = (void *)(CLK_RST_BASE + 0xe0);
enum {
	PLLX_ENABLE = 0x1 << 30
};

static uint32_t *clk_rst_rst_dev_u_clr_ptr = (void *)(CLK_RST_BASE + 0x314);
enum {
	SWR_CSITE_RST = 0x1 << 9
};

static uint32_t *clk_rst_clk_enb_l_set_ptr = (void *)(CLK_RST_BASE + 0x320);
enum {
	CLK_ENB_CPU = 0x1 << 0
};

static uint32_t *clk_rst_clk_out_enb_u_set_ptr =
	(void *)(CLK_RST_BASE + 0x330);
enum {
	CLK_ENB_CSITE = 0x1 << 9
};

static uint32_t *clk_rst_cpu_softrst_ctrl2_ptr =
	(void *)(CLK_RST_BASE + 0x388);
enum {
	CAR2PMC_CPU_ACK_WIDTH_SHIFT = 0,
	CAR2PMC_CPU_ACK_WIDTH_MASK = 0xfff << CAR2PMC_CPU_ACK_WIDTH_SHIFT
};

static uint32_t *clk_rst_clk_enb_v_set_ptr = (void *)(CLK_RST_BASE + 0x440);
enum {
	CLK_ENB_CPUG = 0x1 << 0,
	CLK_ENB_CPULP = 0x1 << 1,
};

static uint32_t *clk_rst_rst_cpulp_cmplx_set_ptr =
	(void *)(CLK_RST_BASE + 0x450);
enum {
	SET_CXRESET0 = 0x1 << 20,
	SET_CXRESET1 = 0x1 << 21
};
static uint32_t *clk_rst_rst_cpug_cmplx_clr_ptr =
	(void *)(CLK_RST_BASE + 0x454);
enum {
	CLR_CPURESET0 = 0x1 << 0,
	CLR_CPURESET1 = 0x1 << 1,
	CLR_CPURESET2 = 0x1 << 2,
	CLR_CPURESET3 = 0x1 << 3,
	CLR_DBGRESET0 = 0x1 << 12,
	CLR_DBGRESET1 = 0x1 << 13,
	CLR_DBGRESET2 = 0x1 << 14,
	CLR_DBGRESET3 = 0x1 << 15,
	CLR_CORERESET0 = 0x1 << 16,
	CLR_CORERESET1 = 0x1 << 17,
	CLR_CORERESET2 = 0x1 << 18,
	CLR_CORERESET3 = 0x1 << 19,
	CLR_CXRESET0 = 0x1 << 20,
	CLR_CXRESET1 = 0x1 << 21,
	CLR_CXRESET2 = 0x1 << 22,
	CLR_CXRESET3 = 0x1 << 23,
	CLR_L2RESET = 0x1 << 24,
	CLR_NONCPURESET = 0x1 << 29,
	CLR_PRESETDBG = 0x1 << 30
};


/* Reset vector. */

static uint32_t *evp_cpu_reset_ptr = (void *)(TEGRA_EVP_BASE + 0x100);



/* Flow controller registers. */
static uint32_t *flow_ctlr_halt_cop_events_ptr =
	(void *)(FLOW_CTLR_BASE + 0x4);
enum {
	EVENT_MSEC = 0x1 << 24,
	EVENT_JTAG = 0x1 << 28,
	FLOW_MODE_SHIFT = 29,
	FLOW_MODE_STOP = 2 << FLOW_MODE_SHIFT,
};

static uint32_t *flow_ctlr_cluster_control_ptr =
	(void *)(FLOW_CTLR_BASE + 0x2c);
enum {
	FLOW_CLUSTER_ACTIVE_LP = 0x1 << 0
};

static uint32_t *flow_ctlr_ram_repair_ptr =
	(void *)(FLOW_CTLR_BASE + 0x40);
static uint32_t *flow_ctlr_ram_repair_cluster1_ptr =
	(void *)(FLOW_CTLR_BASE + 0x58);
enum {
	RAM_REPAIR_REQ = 0x1 << 0,
	RAM_REPAIR_STS = 0x1 << 1,
};


/* Power management controller registers. */
enum {
	PARTID_CRAIL = 0,
	PARTID_CE0 = 14,
	PARTID_C0NC = 15,
};

static uint32_t *pmc_ctlr_clamp_status_ptr = (void *)(PMC_CTLR_BASE + 0x2c);

static uint32_t *pmc_ctlr_pwrgate_toggle_ptr = (void *)(PMC_CTLR_BASE + 0x30);
enum {
	PWRGATE_TOGGLE_START = 0x1 << 8
};

static uint32_t *pmc_ctlr_pwrgate_status_ptr = (void *)(PMC_CTLR_BASE + 0x38);

static uint32_t *pmc_ctlr_cpupwrgood_timer_ptr =
	(void *)(PMC_CTLR_BASE + 0xc8);

static uint32_t *pmc_odmdata_ptr = (void *)(PMC_CTLR_BASE + 0xa0);

static uint32_t *pmc_ctlr_scratch41_ptr = (void *)(PMC_CTLR_BASE + 0x140);
static uint32_t *pmc_ctlr_secure_scratch34_ptr = (void *)(PMC_CTLR_BASE + 0x368);
static uint32_t *pmc_ctlr_secure_scratch35_ptr = (void *)(PMC_CTLR_BASE + 0x36c);

static uint32_t *pmc_ctlr_osc_edpd_over_ptr = (void *)(PMC_CTLR_BASE + 0x1a4);
enum {
	PMC_XOFS_SHIFT = 1,
	PMC_XOFS_MASK = 0x3f << PMC_XOFS_SHIFT
};



/* Memory controller registers. */
static uint32_t *mc_video_protect_size_mb_ptr = (void *)(MC_CTLR_BASE + 0x64c);

static uint32_t *mc_video_protect_reg_ctrl_ptr =
	(void *)(MC_CTLR_BASE + 0x650);
enum {
	VIDEO_PROTECT_WRITE_ACCESS_DISABLE = 0x1 << 0,
	VIDEO_PROTECT_ALLOW_TZ_WRITE_ACCESS = 0x1 << 1
};


/* Utility functions. */

static inline void __attribute__((always_inline))
		   __attribute__((noreturn)) halt(void)
{
	for (;;);
}

static inline uint32_t read32(const void *addr)
{
	return *(volatile uint32_t *)addr;
}

static inline void write32(void *addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}

static inline void setbits32(uint32_t bits, void *addr)
{
	write32(addr, read32(addr) | bits);
}

static inline void clrbits32(uint32_t bits, void *addr)
{
	write32(addr, read32(addr) & ~bits);
}

static void __attribute__((noreturn)) reset(void)
{
	write32(clk_rst_rst_devices_l_ptr, SWR_TRIG_SYS_RST);
	halt();
}

static void udelay(unsigned usecs)
{
	uint32_t start = read32(timer_us_ptr);
	while (read32(timer_us_ptr) - start < usecs)
		;
}

/* UART related defines */
static uint32_t *uart_clk_out_enb_regs[4] = {
	(uint32_t *)0x60006010,
	(uint32_t *)0x60006010,
	(uint32_t *)0x60006014,
	(uint32_t *)0x60006018
};

static uint32_t *uart_rst_devices_regs[4] = {
	(uint32_t *)0x60006004,
	(uint32_t *)0x60006004,
	(uint32_t *)0x60006008,
	(uint32_t *)0x6000600c
};

static uint32_t uart_enable_mask[4] = {
	1 << 6,
	1 << 7,
	1 << 23,
	1 << 1
};

static uint32_t *uart_clk_source_regs[4] = {
	(uint32_t *)0x60006178,
	(uint32_t *)0x6000617c,
	(uint32_t *)0x600061a0,
	(uint32_t *)0x600061c0,
};

static uint32_t *uart_base_regs[4] = {
	(uint32_t *)0x70006000,
	(uint32_t *)0x70006040,
	(uint32_t *)0x70006200,
	(uint32_t *)0x70006300,
};
enum {
	UART_THR_DLAB = 0x0,
	UART_IER_DLAB = 0x1,
	UART_IIR_FCR = 0x2,
	UART_LCR = 0x3
};
enum {
	UART_RATE_115200 = (408000000/115200/16), /* based on 408000000 PLLP */
	FCR_TX_CLR = 0x4,	/* bit 2 of FCR : clear TX FIFO */
	FCR_RX_CLR = 0x2,	/* bit 1 of FCR : clear RX FIFO */
	FCR_EN_FIFO = 0x1,	/* bit 0 of FCR : enable TX & RX FIFO */
	LCR_DLAB = 0x80,	/* bit 7 of LCR : Divisor Latch Access Bit */
	LCR_WD_SIZE_8 = 0x3,	/* bit 1:0 of LCR : word length of 8 */
};

static void enable_uart(void)
{
	uint32_t *uart_clk_enb_reg;
	uint32_t *uart_rst_reg;
	uint32_t *uart_clk_source;
	uint32_t uart_port;
	uint32_t uart_mask;
	uint32_t *uart_base;

	/*
	 * Read odmdata (stored in pmc->odmdata) to determine debug uart port.
	 *
	 * Bits 15-17 of odmdata contains debug uart port.
	 *  0 : UARTA
	 *  1 : UARTB
	 *  2 : UARTC
	 *  3 : UARTD
	 */
	uart_port = (read32(pmc_odmdata_ptr) >> 15) & 0x7;

	/* Default to UARTA if uart_port is out of range */
	if (uart_port >= 4)
		uart_port = 0;

	uart_clk_enb_reg = uart_clk_out_enb_regs[uart_port];
	uart_rst_reg = uart_rst_devices_regs[uart_port];
	uart_mask = uart_enable_mask[uart_port];
	uart_clk_source = uart_clk_source_regs[uart_port];
	uart_base = uart_base_regs[uart_port];

	/* Enable UART clock */
	setbits32(uart_mask, uart_clk_enb_reg);

	/* Reset and unreset UART */
	setbits32(uart_mask, uart_rst_reg);
	clrbits32(uart_mask, uart_rst_reg);

	/* Program UART clock source: PLLP (408000000) */
	write32(uart_clk_source, 0);

	/* Program 115200n8 to the uart port */
	/* baud-rate of 115200 */
	write32((uart_base + UART_LCR), LCR_DLAB);
	write32((uart_base + UART_THR_DLAB), (UART_RATE_115200 & 0xff));
	write32((uart_base + UART_IER_DLAB), (UART_RATE_115200 >> 8));
	/* 8-bit and no parity */
	write32((uart_base + UART_LCR), LCR_WD_SIZE_8);
	/* enable and clear RX/TX FIFO */
	write32((uart_base + UART_IIR_FCR),
		(FCR_TX_CLR + FCR_RX_CLR + FCR_EN_FIFO));
}

/* Accessors. */

static uint32_t get_wakeup_vector(void)
{
	return read32(pmc_ctlr_scratch41_ptr);
}

static unsigned get_osc_freq(void)
{
	return (read32(clk_rst_osc_ctrl_ptr) & OSC_FREQ_MASK) >> OSC_FREQ_SHIFT;
}


/* Jtag configuration. */

static void enable_jtag(void)
{
	write32(misc_pp_config_ctl_ptr, PP_CONFIG_CTL_JTAG);
}

/* Clock configuration. */

static void config_oscillator(void)
{
	// Read oscillator drive strength from OSC_EDPD_OVER.XOFS and copy
	// to OSC_CTRL.XOFS and set XOE.
	uint32_t xofs = (read32(pmc_ctlr_osc_edpd_over_ptr) &
		    PMC_XOFS_MASK) >> PMC_XOFS_SHIFT;

	uint32_t osc_ctrl = read32(clk_rst_osc_ctrl_ptr);
	osc_ctrl &= ~OSC_XOFS_MASK;
	osc_ctrl |= (xofs << OSC_XOFS_SHIFT);
	osc_ctrl |= OSC_XOE;
	write32(clk_rst_osc_ctrl_ptr, osc_ctrl);
}

static void config_pllu(void)
{
	// Figure out what parameters to use for PLLU.
	uint32_t divm, divn, cpcon, lfcon;
	switch (get_osc_freq()) {
	case OSC_FREQ_12:
	case OSC_FREQ_48:
		divm = 0x0c;
		divn = 0x3c0;
		cpcon = 0x0c;
		lfcon = 0x02;
		break;
	case OSC_FREQ_16P8:
		divm = 0x07;
		divn = 0x190;
		cpcon = 0x05;
		lfcon = 0x02;
		break;
	case OSC_FREQ_19P2:
	case OSC_FREQ_38P4:
		divm = 0x04;
		divn = 0xc8;
		cpcon = 0x03;
		lfcon = 0x02;
		break;
	case OSC_FREQ_26:
		divm = 0x1a;
		divn = 0x3c0;
		cpcon = 0x0c;
		lfcon = 0x02;
		break;
	default:
		// Map anything that's not recognized to 13MHz.
		divm = 0x0d;
		divn = 0x3c0;
		cpcon = 0x0c;
		lfcon = 0x02;
	}

	// Configure PLLU.
	uint32_t base = PLLU_BYPASS | PLLU_OVERRIDE |
			(divn << PLLU_DIVN_SHIFT) | (divm << PLLU_DIVM_SHIFT);
	write32(clk_rst_pllu_base_ptr, base);
	uint32_t misc = (cpcon << PLLU_CPCON_SHIFT) |
			(lfcon << PLLU_LFCON_SHIFT);
	write32(clk_rst_pllu_misc_ptr, misc);

	// Enable PLLU.
	base &= ~PLLU_BYPASS;
	base |= PLLU_ENABLE;
	write32(clk_rst_pllu_base_ptr, base);
	misc |= PLLU_LOCK_ENABLE;
	write32(clk_rst_pllu_misc_ptr, misc);
}

static void enable_cpu_clocks(void)
{
	// Enable the CPU complex clock.
	write32(clk_rst_clk_enb_l_set_ptr, CLK_ENB_CPU);
	write32(clk_rst_clk_enb_v_set_ptr, CLK_ENB_CPUG | CLK_ENB_CPULP);
}



/* Function unit configuration. */

static void config_core_sight(void)
{
	// Enable the CoreSight clock.
	write32(clk_rst_clk_out_enb_u_set_ptr, CLK_ENB_CSITE);

	/*
	 * De-assert CoreSight reset.
	 * NOTE: We're leaving the CoreSight clock on the oscillator for
	 *       now. It will be restored to its original clock source
	 *       when the CPU-side restoration code runs.
	 */
	write32(clk_rst_rst_dev_u_clr_ptr, SWR_CSITE_RST);
}


/* Resets. */

static void clear_cpu_resets(void)
{
	/* Hold CPU1 in reset */
	setbits32(SET_CXRESET1, clk_rst_rst_cpulp_cmplx_set_ptr);

	write32(clk_rst_rst_cpug_cmplx_clr_ptr,
		CLR_NONCPURESET | CLR_L2RESET | CLR_PRESETDBG);

	write32(clk_rst_rst_cpug_cmplx_clr_ptr,
		CLR_CPURESET0 | CLR_DBGRESET0 | CLR_CORERESET0 | CLR_CXRESET0);
}



/* RAM repair */

void ram_repair(void)
{
	// Request Cluster0 RAM repair.
	setbits32(RAM_REPAIR_REQ, flow_ctlr_ram_repair_ptr);
	// Poll for Cluster0 RAM repair status.
	while (!(read32(flow_ctlr_ram_repair_ptr) & RAM_REPAIR_STS))
		;

	// Request Cluster1 RAM repair.
	setbits32(RAM_REPAIR_REQ, flow_ctlr_ram_repair_cluster1_ptr);
	// Poll for Cluster1 RAM repair status.
	while (!(read32(flow_ctlr_ram_repair_cluster1_ptr) & RAM_REPAIR_STS))
		;
}


/* Power. */

static void power_on_partition(unsigned id)
{
	uint32_t bit = 0x1 << id;
	if (!(read32(pmc_ctlr_pwrgate_status_ptr) & bit)) {
		// Partition is not on. Turn it on.
		write32(pmc_ctlr_pwrgate_toggle_ptr, id | PWRGATE_TOGGLE_START);

		// Wait until the partition is powerd on.
		while (!(read32(pmc_ctlr_pwrgate_status_ptr) & bit))
			;

		// Wait until clamp is off.
		while (read32(pmc_ctlr_clamp_status_ptr) & bit)
			;
	}
}

static void power_on_main_cpu(void)
{
	/*
	 * Reprogram PMC_CPUPWRGOOD_TIMER register:
	 *
	 * XXX This is a fragile assumption. XXX
	 * The kernel prepares PMC_CPUPWRGOOD_TIMER based on a 32768Hz clock.
	 * Note that PMC_CPUPWRGOOD_TIMER is running at pclk.
	 *
	 * We need to reprogram PMC_CPUPWRGOOD_TIMER based on the current pclk
	 * which is at 204Mhz (pclk = sclk = pllp_out2) after BootROM. Multiply
	 * PMC_CPUPWRGOOD_TIMER by 204M / 32K.
	 *
	 * Save the original PMC_CPUPWRGOOD_TIMER register which we need to
	 * restore after the CPU is powered up.
	 */
	uint32_t orig_timer = read32(pmc_ctlr_cpupwrgood_timer_ptr);

	write32(pmc_ctlr_cpupwrgood_timer_ptr,
		orig_timer * (204000000 / 32768));

	power_on_partition(PARTID_CRAIL);
	power_on_partition(PARTID_C0NC);
	power_on_partition(PARTID_CE0);

	// Restore the original PMC_CPUPWRGOOD_TIMER.
	write32(pmc_ctlr_cpupwrgood_timer_ptr, orig_timer);
}


static void aarch64_trampoline(void)
{
	uint32_t val = 3;	/* bit1: to warm reset; bit0: AARCH64*/

	asm volatile ("mcr p15, 0, %0, c12, c0, 2" : : "r" (val));

	/* unreachable */
	halt();
}


/* Entry point. */

void lp0_resume(void)
{
	// If not on the AVP, reset.
	if (read32(up_tag_ptr) != UP_TAG_AVP)
		reset();

	// Enable JTAG
	enable_jtag();

	config_oscillator();

	// Program SUPER_CCLK_DIVIDER.
	write32(clk_rst_super_cclk_div_ptr, SUPER_CDIV_ENB);

	config_core_sight();

	enable_uart();

	config_pllu();

	/*
	 * Set the CPU reset vector.
	 *
	 * T132 always resets to AARCH32 and SW needs to write RMR_EL3
	 * to bootstrap into AARCH64.
	 */
	write32(pmc_ctlr_secure_scratch34_ptr, get_wakeup_vector());
	write32(pmc_ctlr_secure_scratch35_ptr, 0);
	write32(evp_cpu_reset_ptr, (uint32_t)aarch64_trampoline);

	// Select CPU complex clock source.
	write32(clk_rst_cclk_burst_policy_ptr, CCLK_PLLP_BURST_POLICY);

	// Disable PLLX since it isn't used as CPU clock source.
	clrbits32(PLLX_ENABLE, clk_rst_pllx_base_ptr);

	// Set CAR2PMC_CPU_ACK_WIDTH to 408.
	uint32_t ack_width = read32(clk_rst_cpu_softrst_ctrl2_ptr);
	ack_width &= ~CAR2PMC_CPU_ACK_WIDTH_MASK;
	ack_width |= 408 << CAR2PMC_CPU_ACK_WIDTH_SHIFT;
	write32(clk_rst_cpu_softrst_ctrl2_ptr, ack_width);

	// Disable VPR.
	write32(mc_video_protect_size_mb_ptr, 0);
	write32(mc_video_protect_reg_ctrl_ptr,
		VIDEO_PROTECT_WRITE_ACCESS_DISABLE);

	enable_cpu_clocks();

	power_on_main_cpu();

	// Perform ram repair after cpu is powered on.
	ram_repair();

	clear_cpu_resets();

	// Halt the AVP.
	while (1)
		write32(flow_ctlr_halt_cop_events_ptr,
		        FLOW_MODE_STOP | EVENT_JTAG);
}



/* Header. */

extern uint8_t blob_data;
extern uint8_t blob_data_size;
extern uint8_t blob_total_size;

struct lp0_header {
	uint32_t length_insecure;	// Insecure total length.
	uint32_t reserved[3];
	uint8_t rsa_modulus[256];	// RSA key modulus.
	uint8_t aes_signature[16];	// AES signature.
	uint8_t rsa_signature[256];	// RSA-PSS signature.
	uint8_t random_aes_block[16];	// Random data, may be zero.
	uint32_t length_secure;		// Secure total length.
	uint32_t destination;		// Where to load the blob in iRAM.
	uint32_t entry_point;		// Entry point for the blob.
	uint32_t code_length;		// Length of just the data.
} __attribute__((packed));

struct lp0_header header __attribute__((section(".header"))) =
{
	.length_insecure = (uintptr_t)&blob_total_size,
	.length_secure = (uintptr_t)&blob_total_size,
	.destination = (uintptr_t)&blob_data,
	.entry_point = (uintptr_t)&lp0_resume,
	.code_length = (uintptr_t)&blob_data_size
};
