/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <stdint.h>
#include <commonlib/helpers.h>
#include "aspeed.h"

void lpc_read(uint8_t port, uint32_t addr, uint32_t *value)
{
	uint32_t data = 0;
	uint8_t tmp;
	pnp_devfn_t dev = PNP_DEV(port, LDN_ILPC2AHB);

	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);

	/* Write Address */
	pnp_write_config(dev, LPC2AHB_ADD0_REG, ((addr & 0xff000000) >> 24));
	pnp_write_config(dev, LPC2AHB_ADD1_REG, ((addr & 0x00ff0000) >> 16));
	pnp_write_config(dev, LPC2AHB_ADD2_REG, ((addr & 0x0000ff00) >> 8));
	pnp_write_config(dev, LPC2AHB_ADD3_REG, (addr & 0x000000ff));

	/* Write Mode */
	tmp = pnp_read_config(dev, LPC2AHB_LEN_REG);
	pnp_write_config(dev, LPC2AHB_LEN_REG, (tmp & 0xfc) | LPC2AHB_4_BYTE);

	/* Fire the command */
	outb(LPC2AHB_RW_REG, port);
	tmp = inb(port + 1);

	/* Get Data */
	data |= (pnp_read_config(dev, LPC2AHB_DAT0_REG) << 24) |
		(pnp_read_config(dev, LPC2AHB_DAT1_REG) << 16) |
		(pnp_read_config(dev, LPC2AHB_DAT2_REG) << 8) |
		pnp_read_config(dev, LPC2AHB_DAT3_REG);
	*value = data;

	pnp_set_enable(dev, 0);
	pnp_exit_conf_state(dev);
}

void lpc_write(uint8_t port, uint32_t addr, uint32_t data)
{
	uint8_t tmp;
	pnp_devfn_t dev = PNP_DEV(port, LDN_ILPC2AHB);

	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);

	/* Write Address */
	pnp_write_config(dev, LPC2AHB_ADD0_REG, ((addr & 0xFF000000) >> 24));
	pnp_write_config(dev, LPC2AHB_ADD1_REG, ((addr & 0x00FF0000) >> 16));
	pnp_write_config(dev, LPC2AHB_ADD2_REG, ((addr & 0x0000FF00) >> 8));
	pnp_write_config(dev, LPC2AHB_ADD3_REG, (addr & 0x000000FF));

	/* Write Data */
	pnp_write_config(dev, LPC2AHB_DAT0_REG, ((data & 0xFF000000) >> 24));
	pnp_write_config(dev, LPC2AHB_DAT1_REG, ((data & 0x00FF0000) >> 16));
	pnp_write_config(dev, LPC2AHB_DAT2_REG, ((data & 0x0000FF00) >> 8));
	pnp_write_config(dev, LPC2AHB_DAT3_REG, (data & 0x000000FF));

	/* Write Mode */
	tmp = pnp_read_config(dev, LPC2AHB_LEN_REG);
	pnp_write_config(dev, LPC2AHB_LEN_REG, (tmp & 0xfc) | LPC2AHB_4_BYTE);

	/* Fire */
	pnp_write_config(dev, LPC2AHB_RW_REG, 0xcf);

	pnp_set_enable(dev, 0);
	pnp_exit_conf_state(dev);
}

void aspeed_early_config(pnp_devfn_t dev, config_data *table, uint8_t count)
{
	uint8_t i, t, port;
	uint32_t v, addr;
	port = dev >> 8;
	for (i = 0; i < count; i++) {
		if (table[i].type == MEM) {
			addr = (u32)(table[i].base | table[i].reg);
			lpc_read(port, addr, &v);
			v &= table[i].and;
			v |= table[i].or;
			lpc_write(port, addr, v);
		} else if (table[i].type == SIO) {
			pnp_enter_conf_state(dev);
			pnp_set_logical_device(dev);
			t = pnp_read_config(dev, ACT_REG) | ACTIVATE_VALUE;
			pnp_write_config(dev, ACT_REG, t);
			t = pnp_read_config(dev, (uint8_t)(table[i].reg));
			t &= (uint8_t)(table[i].and);
			t |= (uint8_t)(table[i].or);
			pnp_write_config(dev, (uint8_t)(table[i].reg), t);
			pnp_set_logical_device(dev);
			t = pnp_read_config(dev, ACT_REG) & ~ACTIVATE_VALUE;
			pnp_write_config(dev, ACT_REG, t);
			pnp_exit_conf_state(dev);
		}
	}
}

