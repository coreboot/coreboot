/* $NoKeywords:$ */
/**
 * @file
 *
 * Create outline and references for Build Configuration and Options Component mainpage documentation.
 *
 * Design guides, maintenance guides, and general documentation, are
 * collected using this file onto the documentation mainpage.
 * This file contains doxygen comment blocks, only.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Documentation
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
 ******************************************************************************
 */

/**
 * @page optionmain Build Configuration and Options Documentation
 *
 * Additional documentation for the Build Configuration and Options component consists of
 *
 * - Introduction and Overview to Build Options
 *   - @subpage platforminstall "Platform Build Options"
 *   - @subpage bldcfg "Build Configuration Item Cross Reference"
 *   - @subpage examplecustomizations "Customization Examples"
 * - Maintenance Guides:
 *   -  For debug of the Options system, use compiler options
 *      @n <tt>      /P /EP /C /FAs      </tt> @n
 *      PreProcessor output is produced in an .i file in the directory where the project
 *      file is located.
 * - Design Guides:
 *   - add here >>>
 *
 */

/**
 * @page platforminstall Platform Build Options.
 *
 * Build options are boolean constants. The purpose of build options is to remove code
 * from the build to reduce the overall code size present in the ROM image. Unless
 * otherwise specified, the default action is to include all options. If a build option is
 * not specifically listed as disabled, then it is included into the build.
 *
 * The documented build options are imported from a user controlled file for
 * processing. The build options for all platform solutions are listed below:
 *
 * @anchor BLDOPT_REMOVE_UDIMMS_SUPPORT
 * @li @e BLDOPT_REMOVE_UDIMMS_SUPPORT @n
 *        If unbuffered DIMMs are NOT expected to be required in the system, the code that
 *        handles unbuffered DIMMs can be removed from the build.
 *
 * @anchor BLDOPT_REMOVE_RDIMMS_SUPPORT
 * @li @e  BLDOPT_REMOVE_RDIMMS_SUPPORT @n
 *         If registered DIMMs are NOT expected to be required in the system, the code
 *         that handles registered DIMMs can be removed from the build.
 *
  * @anchor BLDOPT_REMOVE_LRDIMMS_SUPPORT
 * @li @e  BLDOPT_REMOVE_LRDIMMS_SUPPORT @n
 *         If Load Reduced DIMMs are NOT expected to be required in the system, the code
 *         that handles Load Reduced DIMMs can be removed from the build.
 *
 *     @note The above three options operate independently from each other; however, at
 *     least one of the unbuffered , registered or load reduced DIMM options must be present in the build.
 *
 * @anchor BLDOPT_REMOVE_ECC_SUPPORT
 * @li @e  BLDOPT_REMOVE_ECC_SUPPORT @n
 *         Use this option to remove the code for Error Checking & Correction.
 *
 * @anchor BLDOPT_REMOVE_BANK_INTERLEAVE
 * @li @e  BLDOPT_REMOVE_BANK_INTERLEAVE @n
 *         Interleaving is a mechanism to do performance fine tuning. This option
 *         interleaves memory between banks on a DIMM.
 *
 * @anchor BLDOPT_REMOVE_DCT_INTERLEAVE
 * @li @e  BLDOPT_REMOVE_DCT_INTERLEAVE @n
 *         Interleaving is a mechanism to do performance fine tuning. This option
 *         interleaves memory from two DRAM controllers.
 *
 * @anchor BLDOPT_REMOVE_NODE_INTERLEAVE
 * @li @e  BLDOPT_REMOVE_NODE_INTERLEAVE @n
 *         Interleaving is a mechanism to do performance fine tuning. This option
 *         interleaves memory from two HyperTransport nodes.
 *
 * @anchor BLDOPT_REMOVE_PARALLEL_TRAINING
 * @li @e  BLDOPT_REMOVE_PARALLEL_TRAINING @n
 *         For multi-socket systems, training memory in parallel can reduce the time
 *         needed to boot.
 *
 * @anchor BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT
 * @li @e  BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT @n
 *         Online Spare support is removed by this option.
 *
 * @anchor BLDOPT_REMOVE_MULTISOCKET_SUPPORT
 * @li @e BLDOPT_REMOVE_MULTISOCKET_SUPPORT @n
 *        Many systems use only a single socket and may benefit in code space to remove
 *        this code. However, certain processors have multiple HyperTransport nodes
 *        within a single socket. For these processors, the multi-node support is
 *        required and this option has no effect.
 *
 * @anchor BLDOPT_REMOVE_ACPI_PSTATES
 * @li @e  BLDOPT_REMOVE_ACPI_PSTATES @n
 *         This option removes the code that generates the ACPI tables used in power
 *         management.
 *
 * @anchor BLDCFG_PSTATE_HPC_MODE
 * @li @e  BLDCFG_PSTATE_HPC_MODE @n
 *         This option enables PStates high performance computing mode (HPC mode)
 *
 *
 * @anchor BLDOPT_REMOVE_SRAT
 * @li @e  BLDOPT_REMOVE_SRAT @n
 *         This option removes the code that generates the SRAT tables used in performance
 *         tuning.
 *
 * @anchor BLDOPT_REMOVE_SLIT
 * @li @e  BLDOPT_REMOVE_SLIT @n
 *         This option removes the code that generates the SLIT tables used in performance
 *         tuning.
 *
 * @anchor BLDOPT_REMOVE_WHEA
 * @li @e  BLDOPT_REMOVE_WHEA @n
 *         This option removes the code that generates the WHEA tables used in error
 *         handling and reporting.
 *
 * @anchor BLDOPT_REMOVE_DMI
 * @li @e  BLDOPT_REMOVE_DMI @n
 *         This option removes the code that generates the DMI tables used in system
 *         management.
 *
 * @anchor BLDOPT_REMOVE_DQS_TRAINING
 * @li @e  BLDOPT_REMOVE_DQS_TRAINING @n
 *         This option removes the code used in memory performance tuning.
 *
 *
 * @anchor BLDOPT_REMOVE_HT_ASSIST
 * @li @e  BLDOPT_REMOVE_HT_ASSIST @n
 *         This option removes the code which implements the HT Assist feature.
 *
 * @anchor BLDOPT_REMOVE_ATM_MODE
 * @li @e  BLDOPT_REMOVE_ATM_MODE @n
 *         This option removes the code which implements the ATM feature.
 *
 * @anchor BLDOPT_REMOVE_MSG_BASED_C1E
 * @li @e  BLDOPT_REMOVE_MSG_BASED_C1E @n
 *         This option removes the code which implements the Message Based C1e feature.
 *
 * @anchor BLDOPT_REMOVE_C6_STATE
 * @li @e  BLDOPT_REMOVE_C6_STATE @n
 *         This option removes the code which implements the C6 C-state feature.
 *
 * @anchor BLDOPT_REMOVE_MEM_RESTORE_SUPPORT
 * @li @e  BLDOPT_REMOVE_MEM_RESTORE_SUPPORT @n
 *         This option removes the memory context restore feature.
 *
 * @anchor BLDOPT_REMOVE_FAMILY_10_SUPPORT
 * @li @e  BLDOPT_REMOVE_FAMILY_10_SUPPORT @n
 *         If the package contains support for family 10h processors, remove that support.
 *
 * @anchor BLDOPT_REMOVE_FAMILY_12_SUPPORT
 * @li @e  BLDOPT_REMOVE_FAMILY_12_SUPPORT @n
 *         If the package contains support for family 10h processors, remove that support.
 *
 * @anchor BLDOPT_REMOVE_FAMILY_14_SUPPORT
 * @li @e  BLDOPT_REMOVE_FAMILY_14_SUPPORT @n
 *         If the package contains support for family 14h processors, remove that support.
 *
 * @anchor BLDOPT_REMOVE_FAMILY_15_SUPPORT
 * @li @e  BLDOPT_REMOVE_FAMILY_15_SUPPORT @n
 *         If the package contains support for family 15h processors, remove that support.
 *
 * @anchor BLDOPT_REMOVE_AM3_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_AM3_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for AM3 sockets.
 *
 * @anchor BLDOPT_REMOVE_ASB2_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_ASB2_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for ASB2 sockets.
 *
 * @anchor BLDOPT_REMOVE_C32_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_C32_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for C32 sockets.
 *
 * @anchor BLDOPT_REMOVE_FM1_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_FM1_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for FM1 sockets.
 *
 * @anchor BLDOPT_REMOVE_FP1_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_FP1_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for FP1 sockets.
 *
 * @anchor BLDOPT_REMOVE_FS1_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_FS1_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for FS1 sockets.
 *
 * @anchor BLDOPT_REMOVE_FT1_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_FT1_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for FT1 sockets.
 *
 * @anchor BLDOPT_REMOVE_G34_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_G34_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for G34 sockets.
 *
 * @anchor BLDOPT_REMOVE_S1G3_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_S1G3_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for S1G3 sockets.
 *
 * @anchor BLDOPT_REMOVE_S1G4_SOCKET_SUPPORT
 * @li @e  BLDOPT_REMOVE_S1G4_SOCKET_SUPPORT @n
 *         This option removes the code which implements support for processors packaged for S1G4 sockets.
 */

