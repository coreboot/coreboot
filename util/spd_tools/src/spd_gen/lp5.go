/* SPDX-License-Identifier: GPL-2.0-or-later */
package main

import (
	"encoding/json"
	"fmt"
)

/* ------------------------------------------------------------------------------------------ */
/*                                     LP5-defined types                                      */
/* ------------------------------------------------------------------------------------------ */

type lp5 struct {
}

type LP5MemAttributes struct {
	/* Primary attributes - must be provided by JSON file for each part */
	DensityPerDieGb    int
	DiesPerPackage     int
	BitWidthPerChannel int
	RanksPerChannel    int
	SpeedMbps          int

	/*
	 * All the following parameters are optional and required only if the part requires
	 * special parameters as per the datasheet.
	 */
	/* Timing parameters */
	TRFCABNs   int
	TRFCPBNs   int
	TRPABMinNs int
	TRPPBMinNs int
	TCKMinPs   int
	TAAMinPs   int
	TRCDMinNs  int
}

type LP5DensityParams struct {
	DensityEncoding          byte
	RowAddressBitsx8Channel  int
	RowAddressBitsx16Channel int
	TRFCABNs                 int
	TRFCPBNs                 int
}

type LP5SpeedParams struct {
	TCKMinPs      int
	MaxCASLatency int
}

type LP5SPDAttribFunc func(*LP5MemAttributes) byte

type LP5SPDAttribTableEntry struct {
	constVal byte
	getVal   LP5SPDAttribFunc
}

/* ------------------------------------------------------------------------------------------ */
/*                                         Constants                                          */
/* ------------------------------------------------------------------------------------------ */

const (
	/* SPD Byte Index */
	LP5SPDIndexSize                            = 0
	LP5SPDIndexRevision                        = 1
	LP5SPDIndexMemoryType                      = 2
	LP5SPDIndexModuleType                      = 3
	LP5SPDIndexDensityBanks                    = 4
	LP5SPDIndexAddressing                      = 5
	LP5SPDIndexPackageType                     = 6
	LP5SPDIndexOptionalFeatures                = 7
	LP5SPDIndexModuleOrganization              = 12
	LP5SPDIndexBusWidth                        = 13
	LP5SPDIndexTimebases                       = 17
	LP5SPDIndexTCKMin                          = 18
	LP5SPDIndexTAAMin                          = 24
	LP5SPDIndexTRCDMin                         = 26
	LP5SPDIndexTRPABMin                        = 27
	LP5SPDIndexTRPPBMin                        = 28
	LP5SPDIndexTRFCABMinLSB                    = 29
	LP5SPDIndexTRFCABMinMSB                    = 30
	LP5SPDIndexTRFCPBMinLSB                    = 31
	LP5SPDIndexTRFCPBMinMSB                    = 32
	LP5SPDIndexTRPPBMinFineOffset              = 120
	LP5SPDIndexTRPABMinFineOffset              = 121
	LP5SPDIndexTRCDMinFineOffset               = 122
	LP5SPDIndexTAAMinFineOffset                = 123
	LP5SPDIndexTCKMinFineOffset                = 125
	LP5SPDIndexManufacturerPartNumberStartByte = 329
	LP5SPDIndexManufacturerPartNumberEndByte   = 348

	/* SPD Byte Value */

	/*
	 * From JEDEC spec:
	 * 6:4 (Bytes total) = 2 (512 bytes)
	 * 3:0 (Bytes used) = 3 (384 bytes)
	 * Set to 0x23 for LPDDR5.
	 */
	LP5SPDValueSize = 0x23

	/*
	 * Revision 1.0.
	 */
	LP5SPDValueRevision = 0x10

	/*
	 * As per advisory #616599, ADL MRC expects LPDDR5 memory type = 0x13.
	 */
	LP5SPDValueMemoryType = 0x13

	/*
	 * From JEDEC spec:
	 * 7:7 (Hybrid) = 0 (Not hybrid)
	 * 6:4 (Hybrid media) = 000 (Not hybrid)
	 * 3:0 (Base Module Type) = 1110 (Non-DIMM solution)
	 *
	 * This is dependent on hardware design. LPDDR5 only has memory down solution.
	 * Hence this is not hybrid non-DIMM solution.
	 * Set to 0x0E.
	 */
	LP5SPDValueModuleType = 0x0e

	/*
	 * From JEDEC spec:
	 * 5:4 (Maximum Activate Window) = 00 (8192 * tREFI)
	 * 3:0 (Maximum Activate Count) = 1000 (Unlimited MAC)
	 * Set to 0x08.
	 */
	LP5SPDValueOptionalFeatures = 0x08

	/*
	 * For ADL (as per advisory #616599):
	 * 7:5 (Number of system channels) = 000 (1 channel always)
	 * 4:3 (Bus width extension) = 00 (no ECC)
	 * 2:0 (Bus width) = 001 (x16 always)
	 * Set to 0x01.
	 */
	LP5SPDValueBusWidth = 0x01

	/*
	 * From JEDEC spec:
	 * 3:2 (MTB) = 00 (0.125ns)
	 * 1:0 (FTB) = 00 (1ps)
	 * Set to 0x00.
	 */
	LP5SPDValueTimebases = 0x00

	/* As per JEDEC spec, unused digits of manufacturer part number are left as blank. */
	LP5SPDValueManufacturerPartNumberBlank = 0x20
)