void aspeed_enable_port80_direct_gpio(pnp_devfn_t dev, gpio_group_sel g)
{
	struct config_data port80[] = {
		/* Set command source 0 */
		[Step1] = {
			.type = MEM,
			.base = ASPEED_GPIO_BASE,
			.reg = TO_BE_UPDATE,
			.and = AndMask32((g % 4) * 8, (g % 4) * 8),
			.or = (LPC & 0x01) << ((g % 4) * 8)
		},
		/* Set command source 1 */
		[Step2] = {
			.type = MEM,
			.base = ASPEED_GPIO_BASE,
			.reg = TO_BE_UPDATE,
			.and = AndMask32((g % 4) * 8, (g % 4) * 8),
			.or = (LPC & 0x02) << ((g % 4) * 8)
		},
		/* Unlock SCU registers */
		[Step3] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = PRO_KEY_REG,
			.and = 0,
			.or = PRO_KEY_PASSWORD
		},
		/* Program multi-function to GPIO */
		[Step4] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = TO_BE_UPDATE,
			.and = TO_BE_UPDATE,
			.or = TO_BE_UPDATE
		},
		[Step5] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = TO_BE_UPDATE,
			.and = TO_BE_UPDATE,
			.or = TO_BE_UPDATE
		},
		[Step6] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = TO_BE_UPDATE,
			.and = TO_BE_UPDATE,
			.or = TO_BE_UPDATE
		},
		[Step7] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = TO_BE_UPDATE,
			.and = TO_BE_UPDATE,
			.or = TO_BE_UPDATE
		},
		/* Program GPIO as output */
		[Step8] = {
			.type = MEM,
			.base = ASPEED_GPIO_BASE,
			.reg = TO_BE_UPDATE,
			.and = AndMask32((((g % 4) + 1) * 8) - 1, (g % 4) * 8),
			.or = 0xFF << ((g % 4) * 8)
		},
		/* Set snooping address#0 as 80h */
		[Step9] = {
			.type = MEM,
			.base = ASPEED_LPC_BASE,
			.reg = SNPWADR_REG,
			.and = AndMask32(15, 0),
			.or = SNOOP_ADDR_PORT80
		},
		/* Enable snooping address#0 */
		[Step10] = {
			.type = MEM,
			.base = ASPEED_LPC_BASE,
			.reg = HICR5_REG,
			.and = AndMask32(0, 0),
			.or = 1 << SNOOP_ADDR_EN
		},
		/* Lock SCU registers */
		[Step11] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = PRO_KEY_REG,
			.and = 0,
			.or = 0
		},
		/* Select group for port80 GPIO */
		[Step12] = {
			.type = SIO,
			.base = 0,
			.reg = PORT80_GPIO_SEL_REG,
			.and = AndMask32(4, 0),
			.or = g
		},
		/* Enable port80 GPIO */
		[Step13] = {
			.type = SIO,
			.base = 0,
			.reg = ACT_REG,
			.and = AndMask32(8, 8),
			.or = PORT80_GPIO_EN
		},
	};

	switch (g) {
	case GPIOA:
	case GPIOB:
	case GPIOC:
	case GPIOD:
		port80[Step1].reg = A_B_C_D_CMD_SOURCE0_REG;
		port80[Step2].reg = A_B_C_D_CMD_SOURCE1_REG;
		port80[Step8].reg = A_B_C_D_DIRECTION_REG;
		break;
	case GPIOE:
	case GPIOF:
	case GPIOG:
	case GPIOH:
		port80[Step1].reg = E_F_G_H_CMD_SOURCE0_REG;
		port80[Step2].reg = E_F_G_H_CMD_SOURCE1_REG;
		port80[Step8].reg = E_F_G_H_DIRECTION_REG;
		break;
	case GPIOI:
	case GPIOJ:
	case GPIOK:
	case GPIOL:
		port80[Step1].reg = I_J_K_L_CMD_SOURCE0_REG;
		port80[Step2].reg = I_J_K_L_CMD_SOURCE1_REG;
		port80[Step8].reg = I_J_K_L_DIRECTION_REG;
		break;
	case GPIOM:
	case GPION:
	case GPIOO:
	case GPIOP:
		port80[Step1].reg = M_N_O_P_CMD_SOURCE0_REG;
		port80[Step2].reg = M_N_O_P_CMD_SOURCE1_REG;
		port80[Step8].reg = M_N_O_P_DIRECTION_REG;
		break;
	case GPIOQ:
	case GPIOR:
	case GPIOS:
	case GPIOT:
		port80[Step1].reg = Q_R_S_T_CMD_SOURCE0_REG;
		port80[Step2].reg = Q_R_S_T_CMD_SOURCE1_REG;
		port80[Step8].reg = Q_R_S_T_DIRECTION_REG;
		break;
	case GPIOU:
	case GPIOV:
	case GPIOW:
	case GPIOX:
		port80[Step1].reg = U_V_W_X_CMD_SOURCE0_REG;
		port80[Step2].reg = U_V_W_X_CMD_SOURCE1_REG;
		port80[Step8].reg = U_V_W_X_DIRECTION_REG;
		break;
	case GPIOY:
	case GPIOZ:
	case GPIOAA:
	case GPIOAB:
		port80[Step1].reg = Y_Z_AA_AB_CMD_SOURCE0_REG;
		port80[Step2].reg = Y_Z_AA_AB_CMD_SOURCE1_REG;
		port80[Step8].reg = Y_Z_AA_AB_DIRECTION_REG;
		break;
	}

	switch (g) {
	case GPIOA:
		port80[Step4].reg = MUL_FUNC_PIN_CTL1_REG;
		port80[Step4].and = AndMask32(7, 0) & AndMask32(15, 15);
		port80[Step5].reg = MUL_FUNC_PIN_CTL4_REG;
		port80[Step5].and = AndMask32(6, 6) & AndMask32(2, 2) & AndMask32(22, 22);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOB:
		port80[Step4].reg = HW_STRAP_REG;
		port80[Step4].and = AndMask32(23, 23);
		port80[Step5].reg = MUL_FUNC_PIN_CTL1_REG;
		port80[Step5].and = AndMask32(14, 13);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOC:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(0, 0);
		port80[Step5].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step5].and = AndMask32(26, 23);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOD:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(1, 1);
		port80[Step5].reg = MUL_FUNC_PIN_CTL4_REG;
		port80[Step5].and = AndMask32(11, 8);
		port80[Step6].reg = HW_STRAP_REG;
		port80[Step6].and = AndMask32(21, 21);
		port80[Step7].type = NOP;
		break;
	case GPIOE:
		port80[Step4].reg = MUL_FUNC_PIN_CTL1_REG;
		port80[Step4].and = AndMask32(23, 16);
		port80[Step5].reg = MUL_FUNC_PIN_CTL4_REG;
		port80[Step5].and = AndMask32(15, 12);
		port80[Step6].reg = HW_STRAP_REG;
		port80[Step6].and = AndMask32(22, 22);
		port80[Step7].type = NOP;
		break;
	case GPIOF:
		port80[Step4].base = ASPEED_LPC_BASE;
		port80[Step4].reg = LHCR0_REG;
		port80[Step4].and = AndMask32(0, 0);
		port80[Step5].reg = MUL_FUNC_PIN_CTL1_REG;
		port80[Step5].and = AndMask32(31, 24);
		port80[Step6].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step6].and = AndMask32(30, 30);
		port80[Step7].type = NOP;
		break;
	case GPIOG:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(6, 6);
		port80[Step5].reg = MUL_FUNC_PIN_CTL2_REG;
		port80[Step5].and = AndMask32(7, 0);
		port80[Step6].reg = MUL_FUNC_PIN_CTL6_REG;
		port80[Step6].and = AndMask32(12, 12);
		port80[Step7].type = NOP;
		break;
	case GPIOH:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(7, 6);
		port80[Step5].reg = FRQ_CNT_CTL_REG;
		port80[Step5].and = AndMask32(8, 8);
		port80[Step6].reg = MUL_FUNC_PIN_CTL6_REG;
		port80[Step6].and = AndMask32(7, 5);
		port80[Step7].type = NOP;
		break;
	case GPIOI:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(6, 6);
		port80[Step5].reg = HW_STRAP_REG;
		port80[Step5].and = AndMask32(13, 12) & AndMask32(5, 5);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOJ:
		port80[Step4].reg = FRQ_CNT_CTL_REG;
		port80[Step4].and = AndMask32(8, 8);
		port80[Step5].reg = MUL_FUNC_PIN_CTL2_REG;
		port80[Step5].and = AndMask32(15, 8);
		port80[Step6].reg = MUL_FUNC_PIN_CTL6_REG;
		port80[Step6].and = AndMask32(8, 8);
		port80[Step7].type = NOP;
		break;
	case GPIOK:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(21, 18);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOL:
		port80[Step4].reg = MUL_FUNC_PIN_CTL2_REG;
		port80[Step4].and = AndMask32(23, 16);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOM:
		port80[Step4].reg = MUL_FUNC_PIN_CTL2_REG;
		port80[Step4].and = AndMask32(31, 24);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPION:
		port80[Step4].reg = MUL_FUNC_PIN_CTL3_REG;
		port80[Step4].and = AndMask32(7, 0);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOO:
		port80[Step4].reg = MUL_FUNC_PIN_CTL3_REG;
		port80[Step4].and = AndMask32(15, 8);
		port80[Step5].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step5].and = AndMask32(5, 5) & AndMask32(5, 4);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOP:
		port80[Step4].reg = MUL_FUNC_PIN_CTL3_REG;
		port80[Step4].and = AndMask32(23, 16);
		port80[Step5].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step5].and = AndMask32(5, 5) & AndMask32(5, 4) & AndMask32(28, 28);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOQ:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(17, 16) & AndMask32(27, 27);
		port80[Step5].reg = MISC_CTL_REG;
		port80[Step5].and = AndMask32(1, 1) & AndMask32(29, 29);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOR:
		port80[Step4].reg = MUL_FUNC_PIN_CTL3_REG;
		port80[Step4].and = AndMask32(31, 24);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOS:
		port80[Step4].reg = MUL_FUNC_PIN_CTL4_REG;
		port80[Step4].and = AndMask32(7, 0);
		port80[Step5].reg = MUL_FUNC_PIN_CTL6_REG;
		port80[Step5].and = AndMask32(1, 0);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOT:
		port80[Step4].reg = MUL_FUNC_PIN_CTL7_REG;
		port80[Step4].and = AndMask32(7, 0);
		port80[Step4].or = ~AndMask32(7, 0);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOU:
		port80[Step4].reg = MUL_FUNC_PIN_CTL7_REG;
		port80[Step4].and = AndMask32(15, 8);
		port80[Step4].or = ~AndMask32(15, 8);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOV:
		port80[Step4].reg = MUL_FUNC_PIN_CTL7_REG;
		port80[Step4].and = AndMask32(23, 16);
		port80[Step4].or = ~AndMask32(23, 16);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOW:
		port80[Step4].reg = MUL_FUNC_PIN_CTL7_REG;
		port80[Step4].and = AndMask32(31, 24);
		port80[Step4].or = ~AndMask32(31, 24);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOX:
		port80[Step4].reg = MUL_FUNC_PIN_CTL8_REG;
		port80[Step4].and = AndMask32(7, 0);
		port80[Step4].or = ~AndMask32(7, 0);
		port80[Step5].type = NOP;
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOY:
		port80[Step4].reg = HW_STRAP_REG;
		port80[Step4].and = AndMask32(19, 19);
		port80[Step5].reg = MUL_FUNC_PIN_CTL8_REG;
		port80[Step5].and = AndMask32(15, 8);
		port80[Step6].reg = MUL_FUNC_PIN_CTL6_REG;
		port80[Step6].and = AndMask32(11, 10);
		port80[Step7].type = NOP;
		break;
	case GPIOZ:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(31, 31);
		port80[Step5].reg = MUL_FUNC_PIN_CTL8_REG;
		port80[Step5].and = AndMask32(23, 16);
		port80[Step6].reg = MUL_FUNC_PIN_CTL6_REG;
		port80[Step6].and = AndMask32(1, 0);
		port80[Step6].reg = HW_STRAP_REG;
		port80[Step6].and = AndMask32(19, 19);
		break;
	case GPIOAA:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(31, 31);
		port80[Step5].reg = MUL_FUNC_PIN_CTL8_REG;
		port80[Step5].and = AndMask32(31, 24);
		port80[Step6].type = NOP;
		port80[Step7].type = NOP;
		break;
	case GPIOAB:
		port80[Step4].reg = MUL_FUNC_PIN_CTL5_REG;
		port80[Step4].and = AndMask32(31, 31);
		port80[Step5].reg = MUL_FUNC_PIN_CTL9_REG;
		port80[Step5].and = AndMask32(3, 0);
		port80[Step6].reg = MUL_FUNC_PIN_CTL6_REG;
		port80[Step6].and = AndMask32(1, 0);
		port80[Step7].type = NOP;
		break;
	default:
		return;
	}

	aspeed_early_config(dev, port80, ARRAY_SIZE(port80));
}

