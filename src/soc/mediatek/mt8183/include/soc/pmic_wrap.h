/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8183_PMIC_WRAP_H__
#define __SOC_MEDIATEK_MT8183_PMIC_WRAP_H__

#include <soc/addressmap.h>
#include <soc/pmic_wrap_common.h>
#include <types.h>

struct mt8183_pwrap_regs {
	u32 mux_sel;
	u32 wrap_en;
	u32 dio_en;
	u32 si_sample_ctrl;
	u32 si_sample_ctrl_ulposc;
	u32 rddmy;
	u32 cshext_write;
	u32 cshext_read;
	u32 cslext_write;
	u32 cslext_read;
	u32 ext_ck_write;
	u32 ext_ck_read;
	u32 staupd_ctrl;
	u32 staupd_grpen;
	u32 eint_sta0_adr;
	u32 eint_sta1_adr;
	u32 eint_sta;
	u32 eint_clr;
	u32 eint_ctrl;
	u32 staupd_man_trig;
	u32 staupd_sta;
	u32 wrap_sta;
	u32 harb_init;
	u32 harb_hprio;
	u32 hiprio_arb_en;
	u32 harb_sta0;
	u32 harb_sta1;
	u32 harb_sta2;
	u32 man_en;
	u32 man_cmd;
	u32 man_rdata;
	u32 man_vldclr;
	u32 wacs0_en;
	u32 init_done0;
	u32 wacs1_en;
	u32 init_done1;
	u32 wacs2_en;
	u32 init_done2;
	u32 wacs3_en;
	u32 init_done3;
	u32 wacs_p2p_en;
	u32 init_done_p2p;
	u32 wacs_md32_en;
	u32 init_done_md32;
	u32 int0_en;
	u32 int0_flg_raw;
	u32 int0_flg;
	u32 int0_clr;
	u32 int1_en;
	u32 int1_flg_raw;
	u32 int1_flg;
	u32 int1_clr;
	u32 sig_adr;
	u32 sig_mode;
	u32 sig_value;
	u32 sig_errval;
	u32 crc_en;
	u32 timer_en;
	u32 timer_sta;
	u32 wdt_unit;
	u32 wdt_src_en_0;
	u32 wdt_src_en_1;
	u32 wdt_flg_0;
	u32 wdt_flg_1;
	u32 debug_int_sel;
	u32 dvfs_adr0;
	u32 dvfs_wdata0;
	u32 dvfs_adr1;
	u32 dvfs_wdata1;
	u32 dvfs_adr2;
	u32 dvfs_wdata2;
	u32 dvfs_adr3;
	u32 dvfs_wdata3;
	u32 dvfs_adr4;
	u32 dvfs_wdata4;
	u32 dvfs_adr5;
	u32 dvfs_wdata5;
	u32 dvfs_adr6;
	u32 dvfs_wdata6;
	u32 dvfs_adr7;
	u32 dvfs_wdata7;
	u32 dvfs_adr8;
	u32 dvfs_wdata8;
	u32 dvfs_adr9;
	u32 dvfs_wdata9;
	u32 dvfs_adr10;
	u32 dvfs_wdata10;
	u32 dvfs_adr11;
	u32 dvfs_wdata11;
	u32 dvfs_adr12;
	u32 dvfs_wdata12;
	u32 dvfs_adr13;
	u32 dvfs_wdata13;
	u32 dvfs_adr14;
	u32 dvfs_wdata14;
	u32 dvfs_adr15;
	u32 dvfs_wdata15;
	u32 dcxo_enable;
	u32 dcxo_conn_adr0;
	u32 dcxo_conn_wdata0;
	u32 dcxo_conn_adr1;
	u32 dcxo_conn_wdata1;
	u32 dcxo_nfc_adr0;
	u32 dcxo_nfc_wdata0;
	u32 dcxo_nfc_adr1;
	u32 dcxo_nfc_wdata1;
	u32 spminf_sta_0;
	u32 spminf_sta_1;
	u32 spminf_backup_sta;
	u32 scpinf_sta;
	u32 cipher_key_sel;
	u32 cipher_iv_sel;
	u32 cipher_en;
	u32 cipher_rdy;
	u32 cipher_mode;
	u32 cipher_swrst;
	u32 dcm_en;
	u32 dcm_spi_dbc_prd;
	u32 dcm_dbc_prd;
	u32 int_gps_auxadc_cmd_addr;
	u32 int_gps_auxadc_cmd;
	u32 int_gps_auxadc_rdata_addr;
	u32 ext_gps_auxadc_rdata_addr;
	u32 gpsinf_0_sta;
	u32 gpsinf_1_sta;
	u32 md_adcinf_ctrl;
	u32 md_auxadc_rdata_latest_addr;
	u32 md_auxadc_rdata_wp_addr;
	u32 md_auxadc_rdata[32];
	u32 md_adcinf_0_sta_0;
	u32 md_adcinf_0_sta_1;
	u32 md_adcinf_1_sta_0;
	u32 md_adcinf_1_sta_1;
	u32 swrst;
	u32 spm_sleep_gating_ctrl;
	u32 scp_sleep_gating_ctrl;
	u32 priority_user_sel_0;
	u32 priority_user_sel_1;
	u32 priority_user_sel_2;
	u32 priority_user_sel_3;
	u32 priority_user_sel_4;
	u32 arbiter_out_sel_0;
	u32 arbiter_out_sel_1;
	u32 arbiter_out_sel_2;
	u32 arbiter_out_sel_3;
	u32 arbiter_out_sel_4;
	u32 starv_counter_0;
	u32 starv_counter_1;
	u32 starv_counter_2;
	u32 starv_counter_3;
	u32 starv_counter_4;
	u32 starv_counter_5;
	u32 starv_counter_6;
	u32 starv_counter_7;
	u32 starv_counter_8;
	u32 starv_counter_9;
	u32 starv_counter_10;
	u32 starv_counter_11;
	u32 starv_counter_12;
	u32 starv_counter_13;
	u32 starv_counter_14;
	u32 starv_counter_15;
	u32 starv_counter_16;
	u32 starv_counter_0_status;
	u32 starv_counter_1_status;
	u32 starv_counter_2_status;
	u32 starv_counter_3_status;
	u32 starv_counter_4_status;
	u32 starv_counter_5_status;
	u32 starv_counter_6_status;
	u32 starv_counter_7_status;
	u32 starv_counter_8_status;
	u32 starv_counter_9_status;
	u32 starv_counter_10_status;
	u32 starv_counter_11_status;
	u32 starv_counter_12_status;
	u32 starv_counter_13_status;
	u32 starv_counter_14_status;
	u32 starv_counter_15_status;
	u32 starv_counter_16_status;
	u32 starv_counter_clr;
	u32 starv_prio_status;
	u32 monitor_ctrl_0;
	u32 monitor_ctrl_1;
	u32 monitor_ctrl_2;
	u32 monitor_ctrl_3;
	u32 channel_sequence_0;
	u32 channel_sequence_1;
	u32 channel_sequence_2;
	u32 channel_sequence_3;
	u32 cmd_sequence_0;
	u32 cmd_sequence_1;
	u32 cmd_sequence_2;
	u32 cmd_sequence_3;
	u32 cmd_sequence_4;
	u32 cmd_sequence_5;
	u32 cmd_sequence_6;
	u32 cmd_sequence_7;
	u32 wdata_sequence_0;
	u32 wdata_sequence_1;
	u32 wdata_sequence_2;
	u32 wdata_sequence_3;
	u32 wdata_sequence_4;
	u32 wdata_sequence_5;
	u32 wdata_sequence_6;
	u32 wdata_sequence_7;
	u32 debug_sw_driver_0;
	u32 debug_sw_driver_1;
	u32 debug_sw_driver_2;
	u32 debug_sw_driver_3;
	u32 debug_sw_driver_4;
	u32 debug_sw_driver_5;
	u32 bwc_options;
	u32 reserved1[524];
	u32 wacs0_cmd;
	u32 wacs0_rdata;
	u32 wacs0_vldclr;
	u32 reserved2;
	u32 wacs1_cmd;
	u32 wacs1_rdata;
	u32 wacs1_vldclr;
	u32 reserved3;
	u32 wacs2_cmd;
	u32 wacs2_rdata;
	u32 wacs2_vldclr;
	u32 reserved4;
	u32 wacs3_cmd;
	u32 wacs3_rdata;
	u32 wacs3_vldclr;
};

