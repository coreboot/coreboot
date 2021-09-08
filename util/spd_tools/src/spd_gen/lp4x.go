/* SPDX-License-Identifier: GPL-2.0-or-later */
package main

import (
	"encoding/json"
	"fmt"
	"strconv"
	"strings"
)

/* ------------------------------------------------------------------------------------------ */
/*                                     LP4x-defined types                                     */
/* ------------------------------------------------------------------------------------------ */

type lp4x struct {
}

type LP4xMemAttributes struct {
	/* Primary attributes - must be provided by JSON file for each part */
	DensityPerChannelGb int
	Banks               int
	ChannelsPerDie      int
	DiesPerPackage      int
	BitWidthPerChannel  int
	RanksPerChannel     int
	SpeedMbps           int

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
	TCKMaxPs   int
	TAAMinPs   int
	TRCDMinNs  int

	/* CAS */
	CASLatencies  string
	CASFirstByte  byte
	CASSecondByte byte
	CASThirdByte  byte
}

type LP4xSpeedParams struct {
	TCKMinPs               int
	TCKMaxPs               int
	CASLatenciesx16Channel string
	CASLatenciesx8Channel  string
}

type LP4xRefreshTimings struct {
	TRFCABNs int
	TRFCPBNs int
}

type LP4xSPDAttribFunc func(*LP4xMemAttributes) byte

type LP4xSPDAttribTableEntry struct {
	constVal byte
	getVal   LP4xSPDAttribFunc
}

type LP4xSetFunc func(*LP4xMemAttributes) int

type LP4xSet struct {
	getMRCDensity     LP4xSetFunc
	getDiesPerPackage LP4xSetFunc
	busWidthEncoding  byte
	normalizeAttribs  LP4xSetFunc
}

/* ------------------------------------------------------------------------------------------ */
/*                                         Constants                                          */
/* ------------------------------------------------------------------------------------------ */

const (
	/* SPD Byte Index */
	LP4xSPDIndexSize                            = 0
	LP4xSPDIndexRevision                        = 1
	LP4xSPDIndexMemoryType                      = 2
	LP4xSPDIndexModuleType                      = 3
	LP4xSPDIndexDensityBanks                    = 4
	LP4xSPDIndexAddressing                      = 5
	LP4xSPDIndexPackageType                     = 6
	LP4xSPDIndexOptionalFeatures                = 7
	LP4xSPDIndexModuleOrganization              = 12
	LP4xSPDIndexBusWidth                        = 13
	LP4xSPDIndexTimebases                       = 17
	LP4xSPDIndexTCKMin                          = 18
	LP4xSPDIndexTCKMax                          = 19
	LP4xSPDIndexCASFirstByte                    = 20
	LP4xSPDIndexCASSecondByte                   = 21
	LP4xSPDIndexCASThirdByte                    = 22
	LP4xSPDIndexCASFourthByte                   = 23
	LP4xSPDIndexTAAMin                          = 24
	LP4xSPDIndexReadWriteLatency                = 25
	LP4xSPDIndexTRCDMin                         = 26
	LP4xSPDIndexTRPABMin                        = 27
	LP4xSPDIndexTRPPBMin                        = 28
	LP4xSPDIndexTRFCABMinLSB                    = 29
	LP4xSPDIndexTRFCABMinMSB                    = 30
	LP4xSPDIndexTRFCPBMinLSB                    = 31
	LP4xSPDIndexTRFCPBMinMSB                    = 32
	LP4xSPDIndexTRPPBMinFineOffset              = 120
	LP4xSPDIndexTRPABMinFineOffset              = 121
	LP4xSPDIndexTRCDMinFineOffset               = 122
	LP4xSPDIndexTAAMinFineOffset                = 123
	LP4xSPDIndexTCKMaxFineOffset                = 124
	LP4xSPDIndexTCKMinFineOffset                = 125
	LP4xSPDIndexManufacturerPartNumberStartByte = 329
	LP4xSPDIndexManufacturerPartNumberEndByte   = 348

	/* SPD Byte Value */

	/*
	 * From JEDEC spec:
	 * 6:4 (Bytes total) = 2 (512 bytes)
	 * 3:0 (Bytes used) = 3 (384 bytes)
	 * Set to 0x23 for LPDDR4x.
	 */
	LP4xSPDValueSize = 0x23

	/*
	 * From JEDEC spec: Revision 1.1
	 * Set to 0x11.
	 */
	LP4xSPDValueRevision = 0x11

	/* LPDDR4x memory type = 0x11 */
	LP4xSPDValueMemoryType = 0x11

	/*
	 * From JEDEC spec:
	 * 7:7 (Hybrid) = 0 (Not hybrid)
	 * 6:4 (Hybrid media) = 000 (Not hybrid)
	 * 3:0 (Base Module Type) = 1110 (Non-DIMM solution)
	 *
	 * This is dependent on hardware design. LPDDR4x only has memory down solution.
	 * Hence this is not hybrid non-DIMM solution.
	 * Set to 0x0E.
	 */
	LP4xSPDValueModuleType = 0x0e

	/*
	 * From JEDEC spec:
	 * 5:4 (Maximum Activate Window) = 00 (8192 * tREFI)
	 * 3:0 (Maximum Activate Count) = 1000 (Unlimited MAC)
	 *
	 * Needs to come from datasheet, but most parts seem to support unlimited MAC.
	 * MR#24 OP3
	 */
	LP4xSPDValueOptionalFeatures = 0x08

	/*
	 * From JEDEC spec:
	 * 3:2 (MTB) = 00 (0.125ns)
	 * 1:0 (FTB) = 00 (1ps)
	 * Set to 0x00.
	 */
	LP4xSPDValueTimebases = 0x00

	/* CAS fourth byte: All bits are reserved */
	LP4xSPDValueCASFourthByte = 0x00

	/* Write Latency Set A and Read Latency DBI-RD disabled. */
	LP4xSPDValueReadWriteLatency = 0x00

	/* As per JEDEC spec, unused digits of manufacturer part number are left as blank. */
	LP4xSPDValueManufacturerPartNumberBlank = 0x20
)

