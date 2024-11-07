/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pnp.h>
#include <mainboard/superio.h>
#include <superio/hwm5_conf.h>
#include <superio/fintek/common/fintek.h>

/* TODO: move this code to superio/fintek driver */
#define SIO_PORT		0x2e
#define SIO_HWM_BASE		0x290
#define SIO_DEV(n)		PNP_DEV(SIO_PORT, SIO_LDN_##n)

/* Fintek F81966 Logical Device Numbers (LDN) */
#define SIO_LDN_GLOBAL			0x00
#define SIO_LDN_LPT			0x03
#define SIO_LDN_HWMON			0x04
#define SIO_LDN_KBC			0x05
#define SIO_LDN_GPIO			0x06
#define SIO_LDN_WDT			0x07
#define SIO_LDN_PME_ACPI_ERP		0x0a
#define SIO_LDN_SPI			0x0f
#define SIO_LDN_UART1			0x10
#define SIO_LDN_UART2			0x11
#define SIO_LDN_UART3			0x12
#define SIO_LDN_UART4			0x13
#define SIO_LDN_UART5			0x14
#define SIO_LDN_UART6			0x15

#define SIO_REG(i, v)			{.idx = i, .val = v}
#define SIO_ESEL(i, v, m)		{.extra_selector = true, .idx = i, .val = v, .mask = m}
#define SIO_LDN(n)			{.idx = 0x07, .val = SIO_LDN_##n}
#define SIO_LDN_EN(v)			{.idx = 0x30, .val = v}
/* f81966 Port Select (27h): BANK_PROG_SEL[3-2] CLK_TUNE_PROG_EN[0] */
#define SIO_ESEL_27H(bank, en) {						\
			.extra_selector = true,					\
			.idx = 0x27,						\
			.mask = 0xf2,						\
			.val = esel_27h_##bank | esel_27h_clk_tune_##en,	\
		}
/* f81966 Fan Fault Time Register (9Fh): FAN_PROG_SEL[7] */
#define SIO_ESEL_9FH(bank) {					\
			.extra_selector = true,			\
			.idx = 0x9f,				\
			.mask = 0x7f,				\
			.val = esel_9fh_fan_prog_##bank,	\
		}
/* PWM duty-cycle */
#define PWM_DC(x)		(((x << 8) - x) / 100)

enum esel_27h_bank_prog_sel {
	esel_27h_bank0 = 0 << 2,
	esel_27h_bank1 = 1 << 2,
	esel_27h_bank2 = 2 << 2,
	esel_27h_bank3 = 3 << 2,
};
enum esel_27h_clk_tune {
	esel_27h_clk_tune_dis = 0,
	esel_27h_clk_tune_en  = 1,
};

enum esel_9fh_fan_prog {
	esel_9fh_fan_prog_bank0 = 0 << 7,
	esel_9fh_fan_prog_bank1 = 1 << 7,
};

struct sio_reg {
	bool extra_selector;
	uint8_t idx;
	uint8_t val;
	uint8_t mask;
};

static const struct sio_reg ldn_reg_tbl[] = {
	SIO_LDN(GLOBAL),
	SIO_REG(0x27, 0x80), /* disable 0x80 port */
	SIO_REG(0x2d, 0x2e), /* enable KB/Mouse wakeup, 2.8/2.5V hysteresis */
	SIO_ESEL_27H(bank0, dis),
	SIO_REG(0x29, 0xf0), /* set UART fn pins, TTL level */
	SIO_REG(0x2a, 0x45), /* GPIO[10]->LED_VSB, GPIO[11]->LED_VCC */
	SIO_REG(0x2c, 0xe3), /* en GPIO 00,01 */
	SIO_ESEL_27H(bank2, dis),
	SIO_REG(0x2b, 0x03), /* select pin 54 as SCL */
	SIO_REG(0x2c, 0x01), /* GPIO[20]->ALERT# */
	SIO_ESEL_27H(bank3, dis),
	SIO_REG(0x28, 0x59), /* PLL_CNT = (256*48) / 89 = 138 */
	SIO_ESEL_27H(bank0, en),
	/*
	 * - GPIO[90]->LDRQ#, GPIO[91]->KBRST#, GPIO[92]->GA20,   GPIO[93]->MDATA,
	 * - GPIO[94]->MCLK,  GPIO[95]->FANIN1, GPIO[96]->FANIN2, GPIO[97]->SLCT
	 */
	SIO_REG(0x2c, 0x00),
	SIO_ESEL_27H(bank0, dis),

	SIO_LDN(GPIO),
	SIO_REG(0x60, 0x02),
	SIO_REG(0x61, 0x80), /* base address */
	SIO_REG(0xe0, 0x10), /* GPIO[14]->output mode */
	SIO_REG(0xe3, 0x10), /* GPIO[14]->is push pull in output mode */
	SIO_REG(0xe6, 0x10), /* GPIO[14]->SMI event will set if input is changed */
	SIO_REG(0x80, 0xe0), /* GPIO[75,76,77]->is in output mode */
	SIO_REG(0x81, 0xe0), /* GPIO[75,76,77]=1 */
	SIO_REG(0x88, 0x01), /* GPIO[80]->is in output mode */
	SIO_REG(0x89, 0x01), /* GPIO[80]=1 */
	SIO_REG(0x8e, 0xff), /* GPIO[80,81,82,83,84,85,86,87]->use SMI event */
	SIO_LDN_EN(0x01),    /* enable GPIO I/O ports */

	SIO_LDN(HWMON),
	SIO_REG(0x60, SIO_HWM_BASE >> 8),
	SIO_REG(0x61, SIO_HWM_BASE & 0xff),
	SIO_LDN_EN(0x01),    /* enable hardware monitor */

	SIO_LDN(UART1),
	SIO_REG(0xf6, 0x23), /* 128-byte FIFO, FIFO threshold will be 4X of RXFTHR */
	SIO_LDN_EN(0x01),

	SIO_LDN(UART2),
	SIO_REG(0xf6, 0x23),
	SIO_LDN_EN(0x01),

	SIO_LDN(UART3),
	SIO_REG(0xf6, 0x23),
	SIO_LDN_EN(0x01),

	SIO_LDN(UART4),
	SIO_REG(0xf6, 0x23),
	SIO_LDN_EN(0x01),
};

