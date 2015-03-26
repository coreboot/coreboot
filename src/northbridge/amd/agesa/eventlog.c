/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <stdint.h>
#include <string.h>

#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "amdlib.h"
#include "AGESA.h"
#include "AMD.h"


/*
 * Possible AGESA_STATUS values:
 *
 * 0x0 = AGESA_SUCCESS
 * 0x1 = AGESA_UNSUPPORTED
 * 0x2 = AGESA_BOUNDS_CHK
 * 0x3 = AGESA_ALERT
 * 0x4 = AGESA_WARNING
 * 0x5 = AGESA_ERROR
 * 0x6 = AGESA_CRITICAL
 * 0x7 = AGESA_FATAL
 */
static const char * decodeAGESA_STATUS(AGESA_STATUS sret)
{
	const char* statusStrings[] = { "AGESA_SUCCESS", "AGESA_UNSUPPORTED",
					"AGESA_BOUNDS_CHK", "AGESA_ALERT",
					"AGESA_WARNING", "AGESA_ERROR",
					"AGESA_CRITICAL", "AGESA_FATAL"
					};
	if (sret > 7) return "unknown"; /* Non-AGESA error code */
	return statusStrings[sret];
}

#if 0

/**
 *
 */
static void agesa_bound_check(EVENT_PARAMS *event)
{
	switch (event->EventInfo) {
		case CPU_ERROR_HEAP_IS_FULL:
			printk(BIOS_DEBUG, "Heap allocation for specified buffer handle failed as heap is full\n");
			break;

		case CPU_ERROR_HEAP_BUFFER_HANDLE_IS_ALREADY_USED:
			printk(BIOS_DEBUG, "Allocation incomplete as buffer has previously been allocated\n");
			break;

		case CPU_ERROR_HEAP_BUFFER_HANDLE_IS_NOT_PRESENT:
			printk(BIOS_DEBUG, "Unable to locate buffer handle or deallocate heap as buffer handle cannot be located\n");
			break;

		case CPU_ERROR_HEAP_BUFFER_IS_NOT_PRESENT:
			printk(BIOS_DEBUG, "Unable to locate pointer to the heap buffer\n");
			break;

		default:
			break;
	}
}

/**
 *
 */