const (
	/* First Byte */
	LP4xCAS6  = 1 << 1
	LP4xCAS10 = 1 << 4
	LP4xCAS14 = 1 << 7
	/* Second Byte */
	LP4xCAS16 = 1 << 0
	LP4xCAS20 = 1 << 2
	LP4xCAS22 = 1 << 3
	LP4xCAS24 = 1 << 4
	LP4xCAS26 = 1 << 5
	LP4xCAS28 = 1 << 6
	/* Third Byte */
	LP4xCAS32 = 1 << 0
	LP4xCAS36 = 1 << 2
	LP4xCAS40 = 1 << 4
)

const (
	/*
	 * JEDEC spec says that TCKmax should be 100ns for all speed grades.
	 * 100ns in MTB units comes out to be 0x320. But since this is a byte field, set it to
	 * 0xFF i.e. 31.875ns.
	 */
	LP4xTCKMaxPsDefault = 31875
)

/* ------------------------------------------------------------------------------------------ */
/*                                    Global variables                                        */
/* ------------------------------------------------------------------------------------------ */

var LP4xPlatformSetMap = map[int][]int{
	0: {PlatformTGL, PlatformADL},
	1: {PlatformJSL, PlatformCZN},
}

var LP4xSetInfo = map[int]LP4xSet{
	0: {
		getMRCDensity:     LP4xGetMRCDensitySet0,
		getDiesPerPackage: LP4xGetDiesPerPackageSet0,
		/*
		 * As per advisory 616599:
		 * 7:5 (Number of system channels) = 000 (1 channel always)
		 * 2:0 (Bus width) = 001 (x16 always)
		 * Set to 0x01.
		 */
		busWidthEncoding: 0x01,
		normalizeAttribs: LP4xNormalizeAttribsSet0,
	},
	1: {
		getMRCDensity:     LP4xGetMRCDensitySet1,
		getDiesPerPackage: LP4xGetDiesPerPackageSet1,
		/*
		* As per advisory 610202:
		* 7:5 (Number of system channels) = 001 (2 channel always)
		* 2:0 (Bus width) = 010 (x32 always)
		* Set to 0x22.
		 */
		busWidthEncoding: 0x22,
	},
}

var LP4xPartAttributeMap = map[string]LP4xMemAttributes{}
var LP4xCurrSet int

/* This encodes the density in Gb to SPD values as per JESD 21-C */
var LP4xDensityGbToSPDEncoding = map[int]byte{
	4:  0x4,
	6:  0xb,
	8:  0x5,
	12: 0x8,
	16: 0x6,
	24: 0x9,
	32: 0x7,
}

