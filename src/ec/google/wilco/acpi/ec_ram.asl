/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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
Name (OREV, Package () { 0x3a, 0xff, RD })	/* Orientation Events */
Name (OECH, Package () { 0x3a, 0x01, RD })	/* Event: Orientation */
Name (OERL, Package () { 0x3a, 0x02, RD })	/* Event: Rotation Lock */

Name (BCCY, Package () { 0x3e, 0xffff, RD })	/* BCACHE: Cycle Count */

Name (APWR, Package () { 0x47, 0xff, RD })	/* POWER: Full Status */
Name (APAC, Package () { 0x47, 0x01, RD })	/* POWER: AC */
Name (APB1, Package () { 0x47, 0x02, RD })	/* POWER: Main Battery */
Name (APC1, Package () { 0x47, 0x04, RD })	/* POWER: Main Batt Status */

/*
 * EC RAM WRITE
 */

Name (FPTS, Package () { 0x02, 0xff, WR })	/* EC _PTS */
Name (BSEL, Package () { 0x03, 0xff, WR })	/* Battery Select */
Name (SSEL, Package () { 0x04, 0xff, WR })	/* Battery String Select */
Name (ERDY, Package () { 0x05, 0xff, WR })	/* EC Ready */
Name (FWAK, Package () { 0x06, 0xff, WR })	/* EC _WAK */
Name (PS2M, Package () { 0x20, 0xff, WR })	/* EC PS/2 Mouse Emulation */
Name (DWST, Package () { 0x32, 0xff, WR })	/* DPTF: Write State */
Name (DWTI, Package () { 0x33, 0xff, WR })	/* DPTF: Write Thermal Index */
Name (DWTL, Package () { 0x35, 0xff, WR })	/* DPTF: Write Trip Low */
Name (DWTH, Package () { 0x36, 0xff, WR })	/* DPTF: Write Trip High */
Name (DWHY, Package () { 0x37, 0xff, WR })	/* DPTF: Write Hysteresis */
Name (DWTQ, Package () { 0x38, 0xff, WR })	/* DPTF: Write Trip Query */
