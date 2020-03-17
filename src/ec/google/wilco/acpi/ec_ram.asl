/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Name (RD, 0)
Name (WR, 1)

/*
 * EC RAM READ
 */

Name (P1ST, Package () { 0x00, 0xff, RD })	/* PmSt1 */
Name (P1AC, Package () { 0x00, 0x01, RD })	/* PmSt1_AC_AVAIL */
Name (P1BA, Package () { 0x00, 0x02, RD })	/* PmSt1_BAT_AVAIL */
Name (P1L1, Package () { 0x00, 0x04, RD })	/* PmSt1_LOBAT1 */
Name (P1L2, Package () { 0x00, 0x08, RD })	/* PmSt1_LOBAT2 */
Name (P1LC, Package () { 0x00, 0x10, RD })	/* PmSt1_LID_CL */
Name (P1OT, Package () { 0x00, 0x40, RD })	/* PmSt1_OVERTEMP */

Name (P2ST, Package () { 0x01, 0xff, RD })	/* PmSt2 */
name (P2PB, Package () { 0x01, 0x04, RD })	/* PmSt2_PWRB_PRESSED */

Name (P3ST, Package () { 0x02, 0xff, RD })	/* PmSt3 */
Name (P3B1, Package () { 0x02, 0x04, RD })	/* PmSt3_BAT1_PRES */
Name (P3B2, Package () { 0x02, 0x08, RD })	/* PmSt3_BAT2_PRES */
Name (P3FB, Package () { 0x02, 0x40, RD })	/* PmSt3_FORCE_BOM */
Name (P3FT, Package () { 0x02, 0x80, RD })	/* PmSt3_FORCE_THROTTLE */

Name (P4ST, Package () { 0x03, 0xff, RD })	/* PmSt4 */
Name (P4C1, Package () { 0x03, 0x01, RD })	/* PmSt4_BAT1_CHG */
Name (P4C2, Package () { 0x03, 0x02, RD })	/* PmSt4_BAT2_CHG */
Name (P4P1, Package () { 0x03, 0x04, RD })	/* PmSt4_BAT1_PWR */
Name (P4P2, Package () { 0x03, 0x08, RD })	/* PmSt4_BAT2_PWR */
Name (P4BI, Package () { 0x03, 0x10, RD })	/* PmSt4_BAT_IDLE */
Name (P4PS, Package () { 0x03, 0x20, RD })	/* PmSt4_PANEL_STATE */
Name (P4CD, Package () { 0x03, 0x40, RD })	/* PmSt4_BATD_CHG */

Name (P5ST, Package () { 0x04, 0xff, RD })	/* PmSt5 */
Name (P5U1, Package () { 0x04, 0x01, RD })	/* PmSt5_BAT1_UPDATE */

Name (P6ST, Package () { 0x05, 0xff, RD })	/* PmSt6 */
Name (P6AC, Package () { 0x05, 0x08, RD })	/* PmSt6_AC_UPDATE */

Name (PWSR, Package () { 0x06, 0xff, RD })	/* POWER_SOURCE */
Name (ACEX, Package () { 0x06, 0x01, RD })	/* AC Present */
Name (BTEX, Package () { 0x06, 0x02, RD })	/* Battery Present */
Name (BTSC, Package () { 0x06, 0x04, RD })	/* Battery Status Changed */

Name (EVT1, Package () { 0x07, 0xff, RD })	/* Event 1 */
Name (E1PB, Package () { 0x07, 0x01, RD })	/* Power Button */
Name (E1LD, Package () { 0x07, 0x04, RD })	/* Lid */
Name (E1PW, Package () { 0x07, 0x08, RD })	/* Power */
Name (E1SB, Package () { 0x07, 0x40, RD })	/* Sleep Button */
Name (E1SS, Package () { 0x07, 0x80, RD })	/* SMI-SCI */

Name (EVT2, Package () { 0x08, 0xff, RD })	/* Event 2 */
Name (E2BS, Package () { 0x08, 0x02, RD })	/* BSS */
Name (E2OR, Package () { 0x08, 0x04, RD })	/* Orientation */
Name (E2QS, Package () { 0x08, 0x08, RD })	/* QS Event */
Name (E2PN, Package () { 0x08, 0x20, RD })	/* Panel */
Name (E2DP, Package () { 0x08, 0x40, RD })	/* Display Port */
Name (E2VT, Package () { 0x08, 0x80, RD })	/* Video Throttle */

Name (WAKE, Package () { 0x09, 0xff, RD })	/* Wake Events */
Name (WPWB, Package () { 0x09, 0x01, RD })	/* Wake: Power Button */
Name (WLID, Package () { 0x09, 0x02, RD })	/* Wake: Lid */
Name (WUSB, Package () { 0x09, 0x04, RD })	/* Wake: USB */
Name (WPME, Package () { 0x09, 0x10, RD })	/* Wake: PME */
Name (WRTC, Package () { 0x09, 0x20, RD })	/* Wake: RTC */
Name (WBAT, Package () { 0x09, 0x80, RD })	/* Wake: Low Battery */

