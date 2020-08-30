/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Register Table Related Functions
 *
 * Contains code to initialize the CPU MSRs and PCI registers with BKDG recommended values
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision$   @e \$Date$
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2017, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/

#include <check_for_wrapper.h>

#ifndef _CPU_TABLE_H_
#define _CPU_TABLE_H_


/**
 * @page regtableimpl Register Table Implementation Guide
 *
 * This register table implementation is modular and extensible, so that support code as
 * well as table data can be family specific or built out if not needed, and new types
 * of table entries can be added with low overhead.  Because many aspects are now generic,
 * there can be common implementations for CPU revision and platform feature matching and for
 * finding and iterating tables.
 *
 * @par Adding a new table entry type.
 *
 * To add a new table entry type follow these steps.
 * <ul>
 * <li>  Add a member to the enum TABLE_ENTRY_TYPE which is a descriptive name of the entry's purpose
 *       or distinct characteristics.
 *
 * <li>  Create an entry data struct with the customized data needed.  For example, custom register designations,
 *       data and mask sizes, or feature comparisons.  Name your struct by adding "_" and upper-casing the enum name
 *       and adding "_TYPE_ENTRY_DATA" at the end.
 *
 * <li>  Add the entry data type as a member of the TABLE_ENTRY_DATA union.  Be aware of the size of your
 *       entry data struct; all table entries in all tables will share any size increase you introduce!
 *
 * <li>  If your data entry contains any member types except for UINT32, you can't use the generic first union member
 *       for the initializers that make up the actual tables (it's just UINT32's). The generic MSR entry is
 *       an example. Follow the steps below:
 *
 *       <ul>
 *       <li>  Make a union which has your entry data type as the first member.  Use TABLE_ENTRY_DATA as the
 *             second member.  Name this with your register followed by "_DATA_INITIALIZER".
 *
 *       <li>  Make a copy of TABLE_ENTRY_FIELDS, and rename it your register "_TYPE_ENTRY_INITIALIZER".  Rename
 *             the TABLE_ENTRY_DATA member of that struct to have the type you created in the previous step.
 *             This type can be used to declare an array of entries and make a register table in some family specific
 *             file.
 *       </ul>
 *
 * <li>  Add the descriptor that will link table entries of your data type to an implementation for it.
 *       <ul>
 *       <li>  Find the options file which instantiates the CPU_SPECIFIC_SERVICES for each logical model that will
 *             support the new entry type.
 *
 *       <li> From there find the instantiation of its TABLE_ENTRY_TYPE_DESCRIPTOR.  Add a descriptor to the
 *            to the list for your new type.  Provide the name of a function which will implement the
 *            entry data.  The function name should reflect that it implements the action for the entry type.
 *            The function must be an instance of F_DO_TABLE_ENTRY.
 *       </ul>
 *
 * <li>  Implement the function for your entry type data.  (If parts of it are family specific add methods to
 *       CPU_SPECIFIC_SERVICES for that and implement them for each family or model required.)  @n
 *       The definition of the function must conform to F_DO_TABLE_ENTRY.
 *       In the function preamble, include a cross reference to the entry enum:
 *       @code
 *       *
 *       * @TableEntryTypeMethod{::MyRegister}
 *       *
 *       @endcode
 *
 * </ul>
 *
 * @par  Adding a new Register Table
 *
 * To add a new register table for a logical CPU model follow the steps below.
 *
 * <ul>
 * <li>  Find the options file which instantiates the CPU_SPECIFIC_SERVICES for the logical model that
 *       should include the table.
 *
 * <li>  From there find the instantiation of its REGISTER_TABLE list. Add the name of the new register table.
 * </ul>
 *
 */

/*------------------------------------------------------------------------------------------*/
/*
 * Define the supported table entries.
 */
/*------------------------------------------------------------------------------------------*/

/**
 * These are the available types of table entries.
 *
 * Each type corresponds to:
 * - a semantics for the type specific data, for example semantics for a Register value,
 * Data value, and Mask value.
 * - optionally, including a method for type specific matching criteria
 * - a method for writing the desired update to the hardware.
 *
 * All types share in common a method to match CPU Family and Model and a method to match
 * platform feature set.
 *
 * N O T E: We use UINT16 for storing table entry type
 */
typedef enum {
  MsrRegister,                 ///< Processor MSR registers.
  PciRegister,                 ///< Processor Config Space registers.
  FamSpecificWorkaround,       ///< Processor Family Specific Workarounds which are @b not practical using the other types.
  ProfileFixup,                ///< Processor Performance Profile fixups to PCI Config Registers.
  CoreCountsPciRegister,       ///< Processor PCI Config Registers which depend on core counts.
  CompUnitCountsPciRegister,   ///< Processor PCI Config Registers which depend on compute unit counts.
  CompUnitCountsMsr,           ///< Processor MSRs which depend on compute unit counts.
  CpuRevPciRegister,           ///< Processor PCI Config Registers which depend on family / revision.
  CpuRevMsr,                   ///< Processor MSR which depend on family / revision.
  CpuRevFamSpecificWorkaround, ///< Processor Family Specific Workarounds which depend on family / revision.
  SmuIndexReg,                 ///< SMU index data registers.
  ProfileFixupSmuIndexReg,     ///< Performance Profile fixups to SMU index data registers.
  CopyBitField,                ///< Copy bitfield from register A to register B
  TableEntryTypeMax,           ///< Not a valid entry type, use for limit checking.
  TableTerminator = 0xFFFF     ///< A signature to indicate end to Jam table.
} TABLE_ENTRY_TYPE;

