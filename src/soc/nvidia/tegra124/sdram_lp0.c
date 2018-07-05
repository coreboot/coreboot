/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/cache.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/pmc.h>
#include <soc/sdram.h>
#include <stdlib.h>

/*
 * This function reads SDRAM parameters (and a few CLK_RST regsiter values) from
 * the common BCT format and writes them into PMC scratch registers (where the
 * BootROM expects them on LP0 resume). Since those store the same values in a
 * different format, we follow a "translation table" taken from Nvidia's U-Boot
 * implementation to shift bits into the right position.
 *
 * Contrary to U-Boot, we transform the same macros directly into hardcoded
 * assignments (without any pesky function calls or volatile qualifiers) to give
 * the compiler as much room for optimization as possible. For that reason, we
 * also intentionally avoid <arch/io.h> read/write macros, under the assumption
 * that PMC scratch register accesses should not have side effects and can be
 * arbitrarily reordered. For the few accesses that do have side-effects, the
 * code must contain explicit memory barriers.
 */
void sdram_lp0_save_params(const struct sdram_params *sdram)
{
	struct tegra_pmc_regs * pmc = (void *)TEGRA_PMC_BASE;
	struct clk_rst_ctlr * clk_rst = (void *)TEGRA_CLK_RST_BASE;

#define pack(src, src_bits, dst, dst_bits) { \
	_Static_assert((1 ? src_bits) >= (0 ? src_bits) && (1 ? dst_bits) >= \
		(0 ? dst_bits), "byte range flipped (must be MSB:LSB)" ); \
	_Static_assert((1 ? src_bits) - (0 ? src_bits) == (1 ? dst_bits) - \
		(0 ? dst_bits), "src and dst byte range lengths differ" ); \
	u32 mask = 0xffffffff >> (31 - ((1 ? src_bits) - (0 ? src_bits))); \
	dst &= ~(mask << (0 ? dst_bits)); \
	dst |= ((src >> (0 ? src_bits)) & mask) << (0 ? dst_bits); \
}

#define s(param, src_bits, pmcreg, dst_bits) \
	pack(sdram->param, src_bits, pmc->pmcreg, dst_bits)

#define m(clkreg, src_bits, pmcreg, dst_bits) \
	pack(clk_rst->clkreg, src_bits, pmc->pmcreg, dst_bits)

