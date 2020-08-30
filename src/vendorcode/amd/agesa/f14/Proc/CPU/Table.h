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
 * @e \$Revision: 38402 $   @e \$Date: 2010-09-24 02:11:44 +0800 (Fri, 24 Sep 2010) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * ***************************************************************************
 *
 */

#ifndef _CPU_TABLE_H_
#define _CPU_TABLE_H_

#define MAX_ENTRY_TYPE_ITEMS32 6        // The maximum number of initializer items for UINT32 entry data types.

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
 */
typedef enum {
  MsrRegister,                 ///< Processor MSR registers.
  PciRegister,                 ///< Processor Config Space registers.
  FamSpecificWorkaround,       ///< Processor Family Specific Workarounds which are @b not practical using the other types.
  HtPhyRegister,               ///< Processor HT Phy registers.
  HtPhyRangeRegister,          ///< Processor HT Phy range of contiguous registers (ex. 40h:48h).
  DeemphasisRegister,          ///< Processor Deemphasis register (HT Phy special case).
  HtPhyFreqRegister,           ///< Processor Frequency dependent HT Phy settings.
  ProfileFixup,                ///< Processor Performance Profile fixups to PCI Config Registers.
  HtHostPciRegister,           ///< Processor Ht Host capability registers (PCI Config).
  HtHostPerfPciRegister,       ///< Processor Ht Host capability registers which depend on performance features.
  HtTokenPciRegister,          ///< Processor Ht Link Token count registers.
  CoreCountsPciRegister,       ///< Processor PCI Config Registers which depend on core counts.
  ProcCountsPciRegister,       ///< Processor PCI Config Registers which depend on processor counts.
  CompUnitCountsPciRegister,   ///< Processor PCI Config Registers which depend on compute unit counts.
  TokenPciRegister,            ///< Processor northbridge Token Count register which may be dependent on connectivity.
  HtFeatPciRegister,           ///< Processor HT Link feature dependant PCI Config Registers.
  HtPhyProfileRegister,        ///< Processor HT Phy registers which depend on performance features.
  HtLinkPciRegister,           ///< Processor HT Link registers (one per link) not part of HT Host capability.
  TableEntryTypeMax            ///< Not a valid entry type, use for limit checking.
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
  CorePairPrimary,            ///< Select tables which apply to the primary core of a compute unit (SharedC, SharedNc).
  PrimaryCores,               ///< Select tables which apply to primary cores.
  BscCore,                    ///< Select tables which apply to the boot core.
  TableCoreSelectorMax        ///< Not a valid selector, use for limit checking.
} TABLE_CORE_SELECTOR;

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

// Sublink Types are defined so they can match each attribute against either
// sublink zero or one.  The table entry must contain the correct matching
// values based on the register. This is available in the BKDG, for each register
// which sublink it controls.  If the register is independent of sublink, OR values
// together or use HT_LINKTYPE_ALL to match if either sublink matches (ex. E0 - E5).
// Sublink 0 types, bits 0 thru 14
#define HTPHY_LINKTYPE_SL0_HT3          BIT0
#define HTPHY_LINKTYPE_SL0_HT1          BIT1
#define HTPHY_LINKTYPE_SL0_COHERENT     BIT2
#define HTPHY_LINKTYPE_SL0_NONCOHERENT  BIT3
#define HTPHY_LINKTYPE_SL0_LINK0        BIT4
#define HTPHY_LINKTYPE_SL0_LINK1        BIT5
#define HTPHY_LINKTYPE_SL0_LINK2        BIT6
#define HTPHY_LINKTYPE_SL0_LINK3        BIT7
// bit 15 is reserved

// SubLink 1 types, bits 16 thru 30
#define HTPHY_LINKTYPE_SL1_HT3          BIT16
#define HTPHY_LINKTYPE_SL1_HT1          BIT17
#define HTPHY_LINKTYPE_SL1_COHERENT     BIT18
#define HTPHY_LINKTYPE_SL1_NONCOHERENT  BIT19
#define HTPHY_LINKTYPE_SL1_LINK4        BIT20
#define HTPHY_LINKTYPE_SL1_LINK5        BIT21
#define HTPHY_LINKTYPE_SL1_LINK6        BIT22
#define HTPHY_LINKTYPE_SL1_LINK7        BIT23
// bit 31 is reserved

#define HTPHY_LINKTYPE_SL0_ALL          (HTPHY_LINKTYPE_SL0_HT3        | \
                                         HTPHY_LINKTYPE_SL0_HT1        | \
                                         HTPHY_LINKTYPE_SL0_COHERENT   | \
                                         HTPHY_LINKTYPE_SL0_NONCOHERENT | \
                                         HTPHY_LINKTYPE_SL0_LINK0       | \
                                         HTPHY_LINKTYPE_SL0_LINK1       | \
                                         HTPHY_LINKTYPE_SL0_LINK2       | \
                                         HTPHY_LINKTYPE_SL0_LINK3)
