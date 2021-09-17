/* SPDX-License-Identifier: GPL-2.0-or-later */
package main

import (
	"encoding/json"
	"fmt"
	"strconv"
	"strings"
)

/* ------------------------------------------------------------------------------------------ */
/*                                     DDR4-defined types                                     */
/* ------------------------------------------------------------------------------------------ */

type ddr4 struct {
}

type DDR4MemAttributes struct {
	/* Primary attributes - must be provided by JSON file for each part */
	SpeedMTps        int
	CL_nRCD_nRP      int
	CapacityPerDieGb int
	DiesPerPackage   int
	PackageBusWidth  int
	RanksPerPackage  int

	/*
	 * All the following parameters are optional and required only if the part requires
	 * special parameters as per the datasheet.
	 */
	/* Timing parameters */
	TAAMinPs   int
	TRCDMinPs  int
	TRPMinPs   int
	TRASMinPs  int
	TRCMinPs   int
	TCKMinPs   int
	TCKMaxPs   int
	TRFC1MinPs int
	TRFC2MinPs int
	TRFC4MinPs int
	TFAWMinPs  int
	TRRDLMinPs int
	TRRDSMinPs int
	TCCDLMinPs int
	TWRMinPs   int
	TWTRLMinPs int
	TWTRSMinPs int

	/* CAS */
	CASLatencies  string
	CASFirstByte  byte
	CASSecondByte byte
	CASThirdByte  byte
	CASFourthByte byte

	/* The following is for internal-use only and is not overridable */
	dieBusWidth int
}

type DDR4SpeedBinAttributes struct {
	TRASMinPs int
	TCKMaxPs  int
}

type DDR4SPDMemAttribFunc func(*DDR4MemAttributes) byte

type DDR4SPDAttribTableEntry struct {
	constVal byte
	getVal   DDR4SPDMemAttribFunc
}

/* ------------------------------------------------------------------------------------------ */
/*                                         Constants                                          */
/* ------------------------------------------------------------------------------------------ */

const (
	/* SPD Byte Index */
	DDR4SPDIndexSize                            = 0
	DDR4SPDIndexRevision                        = 1
	DDR4SPDIndexMemoryType                      = 2
	DDR4SPDIndexModuleType                      = 3
	DDR4SPDIndexDensityBanks                    = 4
	DDR4SPDIndexAddressing                      = 5
	DDR4SPDIndexPackageType                     = 6
	DDR4SPDIndexOptionalFeatures                = 7
	DDR4SPDIndexModuleOrganization              = 12
	DDR4SPDIndexBusWidth                        = 13
	DDR4SPDIndexTimebases                       = 17
	DDR4SPDIndexTCKMin                          = 18
	DDR4SPDIndexTCKMax                          = 19
	DDR4SPDIndexCASFirstByte                    = 20
	DDR4SPDIndexCASSecondByte                   = 21
	DDR4SPDIndexCASThirdByte                    = 22
	DDR4SPDIndexCASFourthByte                   = 23
	DDR4SPDIndexTAAMin                          = 24
	DDR4SPDIndexTRCDMin                         = 25
	DDR4SPDIndexTRPMin                          = 26
	DDR4SPDIndexTRASRCMinMSNs                   = 27
	DDR4SPDIndexTRASMinLsb                      = 28
	DDR4SPDIndexTRCMinLsb                       = 29
	DDR4SPDIndexTRFC1MinLsb                     = 30
	DDR4SPDIndexTRFC1MinMsb                     = 31
	DDR4SPDIndexTRFC2MinLsb                     = 32
	DDR4SPDIndexTRFC2MinMsb                     = 33
	DDR4SPDIndexTRFC4MinLsb                     = 34
	DDR4SPDIndexTRFC4MinMsb                     = 35
	DDR4SPDIndexTFAWMinMSN                      = 36
	DDR4SPDIndexTFAWMinLsb                      = 37
	DDR4SPDIndexTRRDSMin                        = 38
	DDR4SPDIndexTRRDLMin                        = 39
	DDR4SPDIndexTCCDLMin                        = 40
	DDR4SPDIndexTWRMinMSN                       = 41
	DDR4SPDIndexTWRMinLsb                       = 42
	DDR4SPDIndexTWTRMinMSNs                     = 43
	DDR4SPDIndexWTRSMinLsb                      = 44
	DDR4SPDIndexWTRLMinLsb                      = 45
	DDR4SPDIndexTCCDLMinFineOffset              = 117
	DDR4SPDIndexTRRDLMinFineOffset              = 118
	DDR4SPDIndexTRRDSMinFineOffset              = 119
	DDR4SPDIndexTRCMinFineOffset                = 120
	DDR4SPDIndexTRPMinFineOffset                = 121
	DDR4SPDIndexTRCDMinFineOffset               = 122
	DDR4SPDIndexTAAMinFineOffset                = 123
	DDR4SPDIndexTCKMaxFineOffset                = 124
	DDR4SPDIndexTCKMinFineOffset                = 125
	DDR4SPDIndexManufacturerPartNumberStartByte = 329
	DDR4SPDIndexManufacturerPartNumberEndByte   = 348

	/* SPD Byte Value */

	/*
	 * From JEDEC spec:
	 * 6:4 (Bytes total) = 2 (512 bytes)
	 * 3:0 (Bytes used) = 3 (384 bytes)
	 * Set to 0x23 for DDR4.
	 */
	DDR4SPDValueSize = 0x23

	/*
	 * From JEDEC spec: Revision 1.1
	 * Set to 0x11.
	 */
	DDR4SPDValueRevision = 0x11

	/* DDR4 memory type = 0x0C */
	DDR4SPDValueMemoryType = 0x0C

	/*
	 * From JEDEC spec:
	 * Module Type [0:3] :
	 *  0 = Undefined
	 *  1 = RDIMM (width = 133.35 mm nom)
	 *  2 = UDIMM (width = 133.35 mm nom)
	 *  3 = SO-DIMM (width = 68.60 mm nom)
	 *  4 = LRDIMM (width = 133.35 mm nom)
	 *
	 * DDR4 on TGL uses SO-DIMM type for for both memory down and DIMM config.
	 * Set to 0x03.
	 */
	DDR4SPDValueModuleType = 0x03

	/*
	 * From JEDEC spec:
	 * 5:4 (Maximum Activate Window) = 00 (8192 * tREFI)
	 * 3:0 (Maximum Activate Count) = 1000 (Unlimited MAC)
	 *
	 * Needs to come from datasheet, but most parts seem to support unlimited MAC.
	 * MR#24 OP3
	 */
	DDR4SPDValueOptionalFeatures = 0x08

	/*
	 * From JEDEC spec:
	 * 2:0 Primary Bus Width in Bits = 011 (x64 always)
	 * Set to 0x03.
	 */
	DDR4SPDValueModuleBusWidth = 0x03

	/*
	 * From JEDEC spec:
	 * 3:2 (MTB) = 00 (0.125ns)
	 * 1:0 (FTB) = 00 (1ps)
	 * Set to 0x00.
	 */
	DDR4SPDValueTimebases = 0x00

	/* CAS fourth byte: All bits are reserved */
	DDR4SPDValueCASFourthByte = 0x00

	/* As per JEDEC spec, unused digits of manufacturer part number are left as blank. */
	DDR4SPDValueManufacturerPartNumberBlank = 0x20
)

