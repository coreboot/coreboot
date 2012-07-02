/* $NoKeywords:$ */
/**
 * @file
 *
 * Create outline and references for GNB Component mainpage documentation.
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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */


/**
 * @page gnbmain GNB Component Documentation
 *
 * Additional documentation for the GNB component consists of
 *
 * - Maintenance Guides:
 *   - @subpage F12PcieLaneDescription    "Family 0x12 PCIe/DDI Lane description table"
 *   - @subpage F14ONPcieLaneDescription  "Family 0x14(ON) PCIe/DDI Lane description table"
 *   - @subpage F12LaneConfigurations     "Family 0x12 PCIe port/DDI link configurations"
 *   - @subpage F14ONLaneConfigurations   "Family 0x14(ON) PCIe port/DDI link configurations"
 *   - @subpage F12DualLinkDviDescription "Family 0x12 Dual Link DVI connector description"
 *   - add here >>>
 * - Design Guides:
 *   - @subpage BuildConfigDescription    "Build Configurations"
 *   - add here >>>
 *
 */


/**
 *  @page F12PcieLaneDescription Family 0x12 PCIe/DDI Lanes
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160> Lane ID</TD><TD class="indexkey">Lane group</TD><TD class="indexkey">Pin</TD></TR>
 *     <TR><TD class="indexvalue" > 0 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][0]</TD></TR>
 *     <TR><TD class="indexvalue" > 1 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][1]</TD></TR>
 *     <TR><TD class="indexvalue" > 2 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][2]</TD></TR>
 *     <TR><TD class="indexvalue" > 3 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][3]</TD></TR>
 *     <TR><TD class="indexvalue" > 4 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][0]</TD></TR>
 *     <TR><TD class="indexvalue" > 5 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][1]</TD></TR>
 *     <TR><TD class="indexvalue" > 6 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][2]</TD></TR>
 *     <TR><TD class="indexvalue" > 7 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][3]</TD></TR>
 *     <TR><TD class="indexvalue" > 8 </TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][0]</TD></TR>
 *     <TR><TD class="indexvalue" > 9 </TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][1]</TD></TR>
 *     <TR><TD class="indexvalue" > 10</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][2]</TD></TR>
 *     <TR><TD class="indexvalue" > 11</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][3]</TD></TR>
 *     <TR><TD class="indexvalue" > 12</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][4]</TD></TR>
 *     <TR><TD class="indexvalue" > 13</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][5]</TD></TR>
 *     <TR><TD class="indexvalue" > 14</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][6]</TD></TR>
 *     <TR><TD class="indexvalue" > 15</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][7]</TD></TR>
 *     <TR><TD class="indexvalue" > 16</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][8]</TD></TR>
 *     <TR><TD class="indexvalue" > 17</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][9]</TD></TR>
 *     <TR><TD class="indexvalue" > 18</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][10]</TD></TR>
 *     <TR><TD class="indexvalue" > 19</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][11]</TD></TR>
 *     <TR><TD class="indexvalue" > 20</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][12]</TD></TR>
 *     <TR><TD class="indexvalue" > 21</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][13]</TD></TR>
 *     <TR><TD class="indexvalue" > 22</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][14]</TD></TR>
 *     <TR><TD class="indexvalue" > 23</TD><TD class="indexvalue">GFX</TD><TD class="indexvalue">P_GFX_RX[P/N]/TX[P/N][15]</TD></TR>
 *     <TR><TD class="indexvalue" > 24</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[0]</TD></TR>
 *     <TR><TD class="indexvalue" > 25</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[1]</TD></TR>
 *     <TR><TD class="indexvalue" > 26</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[2]</TD></TR>
 *     <TR><TD class="indexvalue" > 27</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[3]</TD></TR>
 *     <TR><TD class="indexvalue" > 28</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[0]</TD></TR>
 *     <TR><TD class="indexvalue" > 29</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[1]</TD></TR>
 *     <TR><TD class="indexvalue" > 30</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[2]</TD></TR>
 *     <TR><TD class="indexvalue" > 31</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[3]</TD></TR>
 *  </TABLE>
 *
 */