/*
 * Table 5 from JESD209-4C.
 * Maps density per physical channel to row-column encoding as per JESD 21-C for a device with
 * x8 physical channel.
 */
var LP4xDensityGbx8ChannelToRowColumnEncoding = map[int]byte{
	3:  0x21, /* 16 rows, 10 columns */
	4:  0x21, /* 16 rows, 10 columns */
	6:  0x29, /* 17 rows, 10 columns */
	8:  0x29, /* 17 rows, 10 columns */
	12: 0x31, /* 18 rows, 10 columns */
	16: 0x31, /* 18 rows, 10 columns */
}

/*
 * Table 3 from JESD209-4C.
 * Maps density per physical channel to row-column encoding as per JESD 21-C for a device with
 * x16 physical channel.
 */
var LP4xDensityGbx16ChannelToRowColumnEncoding = map[int]byte{
	4:  0x19, /* 15 rows, 10 columns */
	6:  0x21, /* 16 rows, 10 columns */
	8:  0x21, /* 16 rows, 10 columns */
	12: 0x29, /* 17 rows, 10 columns */
	16: 0x29, /* 17 rows, 10 columns */
}

/*
 * Table 112 from JESD209-4C
 * Maps density per physical channel to refresh timings. This is the same for x8 and x16
 * devices.
 */
var LP4xDensityGbPhysicalChannelToRefreshEncoding = map[int]LP4xRefreshTimings{
	3: {
		TRFCABNs: 180,
		TRFCPBNs: 90,
	},
	4: {
		TRFCABNs: 180,
		TRFCPBNs: 90,
	},
	6: {
		TRFCABNs: 280,
		TRFCPBNs: 140,
	},
	8: {
		TRFCABNs: 280,
		TRFCPBNs: 140,
	},
	12: {
		TRFCABNs: 380,
		TRFCPBNs: 190,
	},
	16: {
		TRFCABNs: 380,
		TRFCPBNs: 190,
	},
}

var LP4xBankEncoding = map[int]byte{
	4: 0 << 4,
	8: 1 << 4,
}

var LP4xSpeedMbpsToSPDEncoding = map[int]LP4xSpeedParams{
	4267: {
		TCKMinPs:               468, /* 1/4267 * 2 */
		TCKMaxPs:               LP4xTCKMaxPsDefault,
		CASLatenciesx16Channel: "6 10 14 20 24 28 32 36",
		CASLatenciesx8Channel:  "6 10 16 22 26 32 36 40",
	},
	3733: {
		TCKMinPs:               535, /* 1/3733 * 2 */
		TCKMaxPs:               LP4xTCKMaxPsDefault,
		CASLatenciesx16Channel: "6 10 14 20 24 28 32",
		CASLatenciesx8Channel:  "6 10 16 22 26 32 36",
	},
	3200: {
		TCKMinPs:               625, /* 1/3200 * 2 */
		TCKMaxPs:               LP4xTCKMaxPsDefault,
		CASLatenciesx16Channel: "6 10 14 20 24 28",
		CASLatenciesx8Channel:  "6 10 16 22 26 32",
	},
}