const (
	/*
	 * LPDDR5 has a flexible bank architecture with three programmable bank modes: BG, 8B, 16B.
	 * ADL will use 8B mode for all parts.
	 *
	 * From JEDEC spec:
	 * 7:6 (Bank Group Bits) = 00 (no bank groups)
	 * 5:4 (Bank Address Bits) = 01 (8 banks)
	 * Set bits 7:4 to 0b0001.
	 */
	LP5BankGroupsBanks = 0x1

	/*
	 * Tables 8 and 9 from JESD209-5B.
	 * ADL uses 8B mode for all parts. The column addresses are the same for x8 and x16.
	 * Effective column address bits = column address bits + burst address bits = 6 + 5 = 11.
	 * As per JESD 21-C, this is encoded as 0b010.
	 */
	LP5ColumnAddressBits = 0x2
)

/* ------------------------------------------------------------------------------------------ */
/*                                    Global variables                                        */
/* ------------------------------------------------------------------------------------------ */

var LP5PlatformSetMap = map[int][]int{
	0: {PlatformADL},
}

var LP5PartAttributeMap = map[string]LP5MemAttributes{}
var LP5CurrSet int

/*
 * DensityEncoding: Maps the die density in Gb to the SPD encoding of the die density
 * as per JESD 21-C.
 *
 * RowAddressBits: Maps the die density to the number of row address bits.
 * Tables 6-11 in JESD209-5B (same for all three bank modes).
 *
 * TRFCABNs/TRFCPBNs: Maps the die density to the refresh timings.
 * Tables 235 and 236 in JESD209-5B (same for all three bank modes).
 */