check_member(mt8183_pwrap_regs, bwc_options, 0x3CC);
check_member(mt8183_pwrap_regs, wacs3_vldclr, 0xC38);

static struct mt8183_pwrap_regs * const mtk_pwrap = (void *)PWRAP_BASE;

enum {
	WACS2 = 1 << 2
};

/* PMIC registers */
enum {
	PMIC_BASE                     = 0x0000,
	PMIC_SMT_CON1                 = PMIC_BASE + 0x0030,
	PMIC_DRV_CON1                 = PMIC_BASE + 0x0038,
	PMIC_FILTER_CON0              = PMIC_BASE + 0x0040,
	PMIC_GPIO_PULLEN0_CLR         = PMIC_BASE + 0x0098,
	PMIC_RG_SPI_CON0              = PMIC_BASE + 0x0408,
	PMIC_RG_SPI_RECORD0           = PMIC_BASE + 0x040A,
	PMIC_DEW_DIO_EN               = PMIC_BASE + 0x040C,
	PMIC_DEW_READ_TEST            = PMIC_BASE + 0x040E,
	PMIC_DEW_WRITE_TEST           = PMIC_BASE + 0x0410,
	PMIC_DEW_CRC_EN               = PMIC_BASE + 0x0414,
	PMIC_DEW_CRC_VAL              = PMIC_BASE + 0x0416,
	PMIC_DEW_RDDMY_NO             = PMIC_BASE + 0x0426,
	PMIC_CPU_INT_STA              = PMIC_BASE + 0x042E,
	PMIC_RG_SPI_CON2              = PMIC_BASE + 0x0432,
	PMIC_RG_SPI_CON3              = PMIC_BASE + 0x0434,
	PMIC_RG_SPI_CON4              = PMIC_BASE + 0x0436,
	PMIC_RG_SPI_CON5              = PMIC_BASE + 0x0438,
	PMIC_RG_SPI_CON6              = PMIC_BASE + 0x043A,
	PMIC_RG_SPI_CON7              = PMIC_BASE + 0x043C,
	PMIC_RG_SPI_CON8              = PMIC_BASE + 0x043E,
	PMIC_RG_SPI_CON13             = PMIC_BASE + 0x0448,
	PMIC_SPISLV_KEY               = PMIC_BASE + 0x044A,
	PMIC_PPCCTL0                  = PMIC_BASE + 0x0A08,
	PMIC_AUXADC_ADC17             = PMIC_BASE + 0x10AA,
	PMIC_AUXADC_ADC31             = PMIC_BASE + 0x10C6,
	PMIC_AUXADC_ADC32             = PMIC_BASE + 0x10C8,
	PMIC_AUXADC_ADC35             = PMIC_BASE + 0x10CE,
	PMIC_AUXADC_RQST0             = PMIC_BASE + 0x1108,
	PMIC_AUXADC_RQST1             = PMIC_BASE + 0x110A,
};

