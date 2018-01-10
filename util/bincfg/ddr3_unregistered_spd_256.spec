# Applies to unbuffered DIMM types
#    UDIMM, SO-DIMM, Micro-DIMM, Micro-UDIMM,
#    72b-SO-UDIMM, 16b-SO-UDIMM, 32b-SO-UDIMM


# 4_01_02_11R24.pdf
#
# JEDEC Standard No. 21-C
# Page 4.1.2.11 - 1
# Annex K: Serial Presence Detect (SPD) for DDR3 SDRAM Modules
# DDR3 SPD
# Document Release 6
# UDIMM Revision 1.3
# RDIMM Revision 1.3
# CDIMM Revision 1.3
# LRDIMM Revision 1.2

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
	"Module_Type"							: 4,
	"Byte_3_reserved"						: 4,

	# Byte 4: SDRAM Density and Banks
	"SDRAM_Capacity"						: 4,
	"Bank_Address_Bits"						: 3,
	"Byte_4_reserved"						: 1,

	# Byte 5: SDRAM Addressing
	"Column_Address_Bits"						: 3,
	"Row_Address_Bits"						: 3,
	"Byte_5_reserved"						: 2,

	# Byte 6: Module Nominal Voltage, VDD
	"NOT_1.5_V_Operable"						: 1,
	"1.35_V_Operable"						: 1,
	"1.25_V_Operable"						: 1,
	"Byte_6_reserved"						: 5,

	# Byte 7: Module Organization
	"SDRAM_Device_Width"						: 3,
	"Number_of_Ranks"						: 3,
	"Byte_7_reserved"						: 2,

	# Byte 8: Module Memory Bus Width
	"Primary_Bus_Width"						: 3,
	"Bus_Width_Extension"						: 3,
	"Byte_8_reserved"						: 2,

	# Byte 9: Fine Timebase (FTB) Dividend / Divisor
	"Fine_Timebase_Divisor"						: 4,
	"Fine_Timebase_Dividend"					: 4,

	# Bytes 10 / 11: Medium Timebase (MTB) Dividend / Divisor
	"Medium_Timebase_Dividend"					: 8,
	"Medium_Timebase_Divisor"					: 8,

	# Byte 12: SDRAM Minimum Cycle Time (t CK min)
	"Minimum_SDRAM_Cycle_Time"					: 8,

	# Byte 13: Reserved
	"Byte_13_Reserved"						: 8,

	# Bytes 14 / 15: CAS Latencies Supported
	"CL_4_Supported"						: 1,
	"CL_5_Supported"						: 1,
	"CL_6_Supported"						: 1,
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
	"Byte_15_Reserved"						: 1,

	# Byte 16: Minimum CAS Latency Time (tAAmin)
	"tAAmin"							: 8,

	# Byte 17: Minimum Write Recovery Time (tWRmin)
	"tWRmin"							: 8,

	# Byte 18: Minimum RAS to CAS Delay Time (tRCDmin)
	"tRCDmin"							: 8,

	# Byte 19: Minimum Row Active to Row Active Delay Time (tRRDmin)
	"tRRDmin"							: 8,

	# Byte 20: Minimum Row Precharge Delay Time (tRPmin)
	"tRPmin"							: 8,

	# Bytes 21 - 23: Minimum Active to Precharge Delay Time (tRASmin)
	#               / Minimum Active to Active/Refresh Delay Time
	#               (tRCmin)
	"tRASmin_Most_Significant Nibble"				: 4,
	"tRCmin_Most_Significant Nibble"				: 4,
	"tRASmin_LSB"							: 8,
	"tRCmin_LSB"							: 8,

	# Bytes 24 - 25: Minimum Refresh Recovery Delay Time (tRFCmin)
	"tRFCmin LSB"							: 8,
	"tRFCmin MSB"							: 8,

	# Byte 26: Minimum Internal Write to Read Command Delay Time
	"tWTRmin"							: 8,

	# Byte 27: Minimum Internal Read to Precharge Command Delay Time
	#          (tRTPmin)
	"tRTPmin"							: 8,

	# Byte 28 - 29:  Minimum Four Activate Window Delay Time
	#               (tFAWmin)
	"tFAWmin Most Significant Nibble"				: 4,
	"Byte_28_Reserved"						: 4,
	"tFAWmin Most Significant Byte"					: 8,

	# Byte 30: SDRAM Optional Features
	"RQZ_Div_6_Supported"						: 1,
	"RQZ_Div_7_Supported"						: 1,
	"Byte_30_Reserved"						: 5,
	"DLL_Off_Mode_Supported"					: 1,

	# Byte 31: SDRAM Thermal and Refresh
	#          Options
	"Extended_Temp_range_supported"					: 1,
	"Extended_Temp_Refresh_1x_Refresh"				: 1,
	"Auto_Self_Refresh_Supported"					: 1,
	"On-Die_Thermal_Sensor"						: 1,
	"Byte_31_Reserved"						: 3,
	"Partial_Array_Self_Refresh_Supported"				: 1,

	# Byte 32: Module Thermal Sensor
	"Thermal_Sensor_Accuracy"					: 7,
	"Thermal_Sensor_incorporated"					: 1,

	# Byte 33: SDRAM Device Type
	"Signal_Loading"						: 2,
	"Byte_33_Reserved"						: 2,
	"Die_Count"							: 3,
	"SDRAM_Device_Type"						: 1,

	# Byte 34: Fine Offset for SDRAM Minimum
	#          Cycle Time (tCKmin)
	"tCKmin_Fine_Offset"						: 8,

	#Byte 35: Fine Offset for Minimum CAS Latency Time (tAAmin)
	"tAAmin_Fine_Offset"						: 8,

	# Byte 36: Fine Offset for Minimum RAS to CAS Delay Time
	#          (tRCDmin)
	"tRCDmin_Fine_Offset"						: 8,

	#Byte 37: Fine Offset for Minimum Row Precharge Delay Time
	#         (tRPmin)
	"tRPmin_Fine_Offset"						: 8,

	# Byte 38: Fine Offset for Minimum Active to Active/Refresh
	#          Delay Time (tRCmin)
	"tRCmin_Fine_Offset"						: 8,

	# Bytes 39 / 40: Reserved
	"Byte_39_Reserved"						: 8,
	"Byte_40_Reserved"						: 8,

	# Byte 41: SDRAM Maximum Active Count (MAC) Value
	"Maximum_Activate_Count"					: 4,
	"Maximum_Activate_Window"					: 2,
	"Byte_41_Reserved"						: 2,

	# Bytes 42 - 59: Reserved
	"Reserved_bytes_42_to_59_" [18]					: 8,