var LP4xSPDAttribTable = map[int]LP4xSPDAttribTableEntry{
	LP4xSPDIndexSize:               {constVal: LP4xSPDValueSize},
	LP4xSPDIndexRevision:           {constVal: LP4xSPDValueRevision},
	LP4xSPDIndexMemoryType:         {constVal: LP4xSPDValueMemoryType},
	LP4xSPDIndexModuleType:         {constVal: LP4xSPDValueModuleType},
	LP4xSPDIndexDensityBanks:       {getVal: LP4xEncodeDensityBanks},
	LP4xSPDIndexAddressing:         {getVal: LP4xEncodeSdramAddressing},
	LP4xSPDIndexPackageType:        {getVal: LP4xEncodePackageType},
	LP4xSPDIndexOptionalFeatures:   {constVal: LP4xSPDValueOptionalFeatures},
	LP4xSPDIndexModuleOrganization: {getVal: LP4xEncodeModuleOrganization},
	LP4xSPDIndexBusWidth:           {getVal: LP4xEncodeBusWidth},
	LP4xSPDIndexTimebases:          {constVal: LP4xSPDValueTimebases},
	LP4xSPDIndexTCKMin:             {getVal: LP4xEncodeTCKMin},
	LP4xSPDIndexTCKMax:             {getVal: LP4xEncodeTCKMax},
	LP4xSPDIndexTCKMaxFineOffset:   {getVal: LP4xEncodeTCKMaxFineOffset},
	LP4xSPDIndexTCKMinFineOffset:   {getVal: LP4xEncodeTCKMinFineOffset},
	LP4xSPDIndexCASFirstByte:       {getVal: LP4xEncodeCASFirstByte},
	LP4xSPDIndexCASSecondByte:      {getVal: LP4xEncodeCASSecondByte},
	LP4xSPDIndexCASThirdByte:       {getVal: LP4xEncodeCASThirdByte},
	LP4xSPDIndexCASFourthByte:      {constVal: LP4xSPDValueCASFourthByte},
	LP4xSPDIndexTAAMin:             {getVal: LP4xEncodeTAAMin},
	LP4xSPDIndexTAAMinFineOffset:   {getVal: LP4xEncodeTAAMinFineOffset},
	LP4xSPDIndexReadWriteLatency:   {constVal: LP4xSPDValueReadWriteLatency},
	LP4xSPDIndexTRCDMin:            {getVal: LP4xEncodeTRCDMin},
	LP4xSPDIndexTRCDMinFineOffset:  {getVal: LP4xEncodeTRCDMinFineOffset},
	LP4xSPDIndexTRPABMin:           {getVal: LP4xEncodeTRPABMin},
	LP4xSPDIndexTRPABMinFineOffset: {getVal: LP4xEncodeTRPABMinFineOffset},
	LP4xSPDIndexTRPPBMin:           {getVal: LP4xEncodeTRPPBMin},
	LP4xSPDIndexTRPPBMinFineOffset: {getVal: LP4xEncodeTRPPBMinFineOffset},
	LP4xSPDIndexTRFCABMinLSB:       {getVal: LP4xEncodeTRFCABMinLsb},
	LP4xSPDIndexTRFCABMinMSB:       {getVal: LP4xEncodeTRFCABMinMsb},
	LP4xSPDIndexTRFCPBMinLSB:       {getVal: LP4xEncodeTRFCPBMinLsb},
	LP4xSPDIndexTRFCPBMinMSB:       {getVal: LP4xEncodeTRFCPBMinMsb},
}

/* ------------------------------------------------------------------------------------------ */
/*                                        Functions                                           */
/* ------------------------------------------------------------------------------------------ */

func LP4xGetMRCDensitySet0(memAttribs *LP4xMemAttributes) int {
	/*
	 * Intel MRC on TGL/ADL expects density per logical channel to be encoded in
	 * SPDIndexDensityBanks. Logical channel on TGL/ADL is an x16 channel.
	 */
	return memAttribs.DensityPerChannelGb * 16 / memAttribs.BitWidthPerChannel
}

func LP4xGetMRCDensitySet1(memAttribs *LP4xMemAttributes) int {
	/*
	 * Intel MRC on JSL expects density per die to be encoded in
	 * SPDIndexDensityBanks.
	 */
	return memAttribs.DensityPerChannelGb * memAttribs.ChannelsPerDie
}

func LP4xGetMRCDensity(memAttribs *LP4xMemAttributes) int {
	f, ok := LP4xSetInfo[LP4xCurrSet]

	if ok == false || f.getMRCDensity == nil {
		return 0
	}

	return f.getMRCDensity(memAttribs)
}

func LP4xEncodeDensityBanks(memAttribs *LP4xMemAttributes) byte {
	var b byte

	b = LP4xDensityGbToSPDEncoding[LP4xGetMRCDensity(memAttribs)]
	b |= LP4xBankEncoding[memAttribs.Banks]

	return b
}

func LP4xEncodeSdramAddressing(memAttribs *LP4xMemAttributes) byte {
	densityPerChannelGb := memAttribs.DensityPerChannelGb
	if memAttribs.BitWidthPerChannel == 8 {
		return LP4xDensityGbx8ChannelToRowColumnEncoding[densityPerChannelGb]
	} else {
		return LP4xDensityGbx16ChannelToRowColumnEncoding[densityPerChannelGb]
	}
	return 0
}

