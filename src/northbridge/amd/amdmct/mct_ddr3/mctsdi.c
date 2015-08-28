/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

static uint8_t fam15_dimm_dic(struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t dimm, uint8_t rank, uint8_t package_type)
{
	uint8_t dic;

	/* Calculate DIC based on recommendations in MR1_dct[1:0] */
	if (pDCTstat->Status & (1 << SB_LoadReduced)) {
		/* TODO
		* LRDIMM unimplemented
		*/
		dic = 0x0;
	} else {
		dic = 0x1;
	}

	return dic;
}

static uint8_t fam15_rttwr(struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t dimm, uint8_t rank, uint8_t package_type)
{
	uint8_t term = 0;
	uint8_t number_of_dimms = pDCTstat->MAdimms[dct];
	uint8_t frequency_index;
	uint8_t rank_count = pDCTstat->DimmRanks[(dimm * 2) + dct];

	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;
	uint8_t rank_count_dimm2;

	if (is_fam15h())
		frequency_index = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;
	else
		frequency_index = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x7;

	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	if (is_fam15h()) {
		if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* TODO
			 * LRDIMM unimplemented
			 */
		} else if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			if (package_type == PT_GR) {
				/* Socket G34: Fam15h BKDG v3.14 Table 57 */
				if (MaxDimmsInstallable == 1) {
					if ((frequency_index == 0x4) || (frequency_index == 0x6)
						|| (frequency_index == 0xa) || (frequency_index == 0xe)) {
						/* DDR3-667 - DDR3-1333 */
						if (rank_count < 3)
							term = 0x0;
						else
							term = 0x2;
					} else {
						/* DDR3-1600 */
						term = 0x0;
					}
				} else if (MaxDimmsInstallable == 2) {
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((frequency_index == 0x4) || (frequency_index == 0x6)) {
						/* DDR3-667 - DDR3-800 */
						if ((number_of_dimms == 1) && ((rank_count_dimm0 < 4)
							&& (rank_count_dimm1 < 4)))
							term = 0x0;
						else
							term = 0x2;
					} else if (frequency_index == 0xa) {
						/* DDR3-1066 */
						if (number_of_dimms == 1) {
							if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4))
								term = 0x0;
							else
								term = 0x2;
						} else {
							term = 0x1;
						}
					} else if (frequency_index == 0xe) {
						/* DDR3-1333 */
						term = 0x2;
					} else {
						/* DDR3-1600 */
						if (number_of_dimms == 1)
							term = 0x0;
						else
							term = 0x1;
					}
				} else if (MaxDimmsInstallable == 3) {
					rank_count_dimm2 = pDCTstat->DimmRanks[(2 * 2) + dct];

					if ((frequency_index == 0xa) || (frequency_index == 0xe)) {
						/* DDR3-1066 - DDR3-1333 */
						if (rank_count_dimm2 < 4)
							term = 0x1;
						else
							term = 0x2;
					} else if (frequency_index == 0x12) {
						/* DDR3-1600 */
						term = 0x1;
					} else {
						term = 0x2;
					}
				}
			} else {
				/* TODO
				 * Other sockets unimplemented
				 */
			}
		} else {
			/* UDIMM */
			if (package_type == PT_GR) {
				/* Socket G34: Fam15h BKDG v3.14 Table 56 */
				if (MaxDimmsInstallable == 1) {
					term = 0x0;
				} else if (MaxDimmsInstallable == 2) {
					if ((number_of_dimms == 2) && (frequency_index == 0x12)) {
						term = 0x1;
					} else if (number_of_dimms == 1) {
						term = 0x0;
					} else {
						term = 0x2;
					}
				} else if (MaxDimmsInstallable == 3) {
					if (number_of_dimms == 1) {
						if (frequency_index <= 0xa) {
							term = 0x2;
						} else {
							if (rank_count < 3) {
								term = 0x1;
							} else {
								term = 0x2;
							}
						}
					} else if (number_of_dimms == 2) {
						term = 0x2;
					}
				}
			} else {
				/* TODO
				* Other sockets unimplemented
				*/
			}
		}
	}

	printk(BIOS_INFO, "DIMM %d RttWr: %01x\n", dimm, term);

	return term;
}

