#ifndef REDUNDANT_H
#define REDUNDANT_H

#define DDR3_MR0_PRECHARGE_SLOW    0
#define DDR3_MR0_PRECHARGE_FAST    1
#define DDR3_MR0_MODE_NORMAL       0
#define DDR3_MR0_MODE_TEST         1
#define DDR3_MR0_DLL_RESET_NO      0
#define DDR3_MR0_DLL_RESET_YES     1
#define DDR3_MR0_BURST_TYPE_SEQUENTIAL   0
#define DDR3_MR0_BURST_TYPE_INTERLEAVED  1
#define DDR3_MR0_BURST_LENGTH_FIXED_8    0
#define DDR3_MR0_BURST_LENGTH_CHOP       1
#define DDR3_MR0_BURST_LENGTH_FIXED_4    2
/**
 * \brief Get command address for a DDR3 MR0 command
 */
u16 ddr3_get_mr0(
	char precharge_pd,
	u8 write_recovery,
	char dll_reset,
	char mode,
	u8 cas,
	char interleaved_burst,
	u8 burst_lenght
);

#define DDR3_MR1_TQDS_DISABLE            0
#define DDR3_MR1_TQDS_ENABLE             1
#define DDR3_MR1_QOFF_ENABLE             0
#define DDR3_MR1_QOFF_DISABLE            1
#define DDR3_MR1_WRITE_LEVELING_DISABLE  0
#define DDR3_MR1_WRITE_LEVELING_ENABLE   1
#define DDR3_MR1_RTT_NOM_OFF             0
#define DDR3_MR1_RTT_NOM_RZQ4            1
#define DDR3_MR1_RTT_NOM_RZQ2            2
#define DDR3_MR1_RTT_NOM_RZQ6            3
#define DDR3_MR1_RTT_NOM_RZQ12           4
#define DDR3_MR1_RTT_NOM_RZQ8            5
#define DDR3_MR1_AL_DISABLE              0
#define DDR3_MR1_AL_CL_MINUS_1           1
#define DDR3_MR1_AL_CL_MINUS_2           2
#define DDR3_MR1_ODS_RZQ6                0
#define DDR3_MR1_ODS_RZQ7                1
#define DDR3_MR1_DLL_ENABLE              0
#define DDR3_MR1_DLL_DISABLE             1
/**
 * \brief Get command address for a DDR3 MR1 command
 */
u16 ddr3_get_mr1(
	char q_off,
	char tdqs,
	u8 rtt_nom,
	char write_leveling,
	u8 output_drive_strenght,
	u8 additive_latency,
	u8 dll_disable
);

#define DDR3_MR2_RTT_WR_OFF              0
#define DDR3_MR2_RTT_WR_RZQ4             1
#define DDR3_MR2_RTT_WR_RZQ2             2
/**
 * \brief Get command address for a DDR3 MR2 command
 */
u16 ddr3_get_mr2(
	u8 rtt_wr,
	char extended_temp,
	char auto_self_refresh,
	u8 cas_write
);

/**
 * \brief Get command address for a DDR3 MR3 command
 */
u16 ddr3_get_mr3(char dataflow_from_mpr);

#endif /* REDUNDANT_H */