#define HTPHY_LINKTYPE_SL1_ALL          (HTPHY_LINKTYPE_SL1_HT3        | \
                                         HTPHY_LINKTYPE_SL1_HT1        | \
                                         HTPHY_LINKTYPE_SL1_COHERENT   | \
                                         HTPHY_LINKTYPE_SL1_NONCOHERENT | \
                                         HTPHY_LINKTYPE_SL1_LINK4       | \
                                         HTPHY_LINKTYPE_SL1_LINK5       | \
                                         HTPHY_LINKTYPE_SL1_LINK6       | \
                                         HTPHY_LINKTYPE_SL1_LINK7)
#define HTPHY_LINKTYPE_ALL              (HTPHY_LINKTYPE_SL0_ALL | HTPHY_LINKTYPE_SL1_ALL)

#define HTPHY_REGISTER_MAX              0x0000FFFFul
/**
 * HT PHY Link Features
 */
typedef struct {
  UINT32 HtPhySL0Ht3:1;                       ///< Ht Phy Sub-link 0 Ht3
  UINT32 HtPhySL0Ht1:1;                       ///< Ht Phy Sub-link 0 Ht1
  UINT32 HtPhySL0Coh:1;                       ///< Ht Phy Sub-link 0 Coherent
  UINT32 HtPhySL0NonCoh:1;                    ///< Ht Phy Sub-link 0 NonCoherent
  UINT32 HtPhySL0Link0:1;                     ///< Ht Phy Sub-link 0 specifically for node link 0.
  UINT32 HtPhySL0Link1:1;                     ///< Ht Phy Sub-link 0 specifically for node link 1.
  UINT32 HtPhySL0Link2:1;                     ///< Ht Phy Sub-link 0 specifically for node link 2.
  UINT32 HtPhySL0Link3:1;                     ///< Ht Phy Sub-link 0 specifically for node link 3.
  UINT32 :(15 - 7);                           ///< Ht Phy Sub-link 0 Pad
  UINT32 HtPhySL1Ht3:1;                       ///< Ht Phy Sub-link 1 Ht3
  UINT32 HtPhySL1Ht1:1;                       ///< Ht Phy Sub-link 1 Ht1
  UINT32 HtPhySL1Coh:1;                       ///< Ht Phy Sub-link 1 Coherent
  UINT32 HtPhySL1NonCoh:1;                    ///< Ht Phy Sub-link 1 NonCoherent
  UINT32 HtPhySL1Link4:1;                     ///< Ht Phy Sub-link 1 specifically for node link 4.
  UINT32 HtPhySL1Link5:1;                     ///< Ht Phy Sub-link 1 specifically for node link 5.
  UINT32 HtPhySL1Link6:1;                     ///< Ht Phy Sub-link 1 specifically for node link 6.
  UINT32 HtPhySL1Link7:1;                     ///< Ht Phy Sub-link 1 specifically for node link 7.
  UINT32 :(31 - 23);                          ///< Ht Phy Sub-link 1 Pad
} HT_PHY_LINK_FEATURES;

/**
 * Ht Phy Link Features
 */
typedef union {
  UINT32                  HtPhyLinkValue;     ///< Describe HY Phy Features in UINT32.
  ///< This one goes first, because then initializers use it automatically for the union.
  HT_PHY_LINK_FEATURES    HtPhyLinkFeatures;  ///< Describe HT Phy Features in structure.
} HT_PHY_LINK_FEATS;

// DB Level for initializing Deemphasis
// This must be in sync with DEEMPHASIS_FEATURES and PLATFORM_DEEMPHASIS_LEVEL (agesa.h)
#define DEEMPHASIS_LEVEL_NONE   BIT0
#define DEEMPHASIS_LEVEL__3     BIT1
#define DEEMPHASIS_LEVEL__6     BIT2
#define DEEMPHASIS_LEVEL__8     BIT3
#define DEEMPHASIS_LEVEL__11    BIT4
#define DEEMPHASIS_LEVEL__11_8  BIT5
#define DCV_LEVEL_NONE          BIT16
#define DCV_LEVEL__2            BIT17
#define DCV_LEVEL__3            BIT18
#define DCV_LEVEL__5            BIT19
#define DCV_LEVEL__6            BIT20
#define DCV_LEVEL__7            BIT21
#define DCV_LEVEL__8            BIT22
#define DCV_LEVEL__9            BIT23
#define DCV_LEVEL__11           BIT24
// Note that an "AND" feature doesn't make any sense, levels are mutually exclusive.