#define c(value, pmcreg, dst_bits) \
	pack(value, (1 ? dst_bits) - (0 ? dst_bits) : 0, pmc->pmcreg, dst_bits)

	s(EmcClockSource, 7:0, scratch6, 15:8);
	s(EmcClockSource, 31:29, scratch6, 18:16);
	s(EmcClockSource, 26:26, scratch6, 19:19);
	s(EmcOdtWrite, 5:0, scratch6, 25:20);
	s(EmcOdtWrite, 11:8, scratch6, 29:26);
	s(EmcOdtWrite, 30:30, scratch6, 30:30);
	s(EmcOdtWrite, 31:31, scratch6, 31:31);
	s(EmcXm2DqPadCtrl2, 18:16, scratch7, 22:20);
	s(EmcXm2DqPadCtrl2, 22:20, scratch7, 25:23);
	s(EmcXm2DqPadCtrl2, 26:24, scratch7, 28:26);
	s(EmcXm2DqPadCtrl2, 30:28, scratch7, 31:29);
	s(EmcXm2DqPadCtrl3, 18:16, scratch8, 22:20);
	s(EmcXm2DqPadCtrl3, 22:20, scratch8, 25:23);
	s(EmcXm2DqPadCtrl3, 26:24, scratch8, 28:26);
	s(EmcXm2DqPadCtrl3, 30:28, scratch8, 31:29);
	s(EmcTxsrDll, 11:0, scratch9, 31:20);
	c(0, scratch10, 31:0);
	s(EmcDsrVttgenDrv, 5:0, scratch10, 25:20);
	s(EmcDsrVttgenDrv, 18:16, scratch10, 28:26);
	s(EmcDsrVttgenDrv, 26:24, scratch10, 31:29);
	s(EmcFbioSpare, 31:24, scratch11, 7:0);
	s(EmcFbioSpare, 23:16, scratch11, 15:8);
	s(EmcFbioSpare, 15:8, scratch11, 23:16);
	s(EmcFbioSpare, 7:0, scratch11, 31:24);
	s(EmcCfgRsv, 31:0, scratch12, 31:0);
	s(EmcCdbCntl2, 31:0, scratch13, 31:0);
	s(McEmemArbDaTurns, 31:0, scratch14, 31:0);
	s(EmcCfgDigDll, 0:0, scratch17, 0:0);
	s(EmcCfgDigDll, 25:2, scratch17, 24:1);
	s(EmcCfgDigDll, 31:27, scratch17, 29:25);
	s(EmcCdbCntl1, 29:0, scratch18, 29:0);
	s(McEmemArbMisc0, 14:0, scratch19, 14:0);
	s(McEmemArbMisc0, 30:16, scratch19, 29:15);
	s(EmcXm2DqsPadCtrl, 4:0, scratch22, 4:0);
	s(EmcXm2DqsPadCtrl, 12:8, scratch22, 9:5);
	s(EmcXm2DqsPadCtrl, 31:14, scratch22, 27:10);
	s(EmcRrd, 3:0, scratch22, 31:28);
	s(EmcXm2DqPadCtrl, 31:4, scratch23, 27:0);
	s(EmcRext, 3:0, scratch23, 31:28);
	s(EmcXm2CompPadCtrl, 16:0, scratch24, 16:0);
	s(EmcXm2CompPadCtrl, 24:20, scratch24, 21:17);
	s(EmcXm2CompPadCtrl, 27:27, scratch24, 22:22);
	s(EmcXm2CompPadCtrl, 31:28, scratch24, 26:23);
	s(EmcR2w, 4:0, scratch24, 31:27);
	s(EmcCfg, 9:1, scratch25, 8:0);
	s(EmcCfg, 26:16, scratch25, 19:9);
	s(EmcCfg, 31:28, scratch25, 23:20);
	s(EmcXm2VttGenPadCtrl, 0:0, scratch25, 24:24);
	s(EmcXm2VttGenPadCtrl, 2:2, scratch25, 25:25);
	s(EmcXm2VttGenPadCtrl, 18:16, scratch25, 28:26);
	s(EmcXm2VttGenPadCtrl, 26:24, scratch25, 31:29);
	s(EmcZcalInterval, 23:10, scratch26, 13:0);
	s(EmcZcalInterval, 9:0, scratch26, 23:14);
	s(EmcSelDpdCtrl, 5:2, scratch26, 27:24);
	s(EmcSelDpdCtrl, 8:8, scratch26, 28:28);
	s(EmcSelDpdCtrl, 18:16, scratch26, 31:29);
	s(EmcXm2VttGenPadCtrl3, 22:0, scratch27, 22:0);
	s(EmcXm2VttGenPadCtrl3, 24:24, scratch27, 23:23);
	s(EmcSwizzleRank0ByteCfg, 1:0, scratch27, 25:24);
	s(EmcSwizzleRank0ByteCfg, 5:4, scratch27, 27:26);
	s(EmcSwizzleRank0ByteCfg, 9:8, scratch27, 29:28);
	s(EmcSwizzleRank0ByteCfg, 13:12, scratch27, 31:30);
	s(EmcXm2ClkPadCtrl2, 5:0, scratch28, 5:0);
	s(EmcXm2ClkPadCtrl2, 13:8, scratch28, 11:6);
	s(EmcXm2ClkPadCtrl2, 20:16, scratch28, 16:12);
	s(EmcXm2ClkPadCtrl2, 23:23, scratch28, 17:17);
	s(EmcXm2ClkPadCtrl2, 28:24, scratch28, 22:18);
	s(EmcXm2ClkPadCtrl2, 31:31, scratch28, 23:23);
	s(EmcSwizzleRank1ByteCfg, 1:0, scratch28, 25:24);
	s(EmcSwizzleRank1ByteCfg, 5:4, scratch28, 27:26);
	s(EmcSwizzleRank1ByteCfg, 9:8, scratch28, 29:28);
	s(EmcSwizzleRank1ByteCfg, 13:12, scratch28, 31:30);
	s(McEmemArbDaCovers, 23:0, scratch29, 23:0);
	s(McEmemArbRsv, 7:0, scratch29, 31:24);
	s(EmcAutoCalConfig, 4:0, scratch30, 4:0);
	s(EmcAutoCalConfig, 12:8, scratch30, 9:5);
	s(EmcAutoCalConfig, 18:16, scratch30, 12:10);
	s(EmcAutoCalConfig, 25:20, scratch30, 18:13);
	s(EmcAutoCalConfig, 31:28, scratch30, 22:19);
	s(EmcRfc, 8:0, scratch30, 31:23);
	s(EmcXm2DqsPadCtrl2, 21:0, scratch31, 21:0);
	s(EmcXm2DqsPadCtrl2, 24:24, scratch31, 22:22);
	s(EmcAr2Pden, 8:0, scratch31, 31:23);
	s(EmcXm2ClkPadCtrl, 0:0, scratch32, 0:0);
	s(EmcXm2ClkPadCtrl, 4:2, scratch32, 3:1);
	s(EmcXm2ClkPadCtrl, 7:7, scratch32, 4:4);
	s(EmcXm2ClkPadCtrl, 31:14, scratch32, 22:5);
	s(EmcRfcSlr, 8:0, scratch32, 31:23);
	s(EmcXm2DqsPadCtrl3, 0:0, scratch33, 0:0);
	s(EmcXm2DqsPadCtrl3, 5:5, scratch33, 1:1);
	s(EmcXm2DqsPadCtrl3, 12:8, scratch33, 6:2);
	s(EmcXm2DqsPadCtrl3, 18:14, scratch33, 11:7);
	s(EmcXm2DqsPadCtrl3, 24:20, scratch33, 16:12);
	s(EmcXm2DqsPadCtrl3, 30:26, scratch33, 21:17);
	s(EmcTxsr, 9:0, scratch33, 31:22);
	s(McEmemArbCfg, 8:0, scratch40, 8:0);
	s(McEmemArbCfg, 20:16, scratch40, 13:9);
	s(McEmemArbCfg, 27:24, scratch40, 17:14);
	s(McEmemArbCfg, 31:28, scratch40, 21:18);
	s(EmcMc2EmcQ, 2:0, scratch40, 24:22);
	s(EmcMc2EmcQ, 10:8, scratch40, 27:25);
	s(EmcMc2EmcQ, 27:24, scratch40, 31:28);
	s(EmcAutoCalInterval, 20:0, scratch42, 20:0);
	s(McEmemArbOutstandingReq, 8:0, scratch42, 29:21);
	s(McEmemArbOutstandingReq, 31:30, scratch42, 31:30);
	s(EmcMrsWaitCnt2, 9:0, scratch44, 9:0);
	s(EmcMrsWaitCnt2, 25:16, scratch44, 19:10);
	s(EmcTxdsrvttgen, 11:0, scratch44, 31:20);
	s(EmcMrsWaitCnt, 9:0, scratch45, 9:0);
	s(EmcMrsWaitCnt, 25:16, scratch45, 19:10);
	s(EmcCfgPipe, 1:0, scratch45, 21:20);
	s(EmcCfgPipe, 9:4, scratch45, 27:22);
	s(EmcCfgPipe, 15:12, scratch45, 31:28);
	s(EmcXm2DqsPadCtrl4, 22:18, scratch46, 4:0);
	s(EmcXm2DqsPadCtrl4, 16:12, scratch46, 9:5);
	s(EmcXm2DqsPadCtrl4, 10:6, scratch46, 14:10);
	s(EmcXm2DqsPadCtrl4, 4:0, scratch46, 19:15);
	s(EmcZcalWaitCnt, 9:0, scratch46, 29:20);
	s(EmcXm2DqsPadCtrl5, 22:18, scratch47, 4:0);
	s(EmcXm2DqsPadCtrl5, 16:12, scratch47, 9:5);
	s(EmcXm2DqsPadCtrl5, 10:6, scratch47, 14:10);
	s(EmcXm2DqsPadCtrl5, 4:0, scratch47, 19:15);
	s(EmcXm2VttGenPadCtrl2, 5:0, scratch47, 25:20);
	s(EmcXm2VttGenPadCtrl2, 31:28, scratch47, 29:26);
	s(EmcXm2DqsPadCtrl6, 12:8, scratch48, 4:0);
	s(EmcXm2DqsPadCtrl6, 18:14, scratch48, 9:5);
	s(EmcXm2DqsPadCtrl6, 24:20, scratch48, 14:10);
	s(EmcXm2DqsPadCtrl6, 30:26, scratch48, 19:15);
	s(EmcAutoCalConfig3, 4:0, scratch48, 24:20);
	s(EmcAutoCalConfig3, 12:8, scratch48, 29:25);
	s(EmcFbioCfg5, 1:0, scratch48, 31:30);
	s(EmcDllXformQUse8, 4:0, scratch50, 4:0);
	s(EmcDllXformQUse8, 22:8, scratch50, 19:5);
	s(McEmemArbRing1Throttle, 4:0, scratch50, 24:20);
	s(McEmemArbRing1Throttle, 20:16, scratch50, 29:25);
	s(EmcFbioCfg5, 3:2, scratch50, 31:30);
	s(EmcDllXformQUse9, 4:0, scratch51, 4:0);
	s(EmcDllXformQUse9, 22:8, scratch51, 19:5);
	s(EmcCttTermCtrl, 2:0, scratch51, 22:20);
	s(EmcCttTermCtrl, 12:8, scratch51, 27:23);
	s(EmcCttTermCtrl, 31:31, scratch51, 28:28);
	s(EmcFbioCfg6, 2:0, scratch51, 31:29);
	s(EmcDllXformQUse10, 4:0, scratch56, 4:0);
	s(EmcDllXformQUse10, 22:8, scratch56, 19:5);
	s(EmcXm2CmdPadCtrl, 10:3, scratch56, 27:20);
	s(EmcXm2CmdPadCtrl, 28:28, scratch56, 28:28);
	s(EmcPutermAdj, 1:0, scratch56, 30:29);
	s(EmcPutermAdj, 7:7, scratch56, 31:31);
	s(EmcDllXformQUse11, 4:0, scratch57, 4:0);
	s(EmcDllXformQUse11, 22:8, scratch57, 19:5);
	s(EmcWdv, 3:0, scratch57, 31:28);
	s(EmcDllXformQUse12, 4:0, scratch58, 4:0);
	s(EmcDllXformQUse12, 22:8, scratch58, 19:5);
	s(EmcBurstRefreshNum, 3:0, scratch58, 31:28);
	s(EmcDllXformQUse13, 4:0, scratch59, 4:0);
	s(EmcDllXformQUse13, 22:8, scratch59, 19:5);
	s(EmcWext, 3:0, scratch59, 31:28);
	s(EmcDllXformQUse14, 4:0, scratch60, 4:0);
	s(EmcDllXformQUse14, 22:8, scratch60, 19:5);
	s(EmcClkenOverride, 3:1, scratch60, 30:28);
	s(EmcClkenOverride, 6:6, scratch60, 31:31);
	s(EmcDllXformQUse15, 4:0, scratch61, 4:0);
	s(EmcDllXformQUse15, 22:8, scratch61, 19:5);
	s(EmcR2r, 3:0, scratch61, 31:28);
	s(EmcDllXformDq4, 4:0, scratch62, 4:0);
	s(EmcDllXformDq4, 22:8, scratch62, 19:5);
	s(EmcRc, 6:0, scratch62, 26:20);
	s(EmcW2r, 4:0, scratch62, 31:27);
	s(EmcDllXformDq5, 4:0, scratch63, 4:0);
	s(EmcDllXformDq5, 22:8, scratch63, 19:5);
	s(EmcTfaw, 6:0, scratch63, 26:20);
	s(EmcR2p, 4:0, scratch63, 31:27);
	s(EmcDllXformDq6, 4:0, scratch64, 4:0);
	s(EmcDllXformDq6, 22:8, scratch64, 19:5);
	s(EmcDliTrimTxDqs0, 6:0, scratch64, 26:20);
	s(EmcQSafe, 4:0, scratch64, 31:27);
	s(EmcDllXformDq7, 4:0, scratch65, 4:0);
	s(EmcDllXformDq7, 22:8, scratch65, 19:5);
	s(EmcDliTrimTxDqs1, 6:0, scratch65, 26:20);
	s(EmcTClkStable, 4:0, scratch65, 31:27);
	s(EmcAutoCalConfig2, 4:0, scratch66, 4:0);
	s(EmcAutoCalConfig2, 12:8, scratch66, 9:5);
	s(EmcAutoCalConfig2, 20:16, scratch66, 14:10);
	s(EmcAutoCalConfig2, 28:24, scratch66, 19:15);
	s(EmcDliTrimTxDqs2, 6:0, scratch66, 26:20);
	s(EmcTClkStop, 4:0, scratch66, 31:27);
	s(McEmemArbMisc1, 1:0, scratch67, 1:0);
	s(McEmemArbMisc1, 12:4, scratch67, 10:2);
	s(McEmemArbMisc1, 25:21, scratch67, 15:11);
	s(McEmemArbMisc1, 31:28, scratch67, 19:16);
	s(EmcDliTrimTxDqs3, 6:0, scratch67, 26:20);
	s(EmcEInputDuration, 4:0, scratch67, 31:27);
	s(EmcZcalMrwCmd, 7:0, scratch68, 7:0);
	s(EmcZcalMrwCmd, 23:16, scratch68, 15:8);
	s(EmcZcalMrwCmd, 31:30, scratch68, 17:16);
	s(EmcTRefBw, 13:0, scratch68, 31:18);
	s(EmcXm2CmdPadCtrl2, 31:14, scratch69, 17:0);
	s(EmcDliTrimTxDqs4, 6:0, scratch69, 24:18);
	s(EmcDliTrimTxDqs5, 6:0, scratch69, 31:25);
	s(EmcXm2CmdPadCtrl3, 31:14, scratch70, 17:0);
	s(EmcDliTrimTxDqs6, 6:0, scratch70, 24:18);
	s(EmcDliTrimTxDqs7, 6:0, scratch70, 31:25);
	s(EmcXm2CmdPadCtrl5, 2:0, scratch71, 2:0);
	s(EmcXm2CmdPadCtrl5, 6:4, scratch71, 5:3);
	s(EmcXm2CmdPadCtrl5, 10:8, scratch71, 8:6);
	s(EmcXm2CmdPadCtrl5, 14:12, scratch71, 11:9);
	s(EmcXm2CmdPadCtrl5, 18:16, scratch71, 14:12);
	s(EmcXm2CmdPadCtrl5, 22:20, scratch71, 17:15);
	s(EmcDliTrimTxDqs8, 6:0, scratch71, 24:18);
	s(EmcDliTrimTxDqs9, 6:0, scratch71, 31:25);
	s(EmcCdbCntl3, 17:0, scratch72, 17:0);
	s(EmcDliTrimTxDqs10, 6:0, scratch72, 24:18);
	s(EmcDliTrimTxDqs11, 6:0, scratch72, 31:25);
	s(EmcSwizzleRank0Byte0, 2:0, scratch73, 2:0);
	s(EmcSwizzleRank0Byte0, 6:4, scratch73, 5:3);
	s(EmcSwizzleRank0Byte0, 10:8, scratch73, 8:6);
	s(EmcSwizzleRank0Byte0, 14:12, scratch73, 11:9);
	s(EmcSwizzleRank0Byte0, 18:16, scratch73, 14:12);
	s(EmcSwizzleRank0Byte0, 22:20, scratch73, 17:15);
	s(EmcDliTrimTxDqs12, 6:0, scratch73, 24:18);
	s(EmcDliTrimTxDqs13, 6:0, scratch73, 31:25);
	s(EmcSwizzleRank0Byte1, 2:0, scratch74, 2:0);
	s(EmcSwizzleRank0Byte1, 6:4, scratch74, 5:3);
	s(EmcSwizzleRank0Byte1, 10:8, scratch74, 8:6);
	s(EmcSwizzleRank0Byte1, 14:12, scratch74, 11:9);
	s(EmcSwizzleRank0Byte1, 18:16, scratch74, 14:12);
	s(EmcSwizzleRank0Byte1, 22:20, scratch74, 17:15);
	s(EmcDliTrimTxDqs14, 6:0, scratch74, 24:18);
	s(EmcDliTrimTxDqs15, 6:0, scratch74, 31:25);
	s(EmcSwizzleRank0Byte2, 2:0, scratch75, 2:0);
	s(EmcSwizzleRank0Byte2, 6:4, scratch75, 5:3);
	s(EmcSwizzleRank0Byte2, 10:8, scratch75, 8:6);
	s(EmcSwizzleRank0Byte2, 14:12, scratch75, 11:9);
	s(EmcSwizzleRank0Byte2, 18:16, scratch75, 14:12);
	s(EmcSwizzleRank0Byte2, 22:20, scratch75, 17:15);
	s(McEmemArbTimingRp, 6:0, scratch75, 24:18);
	s(McEmemArbTimingRc, 6:0, scratch75, 31:25);
	s(EmcSwizzleRank0Byte3, 2:0, scratch76, 2:0);
	s(EmcSwizzleRank0Byte3, 6:4, scratch76, 5:3);
	s(EmcSwizzleRank0Byte3, 10:8, scratch76, 8:6);
	s(EmcSwizzleRank0Byte3, 14:12, scratch76, 11:9);
	s(EmcSwizzleRank0Byte3, 18:16, scratch76, 14:12);
	s(EmcSwizzleRank0Byte3, 22:20, scratch76, 17:15);
	s(McEmemArbTimingFaw, 6:0, scratch76, 24:18);
	s(McEmemArbTimingWap2Pre, 6:0, scratch76, 31:25);
	s(EmcSwizzleRank1Byte0, 2:0, scratch77, 2:0);
	s(EmcSwizzleRank1Byte0, 6:4, scratch77, 5:3);
	s(EmcSwizzleRank1Byte0, 10:8, scratch77, 8:6);
	s(EmcSwizzleRank1Byte0, 14:12, scratch77, 11:9);
	s(EmcSwizzleRank1Byte0, 18:16, scratch77, 14:12);
	s(EmcSwizzleRank1Byte0, 22:20, scratch77, 17:15);
	s(EmcRas, 5:0, scratch77, 23:18);
	s(EmcRp, 5:0, scratch77, 29:24);
	s(EmcCfg2, 9:8, scratch77, 31:30);
	s(EmcSwizzleRank1Byte1, 2:0, scratch78, 2:0);
	s(EmcSwizzleRank1Byte1, 6:4, scratch78, 5:3);
	s(EmcSwizzleRank1Byte1, 10:8, scratch78, 8:6);
	s(EmcSwizzleRank1Byte1, 14:12, scratch78, 11:9);
	s(EmcSwizzleRank1Byte1, 18:16, scratch78, 14:12);
	s(EmcSwizzleRank1Byte1, 22:20, scratch78, 17:15);
	s(EmcW2p, 5:0, scratch78, 23:18);
	s(EmcRdRcd, 5:0, scratch78, 29:24);
	s(EmcCfg2, 27:26, scratch78, 31:30);
	s(EmcSwizzleRank1Byte2, 2:0, scratch79, 2:0);
	s(EmcSwizzleRank1Byte2, 6:4, scratch79, 5:3);
	s(EmcSwizzleRank1Byte2, 10:8, scratch79, 8:6);
	s(EmcSwizzleRank1Byte2, 14:12, scratch79, 11:9);
	s(EmcSwizzleRank1Byte2, 18:16, scratch79, 14:12);
	s(EmcSwizzleRank1Byte2, 22:20, scratch79, 17:15);
	s(EmcWrRcd, 5:0, scratch79, 23:18);
	s(EmcQUse, 5:0, scratch79, 29:24);
	s(EmcFbioCfg5, 4:4, scratch79, 31:31);
	s(EmcSwizzleRank1Byte3, 2:0, scratch80, 2:0);
	s(EmcSwizzleRank1Byte3, 6:4, scratch80, 5:3);
	s(EmcSwizzleRank1Byte3, 10:8, scratch80, 8:6);
	s(EmcSwizzleRank1Byte3, 14:12, scratch80, 11:9);
	s(EmcSwizzleRank1Byte3, 18:16, scratch80, 14:12);
	s(EmcSwizzleRank1Byte3, 22:20, scratch80, 17:15);
	s(EmcQRst, 5:0, scratch80, 23:18);
	s(EmcRdv, 5:0, scratch80, 29:24);
	s(EmcFbioCfg5, 6:5, scratch80, 31:30);
	s(EmcDynSelfRefControl, 15:0, scratch81, 15:0);
	s(EmcDynSelfRefControl, 31:31, scratch81, 16:16);
	s(EmcPdEx2Wr, 5:0, scratch81, 22:17);
	s(EmcPdEx2Rd, 5:0, scratch81, 28:23);
	s(EmcRefresh, 5:0, scratch82, 5:0);
	s(EmcRefresh, 15:6, scratch82, 15:6);
	s(EmcCmdQ, 4:0, scratch82, 20:16);
	s(EmcCmdQ, 10:8, scratch82, 23:21);
	s(EmcCmdQ, 14:12, scratch82, 26:24);
	s(EmcCmdQ, 28:24, scratch82, 31:27);
	s(EmcAcpdControl, 15:0, scratch83, 15:0);
	s(EmcCfgDigDllPeriod, 15:0, scratch83, 31:16);
	s(EmcDllXformDqs0, 4:0, scratch84, 4:0);
	s(EmcDllXformDqs0, 22:12, scratch84, 15:5);
	s(EmcDllXformDqs1, 4:0, scratch84, 20:16);
	s(EmcDllXformDqs1, 22:12, scratch84, 31:21);
	s(EmcDllXformDqs2, 4:0, scratch85, 4:0);
	s(EmcDllXformDqs2, 22:12, scratch85, 15:5);
	s(EmcDllXformDqs3, 4:0, scratch85, 20:16);
	s(EmcDllXformDqs3, 22:12, scratch85, 31:21);
	s(EmcDllXformDqs4, 4:0, scratch86, 4:0);
	s(EmcDllXformDqs4, 22:12, scratch86, 15:5);
	s(EmcDllXformDqs5, 4:0, scratch86, 20:16);
	s(EmcDllXformDqs5, 22:12, scratch86, 31:21);
	s(EmcDllXformDqs6, 4:0, scratch87, 4:0);
	s(EmcDllXformDqs6, 22:12, scratch87, 15:5);
	s(EmcDllXformDqs7, 4:0, scratch87, 20:16);
	s(EmcDllXformDqs7, 22:12, scratch87, 31:21);
	s(EmcDllXformDqs8, 4:0, scratch88, 4:0);
	s(EmcDllXformDqs8, 22:12, scratch88, 15:5);
	s(EmcDllXformDqs9, 4:0, scratch88, 20:16);
	s(EmcDllXformDqs9, 22:12, scratch88, 31:21);
	s(EmcDllXformDqs10, 4:0, scratch89, 4:0);
	s(EmcDllXformDqs10, 22:12, scratch89, 15:5);
	s(EmcDllXformDqs11, 4:0, scratch89, 20:16);
	s(EmcDllXformDqs11, 22:12, scratch89, 31:21);
	s(EmcDllXformDqs12, 4:0, scratch90, 4:0);
	s(EmcDllXformDqs12, 22:12, scratch90, 15:5);
	s(EmcDllXformDqs13, 4:0, scratch90, 20:16);
	s(EmcDllXformDqs13, 22:12, scratch90, 31:21);
	s(EmcDllXformDqs14, 4:0, scratch91, 4:0);
	s(EmcDllXformDqs14, 22:12, scratch91, 15:5);
	s(EmcDllXformDqs15, 4:0, scratch91, 20:16);
	s(EmcDllXformDqs15, 22:12, scratch91, 31:21);
	s(EmcDllXformQUse0, 4:0, scratch92, 4:0);
	s(EmcDllXformQUse0, 22:12, scratch92, 15:5);
	s(EmcDllXformQUse1, 4:0, scratch92, 20:16);
	s(EmcDllXformQUse1, 22:12, scratch92, 31:21);
	s(EmcDllXformQUse2, 4:0, scratch93, 4:0);
	s(EmcDllXformQUse2, 22:12, scratch93, 15:5);
	s(EmcDllXformQUse3, 4:0, scratch93, 20:16);
	s(EmcDllXformQUse3, 22:12, scratch93, 31:21);
	s(EmcDllXformQUse4, 4:0, scratch94, 4:0);
	s(EmcDllXformQUse4, 22:12, scratch94, 15:5);
	s(EmcDllXformQUse5, 4:0, scratch94, 20:16);
	s(EmcDllXformQUse5, 22:12, scratch94, 31:21);
	s(EmcDllXformQUse6, 4:0, scratch95, 4:0);
	s(EmcDllXformQUse6, 22:12, scratch95, 15:5);
	s(EmcDllXformQUse7, 4:0, scratch95, 20:16);
	s(EmcDllXformQUse7, 22:12, scratch95, 31:21);
	s(EmcDllXformDq0, 4:0, scratch96, 4:0);
	s(EmcDllXformDq0, 22:12, scratch96, 15:5);
	s(EmcDllXformDq1, 4:0, scratch96, 20:16);
	s(EmcDllXformDq1, 22:12, scratch96, 31:21);
	s(EmcDllXformDq2, 4:0, scratch97, 4:0);
	s(EmcDllXformDq2, 22:12, scratch97, 15:5);
	s(EmcDllXformDq3, 4:0, scratch97, 20:16);
	s(EmcDllXformDq3, 22:12, scratch97, 31:21);
	s(EmcPreRefreshReqCnt, 15:0, scratch98, 15:0);
	s(EmcDllXformAddr0, 4:0, scratch98, 20:16);
	s(EmcDllXformAddr0, 22:12, scratch98, 31:21);
	s(EmcDllXformAddr1, 4:0, scratch99, 4:0);
	s(EmcDllXformAddr1, 22:12, scratch99, 15:5);
	s(EmcDllXformAddr2, 4:0, scratch99, 20:16);
	s(EmcDllXformAddr2, 22:12, scratch99, 31:21);
	s(EmcDllXformAddr3, 4:0, scratch100, 4:0);
	s(EmcDllXformAddr3, 22:12, scratch100, 15:5);
	s(EmcDllXformAddr4, 4:0, scratch100, 20:16);
	s(EmcDllXformAddr4, 22:12, scratch100, 31:21);
	s(EmcDllXformAddr5, 4:0, scratch101, 4:0);
	s(EmcDllXformAddr5, 22:12, scratch101, 15:5);
	s(EmcPChg2Pden, 5:0, scratch102, 5:0);
	s(EmcAct2Pden, 5:0, scratch102, 11:6);
	s(EmcRw2Pden, 5:0, scratch102, 17:12);
	s(EmcTcke, 5:0, scratch102, 23:18);
	s(EmcTrpab, 5:0, scratch102, 29:24);
	s(EmcFbioCfg5, 8:7, scratch102, 31:30);
	s(EmcCtt, 5:0, scratch103, 5:0);
	s(EmcEInput, 5:0, scratch103, 11:6);
	s(EmcPutermExtra, 21:16, scratch103, 17:12);
	s(EmcTckesr, 5:0, scratch103, 23:18);
	s(EmcTpd, 5:0, scratch103, 29:24);
	s(EmcFbioCfg5, 10:9, scratch103, 31:30);
	s(EmcRdvMask, 5:0, scratch104, 5:0);
	s(EmcXm2CmdPadCtrl4, 0:0, scratch104, 6:6);
	s(EmcXm2CmdPadCtrl4, 2:2, scratch104, 7:7);
	s(EmcXm2CmdPadCtrl4, 4:4, scratch104, 8:8);
	s(EmcXm2CmdPadCtrl4, 6:6, scratch104, 9:9);
	s(EmcXm2CmdPadCtrl4, 8:8, scratch104, 10:10);
	s(EmcXm2CmdPadCtrl4, 10:10, scratch104, 11:11);
	s(EmcQpop, 5:0, scratch104, 17:12);
	s(McEmemArbTimingRcd, 5:0, scratch104, 23:18);
	s(McEmemArbTimingRas, 5:0, scratch104, 29:24);
	s(EmcFbioCfg5, 12:11, scratch104, 31:30);
	s(McEmemArbTimingRap2Pre, 5:0, scratch105, 5:0);
	s(McEmemArbTimingR2W, 5:0, scratch105, 11:6);
	s(McEmemArbTimingW2R, 5:0, scratch105, 17:12);
	s(EmcIbdly, 4:0, scratch105, 22:18);
	s(McEmemArbTimingR2R, 4:0, scratch105, 27:23);
	s(EmcW2w, 3:0, scratch105, 31:28);
	s(McEmemArbTimingW2W, 4:0, scratch106, 4:0);
	s(McEmemArbOverride, 27:27, scratch106, 5:5);
	s(McEmemArbOverride, 26:26, scratch106, 6:6);
	s(McEmemArbOverride, 16:16, scratch106, 7:7);
	s(McEmemArbOverride, 10:10, scratch106, 8:8);
	s(McEmemArbOverride, 4:4, scratch106, 9:9);
	s(EmcWdvMask, 3:0, scratch106, 13:10);
	s(EmcCttDuration, 3:0, scratch106, 17:14);
	s(EmcQuseWidth, 3:0, scratch106, 21:18);
	s(EmcPutermWidth, 3:0, scratch106, 25:22);
	s(EmcBgbiasCtl0, 3:0, scratch106, 29:26);
	s(EmcFbioCfg5, 25:24, scratch106, 31:30);
	s(McEmemArbTimingRrd, 3:0, scratch107, 3:0);
	s(EmcFbioCfg5, 23:20, scratch107, 10:7);
	s(EmcFbioCfg5, 15:13, scratch107, 13:11);
	s(EmcCfg2, 5:3, scratch107, 16:14);
	s(EmcFbioCfg5, 26:26, scratch107, 17:17);
	s(EmcFbioCfg5, 28:28, scratch107, 18:18);
	s(EmcCfg2, 2:0, scratch107, 21:19);
	s(EmcCfg2, 7:6, scratch107, 23:22);
	s(EmcCfg2, 15:10, scratch107, 29:24);
	s(EmcCfg2, 23:22, scratch107, 31:30);
	s(EmcCfg2, 25:24, scratch108, 1:0);
	s(EmcCfg2, 31:28, scratch108, 5:2);
	s(BootRomPatchData, 31:0, scratch15, 31:0);
	s(BootRomPatchControl, 31:0, scratch16, 31:0);
	s(EmcDevSelect, 1:0, scratch17, 31:30);
	s(EmcZcalWarmColdBootEnables, 1:0, scratch18, 31:30);
	s(EmcCfgDigDllPeriodWarmBoot, 1:0, scratch19, 31:30);
	s(EmcWarmBootExtraModeRegWriteEnable, 0:0, scratch46, 30:30);
	s(McClkenOverrideAllWarmBoot, 0:0, scratch46, 31:31);
	s(EmcClkenOverrideAllWarmBoot, 0:0, scratch47, 30:30);
	s(EmcMrsWarmBootEnable, 0:0, scratch47, 31:31);
	s(EmcTimingControlWait, 7:0, scratch57, 27:20);
	s(EmcZcalWarmBootWait, 7:0, scratch58, 27:20);
	s(EmcAutoCalWait, 7:0, scratch59, 27:20);
	s(WarmBootWait, 7:0, scratch60, 27:20);
	s(EmcPinProgramWait, 7:0, scratch61, 27:20);
	s(AhbArbitrationXbarCtrlMemInitDone, 0:0, scratch79, 30:30);
	s(EmcExtraRefreshNum, 2:0, scratch81, 31:29);
	s(SwizzleRankByteEncode, 15:0, scratch101, 31:16);
	s(MemoryType, 2:0, scratch107, 6:4);

	switch (sdram->MemoryType) {
	case NvBootMemoryType_LpDdr2:
		s(EmcMrwLpddr2ZcalWarmBoot, 23:16, scratch5, 7:0);
		s(EmcMrwLpddr2ZcalWarmBoot, 7:0, scratch5, 15:8);
		s(EmcWarmBootMrwExtra, 23:16, scratch5, 23:16);
		s(EmcWarmBootMrwExtra, 7:0, scratch5, 31:24);
		s(EmcMrwLpddr2ZcalWarmBoot, 31:30, scratch6, 1:0);
		s(EmcWarmBootMrwExtra, 31:30, scratch6, 3:2);
		s(EmcMrwLpddr2ZcalWarmBoot, 27:26, scratch6, 5:4);
		s(EmcWarmBootMrwExtra, 27:26, scratch6, 7:6);
		s(EmcMrw1, 7:0, scratch7, 7:0);
		s(EmcMrw1, 23:16, scratch7, 15:8);
		s(EmcMrw1, 27:26, scratch7, 17:16);
		s(EmcMrw1, 31:30, scratch7, 19:18);
		s(EmcMrw2, 7:0, scratch8, 7:0);
		s(EmcMrw2, 23:16, scratch8, 15:8);
		s(EmcMrw2, 27:26, scratch8, 17:16);
		s(EmcMrw2, 31:30, scratch8, 19:18);
		s(EmcMrw3, 7:0, scratch9, 7:0);
		s(EmcMrw3, 23:16, scratch9, 15:8);
		s(EmcMrw3, 27:26, scratch9, 17:16);
		s(EmcMrw3, 31:30, scratch9, 19:18);
		s(EmcMrw4, 7:0, scratch10, 7:0);
		s(EmcMrw4, 23:16, scratch10, 15:8);
		s(EmcMrw4, 27:26, scratch10, 17:16);
		s(EmcMrw4, 31:30, scratch10, 19:18);
		break;
	case NvBootMemoryType_Ddr3:
		s(EmcMrs, 13:0, scratch5, 13:0);
		s(EmcEmrs, 13:0, scratch5, 27:14);
		s(EmcMrs, 21:20, scratch5, 29:28);
		s(EmcMrs, 31:30, scratch5, 31:30);
		s(EmcEmrs2, 13:0, scratch7, 13:0);
		s(EmcEmrs, 21:20, scratch7, 15:14);
		s(EmcEmrs, 31:30, scratch7, 17:16);
		s(EmcEmrs2, 21:20, scratch7, 19:18);
		s(EmcEmrs3, 13:0, scratch8, 13:0);
		s(EmcEmrs2, 31:30, scratch8, 15:14);
		s(EmcEmrs3, 21:20, scratch8, 17:16);
		s(EmcEmrs3, 31:30, scratch8, 19:18);
		s(EmcWarmBootMrsExtra, 13:0, scratch9, 13:0);
		s(EmcWarmBootMrsExtra, 31:30, scratch9, 15:14);
		s(EmcWarmBootMrsExtra, 21:20, scratch9, 17:16);
		s(EmcZqCalDdr3WarmBoot, 31:30, scratch9, 19:18);
		s(EmcMrs, 27:26, scratch10, 1:0);
		s(EmcEmrs, 27:26, scratch10, 3:2);
		s(EmcEmrs2, 27:26, scratch10, 5:4);
		s(EmcEmrs3, 27:26, scratch10, 7:6);
		s(EmcWarmBootMrsExtra, 27:27, scratch10, 8:8);
		s(EmcWarmBootMrsExtra, 26:26, scratch10, 9:9);
		s(EmcZqCalDdr3WarmBoot, 0:0, scratch10, 10:10);
		s(EmcZqCalDdr3WarmBoot, 4:4, scratch10, 11:11);
		c(0, scratch116, 31:0);
		c(0, scratch117, 31:0);
		break;
	default:
		printk(BIOS_CRIT, "ERROR: %s() unrecognized MemoryType %d!\n",
		       __func__, sdram->MemoryType);
	}

	s(McVideoProtectGpuOverride0, 31:0, secure_scratch8, 31:0);
	s(McVideoProtectVprOverride, 3:0, secure_scratch9, 3:0);
	s(McVideoProtectVprOverride, 11:6, secure_scratch9, 9:4);
	s(McVideoProtectVprOverride, 23:14, secure_scratch9, 19:10);
	s(McVideoProtectVprOverride, 26:26, secure_scratch9, 20:20);
	s(McVideoProtectVprOverride, 31:29, secure_scratch9, 23:21);
	s(EmcFbioCfg5, 19:16, secure_scratch9, 27:24);
	s(McDisplaySnapRing, 1:0, secure_scratch9, 29:28);
	s(McDisplaySnapRing, 31:31, secure_scratch9, 30:30);
	s(EmcAdrCfg, 0:0, secure_scratch9, 31:31);
	s(McVideoProtectGpuOverride1, 15:0, secure_scratch10, 15:0);
	s(McEmemAdrCfgBankMask0, 15:0, secure_scratch10, 31:16);
	s(McEmemAdrCfgBankMask1, 15:0, secure_scratch11, 15:0);
	s(McEmemAdrCfgBankMask2, 15:0, secure_scratch11, 31:16);
	s(McEmemCfg, 13:0, secure_scratch12, 13:0);
	s(McEmemCfg, 31:31, secure_scratch12, 14:14);
	s(McVideoProtectBom, 31:20, secure_scratch12, 26:15);
	s(McVideoProtectVprOverride1, 1:0, secure_scratch12, 28:27);
	s(McVideoProtectVprOverride1, 4:4, secure_scratch12, 29:29);
	s(McVideoProtectBomAdrHi, 1:0, secure_scratch12, 31:30);
	s(McVideoProtectSizeMb, 11:0, secure_scratch13, 11:0);
	s(McSecCarveoutBom, 31:20, secure_scratch13, 23:12);
	s(McEmemAdrCfgBankSwizzle3, 2:0, secure_scratch13, 26:24);
	s(McVideoProtectWriteAccess, 1:0, secure_scratch13, 28:27);
	s(McSecCarveoutAdrHi, 1:0, secure_scratch13, 30:29);
	s(McEmemAdrCfg, 0:0, secure_scratch13, 31:31);
	s(McSecCarveoutSizeMb, 11:0, secure_scratch14, 11:0);
	s(McMtsCarveoutBom, 31:20, secure_scratch14, 23:12);
	s(McMtsCarveoutAdrHi, 1:0, secure_scratch14, 25:24);
	s(McSecCarveoutProtectWriteAccess, 0:0, secure_scratch14, 26:26);
	s(McMtsCarveoutRegCtrl, 0:0, secure_scratch14, 27:27);
	s(McMtsCarveoutSizeMb, 11:0, secure_scratch15, 11:0);
	s(McEmemAdrCfgDev0, 2:0, secure_scratch15, 14:12);
	s(McEmemAdrCfgDev0, 9:8, secure_scratch15, 16:15);
	s(McEmemAdrCfgDev0, 19:16, secure_scratch15, 20:17);
	s(McEmemAdrCfgDev1, 2:0, secure_scratch15, 23:21);
	s(McEmemAdrCfgDev1, 9:8, secure_scratch15, 25:24);
	s(McEmemAdrCfgDev1, 19:16, secure_scratch15, 29:26);

	/* Make sure all writes complete before we lock the secure_scratchs. */
	dmb();
	c(0x1555555, sec_disable2, 25:0);
	c(0xff, sec_disable, 19:12);

	c(0, scratch2, 31:0);
	m(pllm_base, 15:0, scratch2, 15:0);
	m(pllm_base, 20:20, scratch2, 16:16);
	m(pllm_misc2, 2:0, scratch2, 19:17);
	c(0, scratch35, 31:0);
	m(pllm_misc1, 23:0, scratch35, 23:0);
	m(pllm_misc1, 30:28, scratch35, 30:28);
	c(0, scratch3, 31:0);
	s(PllMInputDivider, 7:0, scratch3, 7:0);
	c(0x3e, scratch3, 15:8);
	c(0, scratch3, 19:16);
	s(PllMKVCO, 0:0, scratch3, 20:20);
	s(PllMKCP, 1:0, scratch3, 22:21);
	c(0, scratch36, 31:0);
	s(PllMSetupControl, 23:0, scratch36, 23:0);
	c(0, scratch4, 31:0);
	s(PllMStableTime, 9:0, scratch4, 9:0);
	s(PllMStableTime, 9:0, scratch4, 19:10);

	s(PllMSelectDiv2, 0:0, pllm_wb0_override2, 27:27);
	s(PllMKVCO, 0:0, pllm_wb0_override2, 26:26);
	s(PllMKCP, 1:0, pllm_wb0_override2, 25:24);
	s(PllMSetupControl, 23:0, pllm_wb0_override2, 23:0);
	s(PllMFeedbackDivider, 7:0, pllm_wb0_override_freq, 15:8);
	s(PllMInputDivider, 7:0, pllm_wb0_override_freq, 7:0);

	/* Need to ensure override params are written before we activate it. */
	dmb();
	c(3, pllp_wb0_override, 12:11);
}
