/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_PMIF_SPI_H__
#define __SOC_MEDIATEK_PMIF_SPI_H__

#include <soc/addressmap.h>
#include <soc/pmif.h>
#include <types.h>

struct mtk_pmicspi_mst_regs {
	u32 reserved1[4];
	u32 other_busy_sta_0;
	u32 wrap_en;
	u32 reserved2[2];
	u32 man_en;
	u32 man_acc;
	u32 reserved3[3];
	u32 mux_sel;
	u32 reserved4[3];
	u32 dio_en;
	u32 rddmy;
	u32 cslext_write;
	u32 cslext_read;
	u32 cshext_write;
	u32 cshext_read;
	u32 ext_ck_write;
	u32 ext_ck_read;
	u32 si_sampling_ctrl;
};

check_member(mtk_pmicspi_mst_regs, other_busy_sta_0, 0x10);
check_member(mtk_pmicspi_mst_regs, man_en, 0x20);
check_member(mtk_pmicspi_mst_regs, mux_sel, 0x34);
check_member(mtk_pmicspi_mst_regs, dio_en, 0x44);

static struct mtk_pmicspi_mst_regs * const mtk_pmicspi_mst = (void *)PMICSPI_MST_BASE;

/* PMIC registers */
enum {
	PMIC_BASE             = 0x0000,
	PMIC_SMT_CON1         = PMIC_BASE + 0x0032,
	PMIC_DRV_CON1         = PMIC_BASE + 0x003a,
	PMIC_FILTER_CON0      = PMIC_BASE + 0x0042,
	PMIC_GPIO_PULLEN0_CLR = PMIC_BASE + 0x0098,
	PMIC_RG_SPI_CON0      = PMIC_BASE + 0x0408,
	PMIC_DEW_DIO_EN       = PMIC_BASE + 0x040c,
	PMIC_DEW_READ_TEST    = PMIC_BASE + 0x040e,
	PMIC_DEW_WRITE_TEST   = PMIC_BASE + 0x0410,
	PMIC_DEW_CRC_EN       = PMIC_BASE + 0x0414,
	PMIC_DEW_CRC_VAL      = PMIC_BASE + 0x0416,
	PMIC_DEW_RDDMY_NO     = PMIC_BASE + 0x0424,
	PMIC_RG_SPI_CON2      = PMIC_BASE + 0x0426,
	PMIC_SPISLV_KEY       = PMIC_BASE + 0x044a,
	PMIC_INT_STA          = PMIC_BASE + 0x0452,
	PMIC_AUXADC_ADC7      = PMIC_BASE + 0x1096,
	PMIC_AUXADC_ADC10     = PMIC_BASE + 0x109c,
	PMIC_AUXADC_RQST0     = PMIC_BASE + 0x1108,
};

#define DEFAULT_SLVID	0

#define PMIF_CMD_STA	BIT(2)
#define SPIMST_STA	BIT(9)

enum {
	SPI_CLK = 0x1,
	SPI_CSN = 0x1 << 1,
	SPI_MOSI = 0x1 << 2,
	SPI_MISO = 0x1 << 3,
	SPI_FILTER = (SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO) << 4,
	SPI_SMT = SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO,
	SPI_PULL_DISABLE = (SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO) << 4,
};

enum {
	SLV_IO_4_MA = 0x8,
};

enum {
	SPI_CLK_SHIFT = 0,
	SPI_CSN_SHIFT = 4,
	SPI_MOSI_SHIFT = 8,
	SPI_MISO_SHIFT = 12,
	SPI_DRIVING = SLV_IO_4_MA << SPI_CLK_SHIFT | SLV_IO_4_MA << SPI_CSN_SHIFT |
		      SLV_IO_4_MA << SPI_MOSI_SHIFT | SLV_IO_4_MA << SPI_MISO_SHIFT,
};

enum {
	OP_WR    = 0x1,
	OP_CSH   = 0x0,
	OP_CSL   = 0x1,
	OP_OUTS  = 0x8,
};

enum {
	DEFAULT_VALUE_READ_TEST  = 0x5aa5,
	WRITE_TEST_VALUE         = 0xa55a,
};

enum {
	DUMMY_READ_CYCLES = 0x8,
};

enum {
	E_CLK_EDGE = 1,
	E_CLK_LAST_SETTING,
};

int pmif_spi_init(struct pmif *arb);
void pmif_spi_iocfg(void);
#endif /* __SOC_MEDIATEK_PMIF_SPI_H__ */