Name (EVT3, Package () { 0x0b, 0xff, RD })	/* Event 3 */
Name (E3CP, Package () { 0x0b, 0x08, RD })	/* CS Power Button */
Name (E3TH, Package () { 0x0b, 0x10, RD })	/* Thermal Event */

Name (EVT4, Package () { 0x0c, 0xff, RD })	/* Event 4 */

Name (BCST, Package () { 0x10, 0xff, RD })	/* BCACHE: BST */
Name (BCRS, Package () { 0x11, 0xff, RD })	/* BCACHE: RSOC */
Name (BCCL, Package () { 0x12, 0xffff, RD })	/* BCACHE: Current */
Name (BCVL, Package () { 0x14, 0xffff, RD })	/* BCACHE: Voltage */
Name (BCYL, Package () { 0x16, 0xffff, RD })	/* BCACHE: Capacity */
Name (BCTL, Package () { 0x18, 0xffff, RD })	/* BCACHE: Temp */
Name (BCML, Package () { 0x1a, 0xffff, RD })	/* BCACHE: Ma */
Name (BSRL, Package () { 0x1c, 0xffff, RD })	/* BSTATIC: Max Error Low */
Name (BSFL, Package () { 0x1e, 0xffff, RD })	/* BSTATIC: Full Cap Low */
Name (BSCL, Package () { 0x20, 0xffff, RD })	/* BSTATIC: Design Cap Low */
Name (BSVL, Package () { 0x22, 0xffff, RD })	/* BSTATIC: Design Volt Low */
Name (BSDL, Package () { 0x24, 0xffff, RD })	/* BSTATIC: Mfg Date Low */
Name (BSSL, Package () { 0x26, 0xffff, RD })	/* BSTATIC: Serial Number Low */
Name (BSMN, Package () { 0x28, 0xff, RD })	/* BSTATIC: Manufacturer Name */
Name (BSDC, Package () { 0x29, 0xff, RD })	/* BSTATIC: Device Chemistry */
Name (BSBS, Package () { 0x2a, 0xff, RD })	/* BSTATIC: Battery String */

Name (QSEC, Package () { 0x2b, 0xff, RD })	/* QS Event Count */
Name (QSEB, Package () { 0x2c, 0xff, RD })	/* QS Event Byte */

Name (DRST, Package () { 0x32, 0xff, RD })	/* DPTF: Read State */
Name (DRTI, Package () { 0x33, 0xff, RD })	/* DPTF: Read Thermal Index */
Name (DRTV, Package () { 0x34, 0xff, RD })	/* DPTF: Read Thermal Value */
Name (DRTL, Package () { 0x35, 0xff, RD })	/* DPTF: Read Trip Low */
Name (DRTH, Package () { 0x36, 0xff, RD })	/* DPTF: Read Trip High */
Name (DRHY, Package () { 0x37, 0xff, RD })	/* DPTF: Read Hysteresis */
Name (DRTQ, Package () { 0x38, 0xff, RD })	/* DPTF: Read Trip Query */

Name (ORST, Package () { 0x39, 0xff, RD })	/* Orientation State */
Name (OTBL, Package () { 0x39, 0x03, RD })	/* Orient: Tablet/Tent mode */
Name (OREV, Package () { 0x3a, 0xff, RD })	/* Orientation Events */
Name (OECH, Package () { 0x3a, 0x01, RD })	/* Event: Orientation */
Name (OERL, Package () { 0x3a, 0x02, RD })	/* Event: Rotation Lock */

Name (BCCY, Package () { 0x3e, 0xffff, RD })	/* BCACHE: Cycle Count */

Name (ESGN, Package () { 0x5c, 0xff, RD })	/* Indicate EC uses signed FW */

Name (EPDT, Package () { 0x5d, 0xff, RD })	/* Privacy Screen Detection */
Name (EPST, Package () { 0x5e, 0xff, RD })	/* Privacy Screen State */

/*
 * EC RAM WRITE
 */

Name (FPTS, Package () { 0x02, 0xff, WR })	/* EC _PTS */
Name (BSEL, Package () { 0x03, 0xff, WR })	/* Battery Select */
Name (SSEL, Package () { 0x04, 0xff, WR })	/* Battery String Select */
Name (ERDY, Package () { 0x05, 0xff, WR })	/* EC Ready */
Name (FWAK, Package () { 0x06, 0xff, WR })	/* EC _WAK */
Name (PS2M, Package () { 0x20, 0xff, WR })	/* EC PS/2 Mouse Emulation */
Name (EPCT, Package () { 0x25, 0xff, WR })	/* Privacy Screen Control */
Name (DWST, Package () { 0x32, 0xff, WR })	/* DPTF: Write State */
Name (DWTI, Package () { 0x33, 0xff, WR })	/* DPTF: Write Thermal Index */
Name (DWTL, Package () { 0x35, 0xff, WR })	/* DPTF: Write Trip Low */
Name (DWTH, Package () { 0x36, 0xff, WR })	/* DPTF: Write Trip High */
Name (DWHY, Package () { 0x37, 0xff, WR })	/* DPTF: Write Hysteresis */
Name (DWTQ, Package () { 0x38, 0xff, WR })	/* DPTF: Write Trip Query */
Name (CSOS, Package () { 0xb8, 0xff, WR })	/* OS support for S0ix */
Name (CSEX, Package () { 0xb9, 0xff, WR })	/* OS enter(1)/exit(0) S0ix */