void aspeed_enable_uart_pin(pnp_devfn_t dev)
{
	struct config_data uart[] = {
		/* Unlock SCU registers */
		[Step1] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = PRO_KEY_REG,
			.and = 0,
			.or = PRO_KEY_PASSWORD
		},
		/* Enable UART function pin */
		[Step2] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = TO_BE_UPDATE,
			.and = TO_BE_UPDATE,
			.or = TO_BE_UPDATE
		},
		[Step3] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = MUL_FUNC_PIN_CTL6_REG,
			.and = AndMask32(1, 0),
			.or = DIGI_VIDEO_OUT_PINS_DIS
		},
		/* Lock SCU registers */
		[Step4] = {
			.type = MEM,
			.base = ASPEED_SCU_BASE,
			.reg = PRO_KEY_REG,
			.and = 0,
			.or = 0
		},
	};

	switch (dev & 0xff) {
	case LDN_SUART1:
		uart[Step2].reg = MUL_FUNC_PIN_CTL2_REG;
		uart[Step2].and = AndMask32(23, 22);
		uart[Step2].or = (1 << UART1_TXD1_EN_BIT) | (1 << UART1_RXD1_EN_BIT);
		break;
	case LDN_SUART2:
		uart[Step2].reg = MUL_FUNC_PIN_CTL2_REG;
		uart[Step2].and = AndMask32(31, 30);
		uart[Step2].or = (1 << UART2_TXD2_EN_BIT) | (1 << UART2_RXD2_EN_BIT);
		break;
	case LDN_SUART3:
		uart[Step2].reg = MUL_FUNC_PIN_CTL1_REG;
		uart[Step2].and = AndMask32(23, 22);
		uart[Step2].or = (1 << UART3_TXD3_EN_BIT) | (1 << UART3_RXD3_EN_BIT);
		uart[Step3].type = NOP;
		break;
	case LDN_SUART4:
		uart[Step2].reg = MUL_FUNC_PIN_CTL1_REG;
		uart[Step2].and = AndMask32(31, 30);
		uart[Step2].or = (1 << UART4_TXD4_EN_BIT) | (1 << UART4_RXD4_EN_BIT);
		uart[Step3].type = NOP;
		break;
	default:
		return;
	}

	aspeed_early_config(dev, uart, ARRAY_SIZE(uart));
}