const (
	/* First Byte */
	DDR4CAS9  = 1 << 2
	DDR4CAS10 = 1 << 3
	DDR4CAS11 = 1 << 4
	DDR4CAS12 = 1 << 5
	DDR4CAS13 = 1 << 6
	DDR4CAS14 = 1 << 7
	/* Second Byte */
	DDR4CAS15 = 1 << 0
	DDR4CAS16 = 1 << 1
	DDR4CAS17 = 1 << 2
	DDR4CAS18 = 1 << 3
	DDR4CAS19 = 1 << 4
	DDR4CAS20 = 1 << 5
	DDR4CAS21 = 1 << 6
	DDR4CAS22 = 1 << 7
	/* Third Byte */
	DDR4CAS24 = 1 << 1
)

const (
	/*
	 * As per Table 75 of Jedec spec 4.1.20-L-5 R29 v103:
	 * tWRMin = 15nS for all DDR4 Speed Bins
	 * Set to 15000 pS
	 */
	DDR4TimingValueTWRMinPs = 15000

	/*
	 * As per Table 78 of Jedec spec 4.1.20-L-5 R29 v103:
	 * tWTR_SMin = 2.5nS for all DDR4 Speed Bins
	 * Set to 2500 pS
	 */
	DDR4TimingValueTWTRSMinPs = 2500

	/*
	 * As per Table 80 of Jedec spec 4.1.20-L-5 R29 v103:
	 * tWTR_LMin = 7.5 nS for all DDR4 Speed Bins
	 * Set to 7500 pS
	 */
	DDR4TimingValueTWTRLMinPs = 7500
)

/* ------------------------------------------------------------------------------------------ */
/*                                    Global variables                                        */
/* ------------------------------------------------------------------------------------------ */

var DDR4PlatformSetMap = map[int][]int{
	0: {PlatformTGL, PlatformPCO},
}

var DDR4PartAttributeMap = map[string]DDR4MemAttributes{}
var DDR4CurrSet int

/* This encodes the density in Gb to SPD low nibble value as per JESD 4.1.2.L-5 R29 */
var DDR4DensityGbToSPDEncoding = map[int]byte{
	2:  0x3,
	4:  0x4,
	8:  0x5,
	16: 0x6,
}

/*
 * Tables 4 thru Table 7 from JESD79-4C.
 * Maps density per die to row-column encoding for a device with x8/x16
 * physical channel.
 */
var DDR4DensityGbx8x16DieCapacityToRowColumnEncoding = map[int]byte{
	2:  0x11, /* 14 rows, 10 columns */
	4:  0x19, /* 15 rows, 10 columns */
	8:  0x21, /* 16 rows, 10 columns */
	16: 0x29, /* 17 rows, 10 columns */
}

/*
 * Tables 169 & 170 in the JESD79-4C spec
 * Maps die density to refresh timings. This is the same for x8 and x16
 * devices.
 */

/* maps die density to rcf1 timing in pico seconds */
var DDR4TRFC1Encoding = map[int]int{
	2:  160000,
	4:  260000,
	8:  350000,
	16: 550000,
}

/* maps die density to rcf2 timing in pico seconds */
var DDR4TRFC2Encoding = map[int]int{
	2:  110000,
	4:  160000,
	8:  260000,
	16: 350000,
}

/* maps die density to rcf4 timing in pico seconds */
var DDR4TRFC4Encoding = map[int]int{
	2:  90000,
	4:  110000,
	8:  160000,
	16: 260000,
}

var DDR4SpeedBinToSPDEncoding = map[int]DDR4SpeedBinAttributes{
	1600: {
		TRASMinPs: 35000,
		TCKMaxPs:  1500,
	},
	1866: {
		TRASMinPs: 34000,
		TCKMaxPs:  1250,
	},
	2133: {
		TRASMinPs: 33000,
		TCKMaxPs:  1071,
	},
	2400: {
		TRASMinPs: 32000,
		TCKMaxPs:  937,
	},
	2666: {
		TRASMinPs: 32000,
		TCKMaxPs:  833,
	},
	2933: {
		TRASMinPs: 32000,
		TCKMaxPs:  750,
	},
	3200: {
		TRASMinPs: 32000,
		TCKMaxPs:  682,
	},
}