/**
 *  @page F14ONPcieLaneDescription Family 0x14(ON) PCIe/DDI Lanes
 *  <TABLE border="0">
 *     <TR><TD class="indexkey" width=160> Lane ID</TD><TD class="indexkey">Lane group</TD><TD class="indexkey">Pin</TD></TR>
 *     <TR><TD class="indexvalue" > 0 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][0]</TD></TR>
 *     <TR><TD class="indexvalue" > 1 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][1]</TD></TR>
 *     <TR><TD class="indexvalue" > 2 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][2]</TD></TR>
 *     <TR><TD class="indexvalue" > 3 </TD><TD class="indexvalue">SB </TD><TD class="indexvalue">P_SB_RX[P/N]/TX[P/N][3]</TD></TR>
 *     <TR><TD class="indexvalue" > 4 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][0]</TD></TR>
 *     <TR><TD class="indexvalue" > 5 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][1]</TD></TR>
 *     <TR><TD class="indexvalue" > 6 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][2]</TD></TR>
 *     <TR><TD class="indexvalue" > 7 </TD><TD class="indexvalue">GPP</TD><TD class="indexvalue">P_GPP_RX[P/N]/TX[P/N][3]</TD></TR>
 *     <TR><TD class="indexvalue" > 8</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[0]</TD></TR>
 *     <TR><TD class="indexvalue" > 9</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[1]</TD></TR>
 *     <TR><TD class="indexvalue" > 10</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[2]</TD></TR>
 *     <TR><TD class="indexvalue" > 11</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP0_TXP/N[3]</TD></TR>
 *     <TR><TD class="indexvalue" > 12</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[0]</TD></TR>
 *     <TR><TD class="indexvalue" > 13</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[1]</TD></TR>
 *     <TR><TD class="indexvalue" > 14</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[2]</TD></TR>
 *     <TR><TD class="indexvalue" > 15</TD><TD class="indexvalue">DDI</TD><TD class="indexvalue">DP1_TXP/N[3]</TD></TR>
 *  </TABLE>
 *
 */


/**
 *  @page F12DualLinkDviDescription Family 0x12 Dual Link DVI connector description
 *  Examples of various Dual Link DVI descriptors.
 *  @code
 *  // Dual Link DVI on dedicated display lanes. DP1_TXP/N[0]..DP1_TXP/N[3] - master, DP0_TXP/N[0]..DP0_TXP/N[3] - slave.
 *  PCIe_PORT_DESCRIPTOR DdiList [] = {
 *    {
 *      DESCRIPTOR_TERMINATE_LIST,   //Descriptor flags
 *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 32),
 *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDualLinkDvi, Aux1, Hdp1, 0)
 *    }
 *  }
 * // Dual Link DVI on dedicated display lanes. DP0_TXP/N[0]..DP0_TXP/N[3] - master, DP1_TXP/N[0]..DP1_TXP/N[3] - slave.
 *  PCIe_PORT_DESCRIPTOR DdiList [] = {
 *    {
 *      DESCRIPTOR_TERMINATE_LIST,   //Descriptor flags
 *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 32, 24),
 *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDualLinkDvi, Aux1, Hdp1, 0)
 *    }
 *  }
 * // Dual Link DVI on PCIe lanes. P_GFX_TXP/N[0]..P_GFX_TXP/N[3] - master, P_GFX_TXP/N[4]..P_GFX_TXP/N[7] - slave.
 *  PCIe_PORT_DESCRIPTOR DdiList [] = {
 *    {
 *      DESCRIPTOR_TERMINATE_LIST,   //Descriptor flags
 *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 8, 15),
 *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDualLinkDvi, Aux1, Hdp1, 0)
 *    }
 *  }
 * // Dual Link DVI on PCIe lanes. P_GFX_TXP/N[7]..P_GFX_TXP/N[4] - master, P_GFX_TXP/N[0]..P_GFX_TXP/N[3] - slave.
 *  PCIe_PORT_DESCRIPTOR DdiList [] = {
 *    {
 *      DESCRIPTOR_TERMINATE_LIST,   //Descriptor flags
 *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 15, 8),
 *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDualLinkDvi, Aux1, Hdp1, 0)
 *    }
 *  }
 * // Dual Link DVI on PCIe lanes. P_GFX_TXP/N[8]..P_GFX_TXP/N[11] - master, P_GFX_TXP/N[12]..P_GFX_TXP/N[15] - slave.
 *  PCIe_PORT_DESCRIPTOR DdiList [] = {
 *    {
 *      DESCRIPTOR_TERMINATE_LIST,   //Descriptor flags
 *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 16, 23),
 *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDualLinkDvi, Aux1, Hdp1, 0)
 *    }
 *  }
 * // Dual Link DVI on PCIe lanes. P_GFX_TXP/N[12]..P_GFX_TXP/N[15] - master, P_GFX_TXP/N[8]..P_GFX_TXP/N[11] - slave.
 *  PCIe_PORT_DESCRIPTOR DdiList [] = {
 *    {
 *      DESCRIPTOR_TERMINATE_LIST,   //Descriptor flags
 *      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 23, 16),
 *      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDualLinkDvi, Aux1, Hdp1, 0)
 *    }
 *  }
 * @endcode
 */

