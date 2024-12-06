/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * These values are used by MediaTek internally.
 * We can find these registers in "CODA_GPU_RPC_202306081135".
 * The setting values are provided by MediaTek designers.
 */

#include <arch/barrier.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/gpueb.h>
#include <soc/mcu_common.h>
#include <soc/symbols.h>
#include <timer.h>

#define DUMP_REG(reg)	printk(BIOS_DEBUG, #reg "(%#x) = %#x\n", (reg), read32p((reg)))

static struct st_auto_dma_fw {
	const void *fw;
	size_t fw_size;
	uint64_t fw_addr;
} auto_dma_fw[PT_FW_NUM];

static void gpu_reset(void)
{
	/* Reset MFG before normal boot */
	write32p(MFG_RPC_MFG36_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG35_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG34_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG33_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG32_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG31_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG30_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG29_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG28_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG27_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG26_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG25_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG20_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG19_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG18_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG17_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG16_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG15_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG14_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG13_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG12_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG11_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG10_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG9_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG23_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG8_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG7_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG5_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG4_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG3_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG2_PWR_CON, MFG_PWR_OFF_VALUE0);
	write32p(MFG_RPC_MFG37_PWR_CON, MFG_PWR_OFF_VALUE1);
	write32p(MFG_RPC_MFG1_PWR_CON, MFG_PWR_OFF_VALUE2);
	write32p(MFG_RPC_MFG0_PWR_CON, MFG_PWR_OFF_VALUE3);
	write32p(MFG_GHPM_CFG0_CON, 0x0);
	write32p(MFG_RPC_GHPM_CFG13_CON, 0x0);
	write32p(MFG_RPCTOP_DUMMY_REG_0, 0x0);
}

static bool is_mfg0_power_on(void)
{
	return (read32p(MFG0_PWR_CON) & MFG0_PWR_ACK_BIT) == MFG0_PWR_ACK_BIT;
}

static u32 get_mfg0_pwr_con(void)
{
	return read32p(MFG_RPC_MFG0_PWR_CON);
}

static void gpueb_dump_register(void)
{
	DUMP_REG(MFG_RPC_MFG0_PWR_CON);
	DUMP_REG(MFG_RPC_SLV_SLP_PROT_RDY_STA);
}

static void gpueb_print_exp_status(void)
{
	printk(BIOS_ERR, "%s: Request mainpll polling ack: failed\n", __func__);
	printk(BIOS_ERR, "%s: MFG0_PWR_CON = %#x\n", __func__, get_mfg0_pwr_con());
	DUMP_REG(MFG_RPC_AO_CLK_CFG);
	DUMP_REG(MAINPLL_SEL);
	DUMP_REG(MFG_SODI_VRF18);
	DUMP_REG(GPUEB_BUSPLL_RES_STA_GPR);
	DUMP_REG(MFG_RPC_GHPM_CFG13_CON);
	DUMP_REG(MFG_GHPM_RO2_CON);
}

static bool wait_status_ready(uintptr_t status_reg, uint32_t mask,
			      uint32_t expect, uint32_t count)
{
	uint32_t i = 0;

	do {
		udelay(MTCMOS_DELAY_TIME_US);
		if ((read32p(status_reg) & mask) == expect)
			return true;
		i++;
	} while (i < count);

	gpueb_dump_register();
	return false;
}

static void gpueb_request_resource(void)
{
	setbits32p(MFG_RPC_GHPM_CFG13_CON, RESOURCE_BITS);
	if (!wait_status_ready(MFG_GHPM_RO2_CON, RESOURCE_ACK_BITS, RESOURCE_ACK_BITS,
			       ACK_SYNC_COUNT)) {
		gpueb_print_exp_status();
		die("%s: Failed to wait for resource ready\n", __func__);
	}
}