static const struct sio_reg hwm_reg_tbl[] = {
	SIO_REG(0x07, 0x4a), /* MXM address */
	SIO_REG(0x0a, 0x01), /* enable PECI access */
	SIO_REG(0x0c, 0x64), /* TCC Temperature : CPU_TEMP = TCC_TEMP + PECI Reading */
	SIO_REG(0x0f, 0x20), /* digital rate selector (Reserved for Fintek use only) */
	SIO_REG(0x6b, 0x00), /* TEMP[1,2] is connected to a thermistor */
	SIO_ESEL_9FH(bank0),
	/*
	 * - FAN[1]->open drain, output PWM mode to control Intel 4-wire fans;
	 * - FAN[2]->push pull, output PWM mode to control fans;
	 * - FAN[3]->use linear fan application circuit to control speed by power terminal
	 */
	SIO_REG(0x94, 0x12),
	SIO_REG(0x9b, 0x1f), /* FAN[1,2]->duty update rate 20Hz */
	SIO_ESEL_9FH(bank1),
	SIO_REG(0x9b, 0x55), /* direct load enable for manual duty mode */
	SIO_ESEL_9FH(bank0),

	SIO_REG(0xa3, 0x75), /* FAN[1] expect PWM duty */
	SIO_REG(0xa6, 70), /* VT[1] boundary 1 temperature */
	SIO_REG(0xa7, 65), /* VT[1] boundary 2 temperature */
	SIO_REG(0xa8, 55), /* VT[1] boundary 3 temperature */
	SIO_REG(0xa9, 45), /* VT[1] boundary 4 temperature */
	SIO_REG(0xaa, PWM_DC(100)), /* FAN[1] segment 1 speed count */
	SIO_REG(0xab, PWM_DC(85)), /* FAN[1] segment 2 speed count */
	SIO_REG(0xac, PWM_DC(70)), /* FAN[1] segment 3 speed count */
	SIO_REG(0xad, PWM_DC(60)), /* FAN[1] segment 4 speed count */
	SIO_REG(0xae, PWM_DC(50)), /* FAN[1] segment 5 speed count */
	/*
	 * - FAN[1] follows PECI temperature;
	 * - FAN[1] duty will directly jump to the value of FAN1_SEG2;
	 * - FAN[1] duty will directly jump to the value of FAN1_SEG1;
	 * - enable the interpolation of the fan expect table;
	 * - [0,0,0]: 23.5 KHz;
	 */
	SIO_REG(0xaf, 0x1c),
	SIO_REG(0xb3, 0x75), /* FAN[2] expect PWM duty */
	SIO_REG(0xb6, 70), /* VT[2] boundary 1 temperature */
	SIO_REG(0xb7, 65), /* VT[2] boundary 2 temperature */
	SIO_REG(0xb8, 55), /* VT[2] boundary 3 temperature */
	SIO_REG(0xb9, 45), /* VT[2] boundary 4 temperature */
	SIO_REG(0xba, PWM_DC(100)), /* FAN[2] segment 1 speed count 0xff */
	SIO_REG(0xbb, PWM_DC(85)), /* FAN[2] segment 2 speed count 0xd9 */
	SIO_REG(0xbc, PWM_DC(70)), /* FAN[2] segment 3 speed count 0xb2 */
	SIO_REG(0xbd, PWM_DC(60)), /* FAN[2] segment 4 speed count 0x99 */
	SIO_REG(0xbe, PWM_DC(50)), /* FAN[2] segment 5 speed count 0x75 */
	/*
	 * - FAN[2] follows PECI temperature;
	 * - FAN[2] duty will directly jump to the value of FAN2_SEG2;
	 * - FAN[2] duty will directly jump to the value of FAN2_SEG1;
	 * - enable the interpolation of the fan expect table;
	 * - [0,0,0]: 23.5 KHz;
	 */
	SIO_REG(0xbf, 0x1c),
};

static inline u8 sio_read(bool is_hwm, uint8_t idx)
{
	return is_hwm ? pnp_read_hwm5_index(SIO_HWM_BASE, idx) : pnp_read_index(SIO_PORT, idx);
}

static inline void sio_write(bool is_hwm, uint8_t idx, uint8_t val)
{
	is_hwm ? pnp_write_hwm5_index(SIO_HWM_BASE, idx, val) : pnp_write_index(SIO_PORT, idx, val);
}

static void sio_regs_setup(const struct sio_reg reg[], int size, bool is_hwm)
{
	for (int i = 0; i < size; i++) {
		uint8_t val = reg[i].val;
		if (reg[i].extra_selector) {
			val = sio_read(is_hwm, reg[i].idx);
			val &= reg[i].mask;
			val |= reg[i].val;
		}
		sio_write(is_hwm, reg[i].idx, val);
	}
}

void mainboard_superio_init(void)
{
	pnp_enter_conf_state(SIO_DEV(GLOBAL));
	sio_regs_setup(ldn_reg_tbl, sizeof(ldn_reg_tbl)/sizeof(struct sio_reg), false);
	pnp_exit_conf_state(SIO_DEV(GLOBAL));

	sio_regs_setup(hwm_reg_tbl, sizeof(hwm_reg_tbl)/sizeof(struct sio_reg), true);
}
