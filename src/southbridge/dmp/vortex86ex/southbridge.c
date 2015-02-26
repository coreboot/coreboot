/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <pc80/keyboard.h>
#include <string.h>
#include <delay.h>
#include "arch/io.h"
#include "chip.h"
#include "southbridge.h"
#include "cpu/dmp/dmp_post_code.h"

/* IRQ number to S/B PCI Interrupt routing table reg(0x58/0xb4) mapping table. */
static const unsigned char irq_to_int_routing[16] = {
	0x0, 0x0, 0x0, 0x2,	// IRQ0-2 is unmappable, IRQ3 = 2.
	0x4, 0x5, 0x7, 0x6,	// IRQ4-7 = 4, 5, 7, 6.
	0x0, 0x1, 0x3, 0x9,	// IRQ8 is unmappable, IRQ9-11 = 1, 3, 9.
	0xb, 0x0, 0xd, 0xf	// IRQ12 = b, IRQ13 is unmappable, IRQ14-15 = d, f.
};

/* S/B PCI Interrupt routing table reg(0x58) field bit shift. */
#define EHCIH_IRQ_SHIFT 28
#define OHCII_IRQ_SHIFT 24
#define MAC_IRQ_SHIFT 16
#define RT3_IRQ_SHIFT 12
#define RT2_IRQ_SHIFT 8
#define RT1_IRQ_SHIFT 4
#define RT0_IRQ_SHIFT 0

/* S/B Extend PCI Interrupt routing table reg(0xb4) field bit shift. */
#define CAN_IRQ_SHIFT 28
#define HDA_IRQ_SHIFT 20
#define USBD_IRQ_SHIFT 16
#define SIDE_IRQ_SHIFT 12
#define PIDE_IRQ_SHIFT 8

/* S/B function 1 Extend PCI Interrupt routing table reg 2(0xb4)
 * field bit shift.
 */
#define SPI1_IRQ_SHIFT 8
#define MOTOR_IRQ_SHIFT 0

/* in-chip PCI device IRQs(0 for disabled). */
#define EHCII_IRQ 5
#define OHCII_IRQ 5
#define MAC_IRQ 6

#define CAN_IRQ 10
#define HDA_IRQ 7
#define USBD_IRQ 6
#define PIDE_IRQ 5

#define SPI1_IRQ 10
#define I2C0_IRQ 10
#define MOTOR_IRQ 11

/* RT0-3 IRQs. */
#define RT3_IRQ 3
#define RT2_IRQ 4
#define RT1_IRQ 5
#define RT0_IRQ 6

/* IDE legacy mode IRQs. */
#define IDE1_LEGACY_IRQ 14
#define IDE2_LEGACY_IRQ 15

/* Internal parallel port */
#define LPT_INT_C 0
#define LPT_INT_ACK_SET 0
#define LPT_UE 1
#define LPT_PDMAS 0
#define LPT_DREQS 0

/* keyboard controller system flag timeout : 400 ms */
#define KBC_TIMEOUT_SYS_FLAG 400

static u8 get_pci_dev_func(device_t dev)
{
	return PCI_FUNC(dev->path.pci.devfn);
}

static void verify_dmp_keyboard_error(void)
{
	post_code(POST_DMP_KBD_FW_VERIFY_ERR);
	die("Internal keyboard firmware verify error!\n");
}

static void upload_dmp_keyboard_firmware(struct device *dev)
{
	u32 reg_sb_c0;
	u32 fwptr;

	// enable firmware uploading function by set bit 10.
	post_code(POST_DMP_KBD_FW_UPLOAD);
	reg_sb_c0 = pci_read_config32(dev, SB_REG_IPFCR);
	pci_write_config32(dev, SB_REG_IPFCR, reg_sb_c0 | 0x400);

	outw(0, 0x62);		// reset upload address to 0.
	// upload 4096 bytes from 0xFFFFE000.
	outsb(0x66, (u8 *) 0xffffe000, 4096);
	// upload 4096 bytes from 0xFFFFC000.
	outsb(0x66, (u8 *) 0xffffc000, 4096);

	outw(0, 0x62);		// reset upload address to 0.
	// verify 4096 bytes from 0xFFFFE000.
	for (fwptr = 0xffffe000; fwptr < 0xfffff000; fwptr++) {
		if (inb(0x66) != *(u8 *) fwptr) {
			verify_dmp_keyboard_error();
		}
	}
	// verify 4096 bytes from 0xFFFFC000.
	for (fwptr = 0xffffc000; fwptr < 0xffffd000; fwptr++) {
		if (inb(0x66) != *(u8 *) fwptr) {
			verify_dmp_keyboard_error();
		}
	}

	// disable firmware uploading.
	pci_write_config32(dev, SB_REG_IPFCR, reg_sb_c0 & ~0x400L);
}

