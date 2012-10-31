/*
 * Copyright (C) 2011 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <config.h>
#include <spi.h>
#include <asm/arch/board.h>
#include <asm/arch/clock.h>
#include <asm/arch-exynos/spi.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/power.h>

DECLARE_GLOBAL_DATA_PTR;

#define OM_STAT		(0x1f << 1)

/**
 * Copy data from SD or MMC device to RAM.
 *
 * @param offset	Block offset of the data
 * @param nblock	Number of blocks
 * @param dst		Destination address
 * @return 1 = True or 0 = False
 */
typedef u32 (*mmc_copy_func_t)(u32 offset, u32 nblock, u32 dst);

/**
 * Copy data from SPI flash to RAM.
 *
 * @param offset	Block offset of the data
 * @param nblock	Number of blocks
 * @param dst		Destination address
 * @return 1 = True or 0 = False
 */
typedef u32 (*spi_copy_func_t)(u32 offset, u32 nblock, u32 dst);


/**
 * Copy data through USB.
 *
 * @return 1 = True or 0 = False
 */
typedef u32 (*usb_copy_func_t)(void);

/*
 * Set/clear program flow prediction and return the previous state.
 */
static int config_branch_prediction(int set_cr_z)
{
	unsigned int cr;

	/* System Control Register: 11th bit Z Branch prediction enable */
	cr = get_cr();
	set_cr(set_cr_z ? cr | CR_Z : cr & ~CR_Z);

	return cr & CR_Z;
}

static void spi_rx_tx(struct exynos_spi *regs, int todo,
			void *dinp, void const *doutp, int i)
{
	uint *rxp = (uint *)(dinp + (i * (32 * 1024)));
	int rx_lvl, tx_lvl;
	uint out_bytes, in_bytes;

	out_bytes = in_bytes = todo;
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	writel(((todo * 8) / 32) | SPI_PACKET_CNT_EN, &regs->pkt_cnt);

	while (in_bytes) {
		uint32_t spi_sts;
		int temp;

		spi_sts = readl(&regs->spi_sts);
		rx_lvl = ((spi_sts >> 15) & 0x7f);
		tx_lvl = ((spi_sts >> 6) & 0x7f);
		while (tx_lvl < 32 && out_bytes) {
			temp = 0xffffffff;
			writel(temp, &regs->tx_data);
			out_bytes -= 4;
			tx_lvl += 4;
		}
		while (rx_lvl >= 4 && in_bytes) {
			temp = readl(&regs->rx_data);
			if (rxp)
				*rxp++ = temp;
			in_bytes -= 4;
			rx_lvl -= 4;
		}
	}
}

/**
 * Copy uboot from spi flash to RAM
 *
 * @parma uboot_size	size of u-boot to copy
 */