/* This takes memAttribs.PackageBusWidth as an index */
var DDR4PageSizefromBusWidthEncoding = map[int]int{
	8:  1,
	16: 2,
}

var DDR4SpeedToTRRDSMinPsOneKPageSize = map[int]int{
	1600: 5000,
	1866: 4200,
	2133: 3700,
	2400: 3300,
	2666: 3000,
	2933: 2700,
	3200: 2500,
}

var DDR4SpeedToTRRDSMinPsTwoKPageSize = map[int]int{
	1600: 6000,
	1866: 5300,
	2133: 5300,
	2400: 5300,
	2666: 5300,
	2933: 5300,
	3200: 5300,
}

var DDR4SPDAttribTable = map[int]DDR4SPDAttribTableEntry{
	DDR4SPDIndexSize:               {constVal: DDR4SPDValueSize},
	DDR4SPDIndexRevision:           {constVal: DDR4SPDValueRevision},
	DDR4SPDIndexMemoryType:         {constVal: DDR4SPDValueMemoryType},
	DDR4SPDIndexModuleType:         {constVal: DDR4SPDValueModuleType},
	DDR4SPDIndexDensityBanks:       {getVal: DDR4EncodeDensityBanks},
	DDR4SPDIndexAddressing:         {getVal: DDR4EncodeSdramAddressing},
	DDR4SPDIndexPackageType:        {getVal: DDR4EncodePackageType},
	DDR4SPDIndexOptionalFeatures:   {constVal: DDR4SPDValueOptionalFeatures},
	DDR4SPDIndexModuleOrganization: {getVal: DDR4EncodeModuleOrganization},
	DDR4SPDIndexBusWidth:           {constVal: DDR4SPDValueModuleBusWidth},
	DDR4SPDIndexTimebases:          {constVal: DDR4SPDValueTimebases},
	DDR4SPDIndexTCKMin:             {getVal: DDR4EncodeTCKMin},
	DDR4SPDIndexTCKMinFineOffset:   {getVal: DDR4EncodeTCKMinFineOffset},
	DDR4SPDIndexTCKMax:             {getVal: DDR4EncodeTCKMax},
	DDR4SPDIndexTCKMaxFineOffset:   {getVal: DDR4EncodeTCKMaxFineOffset},
	DDR4SPDIndexCASFirstByte:       {getVal: DDR4EncodeCASFirstByte},
	DDR4SPDIndexCASSecondByte:      {getVal: DDR4EncodeCASSecondByte},
	DDR4SPDIndexCASThirdByte:       {getVal: DDR4EncodeCASThirdByte},
	DDR4SPDIndexCASFourthByte:      {getVal: DDR4EncodeCASFourthByte},
	DDR4SPDIndexTAAMin:             {getVal: DDR4EncodeTAAMin},
	DDR4SPDIndexTAAMinFineOffset:   {getVal: DDR4EncodeTAAMinFineOffset},
	DDR4SPDIndexTRCDMin:            {getVal: DDR4EncodeTRCDMin},
	DDR4SPDIndexTRCDMinFineOffset:  {getVal: DDR4EncodeTRCDMinFineOffset},
	DDR4SPDIndexTRPMin:             {getVal: DDR4EncodeTRPMin},
	DDR4SPDIndexTRPMinFineOffset:   {getVal: DDR4EncodeTRPMinFineOffset},
	DDR4SPDIndexTRASRCMinMSNs:      {getVal: DDR4EncodeTRASRCMinMSNs},
	DDR4SPDIndexTRASMinLsb:         {getVal: DDR4EncodeTRASMinLsb},
	DDR4SPDIndexTRCMinLsb:          {getVal: DDR4EncodeTRCMinLsb},
	DDR4SPDIndexTRCMinFineOffset:   {getVal: DDR4EncodeTRCMinFineOffset},
	DDR4SPDIndexTRFC1MinLsb:        {getVal: DDR4EncodeTRFC1MinLsb},
	DDR4SPDIndexTRFC1MinMsb:        {getVal: DDR4EncodeTRFC1MinMsb},
	DDR4SPDIndexTRFC2MinLsb:        {getVal: DDR4EncodeTRFC2MinLsb},
	DDR4SPDIndexTRFC2MinMsb:        {getVal: DDR4EncodeTRFC2MinMsb},
	DDR4SPDIndexTRFC4MinLsb:        {getVal: DDR4EncodeTRFC4MinLsb},
	DDR4SPDIndexTRFC4MinMsb:        {getVal: DDR4EncodeTRFC4MinMsb},
	DDR4SPDIndexTFAWMinMSN:         {getVal: DDR4EncodeTFAWMinMSN},
	DDR4SPDIndexTFAWMinLsb:         {getVal: DDR4EncodeTFAWMinLsb},
	DDR4SPDIndexTRRDSMin:           {getVal: DDR4EncodeTRRDSMin},
	DDR4SPDIndexTRRDSMinFineOffset: {getVal: DDR4EncodeTRRDSMinFineOffset},
	DDR4SPDIndexTRRDLMin:           {getVal: DDR4EncodeTRRDLMin},
	DDR4SPDIndexTRRDLMinFineOffset: {getVal: DDR4EncodeTRRDLMinFineOffset},
	DDR4SPDIndexTCCDLMin:           {getVal: DDR4EncodeTCCDLMin},
	DDR4SPDIndexTCCDLMinFineOffset: {getVal: DDR4EncodeTCCDLMinFineOffset},
	DDR4SPDIndexTWRMinMSN:          {getVal: DDR4EncodeTWRMinMSN},
	DDR4SPDIndexTWRMinLsb:          {getVal: DDR4EncodeTWRMinLsb},
	DDR4SPDIndexTWTRMinMSNs:        {getVal: DDR4EncodeTWTRMinMSNs},
	DDR4SPDIndexWTRSMinLsb:         {getVal: DDR4EncodeTWTRSMinLsb},
	DDR4SPDIndexWTRLMinLsb:         {getVal: DDR4EncodeTWTRLMinLsb},
}