// An error check value.
#define VALID_DEEMPHASIS_LEVELS  (DEEMPHASIS_LEVEL_NONE    | \
                                  DEEMPHASIS_LEVEL__3      | \
                                  DEEMPHASIS_LEVEL__6      | \
                                  DEEMPHASIS_LEVEL__8      | \
                                  DEEMPHASIS_LEVEL__11     | \
                                  DEEMPHASIS_LEVEL__11_8   | \
                                  DCV_LEVEL_NONE           | \
                                  DCV_LEVEL__2             | \
                                  DCV_LEVEL__3             | \
                                  DCV_LEVEL__5             | \
                                  DCV_LEVEL__6             | \
                                  DCV_LEVEL__7             | \
                                  DCV_LEVEL__8             | \
                                  DCV_LEVEL__9             | \
                                  DCV_LEVEL__11)

/**
 * Deemphasis Ht Phy Link Deemphasis.
 *
 * This must be in sync with defines above and ::PLATFORM_DEEMPHASIS_LEVEL (agesa.h)
 */
typedef struct {
  UINT32          DeemphasisLevelNone:1;         ///< The deemphasis level None.
  UINT32          DeemphasisLevelMinus3:1;       ///< The deemphasis level minus 3 db.
  UINT32          DeemphasisLevelMinus6:1;       ///< The deemphasis level minus 6 db.
  UINT32          DeemphasisLevelMinus8:1;       ///< The deemphasis level minus 8 db.
  UINT32          DeemphasisLevelMinus11:1;      ///< The deemphasis level minus 11 db.
  UINT32          DeemphasisLevelMinus11w8:1;    ///< The deemphasis level minus 11 db, minus 8 precursor.
  UINT32          :(15 - 5);                     ///< reserved.
  UINT32          DcvLevelNone:1;                ///< The level for DCV None.
  UINT32          DcvLevelMinus2:1;              ///< The level for DCV minus 2 db.
  UINT32          DcvLevelMinus3:1;              ///< The level for DCV minus 3 db.
  UINT32          DcvLevelMinus5:1;              ///< The level for DCV minus 5 db.
  UINT32          DcvLevelMinus6:1;              ///< The level for DCV minus 6 db.
  UINT32          DcvLevelMinus7:1;              ///< The level for DCV minus 7 db.
  UINT32          DcvLevelMinus8:1;              ///< The level for DCV minus 8 db.
  UINT32          DcvLevelMinus9:1;              ///< The level for DCV minus 9 db.
  UINT32          DcvLevelMinus11:1;             ///< The level for DCV minus 11 db.
  UINT32          :(15 - 8);                     ///< reserved.
} DEEMPHASIS_FEATURES;

/**
 * Deemphasis Ht Phy Link Features.
 */
typedef union {
  UINT32                  DeemphasisValues;      ///< Initialize HT Deemphasis in UINT32.
  DEEMPHASIS_FEATURES     DeemphasisLevels;      ///< HT Deemphasis levels.
} DEEMPHASIS_FEATS;

// Initializer bit patterns for PERFORMANCE_PROFILE_FEATS.
#define PERFORMANCE_REFRESH_REQUEST_32B       BIT0
#define PERFORMANCE_PROBEFILTER               BIT1
#define PERFORMANCE_L3_CACHE                  BIT2
#define PERFORMANCE_NO_L3_CACHE               BIT3
#define PERFORMANCE_MCT_ISOC_VARIABLE         BIT4
#define PERFORMANCE_IS_WARM_RESET             BIT5
#define PERFORMANCE_VRM_HIGH_SPEED_ENABLE     BIT6
#define PERFORMANCE_NB_PSTATES_ENABLE         BIT7
#define PERFORMANCE_AND                       BIT31

#define PERFORMANCE_PROFILE_ALL   (PERFORMANCE_REFRESH_REQUEST_32B | \
                                   PERFORMANCE_PROBEFILTER         | \
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
  UINT32       ProbeFilter:1;                  ///< BIT_1    Probe Filter will be enabled.
  UINT32       L3Cache:1;                      ///< BIT_2    L3 Cache is present.
  UINT32       NoL3Cache:1;                    ///< BIT_3    L3 Cache is NOT present.
  UINT32       MctIsocVariable:1;              ///< BIT_4    Mct Isoc Read Priority set to variable.
  UINT32       IsWarmReset:1;                  ///< BIT_5    This boot is on a warm reset, cold reset pass is already completed.
  UINT32       VrmHighSpeed:1;                 ///< BIT_6    Select high speed VRM.
  UINT32       NbPstates:1;                    ///< BIT_7    Northbridge PStates are enabled
  UINT32       :(30 - 7);                      ///< available for future expansion.
  UINT32       AndPerformanceFeats:1;          ///< BIT_31.  AND other selected features.
} PERFORMANCE_PROFILE_FEATURES;

