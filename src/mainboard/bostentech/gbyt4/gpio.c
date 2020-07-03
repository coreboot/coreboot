/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>

/* NCORE GPIOs */
static const struct soc_gpio_map gpncore_gpio_map[] = {
	GPIO_INPUT,			/* GPIO_S0_NC[00] */
	GPIO_INPUT,			/* GPIO_S0_NC[01] */
	GPIO_INPUT,			/* GPIO_S0_NC[02] */
	GPIO_INPUT,			/* GPIO_S0_NC[03] */
	GPIO_INPUT,			/* GPIO_S0_NC[04] */
	GPIO_INPUT,			/* GPIO_S0_NC[05] */
	GPIO_INPUT,			/* GPIO_S0_NC[06] */
	GPIO_INPUT,			/* GPIO_S0_NC[07] */
	GPIO_INPUT,			/* GPIO_S0_NC[08] */
	GPIO_INPUT,			/* GPIO_S0_NC[09] */
	GPIO_INPUT,			/* GPIO_S0_NC[10] */
	GPIO_INPUT,			/* GPIO_S0_NC[11] */
	GPIO_INPUT,			/* GPIO_S0_NC[12] */
	GPIO_INPUT,			/* GPIO_S0_NC[13] */
	GPIO_INPUT,			/* GPIO_S0_NC[14] */
	GPIO_INPUT,			/* GPIO_S0_NC[15] */
	GPIO_INPUT,			/* GPIO_S0_NC[16] */
	GPIO_INPUT,			/* GPIO_S0_NC[17] */
	GPIO_INPUT,			/* GPIO_S0_NC[18] */
	GPIO_INPUT,			/* GPIO_S0_NC[19] */
	GPIO_INPUT,			/* GPIO_S0_NC[20] */
	GPIO_INPUT,			/* GPIO_S0_NC[21] */
	GPIO_INPUT,			/* GPIO_S0_NC[22] */
	GPIO_INPUT,			/* GPIO_S0_NC[23] */
	GPIO_INPUT,			/* GPIO_S0_NC[24] */
	GPIO_INPUT,			/* GPIO_S0_NC[25] */
	GPIO_INPUT,			/* GPIO_S0_NC[26] */
	GPIO_END
};