/* ------------------------------------------------------------------------------------------ */
/*                                        Functions                                           */
/* ------------------------------------------------------------------------------------------ */

func DDR4GetBankGroups(memAttribs *DDR4MemAttributes) byte {
	var bg byte

	switch memAttribs.PackageBusWidth {
	case 8:
		bg = 4
	case 16:
		if memAttribs.DiesPerPackage == 1 {
			bg = 2 /* x16 SDP has 2 bank groups */
		} else {
			bg = 4 /* x16 DDP has 4 bank groups */
		}
	}

	return bg
}

func DDR4EncodeBankGroups(bg byte) byte {
	var val byte

	switch bg {
	case 2:
		val = 1
	case 4:
		val = 2
	}

	return val << 6
}

func DDR4EncodeDensityBanks(memAttribs *DDR4MemAttributes) byte {
	var b byte

	b = DDR4DensityGbToSPDEncoding[memAttribs.CapacityPerDieGb]
	b |= DDR4EncodeBankGroups(DDR4GetBankGroups(memAttribs))
	/* No need to encode banksPerGroup.it's always 4 ([4:5] = 0) */

	return b
}

func DDR4EncodeSdramAddressing(memAttribs *DDR4MemAttributes) byte {
	var b byte

	b = DDR4DensityGbx8x16DieCapacityToRowColumnEncoding[memAttribs.CapacityPerDieGb]

	return b
}

func DDR4EncodePackageDeviceType(dies int) byte {
	var b byte

	if dies > 1 {
		/* If more than one die, then this is a non-monolithic device. */
		b = 1
	} else {
		/* If only single die, then this is a monolithic device. */
		b = 0
	}

	return b << 7
}

func DDR4EncodeSignalLoadingFromDieCount(dies int) byte {
	var loading byte

	/*
	 * If die count = 1, signal loading = "not specified" = 0
	 * If die count > 1, signal loading = "multi" = 2
	 */
	if dies == 1 {
		loading = 0
	} else {
		loading = 1
	}

	return loading
}

func DDR4EncodeDiesPerPackage(dies int) byte {
	var b byte

	b = DDR4EncodePackageDeviceType(dies) /* Monolithic / Non-monolithic device */
	b |= (byte(dies) - 1) << 4

	return b
}

func DDR4EncodePackageType(memAttribs *DDR4MemAttributes) byte {
	var b byte

	b = DDR4EncodeDiesPerPackage(memAttribs.DiesPerPackage)
	b |= DDR4EncodeSignalLoadingFromDieCount(memAttribs.DiesPerPackage)

	return b
}

func DDR4EncodeDataWidth(bitWidthPerDevice int) byte {
	var width byte

	switch bitWidthPerDevice {
	case 8:
		width = 1
	case 16:
		width = 2
	}

	return width
}

func DDR4EncodeRanks(ranks int) byte {
	var b byte

	b = byte(ranks - 1)

	return b << 3
}

func DDR4EncodeModuleOrganization(memAttribs *DDR4MemAttributes) byte {
	var b byte

	b = DDR4EncodeDataWidth(memAttribs.dieBusWidth)
	b |= DDR4EncodeRanks(memAttribs.RanksPerPackage)

	return b
}

func DDR4EncodeTCKMin(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TCKMinPs)
}

func DDR4EncodeTCKMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TCKMinPs)
}

func DDR4EncodeTCKMax(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TCKMaxPs)
}

func DDR4EncodeTCKMaxFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TCKMaxPs)
}

func DDR4EncodeTAAMin(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TAAMinPs)
}

func DDR4EncodeTAAMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TAAMinPs)
}

func DDR4EncodeTRCDMin(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TRCDMinPs)
}

func DDR4EncodeTRCDMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TRCDMinPs)
}

func DDR4EncodeTRPMin(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TRPMinPs)
}

func DDR4EncodeTRCMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TRCMinPs)
}

func DDR4EncodeTRPMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TRPMinPs)
}

func DDR4EncodeTRASRCMinMSNs(memAttribs *DDR4MemAttributes) byte {
	var b byte

	b = byte((convPsToMtb(memAttribs.TRASMinPs) >> 4) & 0xf0)
	b |= byte((convPsToMtb(memAttribs.TRCMinPs) >> 8) & 0x0f)

	return b
}

func DDR4EncodeTRASMinLsb(memAttribs *DDR4MemAttributes) byte {
	return byte(convPsToMtb(memAttribs.TRASMinPs) & 0xff)
}

func DDR4EncodeTRCMinLsb(memAttribs *DDR4MemAttributes) byte {
	return byte(convPsToMtb(memAttribs.TRCMinPs) & 0xff)
}

func DDR4EncodeTRFC1MinLsb(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC1MinPs)

	return byte(mtb & 0xff)
}

func DDR4EncodeTRFC1MinMsb(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC1MinPs)

	return byte((mtb >> 8) & 0xff)
}

func DDR4EncodeTRFC2MinLsb(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC2MinPs)

	return byte(mtb & 0xff)
}