func LP4xEncodePackage(dies int) byte {
	var temp byte

	if dies > 1 {
		/* If more than one die, then this is a non-monolithic device. */
		temp = 1
	} else {
		/* If only single die, then this is a monolithic device. */
		temp = 0
	}

	return temp << 7
}

func LP4xEncodeChannelsPerDie(channels int) byte {
	var temp byte

	temp = byte(channels >> 1)

	return temp << 2
}

func LP4xGetDiesPerPackageSet0(memAttribs *LP4xMemAttributes) int {
	/* Intel MRC expects logical dies to be encoded for TGL/ADL. */
	return memAttribs.ChannelsPerDie * memAttribs.RanksPerChannel * memAttribs.BitWidthPerChannel / 16
}

func LP4xGetDiesPerPackageSet1(memAttribs *LP4xMemAttributes) int {
	/* Intel MRC expects physical dies to be encoded for JSL. */
	/* AMD PSP expects physical dies (ZQ balls) */
	return memAttribs.DiesPerPackage
}

/* Per JESD209-4C Dies = ZQ balls on the package */
/* Note that this can be different than the part's die count */
func LP4xEncodeDiesPerPackage(memAttribs *LP4xMemAttributes) byte {
	var dies int = 0

	f, ok := LP4xSetInfo[LP4xCurrSet]
	if ok != false && f.getDiesPerPackage != nil {
		dies = f.getDiesPerPackage(memAttribs)
	}

	b := LP4xEncodePackage(dies) /* Monolithic / Non-monolithic device */
	b |= (byte(dies) - 1) << 4

	return b
}

func LP4xEncodePackageType(memAttribs *LP4xMemAttributes) byte {
	var b byte

	b |= LP4xEncodeChannelsPerDie(memAttribs.ChannelsPerDie)
	b |= LP4xEncodeDiesPerPackage(memAttribs)

	return b
}

func LP4xEncodeDataWidth(bitWidthPerChannel int) byte {
	return byte(bitWidthPerChannel / 8)
}

func LP4xEncodeRanks(ranks int) byte {
	var b byte
	b = byte(ranks - 1)
	return b << 3
}

func LP4xEncodeModuleOrganization(memAttribs *LP4xMemAttributes) byte {
	var b byte

	b = LP4xEncodeDataWidth(memAttribs.BitWidthPerChannel)
	b |= LP4xEncodeRanks(memAttribs.RanksPerChannel)

	return b
}

func LP4xEncodeBusWidth(memAttribs *LP4xMemAttributes) byte {
	f, ok := LP4xSetInfo[LP4xCurrSet]

	if ok == false {
		return 0
	}

	return f.busWidthEncoding
}

func LP4xEncodeTCKMin(memAttribs *LP4xMemAttributes) byte {
	return convPsToMtbByte(memAttribs.TCKMinPs)
}

func LP4xEncodeTCKMinFineOffset(memAttribs *LP4xMemAttributes) byte {
	return convPsToFtbByte(memAttribs.TCKMinPs)
}

func LP4xEncodeTCKMax(memAttribs *LP4xMemAttributes) byte {
	return convPsToMtbByte(memAttribs.TCKMaxPs)
}

func LP4xEncodeTCKMaxFineOffset(memAttribs *LP4xMemAttributes) byte {
	return convPsToFtbByte(memAttribs.TCKMaxPs)
}

func LP4xEncodeCASFirstByte(memAttribs *LP4xMemAttributes) byte {
	return memAttribs.CASFirstByte
}

func LP4xEncodeCASSecondByte(memAttribs *LP4xMemAttributes) byte {
	return memAttribs.CASSecondByte
}

func LP4xEncodeCASThirdByte(memAttribs *LP4xMemAttributes) byte {
	return memAttribs.CASThirdByte
}

func LP4xEncodeTAAMin(memAttribs *LP4xMemAttributes) byte {
	return convPsToMtbByte(memAttribs.TAAMinPs)
}

func LP4xEncodeTAAMinFineOffset(memAttribs *LP4xMemAttributes) byte {
	return convPsToFtbByte(memAttribs.TAAMinPs)
}

func LP4xEncodeTRCDMin(memAttribs *LP4xMemAttributes) byte {
	return convNsToMtbByte(memAttribs.TRCDMinNs)
}