static void exynos_spi_copy(unsigned int uboot_size)
{
	int upto, todo;
	int i;
	struct exynos_spi *regs = (struct exynos_spi *)samsung_get_base_spi1();

	clock_set_rate(PERIPH_ID_SPI1, 50000000); /* set spi clock to 50Mhz */
	/* set the spi1 GPIO */
	exynos_pinmux_config(PERIPH_ID_SPI1, PINMUX_FLAG_NONE);

	/* set pktcnt and enable it */
	writel(4 | SPI_PACKET_CNT_EN, &regs->pkt_cnt);
	/* set FB_CLK_SEL */
	writel(SPI_FB_DELAY_180, &regs->fb_clk);
	/* set CH_WIDTH and BUS_WIDTH as word */
	setbits_le32(&regs->mode_cfg, SPI_MODE_CH_WIDTH_WORD |
					SPI_MODE_BUS_WIDTH_WORD);
	clrbits_le32(&regs->ch_cfg, SPI_CH_CPOL_L); /* CPOL: active high */

	/* clear rx and tx channel if set priveously */
	clrbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);

	setbits_le32(&regs->swap_cfg, SPI_RX_SWAP_EN |
		SPI_RX_BYTE_SWAP |
		SPI_RX_HWORD_SWAP);

	/* do a soft reset */
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);

	/* now set rx and tx channel ON */
	setbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON | SPI_CH_HS_EN);
	clrbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT); /* CS low */

	/* Send read instruction (0x3h) followed by a 24 bit addr */
	writel((SF_READ_DATA_CMD << 24) | SPI_FLASH_UBOOT_POS, &regs->tx_data);

	/* waiting for TX done */
	while (!(readl(&regs->spi_sts) & SPI_ST_TX_DONE));

	for (upto = 0, i = 0; upto < uboot_size; upto += todo, i++) {
		todo = min(uboot_size - upto, (1 << 15));
		spi_rx_tx(regs, todo, (void *)(CONFIG_SYS_TEXT_BASE),
					(void *)(SPI_FLASH_UBOOT_POS), i);
	}

	setbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);/* make the CS high */

	/*
	 * Let put controller mode to BYTE as
	 * SPI driver does not support WORD mode yet
	 */
	clrbits_le32(&regs->mode_cfg, SPI_MODE_CH_WIDTH_WORD |
					SPI_MODE_BUS_WIDTH_WORD);
	writel(0, &regs->swap_cfg);

	/*
	 * Flush spi tx, rx fifos and reset the SPI controller
	 * and clear rx/tx channel
	 */
	clrsetbits_le32(&regs->ch_cfg, SPI_CH_HS_EN, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_TX_CH_ON | SPI_RX_CH_ON);
}

/* Copy U-Boot image to RAM */
static void copy_uboot_to_ram(void)
{
	unsigned int sec_boot_check;
	unsigned int uboot_size;
	int is_cr_z_set;
	enum boot_mode boot_mode;
	mmc_copy_func_t mmc_copy;

	usb_copy_func_t usb_copy;

	uboot_size = exynos_get_uboot_size();
	boot_mode = exynos_get_boot_device();

	if (boot_mode == BOOT_MODE_OM) {
		/* Read iRAM location to check for secondary USB boot mode */
		sec_boot_check = readl(EXYNOS_IRAM_SECONDARY_BASE);
		if (sec_boot_check == EXYNOS_USB_SECONDARY_BOOT)
			boot_mode = BOOT_MODE_USB;
	}
	debug("U-Boot size %u\n", uboot_size);

	if (boot_mode == BOOT_MODE_OM)
		boot_mode = readl(EXYNOS_POWER_BASE) & OM_STAT;

	switch (boot_mode) {
#if defined(CONFIG_EXYNOS_SPI_BOOT)
	case BOOT_MODE_SERIAL:
		/* let us our own function to copy u-boot from SF */
		exynos_spi_copy(uboot_size);
		break;
#endif
	case BOOT_MODE_MMC:
		mmc_copy = *(mmc_copy_func_t *)EXYNOS_COPY_MMC_FNPTR_ADDR;
		assert(!(uboot_size & 511));
		mmc_copy(BL2_START_OFFSET, uboot_size / 512,
				CONFIG_SYS_TEXT_BASE);
		break;
	case BOOT_MODE_USB:
		/*
		 * iROM needs program flow prediction to be disabled
		 * before copy from USB device to RAM
		 */
		is_cr_z_set = config_branch_prediction(0);
		usb_copy = *(usb_copy_func_t *)
				EXYNOS_COPY_USB_FNPTR_ADDR;
		usb_copy();
		config_branch_prediction(is_cr_z_set);
		break;
	default:
		panic("Invalid boot mode selection\n");
		break;
	}
	debug("U-Boot copied\n");
}

/* The memzero function is not in SPL u-boot, so create one. */
void memzero(void *s, size_t n)
{
	char *ptr = s;
	size_t i;

	for (i = 0; i < n; i++)
		*ptr++ = '\0';
}

/**
 * Set up the U-Boot global_data pointer
 *
 * This sets the address of the global data, and sets up basic values.
 *
 * @param gdp	Value to give to gd
 */
static void setup_global_data(gd_t *gdp)
{
	gd = gdp;
	memzero((void *)gd, sizeof(gd_t));
	gd->flags |= GD_FLG_RELOC;
	gd->baudrate = CONFIG_BAUDRATE;
	gd->have_console = 1;
}