/*------------------------------------------------------------------------------------------*/
/*
 * Useful types and defines: Selectors, Platform Features, and type specific features.
 */
/*------------------------------------------------------------------------------------------*/

/**
 * Select tables for the current core.
 *
 * This allows more efficient register table processing, by allowing cores to skip
 * redundantly setting PCI registers, for example.  This feature is not intended to
 * be relied on for function:  it is valid to have a single register table with all settings
 * processed by every core; it's just slower.
 *
 */
typedef enum {
  AllCores,                   ///< Select only tables which apply to all cores.
  ComputeUnitPrimary,         ///< Select tables which apply to the primary core of a compute unit (SharedC, SharedNc).
  PrimaryCores,               ///< Select tables which apply to primary cores.
  BscCore,                    ///< Select tables which apply to the boot core.
  TableCoreSelectorMax        ///< Not a valid selector, use for limit checking.
} TABLE_CORE_SELECTOR;

/**
 * Possible time points at which register tables can be processed.
 *
 */
typedef enum {
  AmdRegisterTableTpBeforeApLaunch,     ///< Cpu code just prior to launching APs.
  AmdRegisterTableTpAfterApLaunch,      ///< Cpu code just after all APs have been launched.
  AmdRegisterTableTpBeforeApLaunchSecureS3, ///< Cpu code just prior to launching APs for secure S3
  AmdRegisterTableTpAfterApLaunchSecureS3, ///< Cpu code just after all APs have been launched for secure S3
  MaxAmdRegisterTableTps                ///< Not a valid time point, use for limit checking.
} REGISTER_TABLE_TIME_POINT;

//----------------------------------------------------------------------------
//                         CPU PERFORM EARLY INIT ON CORE
//
//----------------------------------------------------------------------------
/// Flag definition.

// Condition
#define PERFORM_EARLY_WARM_RESET    0x1          // bit  0 --- the related function needs to be run if it's warm reset
#define PERFORM_EARLY_COLD_BOOT     0x2          // bit  1 --- the related function needs to be run if it's cold boot

#define PERFORM_EARLY_ANY_CONDITION (PERFORM_EARLY_WARM_RESET | PERFORM_EARLY_COLD_BOOT)

// Initializer bit pattern values for platform features.
// Keep in synch with the PLATFORM_FEATURES struct!

// The 5 control flow modes.
#define AMD_PF_NFCM           BIT0
#define AMD_PF_UMA            BIT1    // UMA_DR
#define AMD_PF_UMA_IFCM       BIT2
#define AMD_PF_IFCM           BIT3
#define AMD_PF_IOMMU          BIT4
// Degree of HT connectivity possible.
#define AMD_PF_SINGLE_LINK    BIT5
#define AMD_PF_MULTI_LINK     BIT6
// For some legacy MSRs, define a couple core count bits.  Do not continue adding
// core counts to the platform feats, if you need more than this design a table entry type.
// Here, provide exactly 1, exactly 2, or anything else.
#define AMD_PF_SINGLE_CORE    BIT7
#define AMD_PF_DUAL_CORE      BIT8
#define AMD_PF_MULTI_CORE     BIT9

// Not a platform type, but treat all others as AND
#define AMD_PF_AND            BIT31

#define AMD_PF_ALL    (AMD_PF_NFCM          | \
                       AMD_PF_UMA           | \
                       AMD_PF_UMA_IFCM      | \
                       AMD_PF_IFCM          | \
                       AMD_PF_IOMMU         | \
                       AMD_PF_SINGLE_LINK   | \
                       AMD_PF_MULTI_LINK    | \
                       AMD_PF_SINGLE_CORE   | \
                       AMD_PF_DUAL_CORE     | \
                       AMD_PF_MULTI_CORE)
// Do not include AMD_PF_AND in AMD_PF_ALL !

/**
 * The current platform features.
 *
 * Keep this in sync with defines above that are used in the initializers!
 *
 * The comments with the bit number are useful for the computing the reserved member size, but
 * do not write code that assumes you know what bit number one of these members is.
 *
 * These platform features are standard for all logical families and models.
 */