/**
 * Performance Profile features.
 */
typedef union {
  UINT32                        PerformanceProfileValue;     ///< Initializer value.
  PERFORMANCE_PROFILE_FEATURES  PerformanceProfileFeatures;  ///< The performance profile features.
} PERFORMANCE_PROFILE_FEATS;

/**
 * Package Type Features
 *
 */
typedef struct {
  UINT32        PkgType0:1;                 ///< Package Type 0
  UINT32        PkgType1:1;                 ///< Package Type 1
  UINT32        PkgType2:1;                 ///< Package Type 2
  UINT32        PkgType3:1;                 ///< Package Type 3
  UINT32        PkgType4:1;                 ///< Package Type 4
  UINT32        PkgType5:1;                 ///< Package Type 5
  UINT32        PkgType6:1;                 ///< Package Type 6
  UINT32        PkgType7:1;                 ///< Package Type 7
  UINT32        PkgType8:1;                 ///< Package Type 8
  UINT32        PkgType9:1;                 ///< Package Type 9
  UINT32        PkgType10:1;                ///< Package Type 10
  UINT32        PkgType11:1;                ///< Package Type 11
  UINT32        PkgType12:1;                ///< Package Type 12
  UINT32        PkgType13:1;                ///< Package Type 13
  UINT32        PkgType14:1;                ///< Package Type 14
  UINT32        PkgType15:1;                ///< Package Type 15
  UINT32        Reserved:15;                ///< Package Type Reserved
  UINT32        ReservedAndFeats:1;         ///< BIT_31.  AND other selected features. Always zero here.
} PACKAGE_TYPE_FEATURES;

// Initializer Values for Package Type
#define PACKAGE_TYPE_ALL              0xFFFF       ///< Package Type apply all packages

// Initializer Values for Ht Host Pci Config Registers
#define HT_HOST_FEAT_COHERENT          BIT0
#define HT_HOST_FEAT_NONCOHERENT       BIT1
#define HT_HOST_FEAT_GANGED            BIT2
#define HT_HOST_FEAT_UNGANGED          BIT3
#define HT_HOST_FEAT_HT3               BIT4
#define HT_HOST_FEAT_HT1               BIT5
#define HT_HOST_AND                    BIT31

#define HT_HOST_FEATURES_ALL    (HT_HOST_FEAT_COHERENT    | \
                                 HT_HOST_FEAT_NONCOHERENT | \
                                 HT_HOST_FEAT_GANGED      | \
                                 HT_HOST_FEAT_UNGANGED    | \
                                 HT_HOST_FEAT_HT3         | \
                                 HT_HOST_FEAT_HT1)

/**
 * HT Host PCI register features.
 *
 * Links which are not connected do not match any of these features.
 */
typedef struct {
  UINT32       Coherent:1;                 ///< BIT_0 Apply to links with a coherent connection.
  UINT32       NonCoherent:1;              ///< BIT_1 Apply to links with a non-coherent connection.
  UINT32       Ganged:1;                   ///< BIT_2 Apply to links with a ganged connection.
  UINT32       UnGanged:1;                 ///< BIT_3 Apply to links with a unganged connection.
  UINT32       Ht3:1;                      ///< BIT_4 Apply to links with HT3 frequency (> 1000 MHz)
  UINT32       Ht1:1;                      ///< BIT_5 Apply to links with HT1 frequency (< 1200 MHz)
  UINT32       :(30 - 5);                  ///< Future expansion.
  UINT32       AndHtHostFeats:1;           ///< BIT_31.  AND other selected features.
} HT_HOST_FEATURES;

/**
 * HT Host features for table data.
 */
typedef union {
  UINT32                 HtHostValue;                ///< Initializer value.
  HT_HOST_FEATURES       HtHostFeatures;             ///< The HT Host Features.
} HT_HOST_FEATS;

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
 * Processor count ranges for table data.
 *
 * Provide a pair of processor count ranges.  If the actual counts are included in either range (OR),
 * the feature should be considered a match.
 */
typedef union {
  UINT32       ProcessorCountRangeValue;             ///< Initializer value.
  COUNT_RANGE_FEATURE ProcessorCountRanges;          ///< The Processor and Node Counts.
} PROCESSOR_COUNTS;

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

/**
 * Connectivity count ranges for table data.
 *
 * Provide a processor count range and a system degree range.  The degree of a system is
 * the maximum degree of any node.  The degree of a node is the number of nodes to which
 * it is directly connected (not considering width or redundant links). If both the actual
 * counts are included in each range (AND), the feature should be considered a match.
 */
typedef union {
  UINT32       ConnectivityCountRangeValue;             ///< Initializer value.
  COUNT_RANGE_FEATURE ConnectivityCountRanges;          ///< The Processor and Degree Counts.
} CONNECTIVITY_COUNT;