func DDR4EncodeTRFC2MinMsb(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC2MinPs)

	return byte((mtb >> 8) & 0xff)
}

func DDR4EncodeTRFC4MinLsb(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC4MinPs)

	return byte(mtb & 0xff)
}

func DDR4EncodeTRFC4MinMsb(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TRFC4MinPs)

	return byte((mtb >> 8) & 0xff)
}

func DDR4EncodeTFAWMinMSN(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TFAWMinPs)

	return byte((mtb >> 8) & 0x0f)
}

func DDR4EncodeTFAWMinLsb(memAttribs *DDR4MemAttributes) byte {
	var mtb int

	mtb = convPsToMtb(memAttribs.TFAWMinPs)

	return byte(mtb & 0xff)
}

func DDR4EncodeCASFirstByte(memAttribs *DDR4MemAttributes) byte {
	return memAttribs.CASFirstByte
}

func DDR4EncodeCASSecondByte(memAttribs *DDR4MemAttributes) byte {
	return memAttribs.CASSecondByte
}

func DDR4EncodeCASThirdByte(memAttribs *DDR4MemAttributes) byte {
	return memAttribs.CASThirdByte
}

func DDR4EncodeCASFourthByte(memAttribs *DDR4MemAttributes) byte {
	return memAttribs.CASFourthByte
}

func DDR4EncodeTRRDSMin(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TRRDSMinPs)
}

func DDR4EncodeTRRDSMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TRRDSMinPs)
}

func DDR4EncodeTRRDLMin(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TRRDLMinPs)
}

func DDR4EncodeTRRDLMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TRRDLMinPs)
}

func DDR4EncodeTCCDLMin(memAttribs *DDR4MemAttributes) byte {
	return convPsToMtbByte(memAttribs.TCCDLMinPs)
}

func DDR4EncodeTCCDLMinFineOffset(memAttribs *DDR4MemAttributes) byte {
	return convPsToFtbByte(memAttribs.TCCDLMinPs)
}

func DDR4EncodeTWRMinMSN(memAttribs *DDR4MemAttributes) byte {
	return byte((convPsToMtb(DDR4TimingValueTWRMinPs) >> 8) & 0x0f)
}

func DDR4EncodeTWRMinLsb(memAttribs *DDR4MemAttributes) byte {
	return byte(convPsToMtb(DDR4TimingValueTWRMinPs) & 0xff)
}

func DDR4EncodeTWTRMinMSNs(memAttribs *DDR4MemAttributes) byte {
	var b byte

	b = byte((convPsToMtb(memAttribs.TWTRLMinPs) >> 4) & 0xf0)
	b |= byte((convPsToMtb(memAttribs.TWTRSMinPs) >> 8) & 0x0f)

	return b
}

func DDR4EncodeTWTRSMinLsb(memAttribs *DDR4MemAttributes) byte {
	return byte(convPsToMtb(memAttribs.TWTRSMinPs) & 0xff)
}

func DDR4EncodeTWTRLMinLsb(memAttribs *DDR4MemAttributes) byte {
	return byte(convPsToMtb(memAttribs.TWTRLMinPs) & 0xff)
}

func DDR4EncodeLatencies(latency int, memAttribs *DDR4MemAttributes) error {
	switch latency {
	case 9:
		memAttribs.CASFirstByte |= DDR4CAS9
	case 10:
		memAttribs.CASFirstByte |= DDR4CAS10
	case 11:
		memAttribs.CASFirstByte |= DDR4CAS11
	case 12:
		memAttribs.CASFirstByte |= DDR4CAS12
	case 13:
		memAttribs.CASFirstByte |= DDR4CAS13
	case 14:
		memAttribs.CASFirstByte |= DDR4CAS14
	case 15:
		memAttribs.CASSecondByte |= DDR4CAS15
	case 16:
		memAttribs.CASSecondByte |= DDR4CAS16
	case 17:
		memAttribs.CASSecondByte |= DDR4CAS17
	case 18:
		memAttribs.CASSecondByte |= DDR4CAS18
	case 19:
		memAttribs.CASSecondByte |= DDR4CAS19
	case 20:
		memAttribs.CASSecondByte |= DDR4CAS20
	case 21:
		memAttribs.CASSecondByte |= DDR4CAS21
	case 22:
		memAttribs.CASSecondByte |= DDR4CAS22
	case 24:
		memAttribs.CASThirdByte |= DDR4CAS24
	default:
		fmt.Errorf("Incorrect CAS Latency: ", latency)
	}

	return nil
}