typedef struct {
  UINT32          PlatformNfcm:1;             ///< BIT_0 Normal Flow Control Mode.
  UINT32          PlatformUma:1;              ///< BIT_1 UMA (Display Refresh) Flow Control.
  UINT32          PlatformUmaIfcm:1;          ///< BIT_2 UMA using Isochronous Flow Control.
  UINT32          PlatformIfcm:1;             ///< BIT_3 Isochronous Flow Control Mode (not UMA).
  UINT32          PlatformIommu:1;            ///< BIT_4 IOMMU (a special case Isochronous mode).
  UINT32          PlatformSingleLink:1;       ///< BIT_5 The processor is in a package which implements only a single HT Link.
  UINT32          PlatformMultiLink:1;        ///< BIT_6 The processor is in a package which implements more than one HT Link.
  UINT32          PlatformSingleCore:1;       ///< BIT_7 Single Core processor, for legacy entries.
  UINT32          PlatformDualCore:1;         ///< BIT_8 Dual Core processor, for legacy entries.
  UINT32          PlatformMultiCore:1;        ///< BIT_9 More than dual Core processor, for legacy entries.
  UINT32          :(30 - 9);                  ///< The possibilities are (not quite) endless.
  UINT32          AndPlatformFeats:1;         ///< BIT_31
} PLATFORM_FEATURES;

/**
 * Platform Features
 */
typedef union {
  UINT32            PlatformValue;            ///< Describe Platform Features in UINT32.
  ///< This one goes first, because then initializers use it automatically for the union.
  PLATFORM_FEATURES PlatformFeatures;         ///< Describe Platform Features in structure
} PLATFORM_FEATS;

// Initializer bit patterns for PERFORMANCE_PROFILE_FEATS.
#define PERFORMANCE_REFRESH_REQUEST_32B       BIT0
#define PERFORMANCE_L3_CACHE                  BIT1
#define PERFORMANCE_NO_L3_CACHE               BIT2
#define PERFORMANCE_MCT_ISOC_VARIABLE         BIT3
#define PERFORMANCE_IS_WARM_RESET             BIT4
#define PERFORMANCE_VRM_HIGH_SPEED_ENABLE     BIT5
#define PERFORMANCE_NB_PSTATES_ENABLE         BIT6
#define PERFORMANCE_AND                       BIT31

#define PERFORMANCE_PROFILE_ALL   (PERFORMANCE_REFRESH_REQUEST_32B | \
                                   PERFORMANCE_L3_CACHE            | \
                                   PERFORMANCE_NO_L3_CACHE         | \
                                   PERFORMANCE_MCT_ISOC_VARIABLE   | \
                                   PERFORMANCE_IS_WARM_RESET       | \
                                   PERFORMANCE_VRM_HIGH_SPEED_ENABLE | \
                                   PERFORMANCE_NB_PSTATES_ENABLE)

/**
 * Performance Profile specific Type Features.
 *
 * Register settings for the different control flow modes can have additional dependencies
 */
typedef struct {
  UINT32       RefreshRequest32Byte:1;         ///< BIT_0.   Display Refresh Requests use 32 bytes (32BE).
  UINT32       L3Cache:1;                      ///< BIT_1    L3 Cache is present.
  UINT32       NoL3Cache:1;                    ///< BIT_2    L3 Cache is NOT present.
  UINT32       MctIsocVariable:1;              ///< BIT_3    Mct Isoc Read Priority set to variable.
  UINT32       IsWarmReset:1;                  ///< BIT_4    This boot is on a warm reset, cold reset pass is already completed.
  UINT32       VrmHighSpeed:1;                 ///< BIT_5    Select high speed VRM.
  UINT32       NbPstates:1;                    ///< BIT_6    Northbridge PStates are enabled
  UINT32       :(30 - 6);                      ///< available for future expansion.
  UINT32       AndPerformanceFeats:1;          ///< BIT_31.  AND other selected features.
} PERFORMANCE_PROFILE_FEATURES;

/**
 * Performance Profile features.
 */
typedef union {
  UINT32                        PerformanceProfileValue;     ///< Initializer value.
  PERFORMANCE_PROFILE_FEATURES  PerformanceProfileFeatures;  ///< The performance profile features.
} PERFORMANCE_PROFILE_FEATS;

// Initializer Values for Package Type
#define PACKAGE_TYPE_ALL              0xFFFF       ///< Package Type apply all packages

// Core Range Initializer values.
#define COUNT_RANGE_LOW            0ul
#define COUNT_RANGE_HIGH           0xFFul

// A count range matching none is often useful as the second range, matching will then be
// based on the first range.  A count range all is provided as a first range for default settings.
#define COUNT_RANGE_NONE           ((((COUNT_RANGE_HIGH) << 8) | (COUNT_RANGE_HIGH)) << 16)
#define COUNT_RANGE_ALL            (((COUNT_RANGE_HIGH) << 8) | (COUNT_RANGE_LOW))
#define IGNORE_FREQ_0              (((COUNT_RANGE_HIGH) << 8) | (COUNT_RANGE_HIGH))
#define IGNORE_PROCESSOR_0         (((COUNT_RANGE_HIGH) << 8) | (COUNT_RANGE_HIGH))