var LP5DensityGbToSPDEncoding = map[int]LP5DensityParams{
	4: {
		DensityEncoding:          0x4,
		RowAddressBitsx8Channel:  15,
		RowAddressBitsx16Channel: 14,
		TRFCABNs:                 180,
		TRFCPBNs:                 90,
	},
	6: {
		DensityEncoding:          0xb,
		RowAddressBitsx8Channel:  16,
		RowAddressBitsx16Channel: 15,
		TRFCABNs:                 210,
		TRFCPBNs:                 120,
	},
	8: {
		DensityEncoding:          0x5,
		RowAddressBitsx8Channel:  16,
		RowAddressBitsx16Channel: 15,
		TRFCABNs:                 210,
		TRFCPBNs:                 120,
	},
	12: {
		DensityEncoding:          0x8,
		RowAddressBitsx8Channel:  17,
		RowAddressBitsx16Channel: 16,
		TRFCABNs:                 280,
		TRFCPBNs:                 140,
	},
	16: {
		DensityEncoding:          0x6,
		RowAddressBitsx8Channel:  17,
		RowAddressBitsx16Channel: 16,
		TRFCABNs:                 280,
		TRFCPBNs:                 140,
	},
	24: {
		DensityEncoding:          0x9,
		RowAddressBitsx8Channel:  18,
		RowAddressBitsx16Channel: 17,
		TRFCABNs:                 380,
		TRFCPBNs:                 190,
	},
	32: {
		DensityEncoding:          0x7,
		RowAddressBitsx8Channel:  18,
		RowAddressBitsx16Channel: 17,
		TRFCABNs:                 380,
		TRFCPBNs:                 190,
	},
}

/*
 * Maps the number of row address bits to the SPD encoding as per JESD 21-C.
 */
var LP5RowAddressBitsEncoding = map[int]byte{
	14: 0x2,
	15: 0x3,
	16: 0x4,
	17: 0x5,
	18: 0x6,
}

/*
 * TCKMinPs:
 * LPDDR5 has two clocks: the command/address clock (CK) and the data clock (WCK). They are
 * related by the WCK:CK ratio, which can be either 4:1 or 2:1. On ADL, 4:1 is used.
 * For ADL, the MRC expects the tCKmin to encode the CK period. This is calculated as:
 *   tCKmin = 1 / CK rate
 *          = 1 / (WCK rate / WCK:CK)
 *          = 1 / (speed grade / 2 / WCK:CK)      // "double data rate"
 *
 * MaxCASLatency:
 * From Table 220 of JESD209-5B, using a 4:1 WCK:CK ratio and Set 0.
 */
var LP5SpeedMbpsToSPDEncoding = map[int]LP5SpeedParams{
	6400: {
		TCKMinPs:      1250, /* 1 / (6400 / 2 / 4) */
		MaxCASLatency: 17,
	},
	5500: {
		TCKMinPs:      1455, /* 1 / (5500 / 2 / 4) */
		MaxCASLatency: 15,
	},
}

var LP5SPDAttribTable = map[int]LP5SPDAttribTableEntry{
	LP5SPDIndexSize:               {constVal: LP5SPDValueSize},
	LP5SPDIndexRevision:           {constVal: LP5SPDValueRevision},
	LP5SPDIndexMemoryType:         {constVal: LP5SPDValueMemoryType},
	LP5SPDIndexModuleType:         {constVal: LP5SPDValueModuleType},
	LP5SPDIndexDensityBanks:       {getVal: LP5EncodeDensityBanks},
	LP5SPDIndexAddressing:         {getVal: LP5EncodeSdramAddressing},
	LP5SPDIndexPackageType:        {getVal: LP5EncodePackageType},
	LP5SPDIndexOptionalFeatures:   {constVal: LP5SPDValueOptionalFeatures},
	LP5SPDIndexModuleOrganization: {getVal: LP5EncodeModuleOrganization},
	LP5SPDIndexBusWidth:           {constVal: LP5SPDValueBusWidth},
	LP5SPDIndexTimebases:          {constVal: LP5SPDValueTimebases},
	LP5SPDIndexTCKMin:             {getVal: LP5EncodeTCKMin},
	LP5SPDIndexTCKMinFineOffset:   {getVal: LP5EncodeTCKMinFineOffset},
	LP5SPDIndexTAAMin:             {getVal: LP5EncodeTAAMin},
	LP5SPDIndexTAAMinFineOffset:   {getVal: LP5EncodeTAAMinFineOffset},
	LP5SPDIndexTRCDMin:            {getVal: LP5EncodeTRCDMin},
	LP5SPDIndexTRCDMinFineOffset:  {getVal: LP5EncodeTRCDMinFineOffset},
	LP5SPDIndexTRPABMin:           {getVal: LP5EncodeTRPABMin},
	LP5SPDIndexTRPABMinFineOffset: {getVal: LP5EncodeTRPABMinFineOffset},
	LP5SPDIndexTRPPBMin:           {getVal: LP5EncodeTRPPBMin},
	LP5SPDIndexTRPPBMinFineOffset: {getVal: LP5EncodeTRPPBMinFineOffset},
	LP5SPDIndexTRFCABMinLSB:       {getVal: LP5EncodeTRFCABMinLsb},
	LP5SPDIndexTRFCABMinMSB:       {getVal: LP5EncodeTRFCABMinMsb},
	LP5SPDIndexTRFCPBMinLSB:       {getVal: LP5EncodeTRFCPBMinLsb},
	LP5SPDIndexTRFCPBMinMSB:       {getVal: LP5EncodeTRFCPBMinMsb},
}