static int kbc_wait_system_flag(void)
{
	/* wait keyboard controller ready by checking system flag
	 * (status port bit 2).
	 */
	post_code(POST_DMP_KBD_CHK_READY);
	u32 timeout;
	for (timeout = KBC_TIMEOUT_SYS_FLAG;
	     timeout && ((inb(0x64) & 0x4) == 0); timeout--)
		mdelay(1);

	if (!timeout) {
		printk(BIOS_WARNING, "Keyboard controller system flag timeout\n");
	}
	return !!timeout;
}

static void pci_routing_fixup(struct device *dev)
{
	const unsigned slot[3] = { 0 };
	const unsigned char slot_irqs[1][4] = {
		{RT0_IRQ, RT1_IRQ, RT2_IRQ, RT3_IRQ},
	};
	const int slot_num = 1;
	int i;
	u32 int_routing = 0;
	u32 ext_int_routing = 0;

	/* assign PCI-e bridge (bus#0, dev#1, fn#0) IRQ to RT0. */
	pci_assign_irqs(0, 1, slot_irqs[0]);

	/* RT0 is enabled. */
	int_routing |= irq_to_int_routing[RT0_IRQ] << RT0_IRQ_SHIFT;

	/* assign PCI slot IRQs. */
	for (i = 0; i < slot_num; i++) {
		pci_assign_irqs(1, slot[i], slot_irqs[i]);
	}

	/* Read PCI slot IRQs to see if RT1-3 is used, and enables it */
	for (i = 0; i < slot_num; i++) {
		unsigned int funct;
		device_t pdev;
		u8 irq;

		/* Each slot may contain up to eight functions. */
		for (funct = 0; funct < 8; funct++) {
			pdev = dev_find_slot(1, (slot[i] << 3) + funct);
			if (!pdev)
				continue;
			irq = pci_read_config8(pdev, PCI_INTERRUPT_LINE);
			if (irq == RT1_IRQ) {
				int_routing |= irq_to_int_routing[RT1_IRQ] << RT1_IRQ_SHIFT;
			} else if (irq == RT2_IRQ) {
				int_routing |= irq_to_int_routing[RT2_IRQ] << RT2_IRQ_SHIFT;
			} else if (irq == RT3_IRQ) {
				int_routing |= irq_to_int_routing[RT3_IRQ] << RT3_IRQ_SHIFT;
			}
		}
	}

	/* Setup S/B PCI Interrupt routing table reg(0x58). */
	int_routing |= irq_to_int_routing[EHCII_IRQ] << EHCIH_IRQ_SHIFT;
	int_routing |= irq_to_int_routing[OHCII_IRQ] << OHCII_IRQ_SHIFT;
	int_routing |= irq_to_int_routing[MAC_IRQ] << MAC_IRQ_SHIFT;
	pci_write_config32(dev, SB_REG_PIRQ_ROUTE, int_routing);

	/* Setup S/B PCI Extend Interrupt routing table reg(0xb4). */
	ext_int_routing |= irq_to_int_routing[CAN_IRQ] << CAN_IRQ_SHIFT;
	ext_int_routing |= irq_to_int_routing[HDA_IRQ] << HDA_IRQ_SHIFT;
	ext_int_routing |= irq_to_int_routing[USBD_IRQ] << USBD_IRQ_SHIFT;
#if CONFIG_IDE_NATIVE_MODE
	/* IDE in native mode, only uses one IRQ. */
	ext_int_routing |= irq_to_int_routing[0] << SIDE_IRQ_SHIFT;
	ext_int_routing |= irq_to_int_routing[PIDE_IRQ] << PIDE_IRQ_SHIFT;
#else
	/* IDE in legacy mode, use IRQ 14, 15. */
	ext_int_routing |= irq_to_int_routing[IDE2_LEGACY_IRQ] << SIDE_IRQ_SHIFT;
	ext_int_routing |= irq_to_int_routing[IDE1_LEGACY_IRQ] << PIDE_IRQ_SHIFT;
#endif
	pci_write_config32(dev, SB_REG_EXT_PIRQ_ROUTE, ext_int_routing);

	/* Assign in-chip PCI device IRQs. */
	if (MAC_IRQ) {
		unsigned char irqs[4] = { MAC_IRQ, 0, 0, 0 };
		pci_assign_irqs(0, 0x8, irqs);
	}
	if ((OHCII_IRQ != 0) && (EHCII_IRQ != 0)) {
		unsigned char irqs[4] = { OHCII_IRQ, EHCII_IRQ, 0, 0 };
		pci_assign_irqs(0, 0xa, irqs);
	}
	if ((CONFIG_IDE_NATIVE_MODE != 0) && (PIDE_IRQ != 0)) {
		/* IDE in native mode, setup PCI IRQ. */
		unsigned char irqs[4] = { PIDE_IRQ, 0, 0, 0 };
		pci_assign_irqs(0, 0xc, irqs);
	}
	if (CAN_IRQ) {
		unsigned char irqs[4] = { CAN_IRQ, 0, 0, 0 };
		pci_assign_irqs(0, 0x11, irqs);
	}
	if (HDA_IRQ) {
		unsigned char irqs[4] = { HDA_IRQ, 0, 0, 0 };
		pci_assign_irqs(0, 0xe, irqs);
	}
	if (USBD_IRQ) {
		unsigned char irqs[4] = { USBD_IRQ, 0, 0, 0 };
		pci_assign_irqs(0, 0xf, irqs);
	}
}