#define CORE_RANGE_0(min, max)            ((((UINT32)(max)) << 8) | (UINT32)(min))
#define CORE_RANGE_1(min, max)            (((((UINT32)(max)) << 8) | (UINT32)(min)) << 16)
#define PROCESSOR_RANGE_0(min, max)       ((((UINT32)(max)) << 8) | (UINT32)(min))
#define PROCESSOR_RANGE_1(min, max)       (((((UINT32)(max)) << 8) | (UINT32)(min)) << 16)
#define DEGREE_RANGE_0(min, max)          ((((UINT32)(max)) << 8) | (UINT32)(min))
#define DEGREE_RANGE_1(min, max)          (((((UINT32)(max)) << 8) | (UINT32)(min)) << 16)
#define FREQ_RANGE_0(min, max)            ((((UINT32)(max)) << 8) | (UINT32)(min))
#define FREQ_RANGE_1(min, max)            (((((UINT32)(max)) << 8) | (UINT32)(min)) << 16)
#define COMPUTE_UNIT_RANGE_0(min, max)    ((((UINT32)(max)) << 8) | (UINT32)(min))
#define COMPUTE_UNIT_RANGE_1(min, max)    (((((UINT32)(max)) << 8) | (UINT32)(min)) << 16)

/**
 * Count Range Feature, two count ranges for core counts, processor counts, or node counts.
 */
typedef struct {
  UINT32       Range0Min:8;           ///< The minimum of the first count range.
  UINT32       Range0Max:8;           ///< The maximum of the first count range.
  UINT32       Range1Min:8;           ///< The minimum of the second count range.
  UINT32       Range1Max:8;           ///< The maximum of the second count range.
} COUNT_RANGE_FEATURE;

/**
 * Core Count Ranges for table data.
 *
 * Provide a pair of core count ranges. If the actual core count is included in either range (OR),
 * the feature should be considered a match.
 */
typedef union {
  UINT32       CoreRangeValue;          ///< Initializer value.
  COUNT_RANGE_FEATURE CoreRanges;       ///< The Core Counts.
} CORE_COUNT_RANGES;

/**
 * Compute unit count ranges for table data.
 *
 * Provide a pair of compute unit count ranges. If the actual counts are included in either ranges (OR),
 * the feature should be considered a match.
 */
typedef union {
  UINT32    ComputeUnitRangeValue;                ///< Initializer value.
  COUNT_RANGE_FEATURE ComputeUnitRanges;          ///< The Processor and Node Counts.
} COMPUTE_UNIT_COUNTS;

/*------------------------------------------------------------------------------------------*/
/*
 * The specific data for each table entry.
 */
/*------------------------------------------------------------------------------------------*/
#define BSU8(u8)      ((UINT8) (u8)  & 0xFF)
#define BSU16(u16)    ((UINT16) (u16) & 0xFF), (((UINT16) (u16) >> 8) & 0xFF)
#define BSU32(u32)    ((UINT32) (u32) & 0xFF), (((UINT32) (u32) >> 8) & 0xFF), (((UINT32) (u32) >> 16) & 0xFF), (((UINT32) (u32) >> 24) & 0xFF)
#define BSU64(u64)    ((UINT64) (u64) & 0xFF), (((UINT64) (u64) >> 8) & 0xFF), (((UINT64) (u64) >> 16) & 0xFF), (((UINT64) (u64) >> 24) & 0xFF), \
                      (((UINT64) (u64) >> 32) & 0xFF), (((UINT64) (u64) >> 40) & 0xFF), (((UINT64) (u64) >> 48) & 0xFF), (((UINT64) (u64) >> 56) & 0xFF)

#define MAKE_ENTRY_TYPE(Type)                      BSU16 (Type)
#define MAKE_PERFORMANCE_PROFILE_FEATS(TypeFeats)  BSU32 (TypeFeats)
#define MAKE_CORE_COUNT_RANGES(CoreCounts)         BSU32 (CoreCounts)
#define MAKE_COMPUTE_UNIT_COUNTS(CUCounts)         BSU32 (CUCounts)
#define MAKE_CPU_LOGICAL_ID(Family, Revision)      BSU16 (Family), BSU16 (Revision)
#define MAKE_TABLE_TERMINATOR BSU16 (TableTerminator)

#define NUMBER_OF_TABLE_ENTRIES(Table) ((sizeof (Table) / sizeof (Table[0])) - 1)

/**
 * Table Entry Data for MSR Registers.
 *
 * Apply data to register after mask, for MSRs.
 */