/* SCORE GPIOs */
static const struct soc_gpio_map gpscore_gpio_map[] = {
	GPIO_INPUT,			/* GPIO_S0_SC[000] */
	GPIO_INPUT,			/* GPIO_S0_SC[001] */
	GPIO_FUNC1,			/* SATA_LED# */
	GPIO_FUNC1,			/* PCIE_CLKREQ[0]# */
	GPIO_FUNC1,			/* PCIE_CLKREQ[1]# */
	GPIO_FUNC1,			/* PCIE_CLKREQ[2]# */
	GPIO_FUNC1,			/* PCIE_CLKREQ[3]# */
	GPIO_INPUT,			/* GPIO_S0_SC[007] */
	GPIO_INPUT,			/* GPIO_S0_SC[008] */
	GPIO_INPUT,			/* GPIO_S0_SC[009] */
	GPIO_INPUT,			/* GPIO_S0_SC[010] */
	GPIO_INPUT,			/* GPIO_S0_SC[011] */
	GPIO_INPUT,			/* GPIO_S0_SC[012] */
	GPIO_INPUT,			/* GPIO_S0_SC[013] */
	GPIO_INPUT,			/* GPIO_S0_SC[014] */
	GPIO_INPUT,			/* GPIO_S0_SC[015] */
	GPIO_INPUT,			/* GPIO_S0_SC[016] */
	GPIO_INPUT,			/* GPIO_S0_SC[017] */
	GPIO_INPUT,			/* GPIO_S0_SC[018] */
	GPIO_INPUT,			/* GPIO_S0_SC[019] */
	GPIO_INPUT,			/* GPIO_S0_SC[020] */
	GPIO_INPUT,			/* GPIO_S0_SC[021] */
	GPIO_INPUT,			/* GPIO_S0_SC[022] */
	GPIO_INPUT,			/* GPIO_S0_SC[023] */
	GPIO_INPUT,			/* GPIO_S0_SC[024] */
	GPIO_INPUT,			/* GPIO_S0_SC[025] */
	GPIO_INPUT,			/* GPIO_S0_SC[026] */
	GPIO_INPUT,			/* GPIO_S0_SC[027] */
	GPIO_INPUT,			/* GPIO_S0_SC[028] */
	GPIO_INPUT,			/* GPIO_S0_SC[029] */
	GPIO_INPUT,			/* GPIO_S0_SC[030] */
	GPIO_INPUT,			/* GPIO_S0_SC[031] */
	GPIO_INPUT,			/* GPIO_S0_SC[032] */
	GPIO_INPUT,			/* GPIO_S0_SC[033] */
	GPIO_INPUT,			/* GPIO_S0_SC[034] */
	GPIO_INPUT,			/* GPIO_S0_SC[035] */
	GPIO_INPUT,			/* GPIO_S0_SC[036] */
	GPIO_INPUT,			/* GPIO_S0_SC[037] */
	GPIO_INPUT,			/* GPIO_S0_SC[038] */
	GPIO_INPUT,			/* GPIO_S0_SC[039] */
	GPIO_INPUT,			/* GPIO_S0_SC[040] */
	GPIO_INPUT,			/* GPIO_S0_SC[041] */
	GPIO_FUNC1,			/* ILB_LPC_AD[0] */
	GPIO_FUNC1,			/* ILB_LPC_AD[1] */
	GPIO_FUNC1,			/* ILB_LPC_AD[2] */
	GPIO_FUNC1,			/* ILB_LPC_AD[3] */
	GPIO_FUNC1,			/* ILB_LPC_FRAME# */
	GPIO_FUNC1,			/* ILB_LPC_CLK[0] */
	GPIO_FUNC1,			/* ILB_LPC_CLK[1] */
	GPIO_FUNC1,			/* ILB_LPC_CLKRUN# */
	GPIO_FUNC1,			/* ILB_LPC_SERIRQ */
	GPIO_FUNC1,			/* PCU_SMB_DATA */
	GPIO_FUNC1,			/* PCU_SMB_CLK */
	GPIO_FUNC1,			/* PCU_SMB_ALERT# */
	GPIO_FUNC1,			/* ILB_8254_SPKR */
	GPIO_INPUT,			/* GPIO_S0_SC[055] */
	GPIO_INPUT,			/* GPIO_S0_SC[056] */
	GPIO_INPUT,			/* GPIO_S0_SC[057] */
	GPIO_INPUT,			/* GPIO_S0_SC[058] */
	GPIO_INPUT,			/* GPIO_S0_SC[059] */
	GPIO_INPUT,			/* GPIO_S0_SC[060] */
	GPIO_INPUT,			/* GPIO_S0_SC[061] */
	GPIO_INPUT,			/* GPIO_S0_SC[062] */
	GPIO_INPUT,			/* GPIO_S0_SC[063] */
	GPIO_INPUT,			/* GPIO_S0_SC[064] */
	GPIO_INPUT,			/* GPIO_S0_SC[065] */
	GPIO_INPUT,			/* GPIO_S0_SC[066] */
	GPIO_INPUT,			/* GPIO_S0_SC[067] */
	GPIO_INPUT,			/* GPIO_S0_SC[068] */
	GPIO_INPUT,			/* GPIO_S0_SC[069] */
	GPIO_INPUT,			/* GPIO_S0_SC[070] */
	GPIO_INPUT,			/* GPIO_S0_SC[071] */
	GPIO_INPUT,			/* GPIO_S0_SC[072] */
	GPIO_INPUT,			/* GPIO_S0_SC[073] */
	GPIO_INPUT,			/* GPIO_S0_SC[074] */
	GPIO_INPUT,			/* GPIO_S0_SC[075] */
	GPIO_INPUT,			/* GPIO_S0_SC[076] */
	GPIO_INPUT,			/* GPIO_S0_SC[077] */
	GPIO_INPUT,			/* GPIO_S0_SC[078] */
	GPIO_INPUT,			/* GPIO_S0_SC[079] */
	GPIO_INPUT,			/* GPIO_S0_SC[080] */
	GPIO_INPUT,			/* GPIO_S0_SC[081] */
	GPIO_INPUT,			/* GPIO_S0_SC[082] */
	GPIO_INPUT,			/* GPIO_S0_SC[083] */
	GPIO_INPUT,			/* GPIO_S0_SC[084] */
	GPIO_INPUT,			/* GPIO_S0_SC[085] */
	GPIO_INPUT,			/* GPIO_S0_SC[086] */
	GPIO_INPUT,			/* GPIO_S0_SC[087] */
	GPIO_INPUT,			/* GPIO_S0_SC[088] */
	GPIO_INPUT,			/* GPIO_S0_SC[089] */
	GPIO_INPUT,			/* GPIO_S0_SC[090] */
	GPIO_INPUT,			/* GPIO_S0_SC[091] */
	GPIO_INPUT,			/* GPIO_S0_SC[092] */
	GPIO_INPUT,			/* GPIO_S0_SC[093] */
	GPIO_INPUT,			/* GPIO_S0_SC[094] */
	GPIO_INPUT,			/* GPIO_S0_SC[095] */
	GPIO_INPUT,			/* GPIO_S0_SC[096] */
	GPIO_INPUT,			/* GPIO_S0_SC[097] */
	GPIO_INPUT,			/* GPIO_S0_SC[098] */
	GPIO_INPUT,			/* GPIO_S0_SC[099] */
	GPIO_INPUT,			/* GPIO_S0_SC[100] */
	GPIO_INPUT,			/* GPIO_S0_SC[101] */
	GPIO_END
};