/* ------------------------------------------------------------------------------------------ */
/*                                        Functions                                           */
/* ------------------------------------------------------------------------------------------ */

func LP5EncodeDensityBanks(memAttribs *LP5MemAttributes) byte {
	var b byte

	// 3:0 Density per die.
	b = LP5DensityGbToSPDEncoding[memAttribs.DensityPerDieGb].DensityEncoding

	// 5:4 Bank address bits.
	// 7:6 Bank group bits.
	b |= LP5BankGroupsBanks << 4

	return b
}

func LP5EncodeSdramAddressing(memAttribs *LP5MemAttributes) byte {
	var b byte

	// 2:0 Column address bits.
	b = LP5ColumnAddressBits

	// 5:3 Row address bits.
	density := memAttribs.DensityPerDieGb
	var rowAddressBits int
	if memAttribs.BitWidthPerChannel == 8 {
		rowAddressBits = LP5DensityGbToSPDEncoding[density].RowAddressBitsx8Channel
	} else {
		rowAddressBits = LP5DensityGbToSPDEncoding[density].RowAddressBitsx16Channel
	}
	b |= LP5RowAddressBitsEncoding[rowAddressBits] << 3

	return b
}

func LP5EncodePackageType(memAttribs *LP5MemAttributes) byte {
	var b byte

	// 1:0 Signal loading index.
	b = 1

	// 3:2 Channels per package.
	// Channels per package = package width (e.g. x32) / bitWidthPerChannel (x8 or x16).
	// This can equivalently be calculated as diesPerPackage / ranksPerChannel.
	// This calculation is used to avoid adding a redundant attribute for package width.
	channels := memAttribs.DiesPerPackage / memAttribs.RanksPerChannel
	b |= byte(channels>>1) << 2

	// 6:4 Dies per package.
	b |= (byte(memAttribs.DiesPerPackage) - 1) << 4

	// 7:7 Package type.
	var packageType byte
	if memAttribs.DiesPerPackage > 1 {
		packageType = 1 // Non-Monolithic
	} else {
		packageType = 0 // Monolithic
	}
	b |= packageType << 7

	return b
}

func LP5EncodeModuleOrganization(memAttribs *LP5MemAttributes) byte {
	var b byte

	// 2:0 Device data width per channel
	b = byte(memAttribs.BitWidthPerChannel / 8)

	// 5:3 Package ranks per channel
	b |= byte(memAttribs.RanksPerChannel-1) << 3

	return b
}

func LP5EncodeTCKMin(memAttribs *LP5MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TCKMinPs)
}

func LP5EncodeTCKMinFineOffset(memAttribs *LP5MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TCKMinPs)
}

func LP5EncodeTAAMin(memAttribs *LP5MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TAAMinPs)
}

func LP5EncodeTAAMinFineOffset(memAttribs *LP5MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TAAMinPs)
}