static void mfg0_power_on(void)
{
	/* Check if the power of MFG0 is OFF; if it is ON, then return */
	if (read32p(MFG_RPC_MFG0_PWR_CON) == MFG0_PWR_ON_VALUE)
		return;

	/* Check if the power condition of MFG0 SRAM is ACK; if not, then return */
	if (!(read32p(MFG_RPC_MFG0_PWR_CON) & MFG0_PWR_SRAM_PDN))
		return;

	setbits32p(MFG_RPC_MFG0_PWR_CON, BIT(28));
	setbits32p(MFG_RPC_MFG0_PWR_CON, BIT(2));
	if (!wait_status_ready(MFG_RPC_MFG0_PWR_CON, BIT(30), BIT(30), POWER_ACK_DELAY_COUNT))
		return;

	udelay(MFG0_PWR_ON_DELAY_US);

	setbits32p(MFG_RPC_MFG0_PWR_CON, BIT(3));
	if (!wait_status_ready(MFG_RPC_MFG0_PWR_CON, GENMASK(31, 30), GENMASK(31, 30),
			       POWER_2ND_ACK_DELAY_COUNT))
		return;

	clrbits32p(MFG_RPC_MFG0_PWR_CON, BIT(4));
	clrbits32p(MFG_RPC_MFG0_PWR_CON, BIT(1));
	setbits32p(MFG_RPC_MFG0_PWR_CON, BIT(0));
	clrbits32p(MFG_RPC_MFG0_PWR_CON, BIT(8));
	if (!wait_status_ready(MFG_RPC_MFG0_PWR_CON, BIT(12), 0, POWER_2ND_ACK_DELAY_COUNT))
		return;

	setbits32p(MFG_RPC_MFG0_PWR_CON, BIT(21));
	clrbits32p(MFG_RPC_MFG0_PWR_CON, BIT(22));
	write32p(MFGSYS_PROTECT_EN_CLR_0, GENMASK(31, 25) | GENMASK(19, 16) | GENMASK(3, 0));
	if (!wait_status_ready(MFGSYS_PROTECT_EN_STA_0,
			       GENMASK(31, 26) | BIT(7) | GENMASK(3, 0), 0,
			       POWER_2ND_ACK_DELAY_COUNT))
		return;

	write32p(MFG_RPC_SLV_SLP_PROT_EN_CLR, BIT(0));
	if (!wait_status_ready(MFG_RPC_SLV_SLP_PROT_RDY_STA, BIT(0), 0,
			       POWER_2ND_ACK_DELAY_COUNT))
		return;

	write32p(MFG_RPC_SLV_SLP_PROT_EN_CLR, BIT(1));
	if (!wait_status_ready(MFG_RPC_SLV_SLP_PROT_RDY_STA, BIT(1), 0,
			       POWER_2ND_ACK_DELAY_COUNT))
		return;

	write32p(MFG_RPC_SLV_WAY_EN_SET, BIT(0));
	wait_status_ready(MFG_RPC_SLV_CTRL_UPDATE, BIT(0), BIT(0), POWER_2ND_ACK_DELAY_COUNT);
}

static void switch_mux_pll(void)
{
	gpueb_request_resource();
	setbits32p(MFG_RPC_AO_CLK_CFG, GPUEB_CKMUX_SEL_26M_BIT);
	udelay(SWITCH_MUX_WAIT_US);
}

static void gpueb_assert_init_success(void)
{
	u32 gpueb_mux;

	gpueb_mux = read32p(MFG_RPC_AO_CLK_CFG);
	if (is_mfg0_power_on() &&
	    ((gpueb_mux & GPUEB_CKMUX_SEL_26M_BIT) == GPUEB_CKMUX_SEL_26M_BIT)) {
		printk(BIOS_INFO, "%s: Done\n", __func__);
		return;
	}
	gpueb_print_exp_status();
	die("%s: Failed to assert init\n", __func__);
}