/**
 * HT Frequency Count Range.
 *
 * Provide a pair of Frequency count ranges, with the frequency encoded as an HT Frequency value
 * (such as would be programmed into the HT Host Link Frequency register).  By converting a NB freq,
 * the same count can be applied for it.  If the actual value is included in either range
 */
typedef union {
  UINT32       HtFreqCountRangeValue;                  ///< Initializer value.
  COUNT_RANGE_FEATURE HtFreqCountRanges;               ///< The HT Freq counts.
} HT_FREQ_COUNTS;

/*------------------------------------------------------------------------------------------*/
/*
 * The specific data for each table entry.
 */
/*------------------------------------------------------------------------------------------*/

/**
 *  Make an extra type so we can use compilers that don't support designated initializers.
 *
 *  All the entry type unions are no more than 5 UINT32's in size.  For entry types which are a struct of UINT32's,
 *  this type can be used so that initializers can be declared TABLE_ENTRY_FIELDS, instead of a special non-union type.
 *  A non-union type then has to be cast back to TABLE_ENTRY_FIELDS in order to process the table, and you can't mix
 *  entry types with non-union initializers in the same table with any other type.
 *
 *  If the entry type contains anything but UINT32's, then it must have a non-union initializer type for creating the
 *  actual tables.  For example, MSR entry has UINT64 and workaround entry has a function pointer.
 */
typedef UINT32 GENERIC_TYPE_ENTRY_INITIALIZER[MAX_ENTRY_TYPE_ITEMS32];

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

/**
 * Table Entry Data for HT Phy Registers.
 *
 * Apply data to register after mask, for HT Phy registers, repeated for all active links.
 */
typedef struct {
  HT_PHY_LINK_FEATS TypeFeats;        ///< HT Phy Link Features
  UINT32            Address;          ///< Address of Ht Phy Register
  UINT32            Data;             ///< Data to be written into PCI device
  UINT32            Mask;             ///< Mask to be used before data write.  Set every bit of all updated fields.
} HT_PHY_TYPE_ENTRY_DATA;

/**
 * Table Entry Data for HT Phy Register Ranges.
 *
 * Apply data to register after mask, for a range of HT Phy registers, repeated for all active links.
 */
typedef struct {
  HT_PHY_LINK_FEATS TypeFeats;        ///< HT Phy Link Features
  UINT32            LowAddress;       ///< Low address of Ht Phy Register range.
  UINT32            HighAddress;      ///< High address of register range.
  UINT32            Data;             ///< Data to be written into PCI device.
  UINT32            Mask;             ///< Mask to be used before data write.  Set every bit of all updated fields.
} HT_PHY_RANGE_TYPE_ENTRY_DATA;

/**
 * Table Entry Data for HT Phy Deemphasis Registers.
 *
 * Apply data to register after mask, for HT Phy registers, repeated for all active links.
 */
typedef struct {
  DEEMPHASIS_FEATS        Levels;       ///< The DCV and Deemphasis levels to match
  HT_PHY_TYPE_ENTRY_DATA  HtPhyEntry;   ///< The HT Phy Entry to set the deemphasis values
} DEEMPHASIS_HT_PHY_TYPE_ENTRY_DATA;

/**
 * Table Entry Date for HT Phy Frequency Count Register updates.
 *
 * Compare the NB freq to a range, the HT freq to a range, the link features.
 * Apply data to register after mask, if all three matched.
 */
typedef struct {
  HT_FREQ_COUNTS       HtFreqCounts;      ///< Specify the HT Frequency range.
  HT_FREQ_COUNTS       NbFreqCounts;      ///< Specify the NB Frequency range.
  HT_PHY_TYPE_ENTRY_DATA HtPhyEntry;      ///< The HT Phy register update to perform.
} HT_PHY_FREQ_TYPE_ENTRY_DATA;

/**
 * Table Entry Data for Profile Fixup Registers.
 *
 * If TypeFeats matches current config, apply data to register after mask for PCI Config registers.
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats; ///< Profile Fixup Features.
  PCI_TYPE_ENTRY_DATA PciEntry;        ///< The PCI Register entry data.
} PROFILE_FIXUP_TYPE_ENTRY_DATA;

/**
 * A variation of PCI register for the HT Host registers.
 *
 * A setting to the HT Host buffer counts needs to be made to all the registers for
 * all the links.  There are also link specific criteria to check.
 */
typedef struct {
  HT_HOST_FEATS     TypeFeats;        ///< Link Features.
  PCI_ADDR          Address;          ///< Address of PCI Register to Fixed Up.
  UINT32            Data;             ///< Data to be written into PCI device
  UINT32            Mask;             ///< Mask to be used before data write.  Set every bit of all updated fields.
} HT_HOST_PCI_TYPE_ENTRY_DATA;