# Module-Specific Section: Bytes 60 - 116 for Unbuffered DIMMS

	# Byte 60 (Unbuffered): Raw Card Extension, Module Nominal Height
	"Module_Nominal_Height"						: 5,
	"Raw_Card_Estension"						: 3,

	# Byte 61 (Unbuffered): Module Maximum Thickness
	"Module_Thickness_Front"					: 4,
	"Module_Thickness_Back"						: 4,

	# Byte 62 (Unbuffered): Reference Raw Card Used
	"Reference_Raw_Card"						: 5,
	"Reference_Raw_Card_Revision"					: 2,
	"Reference_Raw_Card_Extension"					: 1,

	# Byte 63: Address Mapping from Edge Connector to DRAM
	"Rank_1_Mapping_Mirrored"					: 1,
	"Byte_63_Reserved"						: 7,

	# Bytes 64 -116 (Unbuffered): Reserved
	"Module_Specific_Byte_Reserved_"[53]				: 8,

	# Bytes 117 - 118: Module ID: Module Manufacturers JEDEC ID Code
	"Module_Manufacturer_JEDEC_ID_Code"				: 16,

	# Byte 119: Module ID: Module Manufacturing Location
	"Module_Manufacturing_Location"					: 8,

	# Bytes 120 - 121: Module ID: Module Manufacturing Date
	"Module_Manufacturing_Date"					: 16,

	# Bytes 122 - 125: Module ID: Module Serial Number
	"Module_Serial_Number"						: 32,

	# Bytes 126 - 127: Cyclical Redundancy Code
	"Module_CRC"							: 16,

	# Bytes 128 - 145: Module Part Number
	"Module_Part_Number"[18]					: 8,

	# Bytes 146 - 147: Module Revision Code
	"Module_Revision_Code"						: 16,

	# Bytes 148 - 149: DRAM Manufacturers JEDEC ID Code
	"DRAM_Manufacturer_JEDEC_ID_Code"				: 16,

	# Bytes 150 - 175: Manufacturers Specific Data
	"Manufacturer_Specific_Data_byte_" [26]				: 8,

	# Bytes 176 - 255: Open for customer use
	"Customer_use_byte_" [80]					: 8
}