static void vortex_sb_init(struct device *dev)
{
	u32 lpt_reg = 0;

#if CONFIG_LPT_ENABLE
	int ppmod = 0;
#if CONFIG_LPT_MODE_BPP
	ppmod = 0;
#elif CONFIG_LPT_MODE_EPP_19_AND_SPP
	ppmod = 1;
#elif CONFIG_LPT_MODE_ECP
	ppmod = 2;
#elif CONFIG_LPT_MODE_ECP_AND_EPP_19
	ppmod = 3;
#elif CONFIG_LPT_MODE_SPP
	ppmod = 4;
#elif CONFIG_LPT_MODE_EPP_17_AND_SPP
	ppmod = 5;
#elif CONFIG_LPT_MODE_ECP_AND_EPP_17
	ppmod = 7;
#else
#error CONFIG_LPT_MODE error.
#endif

	/* Setup internal parallel port */
	lpt_reg |= (LPT_INT_C << 28);
	lpt_reg |= (LPT_INT_ACK_SET << 27);
	lpt_reg |= (ppmod << 24);
	lpt_reg |= (LPT_UE << 23);
	lpt_reg |= (LPT_PDMAS << 22);
	lpt_reg |= (LPT_DREQS << 20);
	lpt_reg |= (irq_to_int_routing[CONFIG_LPT_IRQ] << 16);
	lpt_reg |= (CONFIG_LPT_IO << 0);
#endif				// CONFIG_LPT_ENABLE
	pci_write_config32(dev, SB_REG_IPPCR, lpt_reg);
}