/* Default CAS Latencies from Speed Bin tables in JEDS79-4C */
func DDR4GetDefaultCASLatencies(memAttribs *DDR4MemAttributes) string {
	var str string

	switch memAttribs.SpeedMTps {
	case 1600:
		switch memAttribs.CL_nRCD_nRP {
		case 10:
			str = "9 10 11 12"
		case 11:
			str = "9 11 12"
		case 12:
			str = "10 12"
		}
	case 1866:
		switch memAttribs.CL_nRCD_nRP {
		case 12:
			str = "9 10 12 13 14"
		case 13:
			str = "9 11 12 13 14"
		case 14:
			str = "10 12 14"
		}
	case 2133:
		switch memAttribs.CL_nRCD_nRP {
		case 14:
			str = "9 10 12 14 15 16"
		case 15:
			str = "9 11 12 13 14 15 16"
		case 16:
			str = "10 12 14 16"
		}
	case 2400:
		switch memAttribs.CL_nRCD_nRP {
		case 15:
			str = "9 10 12 14 15 16 17 18"
		case 16:
			str = "9 11 12 13 14 15 16 17 18"
		case 17:
			str = "10 11 12 13 14 15 16 17 18"
		case 18:
			str = "10 12 14 16 18"
		}
	case 2666:
		switch memAttribs.CL_nRCD_nRP {
		case 17:
			str = "9 10 11 12 13 14 15 16 17 18 19 20"
		case 18:
			str = "9 10 11 12 13 14 15 16 17 18 19 20"
		case 19:
			str = "10 11 12 13 14 15 16 17 18 19 20"
		case 20:
			str = "10 12 14 16 18 20"
		}
	case 2933:
		switch memAttribs.CL_nRCD_nRP {
		case 19:
			str = "9 10 11 12 13 14 15 16 17 18 19 20 21 22"
		case 20:
			str = "10 11 12 13 14 15 16 17 18 19 20 21 22"
		case 21:
			str = "10 11 12 13 14 15 16 17 18 19 20 21 22"
		case 22:
			str = "10 12 14 16 18 20 22"
		}
	case 3200:
		switch memAttribs.CL_nRCD_nRP {
		case 20:
			str = "9 10 11 12 13 14 15 16 17 18 19 20 21 22 24"
		case 22:
			str = "10 11 12 13 14 15 16 17 18 19 20 21 22 24"
		case 24:
			str = "10 12 14 16 18 20 22 24"
		}
	}

	return str
}

func DDR4UpdateDieBusWidth(memAttribs *DDR4MemAttributes) {
	if memAttribs.PackageBusWidth == 16 && memAttribs.RanksPerPackage == 1 &&
		memAttribs.DiesPerPackage == 2 {
		/*
		 * If a x16 part has 2 die with single rank, PackageBusWidth
		 * needs to be converted to match die bus width.
		 */
		memAttribs.dieBusWidth = 8
	} else {
		memAttribs.dieBusWidth = memAttribs.PackageBusWidth
	}
}

func DDR4UpdateCAS(memAttribs *DDR4MemAttributes) error {
	if len(memAttribs.CASLatencies) == 0 {
		memAttribs.CASLatencies = DDR4GetDefaultCASLatencies(memAttribs)
	}

	latencies := strings.Fields(memAttribs.CASLatencies)
	for i := 0; i < len(latencies); i++ {
		latency, err := strconv.Atoi(latencies[i])
		if err != nil {
			return fmt.Errorf("Unable to convert latency ", latencies[i])
		}
		if err := DDR4EncodeLatencies(latency, memAttribs); err != nil {
			return err
		}
	}

	return nil
}

func DDR4GetTAAMinPs(memAttribs *DDR4MemAttributes) int {
	return (memAttribs.CL_nRCD_nRP * 2000000) / memAttribs.SpeedMTps
}

func DDR4UpdateTAAMin(memAttribs *DDR4MemAttributes) {
	if memAttribs.TAAMinPs == 0 {
		memAttribs.TAAMinPs = DDR4GetTAAMinPs(memAttribs)
	}
}

func DDR4UpdateTRCDMin(memAttribs *DDR4MemAttributes) {
	/* tRCDmin is same as tAAmin for all cases */
	if memAttribs.TRCDMinPs == 0 {
		memAttribs.TRCDMinPs = DDR4GetTAAMinPs(memAttribs)
	}
}

func DDR4UpdateTRPMin(memAttribs *DDR4MemAttributes) {
	/* tRPmin is same as tAAmin for all cases */
	if memAttribs.TRPMinPs == 0 {
		memAttribs.TRPMinPs = DDR4GetTAAMinPs(memAttribs)
	}
}

func DDR4UpdateTRASMin(memAttribs *DDR4MemAttributes) {
	if memAttribs.TRASMinPs == 0 {
		memAttribs.TRASMinPs = DDR4SpeedBinToSPDEncoding[memAttribs.SpeedMTps].TRASMinPs
	}
}

func DDR4GetTRCMinPs(memAttribs *DDR4MemAttributes) int {
	return memAttribs.TAAMinPs + memAttribs.TRASMinPs
}

func DDR4UpdateTRCMin(memAttribs *DDR4MemAttributes) {
	if memAttribs.TRCMinPs == 0 {
		memAttribs.TRCMinPs = DDR4GetTRCMinPs(memAttribs)
	}
}

func DDR4GetDefaultTCKMinPs(memAttribs *DDR4MemAttributes) int {
	/* value 2000000 = 2 * 1000000, where 1000000 is to convert mS to pS */
	return 2000000 / memAttribs.SpeedMTps
}

func DDR4UpdateTCK(memAttribs *DDR4MemAttributes) {
	if memAttribs.TCKMinPs == 0 {
		memAttribs.TCKMinPs = DDR4GetDefaultTCKMinPs(memAttribs)
	}
	if memAttribs.TCKMaxPs == 0 {
		memAttribs.TCKMaxPs = DDR4SpeedBinToSPDEncoding[memAttribs.SpeedMTps].TCKMaxPs
	}
}

func DDR4UpdateTWRMin(memAttribs *DDR4MemAttributes) {
	if memAttribs.TWRMinPs == 0 {
		memAttribs.TWRMinPs = DDR4TimingValueTWRMinPs
	}
}

func DDR4UpdateTWTRMin(memAttribs *DDR4MemAttributes) {
	if memAttribs.TWTRLMinPs == 0 {
		memAttribs.TWTRLMinPs = DDR4TimingValueTWTRLMinPs
	}
	if memAttribs.TWTRSMinPs == 0 {
		memAttribs.TWTRSMinPs = DDR4TimingValueTWTRSMinPs
	}
}

/*
 * Per Table 169 & Table 170 of Jedec JESD79-4C
 * tFAW timing is based on :
 *  Speed bin and page size
 */