/**
 * A variation of PCI register for the HT Host performance registers.
 *
 * A setting to the HT Host buffer counts needs to be made to all the registers for
 * all the links.  There are also link specific criteria to check.
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS   PerformanceFeats;    ///< Performance Profile features.
  HT_HOST_PCI_TYPE_ENTRY_DATA HtHostEntry;         ///< Link Features.
} HT_HOST_PERFORMANCE_PCI_TYPE_ENTRY_DATA;

/**
 * A variation of HT Host PCI register for the Link Token registers.
 *
 * Use Link Features, Performance Fixup features, and processor counts to match entries.
 * Link Features are iterated through the connected links.  All the matching Link Token count
 * registers are updated.
 */
typedef struct {
  CONNECTIVITY_COUNT          ConnectivityCount;   ///< Specify Processor count and Degree count range.
  PERFORMANCE_PROFILE_FEATS   PerformanceFeats;    ///< Performance Profile features.
  HT_HOST_FEATS               LinkFeats;           ///< Link Features.
  PCI_ADDR                    Address;             ///< Address of PCI Register to Fixed Up.
  UINT32                      Data;                ///< Data to be written into PCI device
  UINT32                      Mask;                ///< Mask to be used before data write. Set every bit of all updated fields.
} HT_TOKEN_PCI_REGISTER;

/**
 * Core Count dependent PCI registers.
 *
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats; ///< Profile Fixup Features.
  CORE_COUNT_RANGES CoreCounts;        ///< Specify up to two core count ranges to match.
  PCI_TYPE_ENTRY_DATA PciEntry;        ///< The PCI Register entry data.
} CORE_COUNTS_PCI_TYPE_ENTRY_DATA;

/**
 * Processor Count dependent PCI registers.
 *
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats; ///< Profile Fixup Features.
  PROCESSOR_COUNTS  ProcessorCounts;   ///< Specify a processor count range.
  PCI_TYPE_ENTRY_DATA PciEntry;        ///< The PCI Register entry data.
} PROCESSOR_COUNTS_PCI_TYPE_ENTRY_DATA;

/**
 * Compute Unit Count dependent PCI registers.
 *
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats;         ///< Profile Fixup Features.
  COMPUTE_UNIT_COUNTS       ComputeUnitCounts; ///< Specify a compute unit count range.
  PCI_TYPE_ENTRY_DATA       PciEntry;          ///< The PCI Register entry data.
} COMPUTE_UNIT_COUNTS_PCI_TYPE_ENTRY_DATA;

/**
 * System connectivity dependent PCI registers.
 *
 * The topology specific recommended settings are based on the different connectivity of nodes
 * in each configuration:  the more connections, the fewer resources each connection gets.
 * The connectivity criteria translate as:
 * - 2 Socket, half populated  ==   Degree 1
 * - 4 Socket, half populated   ==  Degree 2
 * - 2 Socket, fully populated  ==  Degree 3
 * - 4 Socket, fully populated  ==  Degree > 3.  (4 or 5 if 3P, 6 if 4P)
 *
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS TypeFeats;     ///< Profile Fixup Features.
  CONNECTIVITY_COUNT  ConnectivityCount;   ///< Specify a system degree range.
  PCI_TYPE_ENTRY_DATA PciEntry;            ///< The PCI Register entry data.
} CONNECTIVITY_COUNTS_PCI_TYPE_ENTRY_DATA;

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
  PF_FAM_SPECIFIC_WORKAROUND  DoAction;     ///< A function implementing the workaround.
  UINT32                Data;               ///< This data is passed to DoAction().
} FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_DATA;

/**
 * Package Type Features
 *
 * FamilyPackageType are various among CPU families.
 *
 */
typedef union {
  UINT32                      PackageTypeValue;    ///< Package Type
  PACKAGE_TYPE_FEATURES       FamilyPackageType;   ///< Package Type of CPU family
} PACKAGE_TYPE_FEATS;

/**
 * HT Features dependent Global PCI registers.
 *
 */
typedef struct {
  HT_HOST_FEATS               LinkFeats;           ///< Link Features.
  PACKAGE_TYPE_FEATS          PackageType;         ///< Package Type
  PCI_TYPE_ENTRY_DATA         PciEntry;            ///< The PCI Register entry data.
} HT_FEATURES_PCI_TYPE_ENTRY_DATA;

/**
 * Table Entry Data for HT Phy Registers which depend on performance profile features.
 *
 * Match performance profile features and link features.
 * Apply data to register after mask, for HT Phy registers, repeated for all active links.
 */
