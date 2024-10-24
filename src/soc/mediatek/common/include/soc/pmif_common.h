/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MEDIATEK_SOC_PMIF_COMMON__
#define __MEDIATEK_SOC_PMIF_COMMON__

#include <types.h>

enum {
	PMIF_CMD_REG_0,
	PMIF_CMD_REG,
	PMIF_CMD_EXT_REG,
	PMIF_CMD_EXT_REG_LONG,
};

struct chan_regs {
	u32 ch_send;
	u32 wdata;
	u32 reserved12[3];
	u32 rdata;
	u32 reserved13[3];
	u32 ch_rdy;
	u32 ch_sta;
};

struct pmif {
	struct mtk_pmif_regs *mtk_pmif;
	struct chan_regs *ch;
	u32 swinf_no;
	u32 mstid;
	u32 pmifid;
	void (*read)(struct pmif *arb, u32 slvid, u32 reg, u32 *data);
	void (*write)(struct pmif *arb, u32 slvid, u32 reg, u32 data);
	u32 (*read_field)(struct pmif *arb, u32 slvid, u32 reg, u32 mask, u32 shift);
	void (*write_field)(struct pmif *arb, u32 slvid, u32 reg, u32 val, u32 mask, u32 shift);
	int (*is_pmif_init_done)(struct pmif *arb);
};

enum {
	PMIF_SPI,
	PMIF_SPMI,
};

enum {
	E_IO = 1,		/* I/O error */
	E_BUSY,			/* Device or resource busy */
	E_NODEV,		/* No such device */
	E_INVAL,		/* Invalid argument */
	E_OPNOTSUPP,		/* Operation not supported on transport endpoint */
	E_TIMEOUT,		/* Wait for idle time out */
	E_READ_TEST_FAIL,	/* SPI read fail */
	E_SPI_INIT_RESET_SPI,	/* Reset SPI fail */
	E_SPI_INIT_SIDLY,	/* SPI edge calibration fail */
};

enum pmic_interface {
	PMIF_VLD_RDY = 0,
	PMIF_SLP_REQ,
};

DEFINE_BIT(PMIFSPI_INF_EN_SRCLKEN_RC_HW, 4)

DEFINE_BIT(PMIFSPI_OTHER_INF_DXCO0_EN, 0)
DEFINE_BIT(PMIFSPI_OTHER_INF_DXCO1_EN, 1)

DEFINE_BIT(PMIFSPI_ARB_EN_SRCLKEN_RC_HW, 4)
DEFINE_BIT(PMIFSPI_ARB_EN_DCXO_CONN, 15)
DEFINE_BIT(PMIFSPI_ARB_EN_DCXO_NFC, 16)

DEFINE_BITFIELD(PMIFSPI_SPM_SLEEP_REQ_SEL, 1, 0)
DEFINE_BITFIELD(PMIFSPI_SCP_SLEEP_REQ_SEL, 10, 9)

DEFINE_BIT(PMIFSPI_MD_CTL_PMIF_RDY, 9)
DEFINE_BIT(PMIFSPI_MD_CTL_SRCLK_EN, 10)
DEFINE_BIT(PMIFSPI_MD_CTL_SRVOL_EN, 11)

DEFINE_BITFIELD(PMIFSPMI_SPM_SLEEP_REQ_SEL, 1, 0)
DEFINE_BITFIELD(PMIFSPMI_SCP_SLEEP_REQ_SEL, 10, 9)

DEFINE_BIT(PMIFSPMI_MD_CTL_PMIF_RDY, 9)
DEFINE_BIT(PMIFSPMI_MD_CTL_SRCLK_EN, 10)
DEFINE_BIT(PMIFSPMI_MD_CTL_SRVOL_EN, 11)

/* External API */
struct pmif *get_pmif_controller(int inf, int mstid);
void pmwrap_interface_init(void);
int mtk_pmif_init(void);
void pmif_spmi_read(struct pmif *arb, u32 slvid, u32 reg, u32 *data);
void pmif_spmi_write(struct pmif *arb, u32 slvid, u32 reg, u32 data);
u32 pmif_spmi_read_field(struct pmif *arb, u32 slvid, u32 reg, u32 mask, u32 shift);
void pmif_spmi_write_field(struct pmif *arb, u32 slvid, u32 reg,
			   u32 val, u32 mask, u32 shift);
void pmif_spi_read(struct pmif *arb, u32 slvid, u32 reg, u32 *data);
void pmif_spi_write(struct pmif *arb, u32 slvid, u32 reg, u32 data);
u32 pmif_spi_read_field(struct pmif *arb, u32 slvid, u32 reg, u32 mask, u32 shift);
void pmif_spi_write_field(struct pmif *arb, u32 slvid, u32 reg,
			  u32 val, u32 mask, u32 shift);
int pmif_check_init_done(struct pmif *arb);

extern const struct pmif pmif_spmi_arb[];
extern const size_t pmif_spmi_arb_count;
extern const struct pmif pmif_spi_arb[];
extern const size_t pmif_spi_arb_count;
#endif /*__MEDIATEK_SOC_PMIF_COMMON__*/