func DDR4GetTFAWMinPs(memAttribs *DDR4MemAttributes) int {
	var tFAWFixed int

	if DDR4PageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] == 1 {
		switch memAttribs.SpeedMTps {
		case 1600:
			tFAWFixed = 25000
		case 1866:
			tFAWFixed = 23000
		default:
			tFAWFixed = 21000
		}
	} else if DDR4PageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] == 2 {
		switch memAttribs.SpeedMTps {
		case 1600:
			tFAWFixed = 35000
		default:
			tFAWFixed = 30000
		}
	}

	return tFAWFixed
}

/* Update settings based on data sheet (json) supplied memory attributes */

func DDR4UpdateTFAWMin(memAttribs *DDR4MemAttributes) {
	var tFAWFromTck int

	if memAttribs.TFAWMinPs == 0 {
		memAttribs.TFAWMinPs = DDR4GetTFAWMinPs(memAttribs)
	}

	switch DDR4PageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] {
	case 1:
		tFAWFromTck = 20 * memAttribs.TCKMinPs
	case 2:
		tFAWFromTck = 28 * memAttribs.TCKMinPs
	}

	if memAttribs.TFAWMinPs < tFAWFromTck {
		memAttribs.TFAWMinPs = tFAWFromTck
	}
}

func DDR4UpdateTRFC1Min(memAttribs *DDR4MemAttributes) {
	if memAttribs.TRFC1MinPs == 0 {
		memAttribs.TRFC1MinPs = DDR4TRFC1Encoding[memAttribs.CapacityPerDieGb]
	}
}

func DDR4UpdateTRFC2Min(memAttribs *DDR4MemAttributes) {
	if memAttribs.TRFC2MinPs == 0 {
		memAttribs.TRFC2MinPs = DDR4TRFC2Encoding[memAttribs.CapacityPerDieGb]
	}
}

func DDR4UpdateTRFC4Min(memAttribs *DDR4MemAttributes) {
	if memAttribs.TRFC4MinPs == 0 {
		memAttribs.TRFC4MinPs = DDR4TRFC4Encoding[memAttribs.CapacityPerDieGb]
	}
}

func DDR4GetTRRDLMinPs(memAttribs *DDR4MemAttributes) int {
	var tRRDLFixed int

	/*
	 * Per JESD79-4C Tables 169 & 170, tRRD_L is based on :
	 *  Speed bin and page size
	 */
	switch DDR4PageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] {
	case 1:
		switch memAttribs.SpeedMTps {
		case 1600:
			tRRDLFixed = 6000
		default:
			tRRDLFixed = 5300
		}
	case 2:
		switch memAttribs.SpeedMTps {
		case 1600:
			tRRDLFixed = 7500
		default:
			tRRDLFixed = 6400
		}
	}

	return tRRDLFixed
}

func DDR4UpdateTRRDLMin(memAttribs *DDR4MemAttributes) {
	var tRRDLFromTck int

	if memAttribs.TRRDLMinPs == 0 {
		memAttribs.TRRDLMinPs = DDR4GetTRRDLMinPs(memAttribs)
	}

	tRRDLFromTck = 4 * memAttribs.TCKMinPs

	if memAttribs.TRRDLMinPs < tRRDLFromTck {
		memAttribs.TRRDLMinPs = tRRDLFromTck
	}
}

func DDR4GetTRRDSMinPs(memAttribs *DDR4MemAttributes) int {
	var tRRDFixed int

	switch DDR4PageSizefromBusWidthEncoding[memAttribs.PackageBusWidth] {
	case 1:
		tRRDFixed = DDR4SpeedToTRRDSMinPsOneKPageSize[memAttribs.SpeedMTps]
	case 2:
		tRRDFixed = DDR4SpeedToTRRDSMinPsTwoKPageSize[memAttribs.SpeedMTps]
	}

	return tRRDFixed
}

func DDR4UpdateTRRDSMin(memAttribs *DDR4MemAttributes) {
	var tRRDFromTck int

	if memAttribs.TRRDSMinPs == 0 {
		memAttribs.TRRDSMinPs = DDR4GetTRRDSMinPs(memAttribs)
	}

	tRRDFromTck = 4 * memAttribs.TCKMinPs

	if memAttribs.TRRDSMinPs < tRRDFromTck {
		memAttribs.TRRDSMinPs = tRRDFromTck
	}
}

/*
 * Per JESD79-4C Tables 169 and 170,
 * tCCD_L is based on :
 *  Speed Bin
 */
func DDR4GetTCCDLMinPs(memAttribs *DDR4MemAttributes) int {
	var tCCDLFixed int

	switch memAttribs.SpeedMTps {
	case 1600:
		tCCDLFixed = 6250
	case 1866:
		tCCDLFixed = 5355
	case 2133:
		tCCDLFixed = 5355
	default:
		tCCDLFixed = 5000
	}

	return tCCDLFixed
}

func DDR4UpdateTCCDLMin(memAttribs *DDR4MemAttributes) {
	var tCCDLFromTck int

	if memAttribs.TCCDLMinPs == 0 {
		memAttribs.TCCDLMinPs = DDR4GetTCCDLMinPs(memAttribs)
	}

	tCCDLFromTck = 5 * memAttribs.TCKMinPs

	if memAttribs.TCCDLMinPs < tCCDLFromTck {
		memAttribs.TCCDLMinPs = tCCDLFromTck
	}
}