typedef struct {
  UINT32  Address;                    ///< MSR address
  UINT64  Data;                       ///< Data to set in the MSR
  UINT64  Mask;                       ///< Mask to be applied to the MSR. Set every bit of all updated fields.
} MSR_TYPE_ENTRY_DATA;
#define MAKE_MSR_DATA(Address, Data, Mask) BSU32 (Address), BSU64 (Data), BSU64 (Mask)
#define MAKE_MSR_ENTRY(Address, Data, Mask) MAKE_ENTRY_TYPE (MsrRegister), MAKE_MSR_DATA(Address, Data, Mask)

/**
 * Table Entry Data for PCI Registers.
 *
 * Apply data to register after mask, for PCI Config registers.
 */
typedef struct {
  PCI_ADDR  Address;                  ///< Address should contain Function, Offset only.  It will apply to all CPUs
  UINT32    Data;                     ///< Data to be written into PCI device
  UINT32    Mask;                     ///< Mask to be used before data write. Set every bit of all updated fields.
} PCI_TYPE_ENTRY_DATA;
#define MAKE_PCI_DATA(Address, Data, Mask) BSU32 (Address), BSU32 (Data), BSU32 (Mask)
#define MAKE_PCI_ENTRY(Address, Data, Mask) MAKE_ENTRY_TYPE (PciRegister), MAKE_PCI_DATA(Address, Data, Mask)

/**
 * Table Entry Data for Profile Fixup Registers.
 *
 * If TypeFeats matches current config, apply data to register after mask for PCI Config registers.
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats; ///< Profile Fixup Features.
  PCI_TYPE_ENTRY_DATA PciEntry;        ///< The PCI Register entry data.
} PROFILE_FIXUP_TYPE_ENTRY_DATA;
#define MAKE_PROFILE_FIXUP_ENTRY(TypeFeats, Address, Data, Mask) MAKE_ENTRY_TYPE (ProfileFixup), MAKE_PERFORMANCE_PROFILE_FEATS (TypeFeats), MAKE_PCI_DATA (Address, Data, Mask)

/**
 * Core Count dependent PCI registers.
 *
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats; ///< Profile Fixup Features.
  CORE_COUNT_RANGES CoreCounts;        ///< Specify up to two core count ranges to match.
  PCI_TYPE_ENTRY_DATA PciEntry;        ///< The PCI Register entry data.
} CORE_COUNTS_PCI_TYPE_ENTRY_DATA;
#define MAKE_CORE_COUNTS_PCI_ENTRY(TypeFeats, CoreCounts, Address, Data, Mask) MAKE_ENTRY_TYPE (CoreCountsPciRegister), MAKE_PERFORMANCE_PROFILE_FEATS (TypeFeats), MAKE_CORE_COUNT_RANGES (CoreCounts), MAKE_PCI_DATA (Address, Data, Mask)

/**
 * Compute Unit Count dependent PCI registers.
 *
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats;         ///< Profile Fixup Features.
  COMPUTE_UNIT_COUNTS       ComputeUnitCounts; ///< Specify a compute unit count range.
  PCI_TYPE_ENTRY_DATA       PciEntry;          ///< The PCI Register entry data.
} COMPUTE_UNIT_COUNTS_PCI_TYPE_ENTRY_DATA;
#define MAKE_COMPUTE_UNIT_COUNTS_PCI_ENTRY(TypeFeats, CUCounts, Address, Data, Mask) MAKE_ENTRY_TYPE (CompUnitCountsPciRegister), MAKE_PERFORMANCE_PROFILE_FEATS (TypeFeats), MAKE_COMPUTE_UNIT_COUNTS (CUCounts), MAKE_PCI_DATA (Address, Data, Mask)

/**
 * Compute Unit Count dependent MSR registers.
 *
 */
typedef struct {
  COMPUTE_UNIT_COUNTS       ComputeUnitCounts; ///< Specify a compute unit count range.
  MSR_TYPE_ENTRY_DATA       MsrEntry;          ///< The MSR Register entry data.
} COMPUTE_UNIT_COUNTS_MSR_TYPE_ENTRY_DATA;
#define MAKE_COMPUTE_UNIT_COUNTS_MSR_ENTRY(CUCounts, Address, Data, Mask) MAKE_ENTRY_TYPE (CompUnitCountsMsr), MAKE_COMPUTE_UNIT_COUNTS (CUCounts), MAKE_MSR_DATA (Address, Data, Mask)


/**
 * A Family Specific Workaround method.
 *
 * \@TableTypeFamSpecificInstances.
 *
 * When called, the entry's CPU Logical ID and Platform Features matched the current config.
 * The method must implement any specific criteria checking for the workaround.
 *
 * See if you can use the other entries or make an entry specifically for the fix.
 * After all, the purpose of having a table entry is to @b NOT have code which
 * isn't generic feature code, but is family/model specific.
 *
 * @param[in]     Data       The table data value, for example to indicate which CPU and Platform types matched.
 * @param[in]     StdHeader  Config params for library, services.
 */
typedef VOID F_FAM_SPECIFIC_WORKAROUND (
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );
/// Reference to a method.
typedef F_FAM_SPECIFIC_WORKAROUND *PF_FAM_SPECIFIC_WORKAROUND;

