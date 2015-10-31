/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Andrew Wu <arw@dmp.com.tw>
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

#include <stdint.h>
#include <arch/io.h>
#include <stdlib.h>
#include "arch/x86/romcc_console.c"
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <halt.h>
#include "drivers/pc80/i8254.c"
#include <northbridge/dmp/vortex86ex/northbridge.h>
#include <southbridge/dmp/vortex86ex/southbridge.h>
#include "northbridge/dmp/vortex86ex/raminit.c"
#include <cpu/dmp/dmp_post_code.h>

#define DMP_CPUID_SX      0x31504d44
#define DMP_CPUID_DX      0x32504d44
#define DMP_CPUID_MX      0x33504d44
#define DMP_CPUID_DX2     0x34504d44
#define DMP_CPUID_MX_PLUS 0x35504d44
#define DMP_CPUID_EX      0x37504d44

static u32 get_dmp_id(void)
{
	return pci_read_config32(NB, NB_REG_CID);
}

/* Indirect access registers for Watch-dog timer, GPIO PORT 0,1
 * Index port is for I/O port 22h
 * Index port 13h (00: lock register, C5h: unlock register) for lock/unlock function
 * Index port 37h, 39h, 3Ah, 3Bh, 3Ch for Watchdog timer
 * Index port 46h, 47h, 4Ch, 4Dh, 4Eh, 4Fh for GPIO port 0, 1
 */
static void write_indirect_reg(u8 index, u8 data)
{
	outb(index, 0x22);
	outb(data, 0x23);
}

static void lock_indirect_reg(void)
{
	write_indirect_reg(0x13, 0x00);
}

static void unlock_indirect_reg(void)
{
	write_indirect_reg(0x13, 0xc5);
}

static void disable_watchdog(void)
{
	unlock_indirect_reg();
	// disable watchdog timer
	write_indirect_reg(0x37, 0x0);
}

void set_ex_powerdown_control(void)
{
	u32 powerdown_ctrl;
	powerdown_ctrl = pci_read_config32(SB, 0xbc);
#if CONFIG_TEMP_POWERDOWN
	powerdown_ctrl |= (1 << 31);
#endif
#if CONFIG_SATA_POWERDOWN
	powerdown_ctrl |= (1 << 30);
#endif
#if CONFIG_ADC_POWERDOWN
	powerdown_ctrl |= (1 << 28);
#endif
#if CONFIG_PCIE0_POWERDOWN
	powerdown_ctrl |= (1 << 13);
#endif
#if CONFIG_MAC_POWERDOWN
	powerdown_ctrl |= (1 << 3);
#endif
#if CONFIG_USB1_POWERDOWN
	powerdown_ctrl |= (1 << 1);
#endif
#if CONFIG_IDE_POWERDOWN
	powerdown_ctrl |= (1 << 0);
#endif
	pci_write_config32(SB, 0xbc, powerdown_ctrl);
}

static void set_pci_nb_pmcr(void)
{
	u8 pmcr = pci_read_config8(NB, NB_REG_PMCR);
	/*
	 * Set PCI Master Max Cycle Length (MCL) to 32 PCI clocks.
	 * Set PCI Master Burst Write Length (BL) to Burst length over 3.
	 */
	pmcr |= 0x0f;
	pci_write_config8(NB, NB_REG_PMCR, pmcr);
}

static void set_pci_sb_lpccr(void)
{
	u8 lpccr = pci_read_config8(SB, SB_REG_LPCCR);
	/* Set PCI Soft Reset Control to 1.
	 * (When the CPU soft reset is initialized, PCIRST# will be active.)
	 * Set P92FE to 1. (I/O port 92 Register Write Function Enable.)
	 * Set P92S to 1. (Internal Port 92h Selected.)
	 */
	lpccr |= 0x16;
	pci_write_config8(SB, SB_REG_LPCCR, lpccr);
	/* enable fast ga20 */
	outb(inb(SYSTEM_CTL_PORT) | 0x02, SYSTEM_CTL_PORT);
}

static u32 make_uart_config(u16 base, u8 irq)
{
	/* Set base IO address only, skip IRQ. IRQ will be setup in
	 * southbridge stage. */
	u32 cfg = 0;
	cfg |= 1 << 23;		// UE = enabled.
	cfg |= base;		// UIOA.
	return cfg;
}