typedef struct {
  PERFORMANCE_PROFILE_FEATS  TypeFeats;            ///< Profile Fixup Features.
  HT_PHY_TYPE_ENTRY_DATA     HtPhyEntry;           ///< The HT Phy Entry to set the deemphasis values
} PROFILE_HT_PHY_TYPE_ENTRY_DATA;

/**
 * HT Link PCI registers that are not in the HT Host capability.
 *
 * Some HT Link registers have an instance per link, but are just sequential.  Specify the base register
 * in the table register address (link 0 sublink 0).
 */
typedef struct {
  HT_HOST_FEATS               LinkFeats;           ///< Link Features.
  PCI_TYPE_ENTRY_DATA         PciEntry;            ///< The PCI Register entry data.
} HT_LINK_PCI_TYPE_ENTRY_DATA;

/*------------------------------------------------------------------------------------------*/
/*
 * A complete register table and table entries.
 */
/*------------------------------------------------------------------------------------------*/

/**
 * All the available entry data types.
 */
typedef union {
  GENERIC_TYPE_ENTRY_INITIALIZER          InitialValues;      ///< Not a valid entry type; as the first union item,
                                                              ///< it can be used with initializers.
  MSR_TYPE_ENTRY_DATA                     MsrEntry;           ///< Msr entry.
  PCI_TYPE_ENTRY_DATA                     PciEntry;           ///< PCI entry.
  FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_DATA FamSpecificEntry;   ///< Family Specific Workaround entry.
  HT_PHY_TYPE_ENTRY_DATA                  HtPhyEntry;         ///< HT Phy entry.
  HT_PHY_RANGE_TYPE_ENTRY_DATA            HtPhyRangeEntry;    ///< A range of Ht Phy Registers
  DEEMPHASIS_HT_PHY_TYPE_ENTRY_DATA       DeemphasisEntry;    ///< A HT Deemphasis level's settings.
  HT_PHY_FREQ_TYPE_ENTRY_DATA             HtPhyFreqEntry;     ///< A frequency dependent Ht Phy Register setting.
  PROFILE_FIXUP_TYPE_ENTRY_DATA           FixupEntry;         ///< Profile Fixup entry.
  HT_HOST_PCI_TYPE_ENTRY_DATA             HtHostEntry;        ///< HT Host PCI entry.
  HT_HOST_PERFORMANCE_PCI_TYPE_ENTRY_DATA HtHostPerfEntry;    ///< HT Host Performance PCI entry
  HT_TOKEN_PCI_REGISTER                   HtTokenEntry;       ///< HT Link Token Count entry.
  CORE_COUNTS_PCI_TYPE_ENTRY_DATA         CoreCountEntry;     ///< Core count dependent settings.
  PROCESSOR_COUNTS_PCI_TYPE_ENTRY_DATA    ProcCountEntry;     ///< Processor count entry.
  COMPUTE_UNIT_COUNTS_PCI_TYPE_ENTRY_DATA CompUnitCountEntry; ///< Compute unit count dependent entry.
  CONNECTIVITY_COUNTS_PCI_TYPE_ENTRY_DATA TokenPciEntry;      ///< System connectivity dependent Token register.
  HT_FEATURES_PCI_TYPE_ENTRY_DATA         HtFeatPciEntry;     ///< HT Features PCI entry.
  PROFILE_HT_PHY_TYPE_ENTRY_DATA          HtPhyProfileEntry;  ///< Performance dependent HT Phy register.
  HT_LINK_PCI_TYPE_ENTRY_DATA             HtLinkPciEntry;     ///< Per Link, non HT Host, PCI registers.
} TABLE_ENTRY_DATA;

/**
 * Register Table Entry common fields.
 *
 * All the various types of register table entries are subclasses of this object.
 */
typedef struct {
  TABLE_ENTRY_TYPE       EntryType;                  ///< The type of table entry this is.
  CPU_LOGICAL_ID         CpuRevision;                ///< Common CPU Logical ID match criteria.
  PLATFORM_FEATS         Features;                   ///< Common Platform Features match criteria.
  TABLE_ENTRY_DATA       Entry;                      ///< The type dependent entry data (ex. register, data, mask).
} TABLE_ENTRY_FIELDS;

/**
 * An entire register table.
 */
typedef struct {
  TABLE_CORE_SELECTOR  Selector;                     ///< For efficiency, these cores should process this table
  UINTN                NumberOfEntries;              ///< The number of entries in the table.
  CONST TABLE_ENTRY_FIELDS  *Table;                  ///< The table entries.
} REGISTER_TABLE;

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
  IN       TABLE_ENTRY_DATA       *CurrentEntry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/// Reference to a method
typedef F_DO_TABLE_ENTRY *PF_DO_TABLE_ENTRY;

/**
 * Describe the attributes of a Table Entry Type.
 */