/**
 * Table Entry Data for Family Specific Workarounds.
 *
 * See if you can use the other entries or make an entry specifically for the fix.
 * After all, the purpose of having a table entry is to @b NOT have code which
 * isn't generic feature code, but is family/model specific.
 *
 * Call DoAction passing Data.
 */
typedef struct {
  UINT32    FunctionIndex;            ///< A function implementing the workaround.
  UINT32    Data;                     ///< This data is passed to DoAction().
} FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_DATA;
#define MAKE_FAM_SPECIFIC_WORKAROUND_DATA(FunctionIndex, Data) BSU32 (FunctionIndex), BSU32 (Data)
#define MAKE_FAM_SPECIFIC_WORKAROUND_ENTRY(FunctionIndex, Data) MAKE_ENTRY_TYPE (FamSpecificWorkaround), MAKE_FAM_SPECIFIC_WORKAROUND_DATA(FunctionIndex, Data)

/**
 * Table Entry Data for CPU revision specific PCI Registers.
 *
 * Apply data to register after mask, for PCI Config registers.
 */
typedef struct {
  CPU_LOGICAL_ID  CpuRevision;        ///< Common CPU Logical ID match criteria.
  PCI_ADDR        Address;            ///< Address should contain Function, Offset only.  It will apply to all CPUs
  UINT32          Data;               ///< Data to be written into PCI device
  UINT32          Mask;               ///< Mask to be used before data write. Set every bit of all updated fields.
} CPU_REV_PCI_TYPE_ENTRY_DATA;
#define MAKE_CPU_REV_PCI_ENTRY(Family, Revision, Address, Data, Mask) MAKE_ENTRY_TYPE (CpuRevPciRegister), MAKE_CPU_LOGICAL_ID (Family, Revision), MAKE_PCI_DATA (Address, Data, Mask)

/**
 * Table Entry Data for CPU revision specific MSRs.
 *
 * Apply data to register after mask, for MSRs.
 */
typedef struct {
  CPU_LOGICAL_ID  CpuRevision;        ///< Common CPU Logical ID match criteria.
  UINT32          Address;            ///< MSR Address
  UINT64          Data;               ///< Data to be written into MSR
  UINT64          Mask;               ///< Mask to be used before data write. Set every bit of all updated fields.
} CPU_REV_MSR_TYPE_ENTRY_DATA;
#define MAKE_CPU_REV_MSR_ENTRY(Family, Revision, Address, Data, Mask) MAKE_ENTRY_TYPE (CpuRevMsr), MAKE_CPU_LOGICAL_ID (Family, Revision), MAKE_MSR_DATA (Address, Data, Mask)

/**
 * Table Entry Data for Family Specific Workarounds that depend on CPU revision.
 *
 * See if you can use the other entries or make an entry specifically for the fix.
 * After all, the purpose of having a table entry is to @b NOT have code which
 * isn't generic feature code, but is family/model specific.
 *
 * Call DoAction passing Data.
 */
typedef struct {
  CPU_LOGICAL_ID        CpuRevision;        ///< Common CPU Logical ID match criteria.
  UINT32                FunctionIndex;      ///< A function implementing the workaround.
  UINT32                Data;               ///< This data is passed to DoAction().
} CPU_REV_FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_DATA;
#define MAKE_CPU_REV_FAM_SPECIFIC_WORKAROUND_ENTRY(Family, Revision, FunctionIndex, Data) MAKE_ENTRY_TYPE (CpuRevFamSpecificWorkaround), MAKE_CPU_LOGICAL_ID (Family, Revision), MAKE_FAM_SPECIFIC_WORKAROUND_DATA (FunctionIndex, Data)

/**
 * Table Entry Data for SMU Index/Data D0F0xBC_xxxx_xxxx Registers.
 *
 * Apply data to register after mask, for PCI Config registers.
 */
typedef struct {
  UINT32    Index;                    ///< SMU index address
  UINT32    Data;                     ///< Data to be written into PCI device
  UINT32    Mask;                     ///< Mask to be used before data write. Set every bit of all updated fields.
} SMU_INDEX_ENTRY_DATA;
#define MAKE_SMU_INDEX_ENTRY_DATA(Index, Data, Mask) BSU32 (Index), BSU32 (Data), BSU32 (Mask)
#define MAKE_SMU_INDEX_ENTRY(Index, Data, Mask) MAKE_ENTRY_TYPE (SmuIndexReg), MAKE_SMU_INDEX_ENTRY_DATA(Index, Data, Mask)

#define SMU_INDEX_ADDRESS (MAKE_SBDFO (0, 0, 0, 0, 0xB8))