func DDR4UpdateMemoryAttributes(memAttribs *DDR4MemAttributes) {
	DDR4UpdateDieBusWidth(memAttribs)
	DDR4UpdateTCK(memAttribs)
	DDR4UpdateTAAMin(memAttribs)
	DDR4UpdateTRCDMin(memAttribs)
	DDR4UpdateTRPMin(memAttribs)
	DDR4UpdateTRASMin(memAttribs)
	DDR4UpdateTRCMin(memAttribs)
	DDR4UpdateTWRMin(memAttribs)
	DDR4UpdateTWTRMin(memAttribs)
	DDR4UpdateCAS(memAttribs)
	DDR4UpdateTRFC1Min(memAttribs)
	DDR4UpdateTRFC2Min(memAttribs)
	DDR4UpdateTRFC4Min(memAttribs)
	DDR4UpdateTCCDLMin(memAttribs)
	DDR4UpdateTRRDSMin(memAttribs)
	DDR4UpdateTRRDLMin(memAttribs)
	DDR4UpdateTFAWMin(memAttribs)
}

func DDR4ValidateSpeedMTps(speedBin int) error {
	if _, ok := DDR4SpeedBinToSPDEncoding[speedBin]; ok == false {
		return fmt.Errorf("Incorrect speed bin: DDR4-%d", speedBin)
	}
	return nil
}

func DDR4ValidateCapacityPerDie(capacityPerDieGb int) error {
	if _, ok := DDR4DensityGbToSPDEncoding[capacityPerDieGb]; ok == false {
		return fmt.Errorf("Incorrect capacity per die: %d", capacityPerDieGb)
	}
	return nil
}

func DDR4ValidateDiesPerPackage(dieCount int) error {
	if dieCount >= 1 && dieCount <= 2 {
		return nil
	}
	return fmt.Errorf("Incorrect dies per package count: %d", dieCount)
}

func DDR4ValidatePackageBusWidth(width int) error {
	if width != 8 && width != 16 {
		return fmt.Errorf("Incorrect device bus width: %d", width)
	}
	return nil
}

func DDR4ValidateRanksPerPackage(ranks int) error {
	if ranks >= 1 && ranks <= 2 {
		return nil
	}
	return fmt.Errorf("Incorrect package ranks: %d", ranks)
}

func DDR4ValidateCASLatency(CL int) error {
	if CL >= 10 && CL <= 24 && CL != 23 {
		return nil
	}
	return fmt.Errorf("Incorrect CAS latency: %d", CL)
}

func DDR4VerifySupportedCASLatencies(name string, memAttribs *DDR4MemAttributes) error {
	if memAttribs.CASLatencies == DDR4GetDefaultCASLatencies(memAttribs) {
		return fmt.Errorf("CASLatencies for %s already matches default,\nPlease remove CASLatencies override line from the %s part attributes in the global part list and regenerate SPD Manifest", name, name)
	}

	return nil
}

func DDR4ValidateMemPartAttributes(name string, memAttribs *DDR4MemAttributes) error {
	if err := DDR4ValidateSpeedMTps(memAttribs.SpeedMTps); err != nil {
		return err
	}
	if err := DDR4ValidateCapacityPerDie(memAttribs.CapacityPerDieGb); err != nil {
		return err
	}
	if err := DDR4ValidateDiesPerPackage(memAttribs.DiesPerPackage); err != nil {
		return err
	}
	if err := DDR4ValidatePackageBusWidth(memAttribs.PackageBusWidth); err != nil {
		return err
	}
	if err := DDR4ValidateRanksPerPackage(memAttribs.RanksPerPackage); err != nil {
		return err
	}
	if err := DDR4ValidateCASLatency(memAttribs.CL_nRCD_nRP); err != nil {
		return err
	}

	/* If CAS Latency was supplied, make sure it doesn't match default value */
	if len(memAttribs.CASLatencies) != 0 {
		if err := DDR4VerifySupportedCASLatencies(name, memAttribs); err != nil {
			return err
		}
	}

	return nil
}

func DDR4IsManufacturerPartNumberByte(index int) bool {
	if index >= DDR4SPDIndexManufacturerPartNumberStartByte && index <= DDR4SPDIndexManufacturerPartNumberEndByte {
		return true
	}
	return false
}

/* ------------------------------------------------------------------------------------------ */
/*                                 Interface Functions                                       */
/* ------------------------------------------------------------------------------------------ */

func (ddr4) getSetMap() map[int][]int {
	return DDR4PlatformSetMap
}

func (ddr4) addNewPart(name string, attribs interface{}) error {
	var ddr4MemAttribs DDR4MemAttributes
	eByte, err := json.Marshal(attribs)
	if err != nil {
		return err
	}

	if err := json.Unmarshal(eByte, &ddr4MemAttribs); err != nil {
		return err
	}

	if err := DDR4ValidateMemPartAttributes(name, &ddr4MemAttribs); err != nil {
		return err
	}

	DDR4PartAttributeMap[name] = ddr4MemAttribs
	return nil
}

func (ddr4) getSPDAttribs(name string, set int) (interface{}, error) {
	ddr4Attributes := DDR4PartAttributeMap[name]

	DDR4CurrSet = set

	DDR4UpdateMemoryAttributes(&ddr4Attributes)

	return ddr4Attributes, nil
}

func (ddr4) getSPDLen() int {
	return 512
}

func (ddr4) getSPDByte(index int, attribs interface{}) byte {
	e, ok := DDR4SPDAttribTable[index]
	if ok == false {
		if DDR4IsManufacturerPartNumberByte(index) {
			return DDR4SPDValueManufacturerPartNumberBlank
		}
		return 0x00
	}

	if e.getVal != nil {
		var ddr4Attribs DDR4MemAttributes
		ddr4Attribs = attribs.(DDR4MemAttributes)
		return e.getVal(&ddr4Attribs)

	}

	return e.constVal
}
