{ /* 4GB (16Gb + 16Gb) for dual rank dram setting */
	{
		.impedance_drvp = 0x9,
		.impedance_drvn = 0xb,
		.datlat_ucfirst = 19,

		.ca_train = {
			[CHANNEL_A] = { 2, 1, 1, 4, 3, 0, 0, 0, 0, 0},
			[CHANNEL_B] = { 0, 0, 0, 0, 0, 4, 3, 2, 3, 4}
		},

		.ca_train_center = {
			[CHANNEL_A] = 0,
			[CHANNEL_B] = 0
		},

		.wr_level = {
			[CHANNEL_A] = { 5, 6, 5, 6},
			[CHANNEL_B] = { 6, 4, 5, 3}
		},

		.gating_win = {
			[CHANNEL_A] = {
					{ 29, 64},
					{ 29, 64}
			},
			[CHANNEL_B] = {
					{ 29, 72},
					{ 29, 72}
			}
		},

		.rx_dqs_dly = {
			[CHANNEL_A] = 0xd0b0909,
			[CHANNEL_B] = 0xa090809
		},

		.rx_dq_dly = {
			[CHANNEL_A] = {
						0x0020202,
						0x4030102,
						0x2040100,
						0x1020102,
						0x1050100,
						0x5040403,
						0x5050706,
						0x0030304
			},
			[CHANNEL_B] = {
						0x2020202,
						0x5020200,
						0x1010200,
						0x2000100,
						0x1030100,
						0x2040202,
						0x2040503,
						0x0010103
			}
		},
	},
	{
		.actim     = 0xaafd478c,
		.actim1    = 0x91001f59,
		.actim05t  = 0x000025e1,
		.conf1     = 0x00048403,
		.conf2     = 0x030000a9,
		.ddr2ctl   = 0x000063b1,
		.gddr3ctl1 = 0x11000000,
		.misctl0   = 0x21000000,
		.pd_ctrl   = 0xd1976442,
		.rkcfg     = 0x002156c1,
		.test2_3   = 0xbfc70401,
		.test2_4   = 0x2801110d
	},
	{
		.cona     = 0xa053a057,
		.conb     = 0x17283544,
		.conc     = 0x0a1a0b1a,
		.cond     = 0x00000000,
		.cone     = 0xffff0848,
		.conf     = 0x08420000,
		.cong     = 0x2b2b2a38,
		.conh     = 0x00000000,
		.conm_1   = 0x40000500,
		.conm_2   = 0x400005ff,
		.mdct_1   = 0x80030303,
		.mdct_2   = 0x34220c3f,
		.test0    = 0xcccccccc,
		.test1    = 0xcccccccc,
		.testb    = 0x00060124,
		.testc    = 0x38470000,
		.testd    = 0x00000000,
		.arba     = 0x7f077a49,
		.arbc     = 0xa0a070dd,
		.arbd     = 0x07007046,
		.arbe     = 0x40407046,
		.arbf     = 0xa0a070c6,
		.arbg     = 0xffff7047,
		.arbi     = 0x20406188,
		.arbj     = 0x9719595e,
		.arbk     = 0x64f3fc79,
		.slct_1   = 0x00010800,
		.slct_2   = 0xff03ff00,
		.bmen     = 0x00ff0001
	},
	{
		.mrs_1  = 0x00830001,
		.mrs_2  = 0x001c0002,
		.mrs_3  = 0x00010003,
		.mrs_10 = 0x00ff000a,
		.mrs_11 = 0x0000000b,
		.mrs_63 = 0x0000003f
	},
	.type = TYPE_LPDDR3,
	.dram_freq = 896 * MHz,
},