/**
 * Table Entry Data for Profile Fixup to SMU Index/Data D0F0xBC_xxxx_xxxx Registers.
 *
 * If TypeFeats matches current config, apply data to register after mask for SMU Index/Data D0F0xBC_xxxx_xxxx registers.
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats; ///< Profile Fixup Features.
  SMU_INDEX_ENTRY_DATA SmuIndexEntry;  ///< The SMU Index/Data D0F0xBC_xxxx_xxxx register entry data.
} PROFILE_FIXUP_SMU_INDEX_ENTRY_DATA;
#define MAKE_PROFILE_FIXUP_SMU_INDEX_ENTRY(TypeFeats, Index, Data, Mask) MAKE_ENTRY_TYPE (ProfileFixupSmuIndexReg), MAKE_PERFORMANCE_PROFILE_FEATS (TypeFeats), MAKE_SMU_INDEX_ENTRY_DATA (Index, Data, Mask)

/**
 * Bit field description
 *
 * Describe register type, address, MSB, LSB
 */
typedef struct {
  UINT16           RegType;            ///< Register type
  UINT32           Address;            ///< Address
  UINT8            MSB;                ///< Most Significant Bit
  UINT8            LSB;                ///< Least Significant Bit
} COPY_BIT_FIELD_DESCRIPTION;
#define MAKE_COPY_BIT_FIELD_DESCRIPTION(RegType, Address, Msb, Lsb) MAKE_ENTRY_TYPE (RegType), BSU32 (Address), BSU8 (Msb), BSU8 (Lsb)

/**
 * Table Entry Data for copying bitfield from register A to register B.
 *
 * Copy bitfield from register A to register B.
 */
typedef struct {
  COPY_BIT_FIELD_DESCRIPTION Destination; ///< Destination register descriptor
  COPY_BIT_FIELD_DESCRIPTION Source;      ///< Source register descriptor
} COPY_BIT_FIELD_ENTRY_DATA;
#define COPY_BIT_FIELD_DEST(RegType, Address, Msb, Lsb) MAKE_COPY_BIT_FIELD_DESCRIPTION (RegType, Address, Msb, Lsb)
#define COPY_BIT_FIELD_SOURCE(RegType, Address, Msb, Lsb) MAKE_COPY_BIT_FIELD_DESCRIPTION (RegType, Address, Msb, Lsb)
#define MAKE_COPY_BIT_FIELD_ENTRY(Dest, Src) MAKE_ENTRY_TYPE (CopyBitField), Dest, Src
/*------------------------------------------------------------------------------------------*/
/*
 * A complete register table and table entries.
 */
/*------------------------------------------------------------------------------------------*/

/**
 * Format of table entries :
 *
 *  UINT16          EntryType \
 *  VariableLength  EntryData / one entry
 *  UINT16          EntryType \
 *  VariableLength  EntryData / one entry
 *  ...                       \
 *  ...                       / more entries...
 */

/**
 * All the available entry data types.
 *
 * we use TABLE_ENTRY_DATA in copy bitfield entry
 *
 */
typedef union {
  MSR_TYPE_ENTRY_DATA                              MsrEntry;                ///< MSR entry.
  PCI_TYPE_ENTRY_DATA                              PciEntry;                ///< PCI entry.
  FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_DATA          FamSpecificEntry;        ///< Family Specific Workaround entry.
  PROFILE_FIXUP_TYPE_ENTRY_DATA                    FixupEntry;              ///< Profile Fixup entry.
  CORE_COUNTS_PCI_TYPE_ENTRY_DATA                  CoreCountEntry;          ///< Core count dependent settings.
  COMPUTE_UNIT_COUNTS_PCI_TYPE_ENTRY_DATA          CompUnitCountEntry;      ///< Compute unit count dependent entry.
  COMPUTE_UNIT_COUNTS_MSR_TYPE_ENTRY_DATA          CompUnitCountMsrEntry;   ///< Compute unit count dependent MSR entry.
  CPU_REV_PCI_TYPE_ENTRY_DATA                      CpuRevPciEntry;          ///< CPU revision PCI entry.
  CPU_REV_FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_DATA  CpuRevFamSpecificEntry;  ///< CPU revision Family Specific Workaround entry.
  SMU_INDEX_ENTRY_DATA                             SmuIndexEntry;           ///< SMU Index Data entry.
  PROFILE_FIXUP_SMU_INDEX_ENTRY_DATA               ProfileFixupSmuIndexEntry; ///< Performance Profile fixups to SMU index data registers entry.
  COPY_BIT_FIELD_ENTRY_DATA                        CopyBitFieldEntry;       ///< Copy bitfield entry.
} TABLE_ENTRY_DATA;

/**
 * Register Table Entry common fields.
 *
 * All the various types of register table entries are subclasses of this object.
 */
typedef struct {
  UINT16                 EntryType;                  ///< The type of table entry this is.
  TABLE_ENTRY_DATA       EntryData;                  ///< The pointer to the first entry.
} TABLE_ENTRY_FIELDS;

/**
 * An entire register table.
 */
typedef struct {
  UINT32               Selector;                     ///< For efficiency, these cores should process this table
  CONST UINT8*         Table;                        ///< The table entries.
} REGISTER_TABLE;