/**
 * @page examplecustomizations Customization Examples
 *
 * The Addendum \<plat\>Options.c file for each platform contains the minimum required
 * customizations for that platform.  That is, it contains settings which would be needed
 * to boot a SimNow! bsd for that platform.
 * However, each individual product based on that platform will have customizations necessary for
 * that hardware.  Since the actual customizations needed vary so much, they are not included in
 * the \<plat\>Options.c.  This section provides examples of useful customizations that you can use or
 * modify to suit your needs.
 *
 * @par
 *
 * Source for the examples shown can be found at Addendum\\Examples. @n
 *
 * - @ref DeemphasisExamples "Deemphasis List Examples"
 * - @ref FrequencyLimitExamples "Frequency Limit Examples"
 * - @ref PerfPerWattHt "A performance-per-watt optimization Example"
 *
 * @anchor DeemphasisExamples
 * @par Deemphasis List Examples
 *
 * These examples customize PLATFORM_CONFIGURATION.PlatformDeemphasisList.
 * Source for the deemphasis list examples can be found in DeemphasisExamples.c. @n
 * @dontinclude DeemphasisExamples.c
 * <ul>
 * <li>
 *  The following deemphasis list provides an example for a 2P MCM Max Performance configuration.
 *  High Speed HT frequencies are supported.  There is only one non-coherent chain.  Note the technique of
 *  putting specified link matches before all uses of match any.  It often works well to specify the non-coherent links
 *  and use match any for the coherent links.
 *  @skip DinarDeemphasisList
 *  @until {
 *  The non-coherent chain can run up to 2600 MHz.  The chain is located on Socket 0, package Link 2.
 *  @until {
 *  @line }
 *  @line {
 *  @line }
 *  The coherent links can run up to 3200 MHz.
 *  @until HT_FREQUENCY_MAX
 *  @line }
 *  end of list:
 *  @until }
 *  Make this list the build time customized deemphasis list.
 *  @line define
 *
 * </li><li>
 *
 * The following deemphasis list provides an example for a 4P MCM Max Performance configuration.
 * This system has a backplane with connectors for CPU cards and an IO board. So trace lengths are long.
 * There can be one to four IO Chains, depending on the IO board.
 * @skipline DoubloonDeemphasisList
 * @until DoubloonDeemphasisList
 *
 * </li><li>
 *
 * The following deemphasis list further illustrates complex coherent system deemphasis.  This is the same
 * Dinar system as in an earlier example, but this time all the coherent links are explicitly customized (as
 * might be needed if each link has unique characterization).  For this example, we skip the non-coherent chains.
 * (A real system would have to include them, see example above.)
 *  @skip DinarPerLinkDeemphasisList
 *  @until {
 *  Provide deemphasis settings for the 16 bit, ganged, links, Socket 0 links 0, 1 and Socket 1 links 1 and 2.
 *  Provide entries to customize all HT3 frequencies at which the links may run.  This example covers all HT3 speeds.
 *  @until {
 *  @until DcvLevelMinus6
 *  @until DcvLevelMinus6
 *  @until DcvLevelMinus6
 *  @until DcvLevelMinus6
 *  Link 3 on both sockets connects different internal die:  sublink 0 connects the internal node zeroes, and
 *  sublink 1 connects the internal node ones.  So the link is unganged and both sublinks must be specifically
 *  customized.
 *  @until {
 *  @until DcvLevelMinus6
 *  @until DcvLevelMinus6
 *  @until DcvLevelMinus6
 *  @until DcvLevelMinus6
 *  end of list:
 *  @until define
 *
 * </ul>
 *
 * @anchor FrequencyLimitExamples
 * @par Frequency Limit Examples
 *
 * These examples customize AMD_HT_INTERFACE.CpuToCpuPcbLimitsList and AMD_HT_INTERFACE.IoPcbLimitsList.
 * Source for the frequency limit examples can be found in FrequencyLimitExamples.c. @n
 * @dontinclude FrequencyLimitExamples.c
 * <ul>
 * <li>
 *    The following list provides an example for limiting all coherent links to non-extended frequencies,
 * that is, to 2600 MHz or less.
 * @skipline NonExtendedCpuToCpuLimitList
 * @until {
 *   Provide the limit customization.  Match links from any socket, any package link, to any socket, any package link. Width is not limited.
 * @until HT_FREQUENCY_LIMIT_2600M
 * End of list:
 * @until ;
 * Customize the build to use this cpu to cpu frequency limit.
 * @until NonExtendedCpuToCpuLimitList
 * @n </li>
 * <li>
 *    The following list provides an example for limiting all coherent links to HT 1 frequencies,
 * that is, to 1000 MHz or less.  This is sometimes useful for test and debug.
 * @skipline Ht1CpuToCpuLimitList
 * @until Ht1CpuToCpuLimitList
 * @n </li>
 * <li>
 *    The following list provides an example for limiting all non-coherent links to 2400 MHz or less.
 *    The chain is matched by host processor Socket and package Link.  The depth can be used to select a particular device
 *    to device link on the chain. In this example, the chain consists of a single cave device and depth can be set to match any.
 * @skipline No2600MhzIoLimitList
 * @until No2600MhzIoLimitList
 * @n </li>
 * <li>
 *    The following list provides an example for limiting all non-coherent links to the minimum HT 3 frequency,
 * that is, to 1200 MHz or less.  This can be useful for test and debug.
 * @skipline MinHt3IoLimitList
 * @until MinHt3IoLimitList
 * @n </li>
 *
 * </ul>
 *
 * @anchor PerfPerWattHt
 * @par Performance-per-Watt Optimization Example
 *
 * This example customizes AMD_HT_INTERFACE.SkipRegangList.
 * Source for the Performance-per-watt Optimization example can be found in PerfPerWatt.c. @n
 * @dontinclude PerfPerWatt.c
 * To implement a performance-per-watt optimization for MCM processors, use the skip regang structure shown. @n
 * @skipline PerfPerWatt
 * @until PerfPerWatt
 *
 */