/**
 * @page F12LaneConfigurations Family 0x12 PCIe port/DDI link configurations
 * <div class=WordSection1>
 *
 * <p class=MsoNormal><span style='font-size:14.0pt;line-height:115%'>PCIe port
 * configurations for lane 8 through 23. </span></p>
 *
 * <table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0
 *  style='border-collapse:collapse;border:none'>
 *  <tr>
 *   <td width=167 valign=top style='width:125.25pt;border:solid windowtext 1.5pt;
 *   background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Configuration</p>
 *   </td>
 *   <td width=132 valign=top style='width:99.0pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>PCIe Port Device Number</p>
 *   </td>
 *   <td width=180 valign=top style='width:135.0pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Start Lane (Start Lane in reverse
 *   configuration)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>End Line (End lane in reverse
 *   configuration)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=29 valign=top style='width:125.25pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Config A*</p>
 *   </td>
 *   <td width=132 rowspan=15 valign=top style='width:99.0pt;border-top:none;
 *   border-left:none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>2</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>&nbsp;</p>
 *   </td>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(23)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(8)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(15)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(8)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(11)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>11(8)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(9)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>9(8)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>10(11)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>11(10)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>12(15)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(12)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>12(13)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>13(12)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:15.25pt'>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:15.25pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>14(15)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:15.25pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(14)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(23)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(16)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(19)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>19(16)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(17)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>17(16)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>18(19)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>19(18)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>20(23)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(20)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>20(21)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>21(20)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:15.25pt'>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:15.25pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>22(23)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:15.25pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(22)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=132 rowspan=14 valign=top style='width:99.0pt;border-top:none;
 *   border-left:none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>3</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>&nbsp;</p>
 *   </td>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(15)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(8)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(11)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>11(8)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(9)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>9(8)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>10(11)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>11(10)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>12(15)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(12)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>12(13)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>13(12)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>14(15)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(14)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:15.25pt'>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:15.25pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(23)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:15.25pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(16)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(19)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>19(16)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(17)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>17(16)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>18(19)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>19(18)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>20(23)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(20)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>20(21)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>21(20)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=180 valign=top style='width:135.0pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>22(23)</p>
 *   </td>
 *   <td width=162 valign=top style='width:121.5pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(22)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=641 colspan=4 valign=top style='width:480.75pt;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>* Lanes selection for port 2/3 should not overlap in port configuration</p>
 *   </td>
 *  </tr>
 * </table>
 *
 * <p class=MsoNormal>&nbsp;</p>
 *
 * <p class=MsoNormal><span style='font-size:14.0pt;line-height:115%'>PCIe port
 * configurations for lane 4 through 7.</span></p>
 *
 * <table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0
 *  style='border-collapse:collapse;border:none'>
 *  <tr>
 *   <td width=167 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Configuration</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>PCIe Port Device Number</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Start Lane (Start Lane in reverse
 *   configuration)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>End Line (End lane in reverse
 *   configuration)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=3 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config A</p>
 *   </td>
 *   <td width=135 rowspan=3 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(7)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(5)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=4 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config B</p>
 *   </td>
 *   <td width=135 rowspan=2 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(5)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 rowspan=2 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5 or 6</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6(7)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7(6)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=4 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config C</p>
 *   </td>
 *   <td width=135 rowspan=2 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(5)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5 or 6</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6 or 7</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=4 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config D</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *  </tr>
 * </table>
 *
 * <p class=MsoNormal>&nbsp;</p>
 *
 * <p class=MsoNormal><span style='font-size:14.0pt;line-height:115%'>DDI link
 * configurations for lanes 24 through 31.</span></p>
 *
 * <table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0
 *  style='border-collapse:collapse;border:none'>
 *  <tr>
 *   <td width=167 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Configuration</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Connector type</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Start Lane (Start Lane in reverse
 *   configuration)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>End Line (End lane in reverse
 *   configuration)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:28.35pt'>
 *   <td width=167 valign=top style='width:125.0pt;border-top:none;border-left:
 *   solid windowtext 1.5pt;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:28.35pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config A</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:28.35pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Dual Link DVI-D</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:28.35pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>24(31)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:28.35pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>31(24)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=167 rowspan=2 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config B</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT </p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>24</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>27</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>28</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>31</p>
 *   </td>
 *  </tr>
 * </table>
 *
 * <p class=MsoNormal>&nbsp;</p>
 *
 * <p class=MsoNormal><span style='font-size:14.0pt;line-height:115%'>DDI link
 * configurations for lanes 8 through 23.</span></p>
 *
 * <table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0
 *  style='border-collapse:collapse;border:none'>
 *  <tr>
 *   <td width=167 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Configuration</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Connector type</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Start Lane (Start Lane in reverse
 *   configuration)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>End Line (End lane in reverse
 *   configuration)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:17.85pt'>
 *   <td width=167 rowspan=2 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt;height:17.85pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config A</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:17.85pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Dual Link DVI-D</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:17.85pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(15)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:17.85pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(8)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:16.5pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:16.5pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Dual Link DVI-D</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:16.5pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(23)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:16.5pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(16)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:16.5pt'>
 *   <td width=167 rowspan=3 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt;height:16.5pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config B</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:16.5pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Dual Link DVI-D</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:16.5pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8(15)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:16.5pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15(8)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>19</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>20</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23</p>
 *   </td>
 *  </tr>
 *  <tr style='height:93.0pt'>
 *   <td width=167 rowspan=3 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt;height:93.0pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config C</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:93.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:93.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:93.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>11</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>12</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15</p>
 *   </td>
 *  </tr>
 *  <tr style='height:18.3pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:18.3pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Dual Link DVI-D</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:18.3pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16(23)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:18.3pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23(16)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=167 rowspan=4 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config D</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT </p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>11</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>12</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT </p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>19</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>20</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>23</p>
 *   </td>
 *  </tr>
 * </table>
 *
 * <p class=MsoNormal>&nbsp;</p>
 *  </div>
 */