/**
 * An entire register table at given time point.
 */
typedef struct {
  REGISTER_TABLE_TIME_POINT TimePoint;               ///< Time point
  CONST REGISTER_TABLE* CONST * CONST TableList;     ///< The table list.
} REGISTER_TABLE_AT_GIVEN_TP;
/*------------------------------------------------------------------------------------------*/
/*
 * Describe implementers for table entries.
 */
/*------------------------------------------------------------------------------------------*/

/**
 * Implement the semantics of a Table Entry Type.
 *
 * @TableEntryTypeInstances.
 *
 * @param[in]       CurrentEntry        The type specific entry data to be implemented (that is written).
 * @param[in]       PlatformConfig      Config handle for platform specific information
 * @param[in]       StdHeader           Config params for library, services.
 */
typedef VOID F_DO_TABLE_ENTRY (
  IN       UINT8                 **CurrentEntry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/// Reference to a method
typedef F_DO_TABLE_ENTRY *PF_DO_TABLE_ENTRY;

/**
 * Describe the attributes of a Table Entry Type.
 */
typedef struct {
  UINT16                 EntryType;                  ///< The type of table entry this describes.
  PF_DO_TABLE_ENTRY      DoTableEntry;               ///< Provide all semantics associated with TABLE_ENTRY_DATA
} TABLE_ENTRY_TYPE_DESCRIPTOR;

/*------------------------------------------------------------------------------------------*/
/*
 * Table related function prototypes (many are instance of F_DO_TABLE_ENTRY method).
 */
/*------------------------------------------------------------------------------------------*/

/**
 * Get the next register table
 */
REGISTER_TABLE **GetNextRegisterTable (
  IN       UINT32                  Selector,
  IN       REGISTER_TABLE        **RegisterTableList,
  IN OUT   REGISTER_TABLE       ***RegisterTableHandle,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * If current core is CoreSelector core
 */
BOOLEAN
IsCoreSelector (
  IN       UINT32                    Selector,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

/**
 * Set the registers for this core based on entries in a list of Register Table.
 */
VOID
SetRegistersFromTable (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       UINT8                  *RegisterEntry,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Set the registers for this core based on entries in a list of Register Table.
 */
VOID
SetRegistersFromTableList (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       REGISTER_TABLE         **RegisterTableList,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Processes the register table at the given time point.
 */
AGESA_STATUS
SetRegistersFromTablesAtGivenTimePoint (
  IN       VOID                      *PlatformConfig,
  IN       REGISTER_TABLE_TIME_POINT  TimePoint,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

/**
 * Find the features of the running platform.
 */
VOID
GetPlatformFeatures (
     OUT   PLATFORM_FEATS    *Features,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * Checks register table entry type specific criteria to the platform.
 */
BOOLEAN
DoesEntryTypeSpecificInfoMatch (
  IN       UINT32   PlatformTypeSpecificFeatures,
  IN       UINT32   EntryTypeFeatures
  );

/**
 * Perform the MSR Register Entry.
 */
VOID
SetRegisterForMsrEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the CPU Rev MSR Entry.
 */
VOID
SetRegisterForCpuRevMsrEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the PCI Register Entry.
 */
VOID
SetRegisterForPciEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the PCI Register Entry.
 */
VOID
SetRegisterForCpuRevPciEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Performance Profile PCI Register Entry.
 */
VOID
SetRegisterForPerformanceProfileEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Core Counts Performance PCI Register Entry.
 */
VOID
SetRegisterForCoreCountsPerformanceEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Compute Unit Counts PCI Register Entry.
 */
VOID
SetRegisterForComputeUnitCountsEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Compute Unit Counts MSR Register Entry.
 */
VOID
SetMsrForComputeUnitCountsEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Family Specific Workaround Register Entry.
 */
VOID
SetRegisterForFamSpecificWorkaroundEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Family Specific Workaround Register Entry.
 */
VOID
SetRegisterForCpuRevFamSpecificWorkaroundEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the SMU Index/Data Register Entry.
 */
VOID
SetSmuIndexRegisterEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Performance Profile SMU Index/Data Register Entry.
 */
VOID
SetSmuIndexRegisterForPerformanceEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Copy Bitfield Entry.
 */
VOID
CopyBitFieldEntry (
  IN       UINT8                 **Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Compare counts to a pair of ranges.
 */
BOOLEAN
IsEitherCountInRange (
  IN       UINTN                FirstCount,
  IN       UINTN                SecondCount,
  IN       COUNT_RANGE_FEATURE  Ranges
  );

/**
 * Returns the performance profile features list of the currently running processor core.
 */
VOID
GetPerformanceFeatures (
     OUT   PERFORMANCE_PROFILE_FEATS    *Features,
  IN       PLATFORM_CONFIGURATION       *PlatformConfig,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  );

#endif  // _CPU_TABLE_H_