static void parse_fw(const void *buf_addr, size_t buf_size, const void **img, size_t *img_size)
{
	int i;
	void *addr;
	struct gpueb_pt_hdr *hdr;

	hdr = (struct gpueb_pt_hdr *)buf_addr;

	if (buf_size <= sizeof(*hdr))
		die("%s: Buffer size too small\n", __func__);

	while ((uintptr_t)hdr + sizeof(*hdr) <= (uintptr_t)buf_addr + buf_size &&
	       hdr->magic == GPUEB_PT_MAGIC) {
		addr = (void *)hdr + hdr->hdr_size;

		if (hdr->id == 0 || hdr->id >= PT_FW_NUM) {
			printk(BIOS_WARNING, "%s: unexpected hdr (%d)\n", __func__, hdr->id);
			addr += ALIGN_UP(hdr->img_size, hdr->align);
			hdr = (struct gpueb_pt_hdr *)addr;
			continue;
		}

		if ((uintptr_t)addr + hdr->img_size > (uintptr_t)buf_addr + buf_size)
			break;

		if (hdr->id == PT_FW_IDX_GPUEB_FW) {
			*img = addr;
			*img_size = hdr->img_size;
		} else {
			auto_dma_fw[hdr->id].fw = addr;
			auto_dma_fw[hdr->id].fw_size = hdr->img_size;
			auto_dma_fw[hdr->id].fw_addr = hdr->addr;
		}

		addr += ALIGN_UP(hdr->img_size, hdr->align);
		hdr = (struct gpueb_pt_hdr *)addr;
	}

	if (!*img || !*img_size)
		die("%s: img:%p, img_size:%ld\n", __func__, *img, *img_size);

	for (i = PT_FW_IDX_AUTO_DMA_START; i <= PT_FW_IDX_AUTO_DMA_END; i++) {
		if (!auto_dma_fw[i].fw || !auto_dma_fw[i].fw_size) {
			die("%s: Partition missing (@%p)\n"
			    "fw_id[%d]:%p, size:%ld\n", __func__, buf_addr, i,
			    auto_dma_fw[i].fw, auto_dma_fw[i].fw_size);
		}
	}
}

static void load_fw(enum pt_fw_idx pt_id)
{
	assert(auto_dma_fw[pt_id].fw_addr + auto_dma_fw[pt_id].fw_size <= GPUEB_SRAM_SIZE);
	memcpy((void *)(MFG_GPUEB_BASE + auto_dma_fw[pt_id].fw_addr),
	       auto_dma_fw[pt_id].fw, auto_dma_fw[pt_id].fw_size);
}

static void load_auto_dma_fw(void)
{
	load_fw(PT_FW_IDX_AUTO_DMA_CS_BRCAST_PART1_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CS_BRCAST_PART2_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CS_BRCAST_PART3_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CS_HBVC_LOGGING_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CS_TOP_SESV8_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CS_STK_SESV8_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_SMMU_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_PREOC_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CCMD_CH1_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CCMD_CH2_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_CCMD_CH0_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_HBVC_CONFIG_GRP0_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_HBVC_CONFIG_GRP1_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_GPM_OUT_OF_RATIO_FW);
	load_fw(PT_FW_IDX_AUTO_DMA_GPM_OUT_OF_SUM_MAX_FW);
}

static bool gpueb_is_sram_access(void)
{
	u32 mfg0_pwr_con;

	mfg0_pwr_con = read32p(MFG0_PWR_CON);

	if (mfg0_pwr_con & MFG0_PWR_SRAM_PDN_ACK) {
		printk(BIOS_ERR, "%s: SRAM is power-down(%#x)\n", __func__, mfg0_pwr_con);
		return false;
	}

	if ((mfg0_pwr_con & MFG0_PWR_SRAM_SLEEP_B_ACK) != MFG0_PWR_SRAM_SLEEP_B_ACK) {
		printk(BIOS_ERR, "%s: SRAM is under sleep mode(%#x)\n", __func__, mfg0_pwr_con);
		return false;
	}

	return true;
}

static void gpueb_power_init(void)
{
	mfg0_power_on();
	switch_mux_pll();
	gpueb_assert_init_success();
}