func LP5EncodeTRCDMin(memAttribs *LP5MemAttributes) byte {
	return convNsToMtbByte(memAttribs.TRCDMinNs)
}

func LP5EncodeTRCDMinFineOffset(memAttribs *LP5MemAttributes) byte {
	return convNsToFtbByte(memAttribs.TRCDMinNs)
}

func LP5EncodeTRPABMin(memAttribs *LP5MemAttributes) byte {
	return convNsToMtbByte(memAttribs.TRPABMinNs)
}

func LP5EncodeTRPABMinFineOffset(memAttribs *LP5MemAttributes) byte {
	return convNsToFtbByte(memAttribs.TRPABMinNs)
}

func LP5EncodeTRPPBMin(memAttribs *LP5MemAttributes) byte {
	return convNsToMtbByte(memAttribs.TRPPBMinNs)
}

func LP5EncodeTRPPBMinFineOffset(memAttribs *LP5MemAttributes) byte {
	return convNsToFtbByte(memAttribs.TRPPBMinNs)
}

func LP5EncodeTRFCABMinMsb(memAttribs *LP5MemAttributes) byte {
	return byte((convNsToMtb(memAttribs.TRFCABNs) >> 8) & 0xff)
}

func LP5EncodeTRFCABMinLsb(memAttribs *LP5MemAttributes) byte {
	return byte(convNsToMtb(memAttribs.TRFCABNs) & 0xff)
}

func LP5EncodeTRFCPBMinMsb(memAttribs *LP5MemAttributes) byte {
	return byte((convNsToMtb(memAttribs.TRFCPBNs) >> 8) & 0xff)
}

func LP5EncodeTRFCPBMinLsb(memAttribs *LP5MemAttributes) byte {
	return byte(convNsToMtb(memAttribs.TRFCPBNs) & 0xff)
}

func LP5UpdateTCKMin(memAttribs *LP5MemAttributes) {
	if memAttribs.TCKMinPs == 0 {
		memAttribs.TCKMinPs = LP5SpeedMbpsToSPDEncoding[memAttribs.SpeedMbps].TCKMinPs
	}
}

func LP5UpdateTAAMin(memAttribs *LP5MemAttributes) {
	if memAttribs.TAAMinPs == 0 {
		maxCAS := LP5SpeedMbpsToSPDEncoding[memAttribs.SpeedMbps].MaxCASLatency
		memAttribs.TAAMinPs = memAttribs.TCKMinPs * maxCAS
	}
}

func LP5UpdateTRFCAB(memAttribs *LP5MemAttributes) {
	if memAttribs.TRFCABNs == 0 {
		memAttribs.TRFCABNs = LP5DensityGbToSPDEncoding[memAttribs.DensityPerDieGb].TRFCABNs
	}
}

func LP5UpdateTRFCPB(memAttribs *LP5MemAttributes) {
	if memAttribs.TRFCPBNs == 0 {
		memAttribs.TRFCPBNs = LP5DensityGbToSPDEncoding[memAttribs.DensityPerDieGb].TRFCPBNs
	}
}

func LP5UpdateTRCD(memAttribs *LP5MemAttributes) {
	if memAttribs.TRCDMinNs == 0 {
		/* Table 372 from JESD209-5B */
		memAttribs.TRCDMinNs = 18
	}
}

func LP5UpdateTRPAB(memAttribs *LP5MemAttributes) {
	if memAttribs.TRPABMinNs == 0 {
		/* Table 372 from JESD209-5B */
		memAttribs.TRPABMinNs = 21
	}
}

func LP5UpdateTRPPB(memAttribs *LP5MemAttributes) {
	if memAttribs.TRPPBMinNs == 0 {
		/* Table 372 from JESD209-5B */
		memAttribs.TRPPBMinNs = 18
	}
}