static uint8_t fam15_rttnom(struct DCTStatStruc *pDCTstat, uint8_t dct, uint8_t dimm, uint8_t rank, uint8_t package_type)
{
	uint8_t term = 0;
	uint8_t number_of_dimms = pDCTstat->MAdimms[dct];
	uint8_t frequency_index;

	uint8_t rank_count_dimm0;
	uint8_t rank_count_dimm1;

	if (is_fam15h())
		frequency_index = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x1f;
	else
		frequency_index = Get_NB32_DCT(pDCTstat->dev_dct, dct, 0x94) & 0x7;

	uint8_t MaxDimmsInstallable = mctGet_NVbits(NV_MAX_DIMMS_PER_CH);

	if (is_fam15h()) {
		if (pDCTstat->Status & (1 << SB_LoadReduced)) {
			/* TODO
			 * LRDIMM unimplemented
			 */
		} else if (pDCTstat->Status & (1 << SB_Registered)) {
			/* RDIMM */
			if (package_type == PT_GR) {
				/* Socket G34: Fam15h BKDG v3.14 Table 57 */
				if (MaxDimmsInstallable == 1) {
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];

					if ((frequency_index == 0x4) || (frequency_index == 0x6)) {
						/* DDR3-667 - DDR3-800 */
						if (rank_count_dimm0 < 4) {
							term = 0x2;
						} else {
							if (!rank)
								term = 0x2;
							else
								term = 0x0;
						}
					} else if (frequency_index == 0xa) {
						/* DDR3-1066 */
						term = 0x1;
					} else if (frequency_index == 0xe) {
						/* DDR3-1333 */
						if (rank_count_dimm0 < 4) {
							term = 0x1;
						} else {
							if (!rank)
								term = 0x3;
							else
								term = 0x0;
						}
					} else {
						term = 0x3;
					}
				} else if (MaxDimmsInstallable == 2) {
					rank_count_dimm0 = pDCTstat->DimmRanks[(0 * 2) + dct];
					rank_count_dimm1 = pDCTstat->DimmRanks[(1 * 2) + dct];

					if ((frequency_index == 0x4) || (frequency_index == 0x6)) {
						/* DDR3-667 - DDR3-800 */
						if (number_of_dimms == 1) {
							if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4))
								term = 0x2;
							else if (rank)
								term = 0x0;
							else
								term = 0x2;
						} else {
							if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4)) {
								term = 0x3;
							} else {
								if (rank_count_dimm0 == 4) {
									if (rank_count_dimm1 == 1)
										term = 0x5;
									else
										term = 0x1;
								} else if (rank_count_dimm1 == 4) {
									if (rank_count_dimm0 == 1)
										term = 0x5;
									else
										term = 0x1;
								}
								if (rank)
									term = 0x0;
							}
						}
					} else if (frequency_index == 0xa) {
						/* DDR3-1066 */
						if (number_of_dimms == 1) {
							if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4))
								term = 0x1;
							else if (rank)
								term = 0x0;
							else
								term = 0x1;
						} else {
							if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4)) {
								term = 0x3;
							} else {
								if (rank_count_dimm0 == 4) {
									if (rank_count_dimm1 == 1)
										term = 0x5;
									else
										term = 0x1;
								} else if (rank_count_dimm1 == 4) {
									if (rank_count_dimm0 == 1)
										term = 0x5;
									else
										term = 0x1;
								}
								if (rank)
									term = 0x0;
							}
						}
					} else if (frequency_index == 0xe) {
						/* DDR3-1333 */
						if (number_of_dimms == 1) {
							if ((rank_count_dimm0 < 4) && (rank_count_dimm1 < 4))
								term = 0x1;
							else if (rank)
								term = 0x0;
							else
								term = 0x3;
						} else {
							term = 0x5;
						}
					} else {
						/* DDR3-1600 */
						if (number_of_dimms == 1)
							term = 0x3;
						else
							term = 0x4;
					}
				} else if (MaxDimmsInstallable == 3) {
					/* TODO
					 * 3 DIMM/channel support unimplemented
					 */
				}
			}
		} else {
			/* UDIMM */
			if (package_type == PT_GR) {
				/* Socket G34: Fam15h BKDG v3.14 Table 56 */
				if (MaxDimmsInstallable == 1) {
					if ((frequency_index == 0x4) || (frequency_index == 0x6))
						term = 0x2;
					else if ((frequency_index == 0xa) || (frequency_index == 0xe))
						term = 0x1;
					else
						term = 0x3;
				}
				if (MaxDimmsInstallable == 2) {
					if (number_of_dimms == 1) {
						if (frequency_index <= 0x6) {
							term = 0x2;
						} else if (frequency_index <= 0xe) {
							term = 0x1;
						} else {
							term = 0x3;
						}
					} else {
						if (frequency_index <= 0xa) {
							term = 0x3;
						} else if (frequency_index <= 0xe) {
							term = 0x5;
						} else {
							term = 0x4;
						}
					}
				} else if (MaxDimmsInstallable == 3) {
					if (number_of_dimms == 1) {
						term = 0x0;
					} else if (number_of_dimms == 2) {
						if (frequency_index <= 0xa) {
							if (rank == 1) {
								term = 0x0;
							} else {
								term = 0x3;
							}
						} else if (frequency_index <= 0xe) {
							if (rank == 1) {
								term = 0x0;
							} else {
								term = 0x5;
							}
						}
					}
				}
			} else {
				/* TODO
				 * Other sockets unimplemented
				 */
			}
		}
	}

	printk(BIOS_INFO, "DIMM %d RttNom: %01x\n", dimm, term);
	return term;
}