static void gpueb_reset(struct mtk_mcu *mcu)
{
	const void *gpueb_img = NULL;
	size_t gpueb_img_size = 0;
	u32 tcm_lock_info;

	parse_fw(mcu->load_buffer, mcu->buffer_size, &gpueb_img, &gpueb_img_size);
	if (!gpueb_img || !gpueb_img_size)
		die("%s: Failed to parse gpueb FW", __func__);

	tcm_lock_info = (TCM_LOCK_ADDR >> 12) & GPUEB_TCM_LOCK_MASK;
	tcm_lock_info = tcm_lock_info << 24;

	write32p(GPUEB_CFGREG_SW_RSTN, 0);
	memset((void *)MFG_GPUEB_BASE, 0, GPUEB_SRAM_SIZE);
	assert(gpueb_img_size <= GPUEB_SRAM_SIZE);
	memcpy((void *)MFG_GPUEB_BASE, gpueb_img, gpueb_img_size);

	load_auto_dma_fw();

	write32p(GPUEB_ABNORMAL_BOOT, 0);
	write32p(GPUEB_WARM_BOOT, 0);
	write32p(GPUMPU_RSV_ADDR, (uintptr_t)_resv_mem_gpueb >> PAGE_SHIFT);

	/* Trigger GPUEB */
	write32p(GPUEB_CFGREG_SW_RSTN, 0);
	write32p(GPUEB_CFGREG_SW_RSTN,
		 GPUEB_CFGREG_SW_RSTN_SW_RSTN | GPUEB_CFGREG_SW_RSTN_DMA_BUSY_MASK |
		 tcm_lock_info | GPUEB_CFGREG_PTCM_LOCK);
}

static bool is_boot_up_complete(void)
{
	return read32p(GPUEB_INIT_FOOTPRINT) == GPUEB_BOOT_UP_COMPLETE;
}

static bool gpueb_wait_boot_up_done(void)
{
	if (!wait_us(BOOT_TIMEOUT_US, is_boot_up_complete())) {
		printk(BIOS_CRIT, "%s: GPUEB boot-up timed out!!!\n", __func__);
		DUMP_REG(GPUEB_MUX);
		DUMP_REG(MFG0_PWR_CON);
		DUMP_REG(MFG_RPC_GPUEB_CFG);
		DUMP_REG(GPUEB_CFGREG_SW_RSTN);
		DUMP_REG(GPUEB_CFGREG_DBG_APB_PC);
		DUMP_REG(GPUEB_CFGREG_DBG_APB_LR);
		DUMP_REG(GPUEB_CFGREG_DBG_APB_SP);
		DUMP_REG(GPUEB_INIT_FOOTPRINT);
		DUMP_REG(GPUFREQ_FOOTPRINT_GPR);
		DUMP_REG(GPUPTP3_BAD_SAMPLE);
		DUMP_REG(GPUEB_DIAGNOSIS_GPR);

		write32p(GPUEB_INFO, 0x5A5A5A5A);
		DUMP_REG(GPUEB_INFO);
		return false;
	}

	return true;
}

static struct mtk_mcu gpueb = {
	.firmware_name = CONFIG_GPUEB_FIRMWARE,
	.load_buffer = _dram_dma,
	.buffer_size = REGION_SIZE(dram_dma),
	.reset = gpueb_reset,
};

static void gpueb_load(void)
{
	if (!is_mfg0_power_on() || !gpueb_is_sram_access())
		die("%s: GPUEB(MFG0) NOT power-on!!!", __func__);

	/* Load FW image and trigger GPUEB */
	if (mtk_init_mcu(&gpueb))
		die("%s: Failed to load FW image", __func__);

	if (!gpueb_wait_boot_up_done())
		die("%s: Failed to wait for boot up", __func__);

	printk(BIOS_INFO, "%s: GPUEB boot-up complete (magic: %#x)\n", __func__,
	       read32p(GPUEB_INIT_FOOTPRINT));
}

void gpueb_init(void)
{
	gpu_reset();
	gpueb_power_init();
	gpueb_load();
	printk(BIOS_INFO, "%s: Init done\n", __func__);
}