static void agesa_alert(EVENT_PARAMS *event)
{
	switch (event->EventInfo) {
		case MEM_ALERT_USER_TMG_MODE_OVERRULED:
			printk(BIOS_DEBUG, "Socket %x Dct %x Channel %x "

					"TIMING_MODE_SPECIFIC is requested but can not be applied to current configurations.\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case MEM_ALERT_ORG_MISMATCH_DIMM:
			printk(BIOS_DEBUG, "Socket %x Dct %x Channel %x "
					"DIMM organization miss-match\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case MEM_ALERT_BK_INT_DIS:
			printk(BIOS_DEBUG, "Socket %x Dct %x Channel %x "
					"Bank interleaving disable for internal issue\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case CPU_EVENT_BIST_ERROR:
			printk(BIOS_DEBUG, "BIST error: %x reported on Socket %x Core %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_HW_SYNCFLOOD:
			printk(BIOS_DEBUG, "HT_EVENT_DATA_HW_SYNCFLOOD error on Socket %x Link %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2);
			break;

		case HT_EVENT_HW_HTCRC:
			printk(BIOS_DEBUG, "HT_EVENT_HW_HTCRC error on Socket %x Link %x Lanemask:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		default:
			break;
	}
}

/**
 *
 */
static void agesa_warning(EVENT_PARAMS *event)
{
/*
	if (event->EventInfo == CPU_EVENT_STACK_REENTRY) {
		printk(BIOS_DEBUG,
				"The stack has already been enabled and this is a
				redundant invocation of AMD_ENABLE_STACK. There is no event logged and
				no data values. The event sub-class is returned along with the status code\n");
		return;
	}
*/

	switch (event->EventInfo >> 24) {
		case 0x04:
			printk(BIOS_DEBUG, "Memory: Socket %x Dct %x Channel%x ",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case 0x08:
			printk(BIOS_DEBUG, "Processor: ");
			break;

		case 0x10:
			printk(BIOS_DEBUG, "Hyper Transport: ");
			break;

		default:
			break;
	}

	switch (event->EventInfo) {
		case MEM_WARNING_UNSUPPORTED_QRDIMM:
			printk(BIOS_DEBUG, "QR DIMMs detected but not supported\n");
			break;

		case MEM_WARNING_UNSUPPORTED_UDIMM:
			printk(BIOS_DEBUG, "Unbuffered DIMMs detected but not supported\n");
			break;

		case MEM_WARNING_UNSUPPORTED_SODIMM:
			printk(BIOS_DEBUG, "SO-DIMMs detected but not supported");
			break;

		case MEM_WARNING_UNSUPPORTED_X4DIMM:
			printk(BIOS_DEBUG, "x4 DIMMs detected but not supported");
			break;

		case MEM_WARNING_UNSUPPORTED_RDIMM:
			printk(BIOS_DEBUG, "Registered DIMMs detected but not supported");
			break;

/*
		case MEM_WARNING_UNSUPPORTED_LRDIMM:
			printk(BIOS_DEBUG, "Load Reduced DIMMs detected but not supported");
			break;
*/

		case MEM_WARNING_NO_SPDTRC_FOUND:
			printk(BIOS_DEBUG, "NO_SPDTRC_FOUND");
			break;

		case MEM_WARNING_EMP_NOT_SUPPORTED:
			printk(BIOS_DEBUG, "Processor is not capable for EMP");//
			break;

		case MEM_WARNING_EMP_CONFLICT:
			printk(BIOS_DEBUG, "EMP cannot be enabled if channel interleaving bank interleaving, or bank swizzle is enabled\n");//
			break;

		case MEM_WARNING_EMP_NOT_ENABLED:
			printk(BIOS_DEBUG, "Memory size is not power of two\n");//
			break;

		case MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED:
			printk(BIOS_DEBUG, "MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED\n");
			break;

		case MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED:
			printk(BIOS_DEBUG, "MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED\n");
			break;

		case MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED:
			printk(BIOS_DEBUG, "MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED\n");
			break;

		case MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED:
			printk(BIOS_DEBUG, "MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED\n");
			break;

		case MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED:
			printk(BIOS_DEBUG, "MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED\n");
			break;

/*
		case MEM_WARNING_INITIAL_DDR3VOLT_NONZERO:
			printk(BIOS_DEBUG, "MEM_WARNING_INITIAL_DDR3VOLT_NONZERO\n");
			break;

		case MEM_WARNING_NO_COMMONLY_SUPPORTED_VDDIO:
			printk(BIOS_DEBUG, "MEM_WARNING_NO_COMMONLY_SUPPORTED_VDDIO\n");
			break;
*/

		case CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR:
			printk(BIOS_DEBUG, "Allocation rule number that has been violated:");
			if ((event->EventInfo & 0x000000FF) == 0x01) {
				printk(BIOS_DEBUG, "AGESA_CACHE_SIZE_REDUCED\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x02) {
				printk(BIOS_DEBUG, "AGESA_CACHE_REGIONS_ACROSS_1MB\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x03) {
				printk(BIOS_DEBUG, "AGESA_CACHE_REGIONS_ACROSS_4GB\n");
			}
			printk(BIOS_DEBUG, "cache region index:%x, start:%x size:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case CPU_WARNING_ADJUSTED_LEVELING_MODE:
			printk(BIOS_DEBUG, "CPU_WARNING_ADJUSTED_LEVELING_MODE "
					"requested: %x, actual level:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2);
			break;

		case CPU_EVENT_PM_PSTATE_OVERCURRENT:
			printk(BIOS_DEBUG, "CPU_EVENT_PM_PSTATE_OVERCURRENT "
						"Socket: %x, Pstate:%x\n",
						(unsigned int)event->DataParam1,
						(unsigned int)event->DataParam2);
			break;

		case CPU_WARNING_NONOPTIMAL_HT_ASSIST_CFG:
			printk(BIOS_DEBUG, "CPU_WARNING_NONOPTIMAL_HT_ASSIST_CFG\n");
			break;

/*
		case CPU_EVENT_UNKNOWN_PROCESSOR_REVISION:
			printk(BIOS_DEBUG, "CPU_EVENT_UNKNOWN_PROCESSOR_REVISION, socket: %lx, cpuid:%lx\n",
				event->DataParam1,
				event->DataParam2);
			break;
*/

		case HT_EVENT_OPT_REQUIRED_CAP_RETRY:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_REQUIRED_CAP_RETRY, Socket %lx Link %lx Depth %lx\n",
				event->DataParam1,
				event->DataParam2,
				event->DataParam3);
			break;

		case HT_EVENT_OPT_REQUIRED_CAP_GEN3:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_REQUIRED_CAP_GEN3, Socket %x Link %x Depth %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_OPT_UNUSED_LINKS:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_UNUSED_LINKS, SocketA%x LinkA%x SocketB%x LinkB%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
			break;

		case HT_EVENT_OPT_LINK_PAIR_EXCEED:
			printk(BIOS_DEBUG, "HT_EVENT_OPT_LINK_PAIR_EXCEED, SocketA%x MasterLink%x SocketB%x AltLink%x\n",

					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
		default:
			break;
	}
}

/**
 *
 */
static void agesa_error(EVENT_PARAMS *event)
{

	switch (event->EventInfo >> 24) {
		case 0x04:
			printk(BIOS_DEBUG, "Memory: Socket %x Dct %x Channel%x ",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case 0x08:
			printk(BIOS_DEBUG, "Processor: ");
			break;

		case 0x10:
			printk(BIOS_DEBUG, "Hyper Transport: ");
			break;

		default:
			break;
	}

	switch (event->EventInfo) {
		case MEM_ERROR_NO_DQS_POS_RD_WINDOW:
			printk(BIOS_DEBUG, "No DQS Position window for RD DQS\n");
			break;

		case MEM_ERROR_SMALL_DQS_POS_RD_WINDOW:
			printk(BIOS_DEBUG, "Small DQS Position window for RD DQS\n");
			break;

		case MEM_ERROR_NO_DQS_POS_WR_WINDOW:
			printk(BIOS_DEBUG, "No DQS Position window for WR DQS\n");
			break;

		case MEM_ERROR_SMALL_DQS_POS_WR_WINDOW:
			printk(BIOS_DEBUG, "Small DQS Position window for WR DQS\n");
			break;

		case MEM_ERROR_DIMM_SPARING_NOT_ENABLED:
			printk(BIOS_DEBUG, "DIMM sparing has not been enabled for an internal issues\n");
			break;

		case MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE:
			printk(BIOS_DEBUG, "Receive Enable value is too large\n");
			break;
		case MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW:
			printk(BIOS_DEBUG, "There is no DQS receiver enable window\n");
			break;

		case MEM_ERROR_DRAM_ENABLED_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling DramEnabled bit\n");
			break;

		case MEM_ERROR_DCT_ACCESS_DONE_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling DctAccessDone bit\n");
			break;

		case MEM_ERROR_SEND_CTRL_WORD_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling SendCtrlWord bit\n");
			break;

		case MEM_ERROR_PREF_DRAM_TRAIN_MODE_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling PrefDramTrainMode bit\n");
			break;

		case MEM_ERROR_ENTER_SELF_REF_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling EnterSelfRef bit\n");
			break;

		case MEM_ERROR_FREQ_CHG_IN_PROG_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling FreqChgInProg bit\n");
			break;

		case MEM_ERROR_EXIT_SELF_REF_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling ExitSelfRef bit\n");
			break;

		case MEM_ERROR_SEND_MRS_CMD_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling SendMrsCmd bit\n");
			break;

		case MEM_ERROR_SEND_ZQ_CMD_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling SendZQCmd bit\n");
			break;

		case MEM_ERROR_DCT_EXTRA_ACCESS_DONE_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling DctExtraAccessDone bit\n");
			break;

		case MEM_ERROR_MEM_CLR_BUSY_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling MemClrBusy bit\n");
			break;

		case MEM_ERROR_MEM_CLEARED_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling MemCleared bit\n");
			break;

		case MEM_ERROR_FLUSH_WR_TIME_OUT:
			printk(BIOS_DEBUG, "Time out when polling FlushWr bit\n");
			break;

		case MEM_ERROR_MAX_LAT_NO_WINDOW:
			printk(BIOS_DEBUG, "Fail to find pass during Max Rd Latency training\n");
			break;

		case MEM_ERROR_PARALLEL_TRAINING_LAUNCH_FAIL:
			printk(BIOS_DEBUG, "Fail to launch training code on an AP\n");
			break;

		case MEM_ERROR_PARALLEL_TRAINING_TIME_OUT:
			printk(BIOS_DEBUG, "Fail to finish parallel training\n");
			break;

		case MEM_ERROR_NO_ADDRESS_MAPPING:
			printk(BIOS_DEBUG, "No address mapping found for a dimm\n");
			break;

		case MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW_EQUAL_LIMIT:
			printk(BIOS_DEBUG, "There is no DQS receiver enable window and the value is equal to the largest value\n");
			break;

		case MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE_LIMIT_LESS_ONE:
			printk(BIOS_DEBUG, "Receive Enable value is too large and is 1 less than limit\n");
			break;

		case MEM_ERROR_CHECKSUM_NV_SPDCHK_RESTRT_ERROR:
			printk(BIOS_DEBUG, "SPD Checksum error for NV_SPDCHK_RESTRT\n");
			break;

		case MEM_ERROR_NO_CHIPSELECT:
			printk(BIOS_DEBUG, "No chipselects found\n");
			break;

		case MEM_ERROR_UNSUPPORTED_333MHZ_UDIMM:
			printk(BIOS_DEBUG, "Unbuffered dimm is not supported at 333MHz\n");
			break;

		case MEM_ERROR_WL_PRE_OUT_OF_RANGE:
			printk(BIOS_DEBUG, "Returned PRE value during write levelizzation was out of range\n");
			break;

		case CPU_ERROR_BRANDID_HEAP_NOT_AVAILABLE:
			printk(BIOS_DEBUG, "No heap is allocated for BrandId structure\n");
			break;

		case CPU_ERROR_MICRO_CODE_PATCH_IS_NOT_LOADED:
			printk(BIOS_DEBUG, "Unable to load micro code patch\n");
			break;

		case CPU_ERROR_PSTATE_HEAP_NOT_AVAILABLE:
			printk(BIOS_DEBUG, "No heap is allocated for the Pstate structure\n");
			break;

/*
		case CPU_ERROR_PM_NB_PSTATE_MISMATCH:
			printk(BIOS_DEBUG, "NB P-state indicated by Index was disabled due to mismatch between processors\n");
			break;
*/

		case CPU_EVENT_EXECUTION_CACHE_ALLOCATION_ERROR:
			printk(BIOS_DEBUG, "Allocation rule number that has been violated:");
			if ((event->EventInfo & 0x000000FF) == 0x04) {
				printk(BIOS_DEBUG, "AGESA_REGION_NOT_ALIGNED_ON_BOUNDARY\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x05) {
				printk(BIOS_DEBUG, "AGESA_START_ADDRESS_LESS_D0000\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x06) {
				printk(BIOS_DEBUG, "AGESA_THREE_CACHE_REGIONS_ABOVE_1MB\n");
			} else if ((event->EventInfo & 0x000000FF) == 0x07) {
				printk(BIOS_DEBUG, "AGESA_DEALLOCATE_CACHE_REGIONS\n");
			}
			printk(BIOS_DEBUG, "cache region index:%x, start:%x size:%x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_COH_NO_TOPOLOGY:
			printk(BIOS_DEBUG, "no Matching Topology was found during coherent initializatio TotalHtNodes: %x\n",
					(unsigned int)event->DataParam1);
			break;

		case HT_EVENT_NCOH_BUID_EXCEED:
			printk(BIOS_DEBUG, "there is a limit of 32 unit IDs per chain Socket%x Link%x Depth%x"
					"Current BUID: %x, Unit Count: %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4 >> 16,
					(unsigned int)event->DataParam4 & 0x0000FFFF);
			break;

		 case HT_EVENT_NCOH_BUS_MAX_EXCEED:
			printk(BIOS_DEBUG, "maximum auto bus limit exceeded, Socket %x Link %x Bus %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_NCOH_CFG_MAP_EXCEED:
			printk(BIOS_DEBUG, "there is a limit of four non-coherent chains, Socket %x Link %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2);
			break;

		case HT_EVENT_NCOH_DEVICE_FAILED:
			printk(BIOS_DEBUG, "after assigning an IO Device an ID, it does not respond at the new ID"
					"Socket %x Link %x Depth %x DeviceID %x\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
		default:
			break;
	}
}
/**
 *
 */
static void agesa_critical(EVENT_PARAMS *event)
{
	switch (event->EventInfo) {
		case MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR3:
			printk(BIOS_DEBUG, "Socket: %x, Heap allocation error for DMI table for DDR3\n",
					(unsigned int)event->DataParam1);
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR2:
			printk(BIOS_DEBUG, "Socket: %x, Heap allocation error for DMI table for DDR2\n",
					(unsigned int)event->DataParam1);
			break;

		case MEM_ERROR_UNSUPPORTED_DIMM_CONFIG:
			printk(BIOS_DEBUG, "Socket: %x, Dimm population is not supported\n",
					(unsigned int)event->DataParam1);
			break;

		case HT_EVENT_COH_PROCESSOR_TYPE_MIX:
			printk(BIOS_DEBUG, "Socket %x Link %x TotalSockets %x, HT_EVENT_COH_PROCESSOR_TYPE_MIX \n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case HT_EVENT_COH_MPCAP_MISMATCH:
			printk(BIOS_DEBUG, "Socket %x Link %x MpCap %x TotalSockets %x, HT_EVENT_COH_MPCAP_MISMATCH\n",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3,
					(unsigned int)event->DataParam4);
		default:
			break;
	}
}

/**
 *
 */
static void agesa_fatal(EVENT_PARAMS *event)
{

	switch (event->EventInfo >> 24) {
		case 0x04:
			printk(BIOS_DEBUG, "Memory: Socket %x Dct %x Channel%x ",
					(unsigned int)event->DataParam1,
					(unsigned int)event->DataParam2,
					(unsigned int)event->DataParam3);
			break;

		case 0x08:
			printk(BIOS_DEBUG, "Processor: ");
			break;

		case 0x10:
			printk(BIOS_DEBUG, "Hyper Transport: ");
			break;

		default:
			break;
	}

	switch (event->EventInfo) {
		case MEM_ERROR_MINIMUM_MODE:
			printk(BIOS_DEBUG, "Running in minimum mode\n");
			break;

		case MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM:
			printk(BIOS_DEBUG, "DIMM modules are missmatched\n");
			break;

		case MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM:
			printk(BIOS_DEBUG, "No DIMMs have been foun\n");
			break;

		case MEM_ERROR_MISMATCH_DIMM_CLOCKS:
			printk(BIOS_DEBUG, "DIMM clocks miss-matched\n");
			break;

		case MEM_ERROR_NO_CYC_TIME:
			printk(BIOS_DEBUG, "No cycle time found\n");
			break;
		case MEM_ERROR_HEAP_ALLOCATE_DYN_STORING_OF_TRAINED_TIMINGS:
			printk(BIOS_DEBUG, "Heap allocation error with dynamic storing of trained timings\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs:
			printk(BIOS_DEBUG, "Heap allocation error for DCT_STRUCT and CH_DEF_STRUCT\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_REMOTE_TRAINING_ENV:
			printk(BIOS_DEBUG, "Heap allocation error with REMOTE_TRAINING_ENV\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_SPD:
			printk(BIOS_DEBUG, "Heap allocation error for SPD data\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_RECEIVED_DATA:
			printk(BIOS_DEBUG, "Heap allocation error for RECEIVED_DATA during parallel training\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_S3_SPECIAL_CASE_REGISTERS:
			printk(BIOS_DEBUG, "Heap allocation error for S3 \"SPECIAL_CASE_REGISTER\"\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_TRAINING_DATA:
			printk(BIOS_DEBUG, "Heap allocation error for Training Data\n");
			break;

		case MEM_ERROR_HEAP_ALLOCATE_FOR_IDENTIFY_DIMM_MEM_NB_BLOCK:
			printk(BIOS_DEBUG, "Heap allocation error for  DIMM Identify \"MEM_NB_BLOCK\"\n");
			break;

		case MEM_ERROR_NO_CONSTRUCTOR_FOR_IDENTIFY_DIMM:
			printk(BIOS_DEBUG, "No Constructor for DIMM Identify\n");
			break;

		case MEM_ERROR_VDDIO_UNSUPPORTED:
			printk(BIOS_DEBUG, "VDDIO of the dimms on the board is not supported\n");
			break;

		case CPU_EVENT_PM_ALL_PSTATE_OVERCURRENT:
			printk(BIOS_DEBUG, "Socket: %x, All PStates exceeded the motherboard current limit on specified socket\n",
				(unsigned int)event->DataParam1);
			break;

		default:
			break;
	}
}

/**
 *
 * Interprte the agesa event log to an user readable string
 */
static void interpret_agesa_eventlog(EVENT_PARAMS *event)
{
	switch (event->EventClass) {
		case AGESA_BOUNDS_CHK:
			agesa_bound_check(event);
			break;

		case AGESA_ALERT:
			agesa_alert(event);
			break;

		case AGESA_WARNING:
			agesa_warning(event);
			break;

		case AGESA_ERROR:
			agesa_error(event);
			break;

		case AGESA_CRITICAL:
			agesa_critical(event);
			break;

		case AGESA_FATAL:
			agesa_fatal(event);
			break;

		default:
			break;
	}
}
#endif

static void amd_readeventlog(AMD_CONFIG_PARAMS *StdHeader)
{
	EVENT_PARAMS AmdEventParams;

	memset(&AmdEventParams, 0, sizeof(EVENT_PARAMS));

	AmdEventParams.StdHeader.AltImageBasePtr = 0;
	AmdEventParams.StdHeader.CalloutPtr = (CALLOUT_ENTRY) &GetBiosCallout;
	AmdEventParams.StdHeader.Func = 0;
	AmdEventParams.StdHeader.ImageBasePtr = 0;
	AmdEventParams.StdHeader.HeapStatus = StdHeader->HeapStatus;

	AmdReadEventLog(&AmdEventParams);
	while (AmdEventParams.EventClass != 0) {
		printk(BIOS_DEBUG,"\nEventLog:  EventClass = %x, EventInfo = %x.\n",
				(unsigned int)AmdEventParams.EventClass,
				(unsigned int)AmdEventParams.EventInfo);
		printk(BIOS_DEBUG,"  Param1 = %x, Param2 = %x.\n",
				(unsigned int)AmdEventParams.DataParam1,
				(unsigned int)AmdEventParams.DataParam2);
		printk(BIOS_DEBUG,"  Param3 = %x, Param4 = %x.\n",
				(unsigned int)AmdEventParams.DataParam3,
				(unsigned int)AmdEventParams.DataParam4);
		AmdReadEventLog(&AmdEventParams);
	}
}


void agesawrapper_trace(AGESA_STATUS ret, AMD_CONFIG_PARAMS *StdHeader, const char *func)
{
	printk(BIOS_DEBUG, "%s() returned %s\n", func, decodeAGESA_STATUS(ret));
	if (ret != AGESA_SUCCESS)
		amd_readeventlog(StdHeader);
}
