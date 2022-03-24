/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <arch/hlt.h>
#include <arch/stages.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <program_loading.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <soc/pmc.h>
#include <soc/power.h>

#define BCT_OFFSET_IN_BIT	0x4c
#define ODMDATA_OFFSET_IN_BCT	0x508
#define TEGRA_SRAM_MAX		(TEGRA_SRAM_BASE + TEGRA_SRAM_SIZE)

/* called from assembly in bootblock_asm.S */
void tegra210_main(void);

static void save_odmdata(void)
{
	struct tegra_pmc_regs *pmc = (struct tegra_pmc_regs*)TEGRA_PMC_BASE;
	uintptr_t bct_offset;
	u32 odmdata;

	// pmc.odmdata: [18:19]: console type, [15:17]: UART id.
	// TODO(twarren) ODMDATA is stored in the BCT, from bct/odmdata.cfg.
	// I use the BCT offset in the BIT in SRAM to locate the BCT, and
	// pick up the ODMDATA word at BCT offset 0x6A8. I could use a BCT
	// struct header from cbootimage, but it seems like overkill for this.

	bct_offset = read32((void *)(TEGRA_SRAM_BASE + BCT_OFFSET_IN_BIT));
	if (bct_offset > TEGRA_SRAM_BASE && bct_offset < TEGRA_SRAM_MAX) {
		odmdata = read32((void *)(bct_offset + ODMDATA_OFFSET_IN_BCT));
		write32(&pmc->odmdata, odmdata);
	}
}

void __weak bootblock_mainboard_early_init(void)
{
	/* Empty default implementation. */
}

/*
 * Define operations for the workaround:
 *   OP_SET : [reg] = val;
 *   OP_OR  : [reg] |= val;
 *   OP_AND : [reg] &= val;
 *   OP_UDELAY : udelay(val);
 */
typedef enum {
	OP_SET,
	OP_OR,
	OP_AND,
	OP_UDELAY,	/* use val field as usec delay */
} WAR_OP;

struct workaround_op {
	WAR_OP	op;
	u32	reg;
	u32	val;
};

/*
 * An array defines the sequence to perform the workaround
 */