func LP4xEncodeTRCDMinFineOffset(memAttribs *LP4xMemAttributes) byte {
	return convNsToFtbByte(memAttribs.TRCDMinNs)
}

func LP4xEncodeTRPABMin(memAttribs *LP4xMemAttributes) byte {
	return convNsToMtbByte(memAttribs.TRPABMinNs)
}

func LP4xEncodeTRPABMinFineOffset(memAttribs *LP4xMemAttributes) byte {
	return convNsToFtbByte(memAttribs.TRPABMinNs)
}

func LP4xEncodeTRPPBMin(memAttribs *LP4xMemAttributes) byte {
	return convNsToMtbByte(memAttribs.TRPPBMinNs)
}

func LP4xEncodeTRPPBMinFineOffset(memAttribs *LP4xMemAttributes) byte {
	return convNsToFtbByte(memAttribs.TRPPBMinNs)
}

func LP4xEncodeTRFCABMinMsb(memAttribs *LP4xMemAttributes) byte {
	return byte((convNsToMtb(memAttribs.TRFCABNs) >> 8) & 0xff)
}

func LP4xEncodeTRFCABMinLsb(memAttribs *LP4xMemAttributes) byte {
	return byte(convNsToMtb(memAttribs.TRFCABNs) & 0xff)
}

func LP4xEncodeTRFCPBMinMsb(memAttribs *LP4xMemAttributes) byte {
	return byte((convNsToMtb(memAttribs.TRFCPBNs) >> 8) & 0xff)
}

func LP4xEncodeTRFCPBMinLsb(memAttribs *LP4xMemAttributes) byte {
	return byte(convNsToMtb(memAttribs.TRFCPBNs) & 0xff)
}

func LP4xEncodeLatencies(latency int, memAttribs *LP4xMemAttributes) error {
	switch latency {
	case 6:
		memAttribs.CASFirstByte |= LP4xCAS6
	case 10:
		memAttribs.CASFirstByte |= LP4xCAS10
	case 14:
		memAttribs.CASFirstByte |= LP4xCAS14
	case 16:
		memAttribs.CASSecondByte |= LP4xCAS16
	case 20:
		memAttribs.CASSecondByte |= LP4xCAS20
	case 22:
		memAttribs.CASSecondByte |= LP4xCAS22
	case 24:
		memAttribs.CASSecondByte |= LP4xCAS24
	case 26:
		memAttribs.CASSecondByte |= LP4xCAS26
	case 28:
		memAttribs.CASSecondByte |= LP4xCAS28
	case 32:
		memAttribs.CASThirdByte |= LP4xCAS32
	case 36:
		memAttribs.CASThirdByte |= LP4xCAS36
	case 40:
		memAttribs.CASThirdByte |= LP4xCAS40
	default:
		fmt.Errorf("Incorrect CAS Latency: ", latency)
	}

	return nil
}

func LP4xUpdateTCK(memAttribs *LP4xMemAttributes) {
	if memAttribs.TCKMinPs == 0 {
		memAttribs.TCKMinPs = LP4xSpeedMbpsToSPDEncoding[memAttribs.SpeedMbps].TCKMinPs
	}
	if memAttribs.TCKMaxPs == 0 {
		memAttribs.TCKMaxPs = LP4xSpeedMbpsToSPDEncoding[memAttribs.SpeedMbps].TCKMaxPs
	}
}

func LP4xGetCASLatencies(memAttribs *LP4xMemAttributes) string {
	if memAttribs.BitWidthPerChannel == 16 {
		return LP4xSpeedMbpsToSPDEncoding[memAttribs.SpeedMbps].CASLatenciesx16Channel
	} else if memAttribs.BitWidthPerChannel == 8 {
		return LP4xSpeedMbpsToSPDEncoding[memAttribs.SpeedMbps].CASLatenciesx8Channel
	}

	return ""
}

func LP4xUpdateCAS(memAttribs *LP4xMemAttributes) error {
	if len(memAttribs.CASLatencies) == 0 {
		memAttribs.CASLatencies = LP4xGetCASLatencies(memAttribs)
	}

	latencies := strings.Fields(memAttribs.CASLatencies)
	for i := 0; i < len(latencies); i++ {
		latency, err := strconv.Atoi(latencies[i])
		if err != nil {
			return fmt.Errorf("Unable to convert latency ", latencies[i])
		}
		if err := LP4xEncodeLatencies(latency, memAttribs); err != nil {
			return err
		}
	}
	return nil
}