#define SETUP_UART(n) \
	uart_cfg = make_uart_config(CONFIG_UART##n##_IO, CONFIG_UART##n##_IRQ);\
	outl(uart_cfg, 0xc00 + (n - 1) * 4);

static void ex_uart_early_init(void)
{
#if CONFIG_TTYS0_BASE
	u32 uart_cfg = 0;
	/* Set UART Config I/O base address to 0xc00 */
	pci_write_config16(SB, 0x60, 0xc01);
	/* If serial console base address is defined, find out which
	 * UART uses this address, and setup this UART first. */
#if CONFIG_TTYS0_BASE == CONFIG_UART1_IO
	SETUP_UART(1)
#elif CONFIG_TTYS0_BASE == CONFIG_UART2_IO
	SETUP_UART(2)
#elif CONFIG_TTYS0_BASE == CONFIG_UART3_IO
	SETUP_UART(3)
#elif CONFIG_TTYS0_BASE == CONFIG_UART4_IO
	SETUP_UART(4)
#elif CONFIG_TTYS0_BASE == CONFIG_UART5_IO
	SETUP_UART(5)
#elif CONFIG_TTYS0_BASE == CONFIG_UART6_IO
	SETUP_UART(6)
#elif CONFIG_TTYS0_BASE == CONFIG_UART7_IO
	SETUP_UART(7)
#elif CONFIG_TTYS0_BASE == CONFIG_UART8_IO
	SETUP_UART(8)
#elif CONFIG_TTYS0_BASE == CONFIG_UART9_IO
	SETUP_UART(9)
#elif CONFIG_TTYS0_BASE == CONFIG_UART10_IO
	SETUP_UART(10)
#endif
#endif
}

static void init_wdt1(void)
{
#if CONFIG_WDT1_INITIALIZE
#if CONFIG_WDT1_ENABLE
	outb(0x1 << 6, 0xa8);
#endif
	u8 wdt1_signal_reg = 0;
#if CONFIG_WDT1_SINGAL_NMI
	wdt1_signal_reg = 0x0c << 4;
#elif CONFIG_WDT1_SIGNAL_RESET
	wdt1_signal_reg = 0x0d << 4;
#elif CONFIG_WDT1_SIGNAL_SMI
	wdt1_signal_reg = 0x0e << 4;
#endif
	outb(wdt1_signal_reg, 0xa9);
#endif
}

/* Fill 32bit pattern into specified DRAM region. */
static void fill_dram(u32 * p, u32 pattern, u32 test_len)
{
	if (test_len == 0)
		return;
#if 0
	// C version, very slow.
	for (p = (u32 *) 0; (u32) p < test_len; p++) {
		*p = pattern;
	}
#endif
	// ASM version, much faster.
	asm volatile (
		"cld\n\t"
		"rep\n\t"
		"stosl"
		: /* no output registers */
		: "c" (test_len / 4), "a" (pattern), "D" (p)
	);
}

/* Verify 32bit pattern in specified DRAM region.
 * Return -1 if ok, failed memory address if error. */
static int verify_dram(u32 * p, u32 pattern, u32 test_len)
{
	if (test_len == 0)
		return -1;
#if 0
	// C version, very slow.
	for (p = (u32 *) 0; (u32) p < test_len; p++) {
		if (*p != pattern) {
			return (int)p;
		}
	}
	return -1;
#endif
	u16 flags;
	int addr;
	asm volatile (
		"cld\n\t"
		"rep\n\t"
		"scasl\n\t"
		"lahf\n\t"
		: "=a" (flags), "=D" (addr)
		: "c" (test_len / 4), "a" (pattern), "D" (p)
	);
	if (flags & (1 << (6 + 8)))	// x86 zero flag = bit 6.
	{
		return -1;	// verify ok
	}
	return addr - 4;	// verify error, return error address.
}

static void test_dram_stability(void)
{
	u32 test_len = 2048 * 1024;
	u32 pat = 0x5aa5a55a;
	u32 ext_mem_start = 0xc0000;
	u32 base_mem_test_len = test_len > 640 * 1024 ? 640 * 1024 : test_len;
	u32 ext_mem_test_len = test_len > ext_mem_start ? test_len - ext_mem_start : 0;
	if (ext_mem_test_len > 0) {
		/* Enable all shadow RAM region C0000 - FFFFF. */
		pci_write_config32(NB, NB_REG_MAR, 0x3ffffff0);
	}
	int v;
	fill_dram((u32 *) 0, pat, base_mem_test_len);
	fill_dram((u32 *) ext_mem_start, pat, ext_mem_test_len);
	v = verify_dram((u32 *) 0, pat, base_mem_test_len);
	if (v == -1) {
		v = verify_dram((u32 *) ext_mem_start, pat, ext_mem_test_len);
	}
	/* Change pattern and test again */
	if (v == -1) {
		pat = 0xa55a5aa5;
		fill_dram((u32 *) 0, pat, base_mem_test_len);
		fill_dram((u32 *) ext_mem_start, pat, ext_mem_test_len);
		v = verify_dram((u32 *) 0, pat, base_mem_test_len);
		if (v == -1) {
			v = verify_dram((u32 *) ext_mem_start, pat, ext_mem_test_len);
		}
	}
	if (v != -1) {
		post_code(POST_DMP_DRAM_TEST_ERR);
		print_emerg("DRAM stablility test error!\nADDR = ");
		print_emerg_hex32(v);
		print_emerg(", WRITE = ");
		print_emerg_hex32(pat);
		u32 r = *(u32 *) v;
		print_emerg(", READ = ");
		print_emerg_hex32(r);
		print_emerg(", XOR = ");
		print_emerg_hex32(r ^ pat);
		print_emerg("\n");
		die("System halted.\n");
	}
	if (ext_mem_test_len > 0) {
		/* Disable shadow RAM. */
		pci_write_config32(NB, NB_REG_MAR, 0x0);
	}
}

static void enable_l2_cache(void)
{
	/*
	 * Enable L2 cache by setting PCI N/B function 1 L2 cache
	 * control register (0xe8) bit 0 (L2_EN) and bit 1 (L2_WB_EN).
	 */
	u32 reg_nb_f1_e8;
	reg_nb_f1_e8 = pci_read_config8(NB1, 0xe8);
	reg_nb_f1_e8 |= 3;
	pci_write_config8(NB1, 0xe8, reg_nb_f1_e8);
}

#include <cpu/intel/romstage.h>
static void main(unsigned long bist)
{
	u32 dmp_id;

	dmp_id = get_dmp_id();
	if (dmp_id != DMP_CPUID_EX) {
		/* Not DMP Vortex86EX CPU. */
		post_code(POST_DMP_ID_ERR);
		halt();
	}
	disable_watchdog();
	set_ex_powerdown_control();
	set_pci_nb_pmcr();
	set_pci_sb_lpccr();
	ex_uart_early_init();

	console_init();

	init_wdt1();

	/* Initialize i8254 timers */
	post_code(0x42);
	setup_i8254();

	/* Initialize DRAM */
	u8 reg_nb_f1_cc;
	/* Setup DDR3 Timing reg 0-3 / Config reg */
	pci_write_config16(NB, 0x6e, 0x0a2f);
	pci_write_config32(NB, 0x74, 0x84010200);
	pci_write_config32(NB, 0x78, 0x33405544);
	pci_write_config32(NB, 0x7c, 0x2e0f0e0b);
	/* Disable enhance read push write */
	reg_nb_f1_cc = pci_read_config8(NB1, 0xcc);
	reg_nb_f1_cc &= ~(1 << 4);
	pci_write_config8(NB1, 0xcc, reg_nb_f1_cc);
	if (detect_ddr3_dram_size()) {
		post_code(POST_DMP_DRAM_SIZING_ERR);
		die("DRAM sizing error!\n");
	}
	/* Reset enhance read push write to default(enable) */
	reg_nb_f1_cc |= (1 << 4);
	pci_write_config8(NB1, 0xcc, reg_nb_f1_cc);

	print_ddr3_memory_setup();
	test_dram_stability();

	/* CPU setup, romcc pukes on invd() */
	asm volatile ("invd");
	enable_cache();

	enable_l2_cache();
}