static struct workaround_op workaround_sequence[] = {
	{OP_OR,  0x60006410, (1 << 15)},	/* CLK_SOURCE_SOR1: */
	{OP_AND, 0x60006410, ~(1 << 14)},	/*   CLK_SEL1=1, CLK_SEL0=0 */
	{OP_OR,  0x600060d0, 0x40800000},	/* PLLD_BASE */
	{OP_SET, 0x600062ac, 0x40},		/* clear APE reset */
	{OP_SET, 0x60006294, 0x40000},		/* clear VIC reset */
	{OP_SET, 0x60006304, 0x18000000},	/* clear HOST1X & DISP1 reset */
	{OP_UDELAY, 0, 2},
	{OP_OR,  0x702d10a0, 0x400},		/* I2S0: I2S_CTRL.MASTER=1 */
	{OP_AND, 0x702d1088, ~1},		/* I2S0: I2S_CG.SLCG_ENABLE=0 */
	{OP_OR,  0x702d11a0, 0x400},		/* I2S1: I2S_CTRL.MASTER=1 */
	{OP_AND, 0x702d1188, ~1},		/* I2S1: I2S_CG.SLCG_ENABLE=0 */
	{OP_OR,  0x702d12a0, 0x400},		/* I2S2: I2S_CTRL.MASTER=1 */
	{OP_AND, 0x702d1288, ~1},		/* I2S2: I2S_CG.SLCG_ENABLE=0 */
	{OP_OR,  0x702d13a0, 0x400},		/* I2S3: I2S_CTRL.MASTER=1 */
	{OP_AND, 0x702d1388, ~1},		/* I2S3: I2S_CG.SLCG_ENABLE=0 */
	{OP_OR,  0x702d14a0, 0x400},		/* I2S4: I2S_CTRL.MASTER=1 */
	{OP_AND, 0x702d1488, ~1},		/* I2S4: I2S_CG.SLCG_ENABLE=0 */
	{OP_OR,  0x54200cf8, 4},		/* DC_COM_DSC_TOP_CTL[DSC_SLCG_OVERRIDE]=1 */
	{OP_SET, 0x543400c8, 0xffffffff},	/* NV_PVIC_THI_SLCG_OVERRIDE_LOW_A = 0xFFFF_FFFF */
	{OP_UDELAY, 0, 2},
	{OP_SET, 0x600062a8, 0x40},		/* set APE reset */
	{OP_SET, 0x60006300, 0x18000000},	/* set HOST1X & DISP1 reset */
	{OP_SET, 0x60006290, 0x40000},		/* set VIC reset */
	{OP_SET, 0x60006014, 0x020000c1},	/* CLK_ENB_H */
	{OP_SET, 0x60006010, 0x80400130},	/* CLK_ENB_L */
	{OP_SET, 0x60006018, 0x01f00200},	/* CLK_ENB_U */
	{OP_SET, 0x60006360, 0x80400808},	/* CLK_ENB_V */
	{OP_SET, 0x60006364, 0x402000fc},	/* CLK_ENB_W */
	{OP_SET, 0x60006280, 0x23000780},	/* CLK_ENB_X */
	{OP_SET, 0x60006298, 0x00000340},	/* CLK_ENB_Y */
	{OP_SET, 0x600060f8, 0x00000000},	/* LVL2_CLK_GATE_OVRA */
	{OP_SET, 0x600060fc, 0x00000000},	/* LVL2_CLK_GATE_OVRB */
	{OP_SET, 0x600063a0, 0x00000000},	/* LVL2_CLK_GATE_OVRC */
	{OP_SET, 0x600063a4, 0x01000000},	/* LVL2_CLK_GATE_OVRD, QSPI_CLK_OVR_ON=1 */
	{OP_SET, 0x60006554, 0x00000000},	/* LVL2_CLK_GATE_OVRE */
	{OP_AND, 0x600060d0, 0x1f7fffff},	/* PLLD_BASE: 31,30,29,23 = 0 */
	{OP_AND, 0x60006410, 0xffff3fff},	/* CLK_SOURCE_SOR1 15,14 = 0 */
	{OP_AND, 0x60006148, ~(7 << 29)},	/* CLK_SOURCE_VI: */
	{OP_OR,  0x60006148, (4 << 29)},	/*   SRC=PLLP_OUT0 (4) */
	{OP_AND, 0x60006180, ~(7 << 29)},	/* CLK_SOURCE_HOST1X: */
	{OP_OR,  0x60006180, (4 << 29)},	/*   SRC=PLLP_OUT0 (4) */
	{OP_AND, 0x600066a0, ~(7 << 29)},	/* CLK_SOURCE_NVENC: */
	{OP_OR,  0x600066a0, (4 << 29)}		/*   SRC=PLLP_OUT0 (4) */
};

/*
 * This workaround is to restore CAR CE's, SLCG overrides & PLLD settings
 */
static void mbist_workaround(void)
{
	int i;
	u32 val;
	struct workaround_op *wa_op;

	for (i = 0; i < ARRAY_SIZE(workaround_sequence); ++i) {
		wa_op = &workaround_sequence[i];
		switch (wa_op->op) {
		case OP_SET:
			val = wa_op->val;
			break;
		case OP_OR:
			val = read32((void *)wa_op->reg) | wa_op->val;
			break;
		case OP_AND:
			val = read32((void *)wa_op->reg) & wa_op->val;
			break;
		case OP_UDELAY:
			udelay(wa_op->val);
			__fallthrough;
		default:
			continue;
		}
		write32((void *)wa_op->reg, val);
	}
}

void tegra210_main(void)
{
	// enable JTAG at the earliest stage
	enable_jtag();

	mbist_workaround();

	clock_early_uart();

	/* Configure mselect clock. */
	clock_configure_source(mselect, PLLP, 102000);

	/* Enable AVP cache, timer, APB dma, and mselect blocks.  */
	clock_enable_clear_reset(CLK_L_CACHE2 | CLK_L_TMR,
				 CLK_H_APBDMA,
				 0, CLK_V_MSELECT, 0, 0, 0);

	/* Find ODMDATA in IRAM and save it to scratch reg */
	save_odmdata();

	bootblock_mainboard_early_init();

	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();
		exception_init();
		printk(BIOS_INFO, "T210: Bootblock here\n");
	}

	clock_init();

	printk(BIOS_INFO, "T210 bootblock: Clock init done\n");

	pmc_print_rst_status();

	bootblock_mainboard_init();

	printk(BIOS_INFO, "T210 bootblock: Mainboard bootblock init done\n");

	run_romstage();
}