typedef struct {
  TABLE_ENTRY_TYPE       EntryType;                  ///< The type of table entry this describes.
  PF_DO_TABLE_ENTRY      DoTableEntry;               ///< Provide all semantics associated with TABLE_ENTRY_DATA
} TABLE_ENTRY_TYPE_DESCRIPTOR;

/*------------------------------------------------------------------------------------------*/
/*
 * Non-union initializers for entry data which is not just UINT32.
 */
/*------------------------------------------------------------------------------------------*/

/**
 * A union of data types, that can be initialized with MSR data.
 *
 * This ensures the entry data is the same size as TABLE_ENTRY_DATA.
 */
typedef union {
  MSR_TYPE_ENTRY_DATA    MsrInitializer;            ///< The data in the table initializer is assigned to this member.
  TABLE_ENTRY_DATA       Reserved;                  ///< Make sure the size is the same as the real union.
} MSR_DATA_INITIALIZER;

/**
 * A type suitable for an initializer for MSR Table entries.
 */
typedef struct {
  TABLE_ENTRY_TYPE       Type;                       ///< The type of table entry this is.
  CPU_LOGICAL_ID         CpuRevision;                ///< Common CPU Logical ID match criteria.
  PLATFORM_FEATS         Features;                   ///< Common Platform Features match criteria.
  MSR_DATA_INITIALIZER    EntryData;                 ///< The special union which accepts msr data initializer.
} MSR_TYPE_ENTRY_INITIALIZER;

/**
 * A union of data types, that can be initialized with Family Specific Workaround data.
 *
 * This ensures the entry data is the same size as TABLE_ENTRY_DATA.
 */
typedef union {
  FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_DATA    FamSpecificInitializer; ///< The data in the table initializer is assigned to this member.
  TABLE_ENTRY_DATA       Reserved;                                   ///< Make sure the size is the same as the real union.
} FAM_SPECIFIC_WORKAROUND_DATA_INITIALIZER;

/**
 * A type suitable for an initializer for Family Specific Workaround Table entries.
 */
typedef struct {
  TABLE_ENTRY_TYPE       Type;                             ///< The type of table entry this is.
  CPU_LOGICAL_ID         CpuRevision;                      ///< Common CPU Logical ID match criteria.
  PLATFORM_FEATS         Features;                         ///< Common Platform Features match criteria.
  FAM_SPECIFIC_WORKAROUND_DATA_INITIALIZER    EntryData;   ///< Special union accepts family specific workaround data initializer.
} FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER;

/*------------------------------------------------------------------------------------------*/
/*
 * Table related function prototypes (many are instance of F_DO_TABLE_ENTRY method).
 */
/*------------------------------------------------------------------------------------------*/

/**
 * Set the registers for this core based on entries in a list of Register Tables.
 */
VOID SetRegistersFromTables (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
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
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/**
 * Perform the PCI Register Entry.
 */
VOID
SetRegisterForPciEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/**
 * Perform the Performance Profile PCI Register Entry.
 */
VOID
SetRegisterForPerformanceProfileEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/**
 * Perform the HT Host PCI Register Entry.
 */
VOID
SetRegisterForHtHostEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/**
 * Perform the HT Host Performance PCI Register Entry.
 */
VOID
SetRegisterForHtHostPerfEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Set the HT Link Token Count registers.
 */
VOID
SetRegisterForHtLinkTokenEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Core Counts Performance PCI Register Entry.
 */
VOID
SetRegisterForCoreCountsPerformanceEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/**
 * Perform the Processor Counts PCI Register Entry.
 */
VOID
SetRegisterForProcessorCountsEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/**
 * Perform the Compute Unit Counts PCI Register Entry.
 */
VOID
SetRegisterForComputeUnitCountsEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Family Specific Workaround Register Entry.
 */
VOID
SetRegisterForFamSpecificWorkaroundEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/**
 * Program HT Phy PCI registers.
 */
VOID
SetRegisterForHtPhyEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Program a range of HT Phy PCI registers.
 */
VOID
SetRegisterForHtPhyRangeEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Program Deemphasis registers, for the platform specified levels.
 */
VOID
SetRegisterForDeemphasisEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Program HT Phy PCI registers which have complex frequency dependencies.
 */
VOID
SetRegisterForHtPhyFreqEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the Processor Token Counts PCI Register Entry.
 */
VOID
SetRegisterForTokenPciEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the HT Link Feature PCI Register Entry.
 */
VOID
SetRegisterForHtFeaturePciEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the HT Phy Performance Profile Register Entry.
 */
VOID
SetRegisterForHtPhyProfileEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/**
 * Perform the HT Link PCI Register Entry.
 */
VOID
SetRegisterForHtLinkPciEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
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