func lp5UpdateMemoryAttributes(memAttribs *LP5MemAttributes) {
	LP5UpdateTCKMin(memAttribs)
	LP5UpdateTAAMin(memAttribs)
	LP5UpdateTRFCAB(memAttribs)
	LP5UpdateTRFCPB(memAttribs)
	LP5UpdateTRCD(memAttribs)
	LP5UpdateTRPAB(memAttribs)
	LP5UpdateTRPPB(memAttribs)
}

func LP5ValidateDensity(density int) error {
	if _, ok := LP5DensityGbToSPDEncoding[density]; !ok {
		return fmt.Errorf("Incorrect density per die: %d Gb", density)
	}
	return nil
}

func LP5ValidateDies(dies int) error {
	if dies != 2 && dies != 4 && dies != 8 {
		return fmt.Errorf("Incorrect dies: %d", dies)
	}
	return nil
}

func LP5ValidateDataWidth(width int) error {
	if width != 8 && width != 16 {
		return fmt.Errorf("Incorrect bit width: %d", width)
	}
	return nil
}

func LP5ValidateRanks(ranks int) error {
	if ranks != 1 && ranks != 2 {
		return fmt.Errorf("Incorrect ranks: %d", ranks)
	}
	return nil
}

func LP5ValidateSpeed(speed int) error {
	if _, ok := LP5SpeedMbpsToSPDEncoding[speed]; !ok {
		return fmt.Errorf("Incorrect speed: %d Mbps", speed)
	}
	return nil
}

func lp5ValidateMemPartAttributes(memAttribs *LP5MemAttributes) error {
	if err := LP5ValidateDensity(memAttribs.DensityPerDieGb); err != nil {
		return err
	}
	if err := LP5ValidateDies(memAttribs.DiesPerPackage); err != nil {
		return err
	}
	if err := LP5ValidateDataWidth(memAttribs.BitWidthPerChannel); err != nil {
		return err
	}
	if err := LP5ValidateRanks(memAttribs.RanksPerChannel); err != nil {
		return err
	}
	if err := LP5ValidateSpeed(memAttribs.SpeedMbps); err != nil {
		return err
	}

	return nil
}

func LP5IsManufacturerPartNumberByte(index int) bool {
	if index >= LP5SPDIndexManufacturerPartNumberStartByte &&
		index <= LP5SPDIndexManufacturerPartNumberEndByte {
		return true
	}
	return false
}

/* ------------------------------------------------------------------------------------------ */
/*                                  Interface Functions                                       */
/* ------------------------------------------------------------------------------------------ */

func (lp5) getSetMap() map[int][]int {
	return LP5PlatformSetMap
}

func (lp5) addNewPart(name string, attribs interface{}) error {
	var lp5Attributes LP5MemAttributes
	eByte, err := json.Marshal(attribs)
	if err != nil {
		return err
	}

	if err := json.Unmarshal(eByte, &lp5Attributes); err != nil {
		return err
	}

	if err := lp5ValidateMemPartAttributes(&lp5Attributes); err != nil {
		return err
	}

	LP5PartAttributeMap[name] = lp5Attributes
	return nil
}

func (lp5) getSPDAttribs(name string, set int) (interface{}, error) {
	lp5Attributes := LP5PartAttributeMap[name]

	LP5CurrSet = set

	lp5UpdateMemoryAttributes(&lp5Attributes)

	return lp5Attributes, nil
}

func (lp5) getSPDLen() int {
	return 512
}

func (lp5) getSPDByte(index int, attribs interface{}) byte {
	e, ok := LP5SPDAttribTable[index]
	if !ok {
		if LP5IsManufacturerPartNumberByte(index) {
			return LP5SPDValueManufacturerPartNumberBlank
		}
		return 0x00
	}

	if e.getVal != nil {
		var lp5Attribs LP5MemAttributes
		lp5Attribs = attribs.(LP5MemAttributes)
		return e.getVal(&lp5Attribs)
	}

	return e.constVal
}