/* Tell the loaded U-Boot that it was loaded from SPL */
static void exynos5_set_spl_marker(void)
{
	uint32_t *marker = (uint32_t *)CONFIG_SPL_MARKER;

	*marker = EXYNOS5_SPL_MARKER;
}

/* Board-specific call to see if wakeup is allowed. */
static int __def_board_wakeup_permitted(void)
{
	return 1;
}
int board_wakeup_permitted(void)
	__attribute__((weak, alias("__def_board_wakeup_permitted")));

void board_init_f(unsigned long bootflag)
{
	/*
	 * The gd struct is only needed for serial initialization. Since this
	 * function is called in SPL u-boot. We store the gd struct in the
	 * stack instead of the default memory region which may not be
	 * initialized.
	 */
	__attribute__((aligned(8))) gd_t local_gd;
	__attribute__((noreturn)) void (*uboot)(void);

	exynos5_set_spl_marker();
	setup_global_data(&local_gd);

	/*
	 * Init subsystems, and resume if required. For a normal boot this
	 * will set up the UART and display a message.
	 */
	if (lowlevel_init_subsystems()) {
		if (!board_wakeup_permitted())
			power_reset();
		power_exit_wakeup();
	}

	printf("\n\nU-Boot SPL, board rev %u\n", board_get_revision());

	copy_uboot_to_ram();
	/* Jump to U-Boot image */
	uboot = (void *)CONFIG_SYS_TEXT_BASE;
	uboot();
	/* Never returns Here */
	panic("%s: u-boot jump failed", __func__);
}

/* Place Holders */
void board_init_r(gd_t *id, ulong dest_addr)
{
	/* Function attribute is no-return */
	/* This Function never executes */
	while (1)
		;
}

void save_boot_params(u32 r0, u32 r1, u32 r2, u32 r3) {}

/*
 * The following functions are required when linking console library to SPL.
 *
 * Enabling UART in SPL u-boot requires console library. But some
 * functions we needed in the console library depends on a bunch
 * of library in libgeneric, like lib/ctype.o, lib/div64.o, lib/string.o,
 * and lib/vsprintf.o. Adding them makes the SPL u-boot too large and not
 * fit into the expected size.
 *
 * So we mock these functions in SPL, i.e. vsprintf(), panic(), etc.,
 * in order to cut its dependency.
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *str = buf, *s;
	ulong u;

	/*
	 * We won't implement all full functions of vsprintf().
	 * We only implement %s and %u, and ignore others and directly use
	 * the original format string as its result.
	 */

	while (*fmt) {
		if (*fmt != '%') {
			*str++ = *fmt++;
			continue;
		}
		fmt++;
		switch (*fmt) {
		case '%':
			*str++ = *fmt++;
			break;
		case 's':
			fmt++;
			s = va_arg(args, char *);
			while (*s)
				*str++ = *s++;
			break;
		case 'u':
			fmt++;
			u = va_arg(args, ulong);
			s = simple_itoa(u);
			while (*s)
				*str++ = *s++;
			break;
		default:
			/* Print the original string for unsupported formats */
			*str++ = '%';
			*str++ = *fmt++;
		}
	}
	*str = '\0';
	return str - buf;
}

void panic(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	putc('\n');
	va_end(args);
#if defined(CONFIG_PANIC_HANG)
	hang();
#else
	udelay(100000);		/* allow messages to go out */
	do_reset(NULL, 0, 0, NULL);
#endif
	while (1)
		;
}

void __assert_fail(const char *assertion, const char *file, unsigned line,
		const char *function)
{
	/* This will not return */
	panic("%s:%u: %s: Assertion `%s' failed.", file, line, function,
			assertion);
}

char *simple_itoa(ulong i)
{
	/* 21 digits plus null terminator, good for 64-bit or smaller ints */
	static char local[22] __attribute__((section(".data")));
	char *p = &local[21];

	*p-- = '\0';
	do {
		*p-- = '0' + i % 10;
		i /= 10;
	} while (i > 0);
	return p + 1;
}