enum {
	E_CLK_EDGE = 1,
	E_CLK_LAST_SETTING
};

enum {
	GPS_MAIN = 0x40,
	GPS_SUBSYS = 0x80
};

enum {
	ULPOSC_CLK = 0x1 << 23
};

enum {
	ULPOSC_OFF = 0x1 << 31,
	ULPOSC_INV = 0x1 << 28,
	ULPOSC_SEL_1 = 0x1 << 24,
	ULPOSC_SEL_2 = 0x1 << 25,
};

enum {
	TIMER_CG = 0x1,
	AP_CG = 0x1 << 1,
	MD_CG = 0x1 << 2,
	CONN_CG = 0x1 << 3,
};

enum {
	MODEM_TEMP_SHARE_CG = 0x1 << 8,
};

enum {
	SIG_PMIC_0 = 0x1 << 0,
	INT_STA_PMIC_0 = 0x1 << 2,
	MD_ADC_DATA0 = 0x1 << 4,
	MD_ADC_DATA1 = 0x1 << 5,
	GPS_ADC_DATA0 = 0x1 << 6,
	GPS_ADC_DATA1 = 0x1 << 7,
};

enum {
	MD = 1,
	MD_DVFS = 2,
	POWER_HW = 4,
	POWER_HW_BACKUP = 8,
	ARB_PRIORITY = MD | MD_DVFS | POWER_HW | POWER_HW_BACKUP,
};

