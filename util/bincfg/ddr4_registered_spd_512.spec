# 4_01_02_AnnexL-R25_SPD_for_DDR4_SDRAM_Release_3_Sep2015.pdf
#
# JEDEC Standard No. 21-C
# Page 4.1.2.12.3 – 1
# Annex L: Serial Presence Detect (SPD) for DDR4 SDRAM Modules
# DDR4 SPD Document Release 3
# UDIMM Revision 1.1
# RDIMM Revision 1.1
# LRDIMM Revision 1.1
# NVDIMM Revision 1.0

{
	# Byte 0: Number of Bytes Used / Number of Bytes in SPD Device /
	#         CRC Coverage
	"SPD_Bytes_Used"						: 4,
	"SPD_Bytes_Total"						: 3,
	"CRC_Coverage"							: 1,

	# Byte 1: SPD Revision
	"SPD_Revision"							: 8,

	# Byte 2: Key Byte / DRAM Device Type
	"DRAM_Device_Type"						: 8,

	# Byte 3: Key Byte / Module Type
	"Base_Module_Type"						: 4,
	"Hybrid_Media"							: 3,
	"Is_Hybrid"							: 1,

	# Byte 4: SDRAM Density and Banks
	"SDRAM_Capacity"						: 4,
	"Bank_Address_Bits"						: 2,
	"Bank_Group_Bits"						: 2,

	# Byte 5: SDRAM Addressing
	"Column_Address_Bits"						: 3,
	"Row_Address_Bits"						: 3,
	"Byte_5_reserved"						: 2,

	# Byte 6:  SDRAM Package Type
	"Signal_Loading"						: 2,
	"Byte_6_reserved"						: 2,
	"Die_Count"							: 3,
	"SDRAM_Package_Type"						: 1,

	# Byte 7:  SDRAM Optional Features
	"Maximum_Activate_Count"					: 4,
	"Maximum_Activate_Window"					: 2,
	"Byte_7_reserved"						: 2,

	# Byte 8: SDRAM Thermal and Refresh Options
	"Byte_8_reserved"						: 8,

	# Byte 9:  Other SDRAM Optional Features
	"Byte_9_reserved"						: 5,
	"Soft_PPR"							: 1,
	"Post_Package_Repair"						: 2,

	# Byte 10: Secondary SDRAM Package Type
	"Secondary_Signal_Loading"					: 2,
	"Secondary_DRAM_Densityt_Ratio"					: 2,
	"Secondary_Die_Count"						: 3,
	"Secondary_SDRAM_Package_Type"					: 1,

	# Byte 11:  Module Nominal Voltage, VDD
	"DRAM_VDD_1_2_V"						: 2,
	"Byte_11_reserved"						: 6,

	# Byte 12: Module Organization
	"SDRAM_Device_Width"						: 3,
	"Number_of_Package_Ranks_per_DIMM"				: 3,
	"Rank_Mix"							: 1,
	"Byte_12_reserved"						: 1,

	# Byte 13: Module Memory Bus Width
	"Primary_bus_width_in_bits"					: 3,
	"Bus_width_extension_in_bits"					: 2,
	"Byte_13_reserved"						: 3,

	# Byte 14: Module Thermal Sensor
	"Byte_14_reserved"						: 7,
	"Thermal_Sensor"						: 1,

	# Byte 15:  Extended Module Type
	"Extended_Base_Module_Type"					: 4,
	"Byte_15_reserved"						: 4,

	# Byte 16: Reserved
	"Byte_16_reserved"						: 8,

	# Byte 17: Timebases
	"Fine_Timebase"							: 2,
	"Medium_Timebase"						: 2,
	"Byte_17_reserved"						: 4,

	# Byte 18:  SDRAM Minimum Cycle Time (tCKAVGmin)
	"tCKAVGmin"							: 8,

	# Byte 19:  SDRAM Maximum Cycle Time (tCKAVGmax)
	"tCKAVGmax"							: 8,

	# Bytes 20 - 23: CAS Latencies Supported
	"CL_7_Supported"						: 1,
	"CL_8_Supported"						: 1,
	"CL_9_Supported"						: 1,
	"CL_10_Supported"						: 1,
	"CL_11_Supported"						: 1,
	"CL_12_Supported"						: 1,
	"CL_13_Supported"						: 1,
	"CL_14_Supported"						: 1,

	"CL_15_Supported"						: 1,
	"CL_16_Supported"						: 1,
	"CL_17_Supported"						: 1,
	"CL_18_Supported"						: 1,
	"CL_19_Supported"						: 1,
	"CL_20_Supported"						: 1,
	"CL_21_Supported"						: 1,
	"CL_22_Supported"						: 1,

	"CL_23_Supported"						: 1,
	"CL_24_Supported"						: 1,
	"CL_25_Supported"						: 1,
	"CL_26_Supported"						: 1,
	"CL_27_Supported"						: 1,
	"CL_28_Supported"						: 1,
	"CL_29_Supported"						: 1,
	"CL_30_Supported"						: 1,

	"CL_31_Supported"						: 1,
	"CL_32_Supported"						: 1,
	"CL_33_Supported"						: 1,
	"CL_34_Supported"						: 1,
	"CL_35_Supported"						: 1,
	"CL_36_Supported"						: 1,
	"Byte_23_reserved"						: 1,
	"CL_range"							: 1,

	# Byte 24: Minimum CAS Latency Time (tAAmin)
	"tAAmin"							: 8,

	# Byte 25: Minimum RAS to CAS Delay Time (tRCDmin)
	"tRCDmin"							: 8,

	# Byte 26: Minimum Row Precharge Delay Time (tRPmin)
	"tRPmin"							: 8,

	# Bytes 27 - 29: Minimum Active to Precharge Delay Time (tRASmin)
	#               / Minimum Active to Active/Refresh Delay Time
	#               (tRCmin)
	"tRASmin_MSN"							: 4,
	"tRCmin_MSN"							: 4,
	"tRASmin_LSB"							: 8,
	"tRCmin_LSB"							: 8,

	# Bytes 30 - 31: Minimum Refresh Recovery Delay Time (tRFC1min)
	"tRFC1min_LSB"							: 8,
	"tRFC1min_MSB"							: 8,

	# Bytes 32 - 33: Minimum Refresh Recovery Delay Time (tRFC2min)
	"tRFC2min_LSB"							: 8,
	"tRFC2min_MSB"							: 8,

	# Bytes 34 - 35: Minimum Refresh Recovery Delay Time (tRFC4min)
	"tRFC4min_LSB"							: 8,
	"tRFC4min_MSB"							: 8,

	# Byte 36 - 37:  Minimum Four Activate Window Delay Time
	#               (tFAWmin)
	"tFAWmin_MSN"							: 4,
	"Byte_36_reserved"						: 4,
	"tFAWmin_LSB"							: 8,

	# Bytes 38: Minimum Activate to Activate Delay Time (tRRD_Smin),
	#                different bank group
	"tRRD_Smin"							: 8,

	# Byte 39: Minimum Activate to Activate Delay Time (tRRD_Lmin),
	#          same bank group
	"tRRD_Lmin"							: 8,

	# Byte 40: Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank
	#          group
	"tCCD_Lmin"							: 8,

	# Byte 41 - 42: Minimum Write Recovery Time (tWRmin)
	"tWRmin_MSN"							: 4,
	"Byte_41_reserved"						: 4,
	"tWRmin_MSB"							: 8,

	# Byte 43-45: Minimum Write to Read Time (tWTR_Smin),
	#	      different bank group / Minimum Write to Read Time
	#	      (tWTR_Lmin), same bank group
	"tWTR_Smin_MSN"							: 4,
	"tWTR_Lmin_MSN"							: 4,
	"tWTR_Smin_LSB"							: 8,
	"tWTR_Lmin_LSB"							: 8,

	# Byte 46~59: Reserved, Base Configuration Section
	"Byte_46_59_reserved" [14]					: 8,

	# Byte 60: Connector to SDRAM Bit Mapping (DQ0-3)
	"DQ0_3"								: 8,
	# Byte 61: Connector to SDRAM Bit Mapping (DQ4-7)
	"DQ4_7"								: 8,

	# Byte 62: Connector to SDRAM Bit Mapping (DQ8-11)
	"DQ8_11"							: 8,

	# Byte 63: Connector to SDRAM Bit Mapping (DQ12-15)
	"DQ12_15"							: 8,

	# Byte 64: Connector to SDRAM Bit Mapping (DQ16-19)
	"DQ16_19"							: 8,

	# Byte 65: Connector to SDRAM Bit Mapping (DQ20-23)
	"DQ20_23"							: 8,

	# Byte 66: Connector to SDRAM Bit Mapping (DQ24-27)
	"DQ24_27"							: 8,

	# Byte 67: Connector to SDRAM Bit Mapping (DQ28-31)
	"DQ28_31"							: 8,

	# Byte 68: Connector to SDRAM Bit Mapping (CB0-3)
	"CB0_3"								: 8,

	# Byte 69: Connector to SDRAM Bit Mapping (CB4-7)
	"CB4_7"								: 8,

	# Byte 70: Connector to SDRAM Bit Mapping (DQ32-35)
	"DQ32_35"							: 8,

	# Byte 71: Connector to SDRAM Bit Mapping (DQ36-39)
	"DQ36_39"							: 8,

	# Byte 72: Connector to SDRAM Bit Mapping (DQ40-43)
	"DQ40_43"							: 8,

	# Byte 73: Connector to SDRAM Bit Mapping (DQ44-47)
	"DQ44_47"							: 8,

	# Byte 74: Connector to SDRAM Bit Mapping (DQ48-51)
	"DQ48_51"							: 8,

	# Byte 75: Connector to SDRAM Bit Mapping (DQ52-55)
	"DQ52_55"							: 8,

	# Byte 76: Connector to SDRAM Bit Mapping (DQ56-59)
	"DQ56_59"							: 8,

	# Byte 77: Connector to SDRAM Bit Mapping (DQ60-63)
	"DQ60_63"							: 8,

	# Bytes 78~116: Reserved, Base Configuration Section
	# Must be coded as 0x00
	"Byte_78_116_reserved" [39]					: 8,

	# Byte 117: Fine Offset for Minimum CAS to CAS Delay Time
	#           (tCCD_Lmin), same bank group
	"tCCD_Lmin"							: 8,

	# Byte 118: Fine Offset for Minimum Activate to Activate Delay
	#	    Time (tRRD_Lmin), same bank group
	"tRRD_Lmin"							: 8,

	# Byte 119:  Fine Offset for Minimum Activate to Activate Delay
	#	     Time (tRRD_Smin), different bank group
	"tRRD_Smin"							: 8,

	# Byte 120: Fine Offset for Minimum Active to Active/Refresh
	#	    Delay Time (tRCmin)
	"tRCmin"							: 8,

	# Byte 121: Fine Offset for Minimum Row Precharge Delay
	#           Time (tRPmin)
	"tRPmin"							: 8,

	# Byte 122: Fine Offset for Minimum RAS to CAS Delay
	#	    Time (tRCDmin)
	"tRCDmin"							: 8,

	# Byte 123: Fine Offset for Minimum CAS Latency Time (tAAmin)
	"tAAmin"							: 8,

	# Byte 124: Fine Offset for SDRAM Maximum Cycle Time
	#           (tCKAVGmax)
	"tCKAVGmax"							: 8,

	# Byte 125: Fine Offset for SDRAM Minimum Cycle Time
	#           (tCKAVGmin)
	"tCKAVGmin"							: 8,

	# Byte 126 - 127: Cyclical Redundancy Code (CRC) for
	#	    	  Base Configuration Section
	"CRC_Base_Configuration"					: 16,

# Standard Module Parameters - Overlay Bytes 128~191
#  Module Specific Bytes for Registered Memory Module Types

	# Byte 128: Raw Card Extension, Module Nominal Height
	"Module_Nominal_Height_Max"					: 5,
	"Raw_Card_Extension"						: 3,

	# Byte 129: Module Maximum Thickness
	"Module_Maximum_Thickness_Front"				: 4,
	"Module_Maximum_Thickness_Back"					: 4,

	# Byte 130: Reference Raw Card Used
	"Reference_Raw_Card"						: 5,
	"Reference_Raw_Card_Revision"					: 2,
	"Reference_Raw_Card_Extension"					: 1,

	# Byte 131: DIMM Attributes
	"Number_of_Registers_used_on_RDIMM"				: 2,
	"Number_of_DRAMS_on_RDIMM"					: 2,
	"Register_Type"							: 4,

	# Byte 132: RDIMM Thermal Heat Spreader Solution
	"Heat_Spreader_Thermal_Characteristics"				: 7,
	"Heat_Spreader_Solution"					: 1,

	# Byte 133 - 134: Register Manufacturer ID Code
	"Register_Manufacturer_ID_Code"					: 16,

	# Byte 135: Register Revision Number
	"Register_Revision_Number"					: 8,

	# Byte 136: Address Mapping from Register to DRAM
	"Rank_1_Mapping"						: 1,
	"Byte_136_Reserved"						: 7,

	# Byte 137: Register Output Drive Strength for
	#	    Control and Command/Address
	"Register_Output_Drive_CKE"					: 2,
	"Register_Output_Drive_ODT"					: 2,
	"Register_Output_Drive_Command_Address"				: 2,
	"Register_Output_Drive_Chip_Select"				: 2,

	# Byte 138: Register Output Drive Strength for Clock
	"Register_Output_Drive_Strength_Clock_Y0_Y2"			: 2,
	"Register_Output_Drive_Strength_Clock_Y1_Y3"			: 2,
	"Byte_138_reserved"						: 4,

	# Byte 139 - 191: Reserved
	"Byte_139_191" [53]						: 8,

# Unused
	# Byte 192 - 253: Unused
	"Byte_192_255_unused" [62]					: 8,

	# Byte 254 - 255: CRC for SPD Block 1
	"CRC_SPD_Block_1"						: 16,

# Reserved
	# Byte 256 - 319: Reserved
	"Byte_256_319_reserved" [64]					: 8,

# End User Programmable
	# Byte 384 - 511
	"End_User_Programmable"	[128]					: 8
}