/**
 * @page F14ONLaneConfigurations Family 0x14(ON) PCIe port/DDI link configurations
 * <div class=WordSection1>
 * <p class=MsoNormal><span style='font-size:14.0pt;line-height:115%'>PCIe port
 * configurations for lane 4 through 7.</span></p>
 *
 * <table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0
 *  style='border-collapse:collapse;border:none'>
 *  <tr>
 *   <td width=167 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Configuration</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>PCIe Port Device Number</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Start Lane (Start Lane in reverse
 *   configuration)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>End Line (End lane in reverse
 *   configuration)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=3 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config A</p>
 *   </td>
 *   <td width=135 rowspan=3 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(7)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(5)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=4 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config B</p>
 *   </td>
 *   <td width=135 rowspan=2 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(5)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 rowspan=2 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5 or 6</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6(7)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7(6)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=4 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config C</p>
 *   </td>
 *   <td width=135 rowspan=2 valign=top style='width:100.9pt;border-top:none;
 *   border-left:none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4(5)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5(4)</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5 or 6</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6 or 7</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=167 rowspan=4 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config D</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>4</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>5</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>6</p>
 *   </td>
 *  </tr>
 *  <tr>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>7</p>
 *   </td>
 *  </tr>
 * </table>
 *
 * <p class=MsoNormal>&nbsp;</p>
 *
 * <p class=MsoNormal><span style='font-size:14.0pt;line-height:115%'>CRT/DDI link
 * configurations for lanes 8 through 19.</span></p>
 *
 * <table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0
 *  style='border-collapse:collapse;border:none'>
 *  <tr>
 *   <td width=167 valign=top style='width:125.0pt;border:solid windowtext 1.5pt;
 *   background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Configuration</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Connector type</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Start Lane (Start Lane in reverse
 *   configuration)</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border:solid windowtext 1.5pt;
 *   border-left:none;background:#C6D9F1;padding:0in 5.4pt 0in 5.4pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>End Line (End lane in reverse
 *   configuration)</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=167 rowspan=3 valign=top style='width:125.0pt;border-top:none;
 *   border-left:solid windowtext 1.5pt;border-bottom:solid black 1.0pt;
 *   border-right:solid windowtext 1.5pt;padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>Config A</p>
 *   </td>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-I*</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT </p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>8</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid black 1.0pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>11</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>HDMI</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-D</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Single Link DVI-I*</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>DP </p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>eDP</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-CRT</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Travis DP-to-LVDS</p>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>Hudson2 DP-to-CRT</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>12</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>15</p>
 *   </td>
 *  </tr>
 *  <tr style='height:95.0pt'>
 *   <td width=135 valign=top style='width:100.9pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>CRT*</p>
 *   </td>
 *   <td width=179 valign=top style='width:134.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>16</p>
 *   </td>
 *   <td width=158 valign=top style='width:118.45pt;border-top:none;border-left:
 *   none;border-bottom:solid windowtext 1.5pt;border-right:solid windowtext 1.5pt;
 *   padding:0in 5.4pt 0in 5.4pt;height:95.0pt'>
 *   <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
 *   text-align:center;line-height:normal'>19</p>
 *   </td>
 *  </tr>
 *  <tr style='height:35.85pt'>
 *   <td width=638 colspan=4 valign=top style='width:6.65in;border:solid windowtext 1.5pt;
 *   border-top:none;padding:0in 5.4pt 0in 5.4pt;height:35.85pt'>
 *   <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
 *   normal'>* - Only one connector of this time can exist in configuration</p>
 *   </td>
 *  </tr>
 * </table>
 *
 * <p class=MsoNormal>&nbsp;</p>
 *
 * <p class=MsoNormal>&nbsp;</p>
 *
 * </div>
 */

 /**
 * @page BuildConfigDescription GNB Build Configurations.
 *
 * Build configurations are configuration constants. The purpose
 * of build configurations is to specify configuration info
 * regarding the GNB component.
 *
 * The documented build configurations are imported from a user
 * controlled file for processing. The build configurations for
 * all platform solutions are listed below:
 *
 * @anchor BLDCFG_USE_SYNCFLOOD_AS_NMI
 * @li @e BLDCFG_USE_SYNCFLOOD_AS_NMI @n
 *        This build configuration defines the function of the
 *        SYNCFLOOD/NMI# pin.  If TRUE, then the pin is defined
 *        as NMI#.  If FALSE or undefined, the pin is defined as
 *        SYNCFLOOD_L.
 *
 */