/*
 * EC UCSI
 */
Name (UVR0, Package () { 0x80, 0xff, RD })	/* UCSI Version */
Name (UVR1, Package () { 0x81, 0xff, RD })	/* UCSI Version */
Name (UCI0, Package () { 0x84, 0xff, RD })	/* UCSI Change Indicator */
Name (UCI1, Package () { 0x85, 0xff, RD })	/* UCSI Change Indicator */
Name (UCI2, Package () { 0x86, 0xff, RD })	/* UCSI Change Indicator */
Name (UCI3, Package () { 0x87, 0xff, RD })	/* UCSI Change Indicator */
Name (UCL0, Package () { 0x88, 0xff, WR })	/* UCSI Control */
Name (UCL1, Package () { 0x89, 0xff, WR })	/* UCSI Control */
Name (UCL2, Package () { 0x8a, 0xff, WR })	/* UCSI Control */
Name (UCL3, Package () { 0x8b, 0xff, WR })	/* UCSI Control */
Name (UCL4, Package () { 0x8c, 0xff, WR })	/* UCSI Control */
Name (UCL5, Package () { 0x8d, 0xff, WR })	/* UCSI Control */
Name (UCL6, Package () { 0x8e, 0xff, WR })	/* UCSI Control */
Name (UCL7, Package () { 0x8f, 0xff, WR })	/* UCSI Control */
Name (UMI0, Package () { 0x90, 0xff, RD })	/* UCSI Message In */
Name (UMI1, Package () { 0x91, 0xff, RD })	/* UCSI Message In */
Name (UMI2, Package () { 0x92, 0xff, RD })	/* UCSI Message In */
Name (UMI3, Package () { 0x93, 0xff, RD })	/* UCSI Message In */
Name (UMI4, Package () { 0x94, 0xff, RD })	/* UCSI Message In */
Name (UMI5, Package () { 0x95, 0xff, RD })	/* UCSI Message In */
Name (UMI6, Package () { 0x96, 0xff, RD })	/* UCSI Message In */
Name (UMI7, Package () { 0x97, 0xff, RD })	/* UCSI Message In */
Name (UMI8, Package () { 0x98, 0xff, RD })	/* UCSI Message In */
Name (UMI9, Package () { 0x99, 0xff, RD })	/* UCSI Message In */
Name (UMIA, Package () { 0x9a, 0xff, RD })	/* UCSI Message In */
Name (UMIB, Package () { 0x9b, 0xff, RD })	/* UCSI Message In */
Name (UMIC, Package () { 0x9c, 0xff, RD })	/* UCSI Message In */
Name (UMID, Package () { 0x9d, 0xff, RD })	/* UCSI Message In */
Name (UMIE, Package () { 0x9e, 0xff, RD })	/* UCSI Message In */
Name (UMIF, Package () { 0x9f, 0xff, RD })	/* UCSI Message In */
Name (UMO0, Package () { 0xa0, 0xff, WR })	/* UCSI Message Out */
Name (UMO1, Package () { 0xa1, 0xff, WR })	/* UCSI Message Out */
Name (UMO2, Package () { 0xa2, 0xff, WR })	/* UCSI Message Out */
Name (UMO3, Package () { 0xa3, 0xff, WR })	/* UCSI Message Out */
Name (UMO4, Package () { 0xa4, 0xff, WR })	/* UCSI Message Out */
Name (UMO5, Package () { 0xa5, 0xff, WR })	/* UCSI Message Out */
Name (UMO6, Package () { 0xa6, 0xff, WR })	/* UCSI Message Out */
Name (UMO7, Package () { 0xa7, 0xff, WR })	/* UCSI Message Out */
Name (UMO8, Package () { 0xa8, 0xff, WR })	/* UCSI Message Out */
Name (UMO9, Package () { 0xa9, 0xff, WR })	/* UCSI Message Out */
Name (UMOA, Package () { 0xaa, 0xff, WR })	/* UCSI Message Out */
Name (UMOB, Package () { 0xab, 0xff, WR })	/* UCSI Message Out */
Name (UMOC, Package () { 0xac, 0xff, WR })	/* UCSI Message Out */
Name (UMOD, Package () { 0xad, 0xff, WR })	/* UCSI Message Out */
Name (UMOE, Package () { 0xae, 0xff, WR })	/* UCSI Message Out */
Name (UMOF, Package () { 0xaf, 0xff, WR })	/* UCSI Message Out */
Name (UCTL, Package () { 0xb0, 0xff, WR })	/* UCSI Control Register */