func LP4xGetMinCAS(memAttribs *LP4xMemAttributes) (int, error) {
	if (memAttribs.CASThirdByte & LP4xCAS40) != 0 {
		return 40, nil
	}
	if (memAttribs.CASThirdByte & LP4xCAS36) != 0 {
		return 36, nil
	}
	if (memAttribs.CASThirdByte & LP4xCAS32) != 0 {
		return 32, nil
	}
	if (memAttribs.CASSecondByte & LP4xCAS28) != 0 {
		return 28, nil
	}

	return 0, fmt.Errorf("Unexpected min CAS")
}

func LP4xUpdateTAAMin(memAttribs *LP4xMemAttributes) error {
	if memAttribs.TAAMinPs == 0 {
		minCAS, err := LP4xGetMinCAS(memAttribs)
		if err != nil {
			return err
		}
		memAttribs.TAAMinPs = memAttribs.TCKMinPs * minCAS
	}

	return nil
}

func LP4xUpdateTRFCAB(memAttribs *LP4xMemAttributes) {
	if memAttribs.TRFCABNs == 0 {
		memAttribs.TRFCABNs = LP4xDensityGbPhysicalChannelToRefreshEncoding[memAttribs.DensityPerChannelGb].TRFCABNs
	}
}

func LP4xUpdateTRFCPB(memAttribs *LP4xMemAttributes) {
	if memAttribs.TRFCPBNs == 0 {
		memAttribs.TRFCPBNs = LP4xDensityGbPhysicalChannelToRefreshEncoding[memAttribs.DensityPerChannelGb].TRFCPBNs
	}
}

func LP4xUpdateTRCD(memAttribs *LP4xMemAttributes) {
	if memAttribs.TRCDMinNs == 0 {
		/* JEDEC spec says max of 18ns */
		memAttribs.TRCDMinNs = 18
	}
}

func LP4xUpdateTRPAB(memAttribs *LP4xMemAttributes) {
	if memAttribs.TRPABMinNs == 0 {
		/* JEDEC spec says max of 21ns */
		memAttribs.TRPABMinNs = 21
	}
}

func LP4xUpdateTRPPB(memAttribs *LP4xMemAttributes) {
	if memAttribs.TRPPBMinNs == 0 {
		/* JEDEC spec says max of 18ns */
		memAttribs.TRPPBMinNs = 18
	}
}

func LP4xNormalizeAttribsSet0(memAttribs *LP4xMemAttributes) int {
	/*
	 * TGL does not really use physical organization of dies per package when
	 * generating the SPD. So, set it to 0 here so that deduplication ignores
	 * that field.
	 */
	memAttribs.DiesPerPackage = 0

	return 0
}

func LP4xNormalizeMemoryAttributes(memAttribs *LP4xMemAttributes) {
	f, ok := LP4xSetInfo[LP4xCurrSet]

	if ok == false || f.normalizeAttribs == nil {
		return
	}

	f.normalizeAttribs(memAttribs)
}

func Lp4xUpdateMemoryAttributes(memAttribs *LP4xMemAttributes) error {
	LP4xUpdateTCK(memAttribs)
	if err := LP4xUpdateCAS(memAttribs); err != nil {
		return err
	}
	if err := LP4xUpdateTAAMin(memAttribs); err != nil {
		return err
	}
	LP4xUpdateTRFCAB(memAttribs)
	LP4xUpdateTRFCPB(memAttribs)
	LP4xUpdateTRCD(memAttribs)
	LP4xUpdateTRPAB(memAttribs)
	LP4xUpdateTRPPB(memAttribs)

	LP4xNormalizeMemoryAttributes(memAttribs)

	return nil
}

func LP4xValidateDensityx8Channel(densityPerChannelGb int) error {
	if _, ok := LP4xDensityGbx8ChannelToRowColumnEncoding[densityPerChannelGb]; ok == false {
		return fmt.Errorf("Incorrect x8 density: %d Gb", densityPerChannelGb)
	}
	return nil
}

func LP4xValidateDensityx16Channel(densityPerChannelGb int) error {
	if _, ok := LP4xDensityGbx16ChannelToRowColumnEncoding[densityPerChannelGb]; ok == false {
		return fmt.Errorf("Incorrect x16 density: %d Gb", densityPerChannelGb)
	}
	return nil
}