#define SETUP_GPIO_ADDR(n) \
	u32 cfg##n = (CONFIG_GPIO_P##n##_DIR_ADDR << 16) | (CONFIG_GPIO_P##n##_DATA_ADDR);\
	outl(cfg##n, base + 4 + (n * 4));\
	gpio_enable_mask |= (1 << n);

#define INIT_GPIO(n) \
	outb(CONFIG_GPIO_P##n##_INIT_DIR, CONFIG_GPIO_P##n##_DIR_ADDR);\
	outb(CONFIG_GPIO_P##n##_INIT_DATA, CONFIG_GPIO_P##n##_DATA_ADDR);

static void ex_sb_gpio_init(struct device *dev)
{
	const int base = 0xb00;
	u32 gpio_enable_mask = 0;
	/* S/B register 63h - 62h : GPIO Port Config IO Base Address */
	pci_write_config16(dev, SB_REG_GPIO_CFG_IO_BASE, base | 1);
	/* Set GPIO port 0~9 base address.
	 * Config Base + 04h, 08h, 0ch... : GPIO port 0~9 data/dir decode addr.
	 * Bit 31-16 : DBA, GPIO direction base address.
	 * Bit 15-0  : DPBA, GPIO data port base address.
	 * */
#if CONFIG_GPIO_P0_ENABLE
	SETUP_GPIO_ADDR(0)
#endif
#if CONFIG_GPIO_P1_ENABLE
	SETUP_GPIO_ADDR(1)
#endif
#if CONFIG_GPIO_P2_ENABLE
	SETUP_GPIO_ADDR(2)
#endif
#if CONFIG_GPIO_P3_ENABLE
	SETUP_GPIO_ADDR(3)
#endif
#if CONFIG_GPIO_P4_ENABLE
	SETUP_GPIO_ADDR(4)
#endif
#if CONFIG_GPIO_P5_ENABLE
	SETUP_GPIO_ADDR(5)
#endif
#if CONFIG_GPIO_P6_ENABLE
	SETUP_GPIO_ADDR(6)
#endif
#if CONFIG_GPIO_P7_ENABLE
	SETUP_GPIO_ADDR(7)
#endif
#if CONFIG_GPIO_P8_ENABLE
	SETUP_GPIO_ADDR(8)
#endif
#if CONFIG_GPIO_P9_ENABLE
	SETUP_GPIO_ADDR(9)
#endif
	/* Enable GPIO port 0~9. */
	outl(gpio_enable_mask, base);
	/* Set GPIO port 0-9 initial dir and data. */
#if CONFIG_GPIO_P0_ENABLE
	INIT_GPIO(0)
#endif
#if CONFIG_GPIO_P1_ENABLE
	INIT_GPIO(1)
#endif
#if CONFIG_GPIO_P2_ENABLE
	INIT_GPIO(2)
#endif
#if CONFIG_GPIO_P3_ENABLE
	INIT_GPIO(3)
#endif
#if CONFIG_GPIO_P4_ENABLE
	INIT_GPIO(4)
#endif
#if CONFIG_GPIO_P5_ENABLE
	INIT_GPIO(5)
#endif
#if CONFIG_GPIO_P6_ENABLE
	INIT_GPIO(6)
#endif
#if CONFIG_GPIO_P7_ENABLE
	INIT_GPIO(7)
#endif
#if CONFIG_GPIO_P8_ENABLE
	INIT_GPIO(8)
#endif
#if CONFIG_GPIO_P9_ENABLE
	INIT_GPIO(9)
#endif
	/* Disable GPIO Port Config IO Base Address. */
	pci_write_config16(dev, SB_REG_GPIO_CFG_IO_BASE, 0x0);
}

static u32 make_uart_config(u16 base, u8 irq)
{
	u8 mapped_irq = irq_to_int_routing[irq];
	u32 cfg = 0;
	cfg |= 1 << 23;			// UE = enabled.
	cfg |= (mapped_irq << 16);	// UIRT.
	cfg |= base;			// UIOA.
	return cfg;
}

#define SETUP_UART(n) \
	uart_cfg = make_uart_config(CONFIG_UART##n##_IO, CONFIG_UART##n##_IRQ);\
	outl(uart_cfg, base + (n - 1) * 4);

static void ex_sb_uart_init(struct device *dev)
{
	const int base = 0xc00;
	u32 uart_cfg = 0;
	/* S/B register 61h - 60h : UART Config IO Base Address */
	pci_write_config16(dev, SB_REG_UART_CFG_IO_BASE, base | 1);
	/* setup UART */
#if CONFIG_UART1_ENABLE
	SETUP_UART(1)
#endif
#if CONFIG_UART2_ENABLE
	SETUP_UART(2)
#endif
#if CONFIG_UART3_ENABLE
	SETUP_UART(3)
#endif
#if CONFIG_UART4_ENABLE
	SETUP_UART(4)
#endif
#if CONFIG_UART5_ENABLE
	SETUP_UART(5)
#endif
#if CONFIG_UART6_ENABLE
	SETUP_UART(6)
#endif
#if CONFIG_UART7_ENABLE
	SETUP_UART(7)
#endif
#if CONFIG_UART8_ENABLE
	SETUP_UART(8)
#endif
#if CONFIG_UART9_ENABLE
	SETUP_UART(9)
#endif
#if CONFIG_UART10_ENABLE
	SETUP_UART(10)
#endif
	/* Keep UART Config I/O base address */
	//pci_write_config16(SB, SB_REG_UART_CFG_IO_BASE, 0x0);
}

static void i2c_init(struct device *dev)
{
	u8 mapped_irq = irq_to_int_routing[I2C0_IRQ];
	u32 cfg = 0;
	cfg |= 1 << 31;			// UE = enabled.
	cfg |= (mapped_irq << 16);	// IIRT0.
	cfg |= CONFIG_I2C_BASE;		// UIOA.
	pci_write_config32(dev, SB_REG_II2CCR, cfg);
}

static int get_rtc_update_in_progress(void)
{
	if (cmos_read(RTC_REG_A) & RTC_UIP)
		return 1;
	return 0;
}

static void unsafe_read_cmos_rtc(u8 rtc[7])
{
	rtc[0] = cmos_read(RTC_CLK_ALTCENTURY);
	rtc[1] = cmos_read(RTC_CLK_YEAR);
	rtc[2] = cmos_read(RTC_CLK_MONTH);
	rtc[3] = cmos_read(RTC_CLK_DAYOFMONTH);
	rtc[4] = cmos_read(RTC_CLK_HOUR);
	rtc[5] = cmos_read(RTC_CLK_MINUTE);
	rtc[6] = cmos_read(RTC_CLK_SECOND);
}

static void read_cmos_rtc(u8 rtc[7])
{
	/* Read RTC twice and check update-in-progress flag, to make
	 * sure RTC is correct */
	u8 rtc_old[7], rtc_new[7];
	while (get_rtc_update_in_progress()) ;
	unsafe_read_cmos_rtc(rtc_new);
	do {
		memcpy(rtc_old, rtc_new, 7);
		while (get_rtc_update_in_progress()) ;
		unsafe_read_cmos_rtc(rtc_new);
	} while (memcmp(rtc_new, rtc_old, 7) != 0);
}

/*
 * Convert a number in decimal format into the BCD format.
 * Return 255 if not a valid BCD value.
 */
static u8 bcd2dec(u8 bcd)
{
	u8 h, l;
	h = bcd >> 4;
	l = bcd & 0xf;
	if (h > 9 || l > 9)
		return 255;
	return h * 10 + l;
}

static void fix_cmos_rtc_time(void)
{
	/* Read RTC data. */
	u8 rtc[7];
	read_cmos_rtc(rtc);

	/* Convert RTC from BCD format to binary. */
	u8 bin_rtc[7];
	int i;
	for (i = 0; i < 7; i++) {
		bin_rtc[i] = bcd2dec(rtc[i]);
	}

	/* If RTC date is invalid, fix it. */
	if (bin_rtc[0] > 99 || bin_rtc[1] > 99 || bin_rtc[2] > 12 || bin_rtc[3] > 31) {
		/* Set PC compatible timing mode. */
		cmos_write(0x26, RTC_REG_A);
		cmos_write(0x02, RTC_REG_B);
		/* Now setup a default date 2008/08/08 08:08:08. */
		cmos_write(0x8, RTC_CLK_SECOND);
		cmos_write(0x8, RTC_CLK_MINUTE);
		cmos_write(0x8, RTC_CLK_HOUR);
		cmos_write(0x6, RTC_CLK_DAYOFWEEK);	/* Friday */
		cmos_write(0x8, RTC_CLK_DAYOFMONTH);
		cmos_write(0x8, RTC_CLK_MONTH);
		cmos_write(0x8, RTC_CLK_YEAR);
		cmos_write(0x20, RTC_CLK_ALTCENTURY);
	}
}

static void vortex86_sb_set_io_resv(device_t dev, unsigned index, u32 base, u32 size)
{
	struct resource *res;
	res = new_resource(dev, index);
	res->base = base;
	res->size = size;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void vortex86_sb_set_spi_flash_size(device_t dev, unsigned index, u32 flash_size)
{
	/* SPI flash is in topmost of 4G memory space */
	struct resource *res;
	res = new_resource(dev, index);
	res->base = 0x100000000LL - flash_size;
	res->size = flash_size;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void vortex86_sb_read_resources(device_t dev)
{
	u32 flash_size = 8 * 1024 * 1024;

	pci_dev_read_resources(dev);

	if (dev->device == 0x6011) {
		/* It is EX CPU southbridge */
		if (get_pci_dev_func(dev) != 0) {
			/* only for function 0, skip function 1 */
			return;
		}
		/* default SPI flash ROM is 64MB */
		flash_size = 64 * 1024 * 1024;
	}

	/* Reserve space for legacy I/O */
	vortex86_sb_set_io_resv(dev, 1, 0, 0x1000UL);

	/* Reserve space for flash */
	vortex86_sb_set_spi_flash_size(dev, 2, flash_size);

	/* Reserve space for I2C */
	vortex86_sb_set_io_resv(dev, 3, CONFIG_I2C_BASE, 8);
}

static void southbridge_init_func1(struct device *dev)
{
	/* Handle S/B function 1 PCI IRQ routing. (SPI1/MOTOR) */
	u32 ext_int_routing2 = 0;
	/* Setup S/B function 1 PCI Extend Interrupt routing table reg 2(0xb4). */
	ext_int_routing2 |= irq_to_int_routing[SPI1_IRQ] << SPI1_IRQ_SHIFT;
	ext_int_routing2 |= irq_to_int_routing[MOTOR_IRQ] << MOTOR_IRQ_SHIFT;
	pci_write_config32(dev, SB1_REG_EXT_PIRQ_ROUTE2, ext_int_routing2);

	/* Assign in-chip PCI device IRQs. */
	if ((SPI1_IRQ != 0) || (MOTOR_IRQ != 0)) {
		unsigned char irqs[4] = { MOTOR_IRQ, SPI1_IRQ, 0, 0 };
		pci_assign_irqs(0, 0x10, irqs);
	}
}

static void southbridge_init(struct device *dev)
{
	/* Check it is function 0 or 1. (Same Vendor/Device ID) */
	if (get_pci_dev_func(dev) != 0) {
		southbridge_init_func1(dev);
		return;
	}
	upload_dmp_keyboard_firmware(dev);
	vortex_sb_init(dev);
	if (dev->device == 0x6011) {
		ex_sb_gpio_init(dev);
		ex_sb_uart_init(dev);
		i2c_init(dev);
	}
	pci_routing_fixup(dev);

	fix_cmos_rtc_time();
	cmos_init(0);
	/* Check keyboard controller ready. If timeout, reload firmware code
	 * and try again.
	 */
	u32 retries = 10;
	while (!kbc_wait_system_flag()) {
		if (!retries) {
			post_code(POST_DMP_KBD_IS_BAD);
			die("The keyboard timeout occurred too often. "
			    "Your CPU is probably defect. "
			    "Contact your dealer to replace it\n");
		}
		upload_dmp_keyboard_firmware(dev);
		retries--;
	}
	post_code(POST_DMP_KBD_IS_READY);
	pc_keyboard_init();
}

static struct device_operations vortex_sb_ops = {
	.read_resources   = vortex86_sb_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = &southbridge_init,
	.scan_bus         = scan_lpc_bus,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver pci_driver_6011 __pci_driver = {
	.ops    = &vortex_sb_ops,
	.vendor = PCI_VENDOR_ID_RDC,
	.device = 0x6011,	/* EX CPU S/B ID */
};

struct chip_operations southbridge_dmp_vortex86ex_ops = {
	CHIP_NAME("DMP Vortex86EX Southbridge")
	.enable_dev = 0
};