/* SSUS GPIOs */
static const struct soc_gpio_map gpssus_gpio_map[] = {
	GPIO_INPUT,			/* GPIO_S5[00] */
	GPIO_FUNC6,			/* PMC_WAKE_PCIE[1]# */
	GPIO_FUNC6,			/* PMC_WAKE_PCIE[2]# */
	GPIO_FUNC6,			/* PMC_WAKE_PCIE[3]# */
	GPIO_INPUT,			/* GPIO_S5[04] */
	GPIO_INPUT,			/* GPIO_S5[05] */
	GPIO_INPUT,			/* GPIO_S5[06] */
	GPIO_INPUT,			/* GPIO_S5[07] */
	GPIO_INPUT,			/* GPIO_S5[08] */
	GPIO_INPUT,			/* GPIO_S5[09] */
	GPIO_INPUT,			/* GPIO_S5[10] */
	GPIO_INPUT,			/* GPIO_S5[11] */
	GPIO_INPUT,			/* GPIO_S5[12] */
	GPIO_INPUT,			/* GPIO_S5[13] */
	GPIO_INPUT,			/* GPIO_S5[14] */
	GPIO_FUNC0,			/* PMC_WAKE_PCIE[0]# */
	GPIO_FUNC(0, PULL_UP, 20K),	/* PMC_PWRBTN# */
	GPIO_INPUT,			/* GPIO_S5[17] */
	GPIO_INPUT,			/* GPIO_S5[18] */
	GPIO_INPUT,			/* GPIO_S5[19] */
	GPIO_INPUT,			/* GPIO_S5[20] */
	GPIO_INPUT,			/* GPIO_S5[21] */
	GPIO_INPUT,			/* GPIO_S5[22] */
	GPIO_INPUT,			/* GPIO_S5[23] */
	GPIO_INPUT,			/* GPIO_S5[24] */
	GPIO_INPUT,			/* GPIO_S5[25] */
	GPIO_INPUT,			/* GPIO_S5[26] */
	GPIO_INPUT,			/* GPIO_S5[27] */
	GPIO_INPUT,			/* GPIO_S5[28] */
	GPIO_INPUT,			/* GPIO_S5[29] */
	GPIO_INPUT,			/* GPIO_S5[30] */
	GPIO_INPUT,			/* GPIO_S5[31] */
	GPIO_INPUT,			/* GPIO_S5[32] */
	GPIO_INPUT,			/* GPIO_S5[33] */
	GPIO_INPUT,			/* GPIO_S5[34] */
	GPIO_INPUT,			/* GPIO_S5[35] */
	GPIO_INPUT,			/* GPIO_S5[36] */
	GPIO_INPUT,			/* GPIO_S5[37] */
	GPIO_INPUT,			/* GPIO_S5[38] */
	GPIO_INPUT,			/* GPIO_S5[39] */
	GPIO_INPUT,			/* GPIO_S5[40] */
	GPIO_INPUT,			/* GPIO_S5[41] */
	GPIO_INPUT,			/* GPIO_S5[42] */
	GPIO_INPUT,			/* GPIO_S5[43] */
	GPIO_END
};

static const u8 core_dedicated_irq[GPIO_MAX_DIRQS] = {
};

static const u8 sus_dedicated_irq[GPIO_MAX_DIRQS] = {
};

static struct soc_gpio_config gpio_config = {
	.ncore = gpncore_gpio_map,
	.score = gpscore_gpio_map,
	.ssus  = gpssus_gpio_map,
	.core_dirq = &core_dedicated_irq,
	.sus_dirq = &sus_dedicated_irq,
};

struct soc_gpio_config *mainboard_get_gpios(void)
{
	return &gpio_config;
}