func LP4xValidateDensity(memAttribs *LP4xMemAttributes) error {
	if memAttribs.BitWidthPerChannel == 8 {
		return LP4xValidateDensityx8Channel(memAttribs.DensityPerChannelGb)
	} else if memAttribs.BitWidthPerChannel == 16 {
		return LP4xValidateDensityx16Channel(memAttribs.DensityPerChannelGb)
	}

	return fmt.Errorf("No density table for this bit width: %d", memAttribs.BitWidthPerChannel)
}

func LP4xValidateBanks(banks int) error {
	if banks != 4 && banks != 8 {
		return fmt.Errorf("Incorrect banks: %d", banks)
	}
	return nil
}

func LP4xValidateChannels(channels int) error {
	if channels != 1 && channels != 2 && channels != 4 {
		return fmt.Errorf("Incorrect channels per die: %d ", channels)
	}
	return nil
}

func LP4xValidateDataWidth(width int) error {
	if width != 8 && width != 16 {
		return fmt.Errorf("Incorrect bit width: %d", width)
	}
	return nil
}

func LP4xValidateRanks(ranks int) error {
	if ranks != 1 && ranks != 2 {
		return fmt.Errorf("Incorrect ranks: %d", ranks)
	}
	return nil
}

func LP4xValidateSpeed(speed int) error {
	if _, ok := LP4xSpeedMbpsToSPDEncoding[speed]; ok == false {
		return fmt.Errorf("Incorrect speed: %d Mbps", speed)
	}
	return nil
}

func Lp4xValidateMemPartAttributes(memAttribs *LP4xMemAttributes) error {
	if err := LP4xValidateBanks(memAttribs.Banks); err != nil {
		return err
	}
	if err := LP4xValidateChannels(memAttribs.ChannelsPerDie); err != nil {
		return err
	}
	if err := LP4xValidateDataWidth(memAttribs.BitWidthPerChannel); err != nil {
		return err
	}
	if err := LP4xValidateDensity(memAttribs); err != nil {
		return err
	}
	if err := LP4xValidateRanks(memAttribs.RanksPerChannel); err != nil {
		return err
	}
	if err := LP4xValidateSpeed(memAttribs.SpeedMbps); err != nil {
		return err
	}

	return nil
}

func LP4xIsManufacturerPartNumberByte(index int) bool {
	if index >= LP4xSPDIndexManufacturerPartNumberStartByte &&
		index <= LP4xSPDIndexManufacturerPartNumberEndByte {
		return true
	}
	return false
}

/* ------------------------------------------------------------------------------------------ */
/*                                 Interface Functions                                       */
/* ------------------------------------------------------------------------------------------ */

func (lp4x) getSetMap() map[int][]int {
	return LP4xPlatformSetMap
}

func (lp4x) addNewPart(name string, attribs interface{}) error {
	var lp4xAttributes LP4xMemAttributes
	eByte, err := json.Marshal(attribs)
	if err != nil {
		return err
	}

	if err := json.Unmarshal(eByte, &lp4xAttributes); err != nil {
		return err
	}

	if err := Lp4xValidateMemPartAttributes(&lp4xAttributes); err != nil {
		return err
	}

	LP4xPartAttributeMap[name] = lp4xAttributes
	return nil
}

func (lp4x) getSPDAttribs(name string, set int) (interface{}, error) {
	lp4xAttributes := LP4xPartAttributeMap[name]

	LP4xCurrSet = set

	if err := Lp4xUpdateMemoryAttributes(&lp4xAttributes); err != nil {
		return lp4xAttributes, err
	}

	return lp4xAttributes, nil
}

func (lp4x) getSPDLen() int {
	return 512
}

func (lp4x) getSPDByte(index int, attribs interface{}) byte {
	e, ok := LP4xSPDAttribTable[index]
	if ok == false {
		if LP4xIsManufacturerPartNumberByte(index) {
			return LP4xSPDValueManufacturerPartNumberBlank
		}
		return 0x00
	}

	if e.getVal != nil {
		var lp4xAttribs LP4xMemAttributes
		lp4xAttribs = attribs.(LP4xMemAttributes)
		return e.getVal(&lp4xAttribs)
	}

	return e.constVal
}