static void mct_DramControlReg_Init_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);

static void mct_DCTAccessDone(struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	do {
		val = Get_NB32_DCT(dev, dct, 0x98);
	} while (!(val & (1 << DctAccessDone)));

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static u32 swapAddrBits(struct DCTStatStruc *pDCTstat, u32 MR_register_setting, u8 MrsChipSel, u8 dct)
{
	u16 word;
	u32 ret;

	if (!(pDCTstat->Status & (1 << SB_Registered))) {
		word = pDCTstat->MirrPresU_NumRegR;
		if (dct == 0) {
			word &= 0x55;
			word <<= 1;
		} else
			word &= 0xAA;

		if (word & (1 << MrsChipSel)) {
			/* A3<->A4,A5<->A6,A7<->A8,BA0<->BA1 */
			ret = 0;
			if (MR_register_setting & (1 << 3)) ret |= 1 << 4;
			if (MR_register_setting & (1 << 4)) ret |= 1 << 3;
			if (MR_register_setting & (1 << 5)) ret |= 1 << 6;
			if (MR_register_setting & (1 << 6)) ret |= 1 << 5;
			if (MR_register_setting & (1 << 7)) ret |= 1 << 8;
			if (MR_register_setting & (1 << 8)) ret |= 1 << 7;
			if (is_fam15h()) {
				if (MR_register_setting & (1 << 18)) ret |= 1 << 19;
				if (MR_register_setting & (1 << 19)) ret |= 1 << 18;
				MR_register_setting &= ~0x000c01f8;
			} else {
				if (MR_register_setting & (1 << 16)) ret |= 1 << 17;
				if (MR_register_setting & (1 << 17)) ret |= 1 << 16;
				MR_register_setting &= ~0x000301f8;
			}
			MR_register_setting |= ret;
		}
	}
	return MR_register_setting;
}

static void mct_SendMrsCmd(struct DCTStatStruc *pDCTstat, u8 dct, u32 EMRS)
{
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	val = Get_NB32_DCT(dev, dct, 0x7c);
	val &= ~0x00ffffff;
	val |= EMRS;
	val |= 1 << SendMrsCmd;
	Set_NB32_DCT(dev, dct, 0x7c, val);

	do {
		val = Get_NB32_DCT(dev, dct, 0x7c);
	} while (val & (1 << SendMrsCmd));

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

static u32 mct_MR2(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret;

	/* The formula for chip select number is: CS = dimm*2+rank */
	uint8_t dimm = MrsChipSel / 2;
	uint8_t rank = MrsChipSel % 2;

	if (is_fam15h()) {
		uint8_t package_type = mctGet_NVbits(NV_PACK_TYPE);

		/* FIXME: These parameters should be configurable
		 * For now, err on the side of caution and enable automatic 2x refresh
		 * when the DDR temperature rises above the internal limits
		 */
		uint8_t force_2x_self_refresh = 0;	/* ASR */
		uint8_t auto_2x_self_refresh = 1;	/* SRT */

		ret = 0x80000;
		ret |= (MrsChipSel << 21);

		/* Set self refresh parameters */
		ret |= (force_2x_self_refresh << 6);
		ret |= (auto_2x_self_refresh << 7);

		/* Obtain Tcwl, adjust, and set CWL with the adjusted value */
		dword = Get_NB32_DCT(dev, dct, 0x20c) & 0x1f;
		ret |= ((dword - 5) << 3);

		/* Obtain and set RttWr */
		ret |= (fam15_rttwr(pDCTstat, dct, dimm, rank, package_type) << 9);
	} else {
		ret = 0x20000;
		ret |= (MrsChipSel << 20);

		/* program MrsAddress[5:3]=CAS write latency (CWL):
		 * based on F2x[1,0]84[Tcwl] */
		dword = Get_NB32_DCT(dev, dct, 0x84);
		dword = mct_AdjustSPDTimings(pMCTstat, pDCTstat, dword);

		ret |= ((dword >> 20) & 7) << 3;

		/* program MrsAddress[6]=auto self refresh method (ASR):
		 * based on F2x[1,0]84[ASR]
		 * program MrsAddress[7]=self refresh temperature range (SRT):
		 * based on F2x[1,0]84[ASR and SRT]
		 */
		ret |= ((dword >> 18) & 3) << 6;

		/* program MrsAddress[10:9]=dynamic termination during writes (RTT_WR)
		 * based on F2x[1,0]84[DramTermDyn]
		 */
		ret |= ((dword >> 10) & 3) << 9;
	}

	printk(BIOS_SPEW, "Going to send DCT %d DIMM %d rank %d MR2 control word %08x\n", dct, dimm, rank, ret);

	return ret;
}

static u32 mct_MR3(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret;

	/* The formula for chip select number is: CS = dimm*2+rank */
	uint8_t dimm = MrsChipSel / 2;
	uint8_t rank = MrsChipSel % 2;

	if (is_fam15h()) {
		ret = 0xc0000;
		ret |= (MrsChipSel << 21);

		/* Program MPR and MPRLoc to 0 */
		// ret |= 0x0;		/* MPR */
		// ret |= (0x0 << 2);	/* MPRLoc */
	} else {
		ret = 0x30000;
		ret |= (MrsChipSel << 20);

		/* program MrsAddress[1:0]=multi purpose register address location
		 * (MPR Location):based on F2x[1,0]84[MprLoc]
		 * program MrsAddress[2]=multi purpose register
		 * (MPR):based on F2x[1,0]84[MprEn]
		 */
		dword = Get_NB32_DCT(dev, dct, 0x84);
		ret |= (dword >> 24) & 7;
	}

	printk(BIOS_SPEW, "Going to send DCT %d DIMM %d rank %d MR3 control word %08x\n", dct, dimm, rank, ret);

	return ret;
}

static u32 mct_MR1(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret;

	/* The formula for chip select number is: CS = dimm*2+rank */
	uint8_t dimm = MrsChipSel / 2;
	uint8_t rank = MrsChipSel % 2;

	if (is_fam15h()) {
		uint8_t package_type = mctGet_NVbits(NV_PACK_TYPE);

		/* Set defaults */
		uint8_t qoff = 0;	/* Enable output buffers */
		uint8_t wrlvl = 0;	/* Disable write levelling */
		uint8_t tqds = 0;
		uint8_t rttnom = 0;
		uint8_t dic = 0;
		uint8_t additive_latency = 0;
		uint8_t dll_enable = 0;

		ret = 0x40000;
		ret |= (MrsChipSel << 21);

		/* Determine if TQDS should be set */
		if ((pDCTstat->Dimmx8Present & (1 << dimm))
			&& (((dimm & 0x1)?(pDCTstat->Dimmx4Present&0x55):(pDCTstat->Dimmx4Present&0xaa)) != 0x0)
			&& (pDCTstat->Status & (1 << SB_LoadReduced)))
			tqds = 1;

		/* Obtain RttNom */
		rttnom = fam15_rttnom(pDCTstat, dct, dimm, rank, package_type);

		/* Obtain DIC */
		dic = fam15_dimm_dic(pDCTstat, dct, dimm, rank, package_type);

		/* Load data into MRS word */
		ret |= (qoff & 0x1) << 12;
		ret |= (tqds & 0x1) << 11;
		ret |= ((rttnom & 0x4) >> 2) << 9;
		ret |= ((rttnom & 0x2) >> 1) << 6;
		ret |= ((rttnom & 0x1) >> 0) << 2;
		ret |= (wrlvl & 0x1) << 7;
		ret |= ((dic & 0x2) >> 1) << 5;
		ret |= ((dic & 0x1) >> 0) << 1;
		ret |= (additive_latency & 0x3) << 3;
		ret |= (dll_enable & 0x1);
	} else {
		ret = 0x10000;
		ret |= (MrsChipSel << 20);

		/* program MrsAddress[5,1]=output driver impedance control (DIC):
		 * based on F2x[1,0]84[DrvImpCtrl]
		 */
		dword = Get_NB32_DCT(dev, dct, 0x84);
		if (dword & (1 << 3))
			ret |= 1 << 5;
		if (dword & (1 << 2))
			ret |= 1 << 1;

		/* program MrsAddress[9,6,2]=nominal termination resistance of ODT (RTT):
		 * based on F2x[1,0]84[DramTerm]
		 */
		if (!(pDCTstat->Status & (1 << SB_Registered))) {
			if (dword & (1 << 9))
				ret |= 1 << 9;
			if (dword & (1 << 8))
				ret |= 1 << 6;
			if (dword & (1 << 7))
				ret |= 1 << 2;
		} else {
			ret |= mct_MR1Odt_RDimm(pMCTstat, pDCTstat, dct, MrsChipSel);
		}

		/* program MrsAddress[11]=TDQS: based on F2x[1,0]94[RDqsEn] */
		if (Get_NB32_DCT(dev, dct, 0x94) & (1 << RDqsEn)) {
			u8 bit;
			/* Set TDQS=1b for x8 DIMM, TDQS=0b for x4 DIMM, when mixed x8 & x4 */
			bit = (ret >> 21) << 1;
			if ((dct & 1) != 0)
				bit ++;
			if (pDCTstat->Dimmx8Present & (1 << bit))
				ret |= 1 << 11;
		}

		/* program MrsAddress[12]=QOFF: based on F2x[1,0]84[Qoff] */
		if (dword & (1 << 13))
			ret |= 1 << 12;
	}

	printk(BIOS_SPEW, "Going to send DCT %d DIMM %d rank %d MR1 control word %08x\n", dct, dimm, rank, ret);

	return ret;
}

static u32 mct_MR0(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel)
{
	u32 dev = pDCTstat->dev_dct;
	u32 dword, ret, dword2;

	/* The formula for chip select number is: CS = dimm*2+rank */
	uint8_t dimm = MrsChipSel / 2;
	uint8_t rank = MrsChipSel % 2;

	if (is_fam15h()) {
		ret = 0x00000;
		ret |= (MrsChipSel << 21);

		/* Set defaults */
		uint8_t ppd = 0;
		uint8_t wr_ap = 0;
		uint8_t dll_reset = 1;
		uint8_t test_mode = 0;
		uint8_t cas_latency = 0;
		uint8_t read_burst_type = 1;
		uint8_t burst_length = 0;

		/* Obtain PchgPDModeSel */
		dword = Get_NB32_DCT(dev, dct, 0x84);
		ppd = (dword >> 23) & 0x1;

		/* Obtain Twr */
		dword = Get_NB32_DCT(dev, dct, 0x22c) & 0x1f;

		/* Calculate wr_ap (Fam15h BKDG v3.14 Table 82) */
		if (dword == 0x10)
			wr_ap = 0x0;
		else if (dword == 0x5)
			wr_ap = 0x1;
		else if (dword == 0x6)
			wr_ap = 0x2;
		else if (dword == 0x7)
			wr_ap = 0x3;
		else if (dword == 0x8)
			wr_ap = 0x4;
		else if (dword == 0xa)
			wr_ap = 0x5;
		else if (dword == 0xc)
			wr_ap = 0x6;
		else if (dword == 0xe)
			wr_ap = 0x7;

		/* Obtain Tcl */
		dword = Get_NB32_DCT(dev, dct, 0x200) & 0x1f;

		/* Calculate cas_latency (Fam15h BKDG v3.14 Table 83) */
		if (dword == 0x5)
			cas_latency = 0x2;
		else if (dword == 0x6)
			cas_latency = 0x4;
		else if (dword == 0x7)
			cas_latency = 0x6;
		else if (dword == 0x8)
			cas_latency = 0x8;
		else if (dword == 0x9)
			cas_latency = 0xa;
		else if (dword == 0xa)
			cas_latency = 0xc;
		else if (dword == 0xb)
			cas_latency = 0xe;
		else if (dword == 0xc)
			cas_latency = 0x1;
		else if (dword == 0xd)
			cas_latency = 0x3;
		else if (dword == 0xe)
			cas_latency = 0x5;
		else if (dword == 0xf)
			cas_latency = 0x7;
		else if (dword == 0x10)
			cas_latency = 0x9;

		/* Obtain BurstCtrl */
		burst_length = Get_NB32_DCT(dev, dct, 0x84) & 0x3;

		/* Load data into MRS word */
		ret |= (ppd & 0x1) << 12;
		ret |= (wr_ap & 0x3) << 9;
		ret |= (dll_reset & 0x1) << 8;
		ret |= (test_mode & 0x1) << 7;
		ret |= ((cas_latency & 0xe) >> 1) << 4;
		ret |= ((cas_latency & 0x1) >> 0) << 2;
		ret |= (read_burst_type & 0x1) << 3;
		ret |= (burst_length & 0x3);
	} else {
		ret = 0x00000;
		ret |= (MrsChipSel << 20);

		/* program MrsAddress[1:0]=burst length and control method
		(BL):based on F2x[1,0]84[BurstCtrl] */
		dword = Get_NB32_DCT(dev, dct, 0x84);
		ret |= dword & 3;

		/* program MrsAddress[3]=1 (BT):interleaved */
		ret |= 1 << 3;

		/* program MrsAddress[6:4,2]=read CAS latency
		(CL):based on F2x[1,0]88[Tcl] */
		dword2 = Get_NB32_DCT(dev, dct, 0x88);
		ret |= (dword2 & 0x7) << 4;		/* F2x88[2:0] to MrsAddress[6:4] */
		ret |= ((dword2 & 0x8) >> 3) << 2;	/* F2x88[3] to MrsAddress[2] */

		/* program MrsAddress[12]=0 (PPD):slow exit */
		if (dword & (1 << 23))
			ret |= 1 << 12;

		/* program MrsAddress[11:9]=write recovery for auto-precharge
		(WR):based on F2x[1,0]84[Twr] */
		ret |= ((dword >> 4) & 7) << 9;

		/* program MrsAddress[8]=1 (DLL):DLL reset
		just issue DLL reset at first time */
		ret |= 1 << 8;
	}

	printk(BIOS_SPEW, "Going to send DCT %d DIMM %d rank %d MR0 control word %08x\n", dct, dimm, rank, ret);

	return ret;
}

static void mct_SendZQCmd(struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 dev = pDCTstat->dev_dct;
	u32 dword;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	/*1.Program MrsAddress[10]=1
	  2.Set SendZQCmd=1
	 */
	dword = Get_NB32_DCT(dev, dct, 0x7C);
	dword &= ~0xFFFFFF;
	dword |= 1 << 10;
	dword |= 1 << SendZQCmd;
	Set_NB32_DCT(dev, dct, 0x7C, dword);

	/* Wait for SendZQCmd=0 */
	do {
		dword = Get_NB32_DCT(dev, dct, 0x7C);
	} while (dword & (1 << SendZQCmd));

	/* 4.Wait 512 MEMCLKs */
	mct_Wait(300);

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}

void mct_DramInit_Sw_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 MrsChipSel;
	u32 dword;
	u32 dev = pDCTstat->dev_dct;

	printk(BIOS_DEBUG, "%s: Start\n", __func__);

	if (pDCTstat->DIMMAutoSpeed == mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK))) {
		/* 3.Program F2x[1,0]7C[EnDramInit]=1 */
		dword = Get_NB32_DCT(dev, dct, 0x7c);
		dword |= 1 << EnDramInit;
		Set_NB32_DCT(dev, dct, 0x7c, dword);
		mct_DCTAccessDone(pDCTstat, dct);

		/* 4.wait 200us */
		mct_Wait(40000);

		/* 5.Program F2x[1, 0]7C[DeassertMemRstX] = 1. */
		dword = Get_NB32_DCT(dev, dct, 0x7c);
		dword |= 1 << DeassertMemRstX;
		Set_NB32_DCT(dev, dct, 0x7c, dword);

		/* 6.wait 500us */
		mct_Wait(200000);

		/* 7.Program F2x[1,0]7C[AssertCke]=1 */
		dword = Get_NB32_DCT(dev, dct, 0x7c);
		dword |= 1 << AssertCke;
		Set_NB32_DCT(dev, dct, 0x7c, dword);

		/* 8.wait 360ns */
		mct_Wait(80);

		/* Set up address parity */
		if ((pDCTstat->Status & (1 << SB_Registered))
			|| (pDCTstat->Status & (1 << SB_LoadReduced))) {
			if (is_fam15h()) {
				dword = Get_NB32_DCT(dev, dct, 0x90);
				dword |= 1 << ParEn;
				Set_NB32_DCT(dev, dct, 0x90, dword);
			}
		}

		/* The following steps are performed with registered DIMMs only and
		 * must be done for each chip select pair */
		if (pDCTstat->Status & (1 << SB_Registered))
			mct_DramControlReg_Init_D(pMCTstat, pDCTstat, dct);

		/* The following steps are performed with load reduced DIMMs only and
		 * must be done for each DIMM */
		// if (pDCTstat->Status & (1 << SB_LoadReduced))
			/* TODO
			 * Implement LRDIMM configuration
			 */
	}

	pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[dct];
	if (pDCTstat->GangedMode & 1)
		pDCTstat->CSPresent = pDCTstat->CSPresent_DCT[0];

	/* The following steps are performed once for unbuffered DIMMs and once for each
	 * chip select on registered DIMMs: */
	for (MrsChipSel = 0; MrsChipSel < 8; MrsChipSel++) {
		if (pDCTstat->CSPresent & (1 << MrsChipSel)) {
			u32 EMRS;
			/* 13.Send EMRS(2) */
			EMRS = mct_MR2(pMCTstat, pDCTstat, dct, MrsChipSel);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);
			/* 14.Send EMRS(3). Ordinarily at this time, MrsAddress[2:0]=000b */
			EMRS= mct_MR3(pMCTstat, pDCTstat, dct, MrsChipSel);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);
			/* 15.Send EMRS(1) */
			EMRS= mct_MR1(pMCTstat, pDCTstat, dct, MrsChipSel);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);
			/* 16.Send MRS with MrsAddress[8]=1(reset the DLL) */
			EMRS= mct_MR0(pMCTstat, pDCTstat, dct, MrsChipSel);
			EMRS = swapAddrBits(pDCTstat, EMRS, MrsChipSel, dct);
			mct_SendMrsCmd(pDCTstat, dct, EMRS);

			if (pDCTstat->DIMMAutoSpeed == mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK)))
				if (!(pDCTstat->Status & (1 << SB_Registered)))
					break; /* For UDIMM, only send MR commands once per channel */
		}
		if (pDCTstat->LogicalCPUID & (AMD_DR_Bx/* | AMD_RB_C0 */)) /* TODO: We dont support RB_C0 now. need to be added and tested. */
			if (!(pDCTstat->Status & (1 << SB_Registered)))
				MrsChipSel ++;
	}

	if (pDCTstat->DIMMAutoSpeed == mhz_to_memclk_config(mctGet_NVbits(NV_MIN_MEMCLK))) {
		/* 17.Send two ZQCL commands */
		mct_SendZQCmd(pDCTstat, dct);
		mct_SendZQCmd(pDCTstat, dct);

		/* 18.Program F2x[1,0]7C[EnDramInit]=0 */
		dword = Get_NB32_DCT(dev, dct, 0x7C);
		dword &= ~(1 << EnDramInit);
		Set_NB32_DCT(dev, dct, 0x7C, dword);
		mct_DCTAccessDone(pDCTstat, dct);
	}

	printk(BIOS_DEBUG, "%s: Done\n", __func__);
}