enum {
	ARB_WACS0 = 0x1,
	ARB_WACS2 = 0x1 << 2,
	ARB_WACS_P2P = 0x1 << 4,
	ARB_WACS_MD32 = 0x1 << 5,
	ARB_MD = 0x1 << 6,
	ARB_WACS_POWER_HW = 0x1 << 9,
	ARB_DCXO_CONN = 0x1 << 11,
	ARB_DCXO_NFC = 0x1 << 12,
	ARB_MD_ADC0 = 0x1 << 13,
	ARB_MD_ADC1 = 0x1 << 14,
	ARB_GPS_0 = 0x1 << 15,
	ARB_GPS_1 = 0x1 << 16,
	STAUPD_HARB = 0x1 << 17,
	ARB_USER_EN = ARB_WACS0 | ARB_WACS2 | ARB_WACS_P2P | ARB_WACS_MD32 |
		ARB_MD | ARB_WACS_POWER_HW | ARB_DCXO_CONN | ARB_DCXO_NFC |
		ARB_MD_ADC0 | ARB_MD_ADC1 | ARB_GPS_0 | ARB_GPS_1 | STAUPD_HARB,
};

enum {
	STA_PD_98_5_US = 0x5,
};

enum {
	WATCHDOG_TIMER_7_5_MS = 0xF,
};

enum {
	WDT_MONITOR_ALL = 0xFFFF,
};

enum {
	MONITOR_LATCH_MATCHED_TRANS = 0x1 << 28,
	STARV_15 = 0x1 << 24,
	DCXO = 0x1 << 19,
	MONITOR_ALL_INT = 0xFFFFFFFF,
	INT0_MONITOR = MONITOR_ALL_INT,
	INT1_MONITOR = MONITOR_ALL_INT &
		~MONITOR_LATCH_MATCHED_TRANS & ~STARV_15 & ~DCXO,
};

enum {
	SPI_CLK = 0x1,
	SPI_CSN = 0x1 << 1,
	SPI_MOSI = 0x1 << 2,
	SPI_MISO = 0x1 << 3,
	SPI_FILTER = (SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO) << 4,
	SPI_SMT = (SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO),
	SPI_PULL_DISABLE = (SPI_CLK | SPI_CSN | SPI_MOSI | SPI_MISO) << 4,
};

enum {
	IO_4_MA = 0x8,
};

enum {
	SPI_CLK_SHIFT = 0,
	SPI_CSN_SHIFT = 4,
	SPI_MOSI_SHIFT = 8,
	SPI_MISO_SHIFT = 12,
	SPI_DRIVING = (IO_4_MA << SPI_CLK_SHIFT | IO_4_MA << SPI_CSN_SHIFT |
		IO_4_MA << SPI_MOSI_SHIFT | IO_4_MA << SPI_MISO_SHIFT),
};

enum {
	DUMMY_READ_CYCLES = 0x8,
};
#endif /* __SOC_MEDIATEK_MT8183_PMIC_WRAP_H__ */
