package test

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
)

var Suite = []parser.Entry{
	// {2  ------- GPIO Community 0 ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Community 0 -------"},
	// {2  ------- GPIO Group GPP_A ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Group GPP_A -------"},
	{ // {1 GPP_A0 RCIN# 84000502 00000000 0}
		EType:    parser.EntryPad,
		Function: "RCIN#",
		ID:       "GPP_A0",
		DW0:      0x84000502,
	},
	{ // {1 GPP_A1 LAD0 84000402 00003000 0}
		EType:    parser.EntryPad,
		Function: "LAD0",
		ID:       "GPP_A1",
		DW0:      0x84000402,
		DW1:      0x00003000,
	},
	{ // {1 GPP_A5 LFRAME# 84000600 00000000 0}
		EType:    parser.EntryPad,
		Function: "LFRAME#",
		ID:       "GPP_A5",
		DW0:      0x84000600,
	},
	{ // {1 GPP_A13 SUSWARN#/SUSPWRDNACK 44000600 00000000 0}
		EType:    parser.EntryPad,
		Function: "SUSWARN#/SUSPWRDNACK",
		ID:       "GPP_A13",
		DW0:      0x44000600,
	},
	{ // {1 GPP_A23 GPIO 84000102 00000000 0}
		EType:    parser.EntryPad,
		Function: "GPIO",
		ID:       "GPP_A23",
		DW0:      0x84000102,
	},
	// {2  ------- GPIO Group GPP_B ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Group GPP_B -------"},
	{ // {3 GPP_C1 RESERVED ffffffff ffffff00 0}
		EType:    parser.EntryReserved,
		Function: "RESERVED",
		ID:       "GPP_C1",
		DW0:      0xffffffff,
		DW1:      0xffffff00,
	},
	{ // {1 GPP_B0 GPIO 84000100 00000000 0}
		EType:    parser.EntryPad,
		Function: "GPIO",
		ID:       "GPP_B0",
		DW0:      0x84000100,
	},
	{ // {1 GPP_B23 PCHHOT# 84000a01 00001000 0}
		EType:    parser.EntryPad,
		Function: "PCHHOT#",
		ID:       "GPP_B23",
		DW0:      0x84000a01,
		DW1:      0x00001000,
	},
	// {2  ------- GPIO Community 1 ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Community 1 -------"},
	// {2  ------- GPIO Group GPP_C ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Group GPP_C -------"},
	{ // {1 GPP_C0 SMBCLK 44000502 00000000 0}
		EType:    parser.EntryPad,
		Function: "SMBCLK",
		ID:       "GPP_C0",
		DW0:      0x44000502,
	},
	{ // {1 GPP_C5 GPIO 84000100 00000000 0}
		EType:    parser.EntryPad,
		Function: "GPIO",
		ID:       "GPP_C5",
		DW0:      0x84000100,
	},
	{ // {3 GPP_C6 RESERVED ffffffff ffffff00 0}
		EType:    parser.EntryReserved,
		Function: "RESERVED",
		ID:       "GPP_C6",
		DW0:      0xffffffff,
		DW1:      0xffffff00,
	},
	{ // {3 GPP_C7 RESERVED ffffffff ffffff00 0}
		EType:    parser.EntryReserved,
		Function: "RESERVED",
		ID:       "GPP_C7",
		DW0:      0xffffffff,
		DW1:      0xffffff00,
	},
	{ // {1 GPP_C22 UART2_RTS# 84000600 00000000 0}
		EType:    parser.EntryPad,
		Function: "UART2_RTS#",
		ID:       "GPP_C22",
		DW0:      0x84000600,
	},
	// {2  ------- GPIO Group GPP_D ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Group GPP_D -------"},
	// {2  ------- GPIO Group GPP_E ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Group GPP_E -------"},
	{ // {1 GPP_E0 SATAXPCIE0 84000502 00003000 0}
		EType:    parser.EntryPad,
		Function: "SATAXPCIE0",
		ID:       "GPP_E0",
		DW0:      0x84000502,
		DW1:      0x00003000,
	},
	// {2  ------- GPIO Group GPP_G ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Group GPP_G -------"},
	{ // {1 GPP_G19 SMI# 84000500 00000000 0}
		EType:    parser.EntryPad,
		Function: "SMI#",
		ID:       "GPP_G19",
		DW0:      0x84000500,
	},
	// {2  ------- GPIO Community 2 ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Community 2 -------"},
	// {2  -------- GPIO Group GPD -------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "-------- GPIO Group GPD --------"},
	{ // {1 GPD9 SLP_WLAN# 04000600 00000000 0}
		EType:    parser.EntryPad,
		Function: "SLP_WLAN#",
		ID:       "GPD9",
		DW0:      0x04000600,
	},
	// {2  ------- GPIO Community 3 ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Community 3 -------"},
	// {2  ------- GPIO Group GPP_I ------- 00000000 00000000 0}
	{EType: parser.EntryGroup, Function: "------- GPIO Group GPP_I -------"},
	{ // {1 GPP_I0 DDPB_HPD0 84000500 00000000 0}
		EType:    parser.EntryPad,
		Function: "DDPB_HPD0",
		ID:       "GPP_I0",
		DW0:      0x84000500,
	},
	{ // {1 GPP_I1 DDPC_HPD1 84000502 00000000 0}
		EType:    parser.EntryPad,
		Function: "DDPC_HPD1",
		ID:       "GPP_I1",
		DW0:      0x84000502,
	},
	{ // {1 GPP_I2 DDPD_HPD2 84000502 00000000 0}
		EType:    parser.EntryPad,
		Function: "DDPD_HPD2",
		ID:       "GPP_I2",
		DW0:      0x84000502,
	},
}
