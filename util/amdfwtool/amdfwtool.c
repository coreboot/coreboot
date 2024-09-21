/* SPDX-License-Identifier: GPL-2.0-only */

/*
 *  ROMSIG At ROMBASE + 0x[0,2,4,8]20000:
 *  0            4               8                C
 *  +------------+---------------+----------------+------------+
 *  | 0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
 *  +------------+---------------+----------------+------------+
 *  | PSPDIR ADDR|PSPDIR ADDR(C) |   BDT ADDR 0   | BDT ADDR 1 |
 *  +------------+---------------+----------------+------------+
 *  | BDT ADDR 2 |               |  BDT ADDR 3(C) |            |
 *  +------------+---------------+----------------+------------+
 *  (C): Could be a combo header
 *
 *  EC ROM should be 64K aligned.
 *
 *  PSP directory (Where "PSPDIR ADDR" points)
 *  +------------+---------------+----------------+------------+
 *  | 'PSP$'     | Fletcher      |    Count       | Reserved   |
 *  +------------+---------------+----------------+------------+
 *  |  0         | size          | Base address   | Reserved   | Pubkey
 *  +------------+---------------+----------------+------------+
 *  |  1         | size          | Base address   | Reserved   | Bootloader
 *  +------------+---------------+----------------+------------+
 *  |  8         | size          | Base address   | Reserved   | Smu Firmware
 *  +------------+---------------+----------------+------------+
 *  |  3         | size          | Base address   | Reserved   | Recovery Firmware
 *  +------------+---------------+----------------+------------+
 *  |                                                          |
 *  |                                                          |
 *  |             Other PSP Firmware                          |
 *  |                                                          |
 *  +------------+---------------+----------------+------------+
 *  |  40        | size          | Base address   | Reserved   |---+
 *  +------------+---------------+----------------+------------+   |
 *  :or 48(A/B A): size          : Base address   : Reserved   :   |
 *  +   -    -   +    -     -    +    -      -    +  -    -    +   |
 *  :   4A(A/B B): size          : Base address   : Reserved   :   |
 *  +------------+---------------+----------------+------------+   |
 *  (A/B A) & (A/B B): Similar as 40, pointing to PSP level 2      |
 *  for A/B recovery                                               |
 *                                                                 |
 *                                                                 |
 *  +------------+---------------+----------------+------------+   |
 *  | '2LP$'     | Fletcher      |    Count       | Reserved   |<--+
 *  +------------+---------------+----------------+------------+
 *  |                                                          |
 *  |                                                          |
 *  |             PSP Firmware                                |
 *  |      (2nd-level is not required on all families)         |
 *  |                                                          |
 *  +------------+---------------+----------------+------------+
 *  BIOS Directory Table (BDT) is similar
 *
 *  PSP Combo directory
 *  +------------+---------------+----------------+------------+
 *  | 'PSP2'     | Fletcher      |    Count       |Look up mode|
 *  +------------+---------------+----------------+------------+
 *  |            R e s e r v e d                               |
 *  +------------+---------------+----------------+------------+
 *  | ID-Sel     | PSP ID        |   PSPDIR ADDR  |            | 1st PSP directory
 *  +------------+---------------+----------------+------------+
 *  | ID-Sel     | PSP ID        |   PSPDIR ADDR  |            | 2nd PSP directory
 *  +------------+---------------+----------------+------------+
 *  |                                                          |
 *  |        Other PSP                                         |
 *  |                                                          |
 *  +------------+---------------+----------------+------------+
 *  BDT Combo is similar
 */

#include <commonlib/bsd/helpers.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdint.h>

#include "amdfwtool.h"

#define AMD_ROMSIG_OFFSET	0x20000

#define _MAX(A, B) (((A) > (B)) ? (A) : (B))

static void output_manifest(int manifest_fd, amd_fw_entry *fw_entry);

/*
 * Beginning with Family 15h Models 70h-7F, a.k.a Stoney Ridge, the PSP
 * can support an optional "combo" implementation.  If the PSP sees the
 * PSP2 cookie, it interprets the table as a roadmap to additional PSP
 * tables.  Using this, support for multiple product generations may be
 * built into one image.  If the PSP$ cookie is found, the table is a
 * normal directory table.
 *
 * Modern generations supporting the combo directories require the
 * pointer to be at offset 0x14 of the Embedded Firmware Structure,
 * regardless of the type of directory used.  The --use-combo
 * argument enforces this placement.
 *
 * TODO: Future work may require fully implementing the PSP_COMBO feature.
 */

/*
 * Creates the OSI Fletcher checksum. See 8473-1, Appendix C, section C.3.
 * The checksum field of the passed PDU does not need to be reset to zero.
 *
 * The "Fletcher Checksum" was proposed in a paper by John G. Fletcher of
 * Lawrence Livermore Labs.  The Fletcher Checksum was proposed as an
 * alternative to cyclical redundancy checks because it provides error-
 * detection properties similar to cyclical redundancy checks but at the
 * cost of a simple summation technique.  Its characteristics were first
 * published in IEEE Transactions on Communications in January 1982.  One
 * version has been adopted by ISO for use in the class-4 transport layer
 * of the network protocol.
 *
 * This program expects:
 *    stdin:    The input file to compute a checksum for.  The input file
 *              not be longer than 256 bytes.
 *    stdout:   Copied from the input file with the Fletcher's Checksum
 *              inserted 8 bytes after the beginning of the file.
 *    stderr:   Used to print out error messages.
 */
static uint32_t fletcher32(const void *data, int length)
{
	uint32_t c0;
	uint32_t c1;
	uint32_t checksum;
	int index;
	const uint16_t *pptr = data;

	length /= 2;

	c0 = 0xFFFF;
	c1 = 0xFFFF;

	while (length) {
		index = length >= 359 ? 359 : length;
		length -= index;
		do {
			c0 += *(pptr++);
			c1 += c0;
		} while (--index);
		c0 = (c0 & 0xFFFF) + (c0 >> 16);
		c1 = (c1 & 0xFFFF) + (c1 >> 16);
	}

	/* Sums[0,1] mod 64K + overflow */
	c0 = (c0 & 0xFFFF) + (c0 >> 16);
	c1 = (c1 & 0xFFFF) + (c1 >> 16);
	checksum = (c1 << 16) | c0;

	return checksum;
}

amd_fw_entry amd_psp_fw_table[] = {
	{ .type = AMD_FW_PSP_PUBKEY, .level = PSP_BOTH | PSP_LVL2_AB, .skip_hashing = true },
	{ .type = AMD_FW_PSP_BOOTLOADER, .level = PSP_BOTH | PSP_LVL2_AB,
		.generate_manifest = true },
	{ .type = AMD_FW_PSP_SECURED_OS, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_RECOVERY, .level = PSP_LVL1 },
	{ .type = AMD_FW_PSP_NVRAM, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_RTM_PUBKEY, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 0, .level = PSP_BOTH | PSP_LVL2_AB,
		.generate_manifest = true },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SECURED_DEBUG, .level = PSP_LVL2 | PSP_LVL2_AB,
	  .skip_hashing = true },
	{ .type = AMD_FW_PSP_TEEIPKEY, .subprog = 0, .level = PSP_LVL2 | PSP_LVL2_AB,
	  .skip_hashing = true },
	{ .type = AMD_FW_ABL_PUBKEY, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_PSP_FUSE_CHAIN, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_TRUSTLETS, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_TRUSTLETKEY, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_BOOT_DRIVER, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SOC_DRIVER, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_DEBUG_DRIVER, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_INTERFACE_DRIVER, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_DEBUG_UNLOCK, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_HW_IPCFG, .subprog = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_HW_IPCFG, .subprog = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_HW_IPCFG, .subprog = 2, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_WRAPPED_IKEK, .level = PSP_BOTH | PSP_LVL2_AB, .skip_hashing = true },
	{ .type = AMD_TOKEN_UNLOCK, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SEC_GASKET, .subprog = 0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SEC_GASKET, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SEC_GASKET, .subprog = 2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_MP2_FW, .subprog = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_MP2_FW, .subprog = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_MP2_FW, .subprog = 2, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_DRIVER_ENTRIES, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_KVM_IMAGE, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MP5, .subprog = 0, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_FW_MP5, .subprog = 1, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_FW_MP5, .subprog = 2, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_S0I3_DRIVER, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_ABL0, .level = PSP_BOTH | PSP_LVL2_AB,
		.generate_manifest = true },
	{ .type = AMD_ABL1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL3, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL4, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL5, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL6, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL7, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SEV_DATA, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_SEV_CODE, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_WHITELIST, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_VBIOS_BTLOADER, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_DXIO, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_FW_USB_PHY, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_TOS_SEC_POLICY, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_DRTM_TA, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_KEYDB_BL, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_KEYDB_TOS, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_VERSTAGE, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_VERSTAGE_SIG, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_RPMC_NVRAM, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_SPL, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_DMCU_ERAM, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_DMCU_ISR, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MSMU, .subprog = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MSMU, .subprog = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_SPIROM_CFG, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MPIO, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMUSCS, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_DMCUB, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_BOOTLOADER_AB, .level = PSP_LVL2 | PSP_LVL2_AB,
		.generate_manifest = true },
	{ .type = AMD_RIB, .subprog = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_RIB, .subprog = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_RIB, .subprog = 2, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MPDMA_TF, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_TA_IKEK, .level = PSP_BOTH | PSP_LVL2_AB, .skip_hashing = true },
	{ .type = AMD_FW_GMI3_PHY, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_FW_MPDMA_PM, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_FW_AMF_SRAM, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_AMF_DRAM, .inst = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_AMF_DRAM, .inst = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MFD_MPM, .inst = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MFD_MPM, .inst = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_FCFG_TABLE, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_AMF_WLAN, .inst = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_AMF_WLAN, .inst = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_AMF_WLAN, .inst = 2, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_AMF_WLAN, .inst = 3, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_AMF_MFD, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_TA_IKEK, .level = PSP_BOTH | PSP_LVL2_AB, .skip_hashing = true },
	{ .type = AMD_FW_MPCCX, .subprog = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MPCCX, .subprog = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_LSDMA, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_C20_MP, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_MINIMSMU, .inst = 0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_MINIMSMU, .inst = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_MINIMSMU, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_GFXIMU_0, .subprog = 0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_GFXIMU_0, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_GFXIMU_1, .subprog = 0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_GFXIMU_1, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_GFXIMU_2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_SRAM_FW_EXT, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_UMSMU, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_S3IMG, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_USBDP, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_USBSS, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_USB4, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_INVALID },
};

amd_fw_entry amd_fw_table[] = {
	{ .type = AMD_FW_XHCI },
	{ .type = AMD_FW_IMC },
	{ .type = AMD_FW_GEC },
	{ .type = AMD_FW_INVALID },
};

amd_bios_entry amd_bios_table[] = {
	{ .type = AMD_BIOS_RTM_PUBKEY, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_SIG, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 2, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 3, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 4, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 5, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 6, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 7, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 8, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 9, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 10, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 11, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 12, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 13, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 14, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB, .inst = 15, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 2, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 3, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 4, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 5, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 6, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 7, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 8, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 9, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 10, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 11, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 12, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 13, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 14, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APCB_BK, .inst = 15, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APOB, .level = BDT_BOTH },
	{ .type = AMD_BIOS_BIN,
			.reset = 1, .copy = 1, .zlib = 1, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APOB_NV, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_NV_ST, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_PMUI, .inst = 1, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 1, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 2, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 2, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 3, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 3, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 4, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 4, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 5, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 5, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 6, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 6, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 7, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 7, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 9, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 9, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 10, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 10, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 11, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 11, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 12, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 12, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 13, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 13, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 1, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 1, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 2, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 2, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 3, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 3, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 4, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 4, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 5, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 5, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 6, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 6, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 7, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 7, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 9, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 9, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 10, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 10, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 11, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 11, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 12, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 12, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 13, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 13, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_UCODE, .inst = 0, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 1, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 2, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 3, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 4, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 5, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 6, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_MP2_CFG, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_PSP_SHARED_MEM, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_INVALID },
};

#define RUN_BASE(ctx) (0xFFFFFFFF - (ctx).rom_size + 1)
#define RUN_OFFSET_MODE(ctx, offset, mode) \
		((mode) == AMD_ADDR_PHYSICAL ? RUN_BASE(ctx) + (offset) :	\
		((mode) == AMD_ADDR_REL_BIOS ? (offset) :	\
		((mode) == AMD_ADDR_REL_TAB ? (offset) - (ctx).current_table : (offset))))
#define RUN_OFFSET(ctx, offset) RUN_OFFSET_MODE((ctx), (offset), (ctx).address_mode)
#define RUN_TO_OFFSET(ctx, run) ((ctx).address_mode == AMD_ADDR_PHYSICAL ?	\
				(run) - RUN_BASE(ctx) : (run)) /* TODO: */
#define RUN_CURRENT(ctx) RUN_OFFSET((ctx), (ctx).current)
/* The mode in entry can not be higher than the header's.
   For example, if table mode is 0, all the entry mode will be 0. */
#define RUN_CURRENT_MODE(ctx, mode) RUN_OFFSET_MODE((ctx), (ctx).current, \
		(ctx).address_mode < (mode) ? (ctx).address_mode : (mode))
#define BUFF_OFFSET(ctx, offset) ((void *)((ctx).rom + (offset)))
#define BUFF_CURRENT(ctx) BUFF_OFFSET((ctx), (ctx).current)
#define BUFF_TO_RUN(ctx, ptr) RUN_OFFSET((ctx), ((char *)(ptr) - (ctx).rom))
#define BUFF_TO_RUN_MODE(ctx, ptr, mode) RUN_OFFSET_MODE((ctx), ((char *)(ptr) - (ctx).rom), \
		(ctx).address_mode < (mode) ? (ctx).address_mode : (mode))
#define BUFF_ROOM(ctx) ((ctx).rom_size - (ctx).current)
/* AMD PSP Spec: Only set the address mode in entry if the table is mode 2 or 3. */
/* For address mode 3, it is not be used in any SOC family yet.
   For address mode 1, we can use it to store and transfer the address mode.
   It can reduce the complexity. */
#define SET_ADDR_MODE(table, mode) \
		((table)->header.additional_info_fields.address_mode == AMD_ADDR_REL_TAB ||  \
		 (table)->header.additional_info_fields.address_mode == AMD_ADDR_REL_BIOS || \
		 (table)->header.additional_info_fields.address_mode == AMD_ADDR_REL_SLOT    \
		 ? (mode) : 0)
#define SET_ADDR_MODE_BY_TABLE(table) \
		SET_ADDR_MODE((table), (table)->header.additional_info_fields.address_mode)


static void free_psp_firmware_filenames(amd_fw_entry *fw_table)
{
	amd_fw_entry *index;

	for (index = fw_table; index->type != AMD_FW_INVALID; index++) {
		if (index->filename &&
				index->type != AMD_FW_VERSTAGE_SIG &&
				index->type != AMD_FW_PSP_VERSTAGE &&
				index->type != AMD_FW_SPL &&
				index->type != AMD_FW_PSP_WHITELIST) {
			free(index->filename);
			index->filename = NULL;
		}
	}
}

static void free_bdt_firmware_filenames(amd_bios_entry *fw_table)
{
	amd_bios_entry *index;

	for (index = fw_table; index->type != AMD_BIOS_INVALID; index++) {
		if (index->filename &&
				index->type != AMD_BIOS_APCB &&
				index->type != AMD_BIOS_BIN &&
				index->type != AMD_BIOS_APCB_BK &&
				index->type != AMD_BIOS_UCODE) {
			free(index->filename);
			index->filename = NULL;
		}
	}
}

static void amdfwtool_cleanup(context *ctx)
{
	free(ctx->rom);
	ctx->rom = NULL;

	/* Free the filename. */
	free_psp_firmware_filenames(amd_psp_fw_table);
	free_bdt_firmware_filenames(amd_bios_table);

	free(ctx->amd_psp_fw_table_clean);
	ctx->amd_psp_fw_table_clean = NULL;
	free(ctx->amd_bios_table_clean);
	ctx->amd_bios_table_clean = NULL;
}

void assert_fw_entry(uint32_t count, uint32_t max, context *ctx)
{
	if (count >= max) {
		fprintf(stderr, "Error: BIOS entries (%d) exceeds max allowed items "
			"(%d)\n", count, max);
		amdfwtool_cleanup(ctx);
		exit(1);
	}
}

static void set_current_pointer(context *ctx, uint32_t value)
{
	if (ctx->current_pointer_saved != 0xFFFFFFFF &&
			ctx->current_pointer_saved != ctx->current) {
		fprintf(stderr, "Error: The pointer is changed elsewhere\n");
		amdfwtool_cleanup(ctx);
		exit(1);
	}

	ctx->current = value;

	if (ctx->current > ctx->rom_size) {
		fprintf(stderr, "Error: Packing data causes overflow\n");
		amdfwtool_cleanup(ctx);
		exit(1);
	}

	ctx->current_pointer_saved = ctx->current;
}

static void adjust_current_pointer(context *ctx, uint32_t add, uint32_t align)
{
	/* Get */
	set_current_pointer(ctx, ALIGN_UP(ctx->current + add, align));
}

static void *new_psp_dir(context *ctx, int multi, uint32_t cookie)
{
	void *ptr;

	/*
	 * Force both onto boundary when multi.  Primary table is after
	 * updatable table, so alignment ensures primary can stay intact
	 * if secondary is reprogrammed.
	 */
	if (multi)
		adjust_current_pointer(ctx, 0, TABLE_ERASE_ALIGNMENT);
	else
		adjust_current_pointer(ctx, 0, TABLE_ALIGNMENT);

	ptr = BUFF_CURRENT(*ctx);
	((psp_directory_header *)ptr)->cookie = cookie;
	((psp_directory_header *)ptr)->num_entries = 0;
	((psp_directory_header *)ptr)->additional_info = 0;
	((psp_directory_header *)ptr)->additional_info_fields.address_mode = ctx->address_mode;
	((psp_directory_header *)ptr)->additional_info_fields.spi_block_size = 1;
	((psp_directory_header *)ptr)->additional_info_fields.base_addr = 0;
	adjust_current_pointer(ctx,
		sizeof(psp_directory_header) + MAX_PSP_ENTRIES * sizeof(psp_directory_entry),
		1);
	return ptr;
}

static void *new_ish_dir(context *ctx)
{
	void *ptr;
	adjust_current_pointer(ctx, 0, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	adjust_current_pointer(ctx, TABLE_ALIGNMENT, 1);

	return ptr;
}

static void *new_combo_dir(context *ctx, uint32_t cookie)
{
	void *ptr;

	adjust_current_pointer(ctx, 0, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	((psp_combo_header *)ptr)->cookie = cookie;
	/* lookup mode is hardcoded for now. */
	((psp_combo_header *)ptr)->lookup = 1;
	((psp_combo_header *)ptr)->reserved[0] = 0;
	((psp_combo_header *)ptr)->reserved[1] = 0;

	adjust_current_pointer(ctx,
		sizeof(psp_combo_header) + MAX_COMBO_ENTRIES * sizeof(psp_combo_entry),
		1);
	return ptr;
}

/*
 * For some SOC generations the APOB_NV binary seems to be treated special regarding the
 * interpretaion of the source address. No matter the address_mode specified for the address
 * the memory ABL always seems to the interpret the source address as MMIO address even if
 * AMD_ADDR_REL_BIOS is specified. So for them we need to always use an MMIO address.
 * This seems to be a bug which affects all SOCs before phoenix generation.
 */
static bool has_apob_nv_quirk(enum platform platform_type)
{
	switch (platform_type) {
	case PLATFORM_CARRIZO:
	case PLATFORM_STONEYRIDGE:
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
	case PLATFORM_RENOIR:
	case PLATFORM_CEZANNE:
	case PLATFORM_MENDOCINO:
	case PLATFORM_LUCIENNE:
		return true;
	default:
		return false;
	}
}

static void copy_psp_header(void *bak, void *orig)
{
	uint32_t count = ((psp_directory_header *)orig)->num_entries;
	memcpy(bak, orig, count * sizeof(bios_directory_entry) + sizeof(psp_directory_table));
}

static void fill_dir_header(void *directory, uint32_t count, context *ctx)
{
	if (ctx == NULL || directory == NULL) {
		fprintf(stderr, "Calling %s with NULL pointers\n", __func__);
		return;
	}

	psp_combo_directory *cdir = directory;
	psp_directory_table *dir = directory;
	bios_directory_table *bdir = directory;
	/* The cookies have same offsets. */
	uint32_t cookie = ((psp_directory_table *)directory)->header.cookie;
	uint32_t table_size = 0;

	/* The table size needs to be 0x1000 aligned. So align the end of table. */
	adjust_current_pointer(ctx, 0, TABLE_ALIGNMENT);

	switch (cookie) {
	case PSP2_COOKIE:
	case BHD2_COOKIE:
		cdir->header.num_entries = count;
		/* checksum everything that comes after the Checksum field */
		cdir->header.checksum = fletcher32(&cdir->header.num_entries,
					count * sizeof(psp_combo_entry)
					+ sizeof(cdir->header.num_entries)
					+ sizeof(cdir->header.lookup)
					+ 2 * sizeof(cdir->header.reserved[0]));
		break;
	case PSP_COOKIE:
	case PSPL2_COOKIE:
		/* The table size is only set once. Later calls only update
		 * the count and fletcher. So does the BIOS table. */
		if (dir->header.additional_info_fields.dir_size == 0) {
			table_size = ctx->current - ctx->current_table;
			if ((table_size % TABLE_ALIGNMENT) != 0 &&
				(table_size / TABLE_ALIGNMENT) != 0) {
				fprintf(stderr, "The PSP table size should be 4K aligned\n");
				amdfwtool_cleanup(ctx);
				exit(1);
			}
			dir->header.additional_info_fields.dir_size =
					table_size / TABLE_ALIGNMENT;
		}
		dir->header.num_entries = count;
		/* checksum everything that comes after the Checksum field */
		dir->header.checksum = fletcher32(&dir->header.num_entries,
					count * sizeof(psp_directory_entry)
					+ sizeof(dir->header.num_entries)
					+ sizeof(dir->header.additional_info));
		break;
	case BHD_COOKIE:
	case BHDL2_COOKIE:
		if (bdir->header.additional_info_fields.dir_size == 0) {
			table_size = ctx->current - ctx->current_table;
			if ((table_size % TABLE_ALIGNMENT) != 0 &&
				table_size / TABLE_ALIGNMENT != 0) {
				fprintf(stderr, "The BIOS table size should be 4K aligned\n");
				amdfwtool_cleanup(ctx);
				exit(1);
			}
			bdir->header.additional_info_fields.dir_size =
				table_size / TABLE_ALIGNMENT;
		}
		bdir->header.num_entries = count;
		/* checksum everything that comes after the Checksum field */
		bdir->header.checksum = fletcher32(&bdir->header.num_entries,
					count * sizeof(bios_directory_entry)
					+ sizeof(bdir->header.num_entries)
					+ sizeof(bdir->header.additional_info));
		break;
	}
}

static void fill_psp_directory_to_efs(embedded_firmware *amd_romsig, void *pspdir,
	context *ctx, amd_cb_config *cb_config)
{
	switch (cb_config->soc_id) {
	case PLATFORM_UNKNOWN:
		amd_romsig->psp_directory =
			BUFF_TO_RUN_MODE(*ctx, pspdir, AMD_ADDR_REL_BIOS);
		break;
	case PLATFORM_CEZANNE:
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_GLINDA:
	case PLATFORM_CARRIZO:
	case PLATFORM_STONEYRIDGE:
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
	case PLATFORM_LUCIENNE:
	case PLATFORM_RENOIR:
	case PLATFORM_GENOA:
	case PLATFORM_FAEGAN:
	default:
		/* for combo, it is also combo_psp_directory */
		amd_romsig->new_psp_directory =
			BUFF_TO_RUN_MODE(*ctx, pspdir, AMD_ADDR_REL_BIOS);
		break;
	}
}

static void fill_psp_bak_directory_to_efs(embedded_firmware *amd_romsig, void *pspdir_bak,
	context *ctx, amd_cb_config *cb_config)
{
	if (cb_config->recovery_ab)
		amd_romsig->psp_bak_directory =
			BUFF_TO_RUN_MODE(*ctx, pspdir_bak, AMD_ADDR_REL_BIOS);
}

static void fill_bios_directory_to_efs(embedded_firmware *amd_romsig, void *biosdir,
	context *ctx, amd_cb_config *cb_config)
{
	switch (cb_config->soc_id) {
	case PLATFORM_RENOIR:
	case PLATFORM_LUCIENNE:
	case PLATFORM_CEZANNE:
	case PLATFORM_GENOA:
		if (!cb_config->recovery_ab)
			amd_romsig->bios3_entry =
				BUFF_TO_RUN_MODE(*ctx, biosdir, AMD_ADDR_REL_BIOS);
		break;
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_GLINDA:
	case PLATFORM_FAEGAN:
		break;
	case PLATFORM_CARRIZO:
	case PLATFORM_STONEYRIDGE:
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
	default:
		amd_romsig->bios1_entry =
			BUFF_TO_RUN_MODE(*ctx, biosdir, AMD_ADDR_REL_BIOS);
		break;
	}
}

static uint32_t get_psp_id(enum platform soc_id)
{
	uint32_t psp_id;
	switch (soc_id) {
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
		psp_id = 0xBC0A0000;
		break;
	case PLATFORM_RENOIR:
	case PLATFORM_LUCIENNE:
		psp_id = 0xBC0C0000;
		break;
	case PLATFORM_CEZANNE:
		psp_id = 0xBC0C0140;
		break;
	case PLATFORM_MENDOCINO:
		psp_id = 0xBC0D0900;
		break;
	case PLATFORM_STONEYRIDGE:
		psp_id = 0x10220B00;
		break;
	case PLATFORM_GLINDA:
		psp_id = 0xBC0E0200;
		break;
	case PLATFORM_PHOENIX:
		psp_id = 0xBC0D0400;
		break;
	case PLATFORM_GENOA:
		psp_id = 0xBC0C0111;
		break;
	case PLATFORM_FAEGAN:
		psp_id = 0xbc0e1000;
		break;
	case PLATFORM_CARRIZO:
	default:
		psp_id = 0;
		break;
	}
	return psp_id;
}

static void integrate_firmwares(context *ctx,
				embedded_firmware *romsig,
				amd_fw_entry *fw_table)
{
	ssize_t bytes;
	uint32_t i;

	adjust_current_pointer(ctx, 0, BLOB_ALIGNMENT);

	for (i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (fw_table[i].filename != NULL) {
			switch (fw_table[i].type) {
			case AMD_FW_IMC:
				adjust_current_pointer(ctx, 0, 0x10000U);
				romsig->imc_entry = RUN_CURRENT(*ctx);
				break;
			case AMD_FW_GEC:
				romsig->gec_entry = RUN_CURRENT(*ctx);
				break;
			case AMD_FW_XHCI:
				romsig->xhci_entry = RUN_CURRENT(*ctx);
				break;
			default:
				/* Error */
				break;
			}

			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes < 0) {
				amdfwtool_cleanup(ctx);
				exit(1);
			}

			adjust_current_pointer(ctx, bytes, BLOB_ALIGNMENT);
		}
	}
}

static void output_manifest(int manifest_fd, amd_fw_entry *fw_entry)
{
	struct amd_fw_header hdr;
	int blob_fd;
	ssize_t bytes;

	blob_fd = open(fw_entry->filename, O_RDONLY);
	if (blob_fd < 0) {
		fprintf(stderr, "Error opening file: %s: %s\n",
		       fw_entry->filename, strerror(errno));
		return;
	}

	bytes = read(blob_fd, &hdr, sizeof(hdr));
	if (bytes != sizeof(hdr)) {
		close(blob_fd);
		fprintf(stderr, "Error while reading %s\n", fw_entry->filename);
		return;
	}

	dprintf(manifest_fd, "type: 0x%02x ver:%02x.%02x.%02x.%02x\n",
	    fw_entry->type, hdr.version[3], hdr.version[2],
	    hdr.version[1], hdr.version[0]);

	close(blob_fd);

}

static void dump_blob_version(char *manifest_file, amd_fw_entry *fw_table)
{
	amd_fw_entry *index;
	int manifest_fd;

	manifest_fd = open(manifest_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (manifest_fd < 0) {
		fprintf(stderr, "Error opening file: %s: %s\n",
		       manifest_file, strerror(errno));
		return;
	}

	for (index = fw_table; index->type != AMD_FW_INVALID; index++) {
		if (!(index->filename))
			continue;

		if (index->generate_manifest == true)
			output_manifest(manifest_fd, index);
	}

	close(manifest_fd);
}

/* For debugging */
static void dump_psp_firmwares(amd_fw_entry *fw_table)
{
	amd_fw_entry *index;

	printf("PSP firmware components:\n");
	for (index = fw_table; index->type != AMD_FW_INVALID; index++) {
		if (index->type == AMD_PSP_FUSE_CHAIN)
			printf("  %2x: level=%x, subprog=%x, inst=%x\n",
				index->type, index->level, index->subprog, index->inst);
		else if (index->filename)
			printf("  %2x: level=%x, subprog=%x, inst=%x, %s\n",
				index->type, index->level, index->subprog, index->inst,
				index->filename);
	}
}

static void dump_bdt_firmwares(amd_bios_entry *fw_table)
{
	amd_bios_entry *index;

	printf("BIOS Directory Table (BDT) components:\n");
	for (index = fw_table; index->type != AMD_BIOS_INVALID; index++) {
		if (index->filename)
			printf("  %2x: level=%x, %s\n",
				index->type, index->level, index->filename);
	}
}

static void dump_image_addresses(context *ctx)
{
	printf("romsig offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->amd_romsig_ptr));
	printf("PSP L1 offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->pspdir));
	if (ctx->pspdir_bak != NULL)
		printf("PSP L1 backup offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->pspdir_bak));
	if (ctx->pspdir2 != NULL)
		printf("PSP L2(A) offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->pspdir2));
	if (ctx->ish_a_dir != NULL)
		printf("ISHA offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->ish_a_dir));
	if (ctx->ish_b_dir != NULL)
		printf("ISHB offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->ish_b_dir));
	if (ctx->pspdir2_b != NULL)
		printf("PSP L2B offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->pspdir2_b));
	if (ctx->biosdir != NULL)
		printf("BHD offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->biosdir));
	if (ctx->biosdir2 != NULL)
		printf("BHD L2(A) offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->biosdir2));
	if (ctx->biosdir2_b != NULL)
		printf("BHD L2B offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->biosdir2_b));
	if (ctx->psp_combo_dir != NULL)
		printf("PSP combo offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->psp_combo_dir));
	if (ctx->bhd_combo_dir != NULL)
		printf("BHD combo offset:%lx\n", BUFF_TO_RUN(*ctx, ctx->bhd_combo_dir));
}

static void integrate_psp_ab(context *ctx, psp_directory_table *pspdir,
		psp_directory_table *pspdir2, ish_directory_table *ish,
		amd_fw_type ab, enum platform soc_id)
{
	uint32_t count;
	uint32_t current_table_save;

	current_table_save = ctx->current_table;
	ctx->current_table = BUFF_TO_RUN_MODE(*ctx, pspdir, AMD_ADDR_REL_BIOS);
	count = pspdir->header.num_entries;
	assert_fw_entry(count, MAX_PSP_ENTRIES, ctx);
	pspdir->entries[count].type = (uint8_t)ab;
	pspdir->entries[count].subprog = 0;
	pspdir->entries[count].rsvd = 0;
	if (ish != NULL) {
		ish->pl2_location = BUFF_TO_RUN_MODE(*ctx, pspdir2, AMD_ADDR_REL_BIOS);
		ish->boot_priority = ab == AMD_FW_RECOVERYAB_A ? 0xFFFFFFFF : 1;
		ish->update_retry_count = 2;
		ish->glitch_retry_count = 0;
		ish->psp_id = get_psp_id(soc_id);
		ish->checksum = fletcher32(&ish->boot_priority,
				sizeof(ish_directory_table) - sizeof(uint32_t));
		pspdir->entries[count].addr =
				BUFF_TO_RUN_MODE(*ctx, ish, AMD_ADDR_REL_BIOS);
		pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, AMD_ADDR_REL_BIOS);
		pspdir->entries[count].size = TABLE_ALIGNMENT;
	} else {
		pspdir->entries[count].addr =
				BUFF_TO_RUN_MODE(*ctx, pspdir2, AMD_ADDR_REL_BIOS);
		pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, AMD_ADDR_REL_BIOS);
		pspdir->entries[count].size = _MAX(TABLE_L2_SIZE_MAX,
				pspdir2->header.num_entries *
				sizeof(psp_directory_entry) +
				sizeof(psp_directory_header));
	}

	count++;
	fill_dir_header(pspdir, count, ctx);
	ctx->current_table = current_table_save;
}

static void integrate_psp_levels(context *ctx,
				amd_cb_config *cb_config)
{
	uint32_t current_table_save;
	bool recovery_ab = cb_config->recovery_ab;
	unsigned int count;
	psp_directory_table *pspdir, *pspdir2, *pspdir2_b;
	bool use_only_a = (cb_config->soc_id == PLATFORM_PHOENIX); /* TODO: b:285390041 */

	pspdir = ctx->pspdir;
	pspdir2 = ctx->pspdir2;
	pspdir2_b = ctx->pspdir2_b;
	count = pspdir->header.num_entries;

	current_table_save = ctx->current_table;
	ctx->current_table = BUFF_TO_RUN_MODE(*ctx, pspdir, AMD_ADDR_REL_BIOS);
	if (recovery_ab && (pspdir2 != NULL)) {
		integrate_psp_ab(ctx, pspdir, pspdir2, ctx->ish_a_dir,
			AMD_FW_RECOVERYAB_A, cb_config->soc_id);
		if (pspdir2_b != NULL)
			integrate_psp_ab(ctx, pspdir, pspdir2_b, ctx->ish_b_dir,
				use_only_a ? AMD_FW_RECOVERYAB_A : AMD_FW_RECOVERYAB_B,
				cb_config->soc_id);
		else
			integrate_psp_ab(ctx, pspdir, pspdir2, ctx->ish_a_dir,
				use_only_a ? AMD_FW_RECOVERYAB_A : AMD_FW_RECOVERYAB_B,
				cb_config->soc_id);

		copy_psp_header(ctx->pspdir_bak, ctx->pspdir);
	} else if (pspdir2 != NULL) {
		assert_fw_entry(count, MAX_PSP_ENTRIES, ctx);
		pspdir->entries[count].type = AMD_FW_L2_PTR;
		pspdir->entries[count].subprog = 0;
		pspdir->entries[count].rsvd = 0;
		pspdir->entries[count].size = sizeof(pspdir2->header)
					+ pspdir2->header.num_entries
					* sizeof(psp_directory_entry);

		pspdir->entries[count].addr =
				BUFF_TO_RUN_MODE(*ctx, pspdir2, AMD_ADDR_REL_BIOS);
		pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, AMD_ADDR_REL_BIOS);
		count++;
		fill_dir_header(pspdir, count, ctx);
	}
	ctx->current_table = current_table_save;
}

static void integrate_psp_firmwares(context *ctx,
					amd_fw_entry *fw_table,
					uint32_t cookie,
					amd_cb_config *cb_config)
{
	ssize_t bytes;
	unsigned int i, count;
	int level;
	uint32_t size;
	psp_directory_table *pspdir = NULL;
	uint64_t addr;
	uint32_t current_table_save;
	bool recovery_ab = cb_config->recovery_ab;

	/* This function can create a primary table, a secondary table, or a
	 * flattened table which contains all applicable types.  These if-else
	 * statements infer what the caller intended.  If a 2nd-level cookie
	 * is passed, clearly a 2nd-level table is intended.  However, a
	 * 1st-level cookie may indicate level 1 or flattened.
	 */
	current_table_save = ctx->current_table;

	if (cookie == PSP_COOKIE) {
		if (!cb_config->combo_new_rab || ctx->combo_index == 0) {
			pspdir = new_psp_dir(ctx, cb_config->multi_level, cookie);
			ctx->pspdir = pspdir;
			if (recovery_ab)
				ctx->pspdir_bak = new_psp_dir(ctx, cb_config->multi_level, cookie);
		}
		/* The ISH tables are with PSP L1. */
		if (cb_config->need_ish && ctx->ish_a_dir == NULL)	/* Need ISH */
			ctx->ish_a_dir = new_ish_dir(ctx);
		if (cb_config->need_ish && ctx->ish_b_dir == NULL)	/* Need ISH */
			ctx->ish_b_dir = new_ish_dir(ctx);
	} else if (cookie == PSPL2_COOKIE) {
		if (ctx->pspdir2 == NULL) {
			pspdir = new_psp_dir(ctx, cb_config->multi_level, cookie);
			ctx->pspdir2 = pspdir;
		} else if (ctx->pspdir2_b == NULL) {
			pspdir = new_psp_dir(ctx, cb_config->multi_level, cookie);
			ctx->pspdir2_b = pspdir;
		}
	}
	if (pspdir == NULL) {
		goto out;
	}

	if (!cb_config->multi_level)
		level = PSP_BOTH;
	else if (cookie == PSPL2_COOKIE)
		level = PSP_LVL2;
	else if (cookie == PSP_COOKIE)
		level = PSP_LVL1;
	else
		level = PSP_BOTH;

	if (recovery_ab) {
		if (cookie == PSPL2_COOKIE)
			level = PSP_LVL2_AB;
		else if (cookie == PSP_COOKIE)
			level = PSP_LVL1_AB;
		else
			level = PSP_BOTH_AB;
	}

	ctx->current_table = BUFF_TO_RUN_MODE(*ctx, pspdir, AMD_ADDR_REL_BIOS);
	adjust_current_pointer(ctx, 0, TABLE_ALIGNMENT);

	for (i = 0, count = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (!(fw_table[i].level & level))
			continue;

		assert_fw_entry(count, MAX_PSP_ENTRIES, ctx);

		if (fw_table[i].type == AMD_TOKEN_UNLOCK) {
			if (!fw_table[i].other)
				continue;
			adjust_current_pointer(ctx, 0, ERASE_ALIGNMENT);
			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].size = 4096; /* TODO: doc? */
			pspdir->entries[count].addr = RUN_CURRENT(*ctx);
			pspdir->entries[count].address_mode = SET_ADDR_MODE_BY_TABLE(pspdir);
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			adjust_current_pointer(ctx, 4096, 0x100U);
			count++;
		} else if (fw_table[i].type == AMD_PSP_FUSE_CHAIN) {
			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].size = 0xFFFFFFFF;
			pspdir->entries[count].addr = fw_table[i].other;
			pspdir->entries[count].address_mode = 0;
			count++;
		} else if (fw_table[i].type == AMD_FW_PSP_NVRAM ||
			   fw_table[i].type == AMD_RPMC_NVRAM) {
			if (fw_table[i].filename == NULL) {
				if (fw_table[i].size == 0)
					continue;
				size = fw_table[i].size;
				addr = fw_table[i].dest;
				if (addr != ALIGN_UP(addr, ERASE_ALIGNMENT)) {
					fprintf(stderr,
						"Error: PSP NVRAM section not aligned with erase block size.\n\n");
					amdfwtool_cleanup(ctx);
					exit(1);
				}
			} else {
				adjust_current_pointer(ctx, 0, ERASE_ALIGNMENT);
				bytes = copy_blob(BUFF_CURRENT(*ctx),
						fw_table[i].filename, BUFF_ROOM(*ctx));
				if (bytes <= 0) {
					amdfwtool_cleanup(ctx);
					exit(1);
				}

				size = ALIGN_UP(bytes, ERASE_ALIGNMENT);
				addr = RUN_CURRENT(*ctx);
				adjust_current_pointer(ctx, bytes, BLOB_ERASE_ALIGNMENT);
			}

			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].size = size;
			pspdir->entries[count].addr = addr;

			pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, AMD_ADDR_REL_BIOS);

			count++;
		} else if (fw_table[i].filename != NULL) {
			if (fw_table[i].addr_signed) {
				pspdir->entries[count].addr =
					RUN_OFFSET(*ctx, fw_table[i].addr_signed);
				pspdir->entries[count].address_mode =
							SET_ADDR_MODE_BY_TABLE(pspdir);
				bytes = fw_table[i].file_size;
			} else {
				bytes = copy_blob(BUFF_CURRENT(*ctx),
						fw_table[i].filename, BUFF_ROOM(*ctx));
				if (bytes < 0) {
					amdfwtool_cleanup(ctx);
					exit(1);
				}
				pspdir->entries[count].addr = RUN_CURRENT(*ctx);
				pspdir->entries[count].address_mode =
							SET_ADDR_MODE_BY_TABLE(pspdir);
				adjust_current_pointer(ctx, bytes, BLOB_ALIGNMENT);
			}

			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].inst = fw_table[i].inst;
			pspdir->entries[count].size = (uint32_t)bytes;

			count++;
		} else {
			/* This APU doesn't have this firmware. */
		}
	}

	fill_dir_header(pspdir, count, ctx);
out:
	ctx->current_table = current_table_save;
}

static void add_psp_firmware_entry(context *ctx,
					psp_directory_table *pspdir,
					void *table, amd_fw_type type, uint32_t size)
{
	uint32_t count = pspdir->header.num_entries;
	uint32_t index;
	uint32_t current_table_save;

	current_table_save = ctx->current_table;
	ctx->current_table = BUFF_TO_RUN_MODE(*ctx, pspdir, AMD_ADDR_REL_BIOS);

	/* If there is an entry of "type", replace it. */
	for (index = 0; index < count; index++) {
		if (pspdir->entries[index].type == (uint8_t)type)
			break;
	}

	assert_fw_entry(count, MAX_PSP_ENTRIES, ctx);
	pspdir->entries[index].type = (uint8_t)type;
	pspdir->entries[index].subprog = 0;
	pspdir->entries[index].rsvd = 0;
	pspdir->entries[index].addr = BUFF_TO_RUN(*ctx, table);
	pspdir->entries[index].address_mode = SET_ADDR_MODE_BY_TABLE(pspdir);
	pspdir->entries[index].size = size;
	if (index == count)
		count++;

	fill_dir_header(pspdir, count, ctx);
	ctx->current_table = current_table_save;
}

static void *new_bios_dir(context *ctx, bool multi, uint32_t cookie)
{
	void *ptr;

	/*
	 * Force both onto boundary when multi.  Primary table is after
	 * updatable table, so alignment ensures primary can stay intact
	 * if secondary is reprogrammed.
	 */
	if (multi)
		adjust_current_pointer(ctx, 0, TABLE_ERASE_ALIGNMENT);
	else
		adjust_current_pointer(ctx, 0, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	((bios_directory_hdr *) ptr)->cookie = cookie;
	((bios_directory_hdr *) ptr)->additional_info = 0;
	((bios_directory_hdr *) ptr)->additional_info_fields.address_mode = ctx->address_mode;
	((bios_directory_hdr *) ptr)->additional_info_fields.spi_block_size = 1;
	((bios_directory_hdr *) ptr)->additional_info_fields.base_addr = 0;
	adjust_current_pointer(ctx,
		sizeof(bios_directory_hdr) + MAX_BIOS_ENTRIES * sizeof(bios_directory_entry),
		1);
	return ptr;
}

static int locate_bdt_bios(bios_directory_table *level,
					uint64_t *source, uint32_t *size)
{
	uint32_t i;

	*source = 0;
	*size = 0;
	if (!level)
		return 0;

	for (i = 0 ; i < level->header.num_entries ; i++) {
		if (level->entries[i].type == AMD_BIOS_BIN) {
			*source = level->entries[i].source;
			*size = level->entries[i].size;
			return 1;
		}
	}
	return 0;
}

static int have_bios_tables(amd_bios_entry *table)
{
	int i;

	for (i = 0 ; table[i].type != AMD_BIOS_INVALID; i++) {
		if (table[i].level & BDT_LVL1 && table[i].filename)
			return 1;
	}
	return 0;
}

int find_bios_entry(amd_bios_type type)
{
	int i;

	for (i = 0; amd_bios_table[i].type != AMD_BIOS_INVALID; i++) {
		if (amd_bios_table[i].type == type)
			return i;
	}
	return -1;
}

static void add_bios_apcb_bk_entry(bios_directory_table *biosdir, unsigned int idx,
						int inst, uint32_t size, uint64_t source)
{
	int i;

	for (i = 0; amd_bios_table[i].type != AMD_BIOS_INVALID; i++) {
		if (amd_bios_table[i].type == AMD_BIOS_APCB_BK &&
					amd_bios_table[i].inst == inst)
			break;
	}

	if (amd_bios_table[i].type != AMD_BIOS_APCB_BK)
		return;

	biosdir->entries[idx].type = amd_bios_table[i].type;
	biosdir->entries[idx].region_type = amd_bios_table[i].region_type;
	biosdir->entries[idx].dest = amd_bios_table[i].dest ?
					amd_bios_table[i].dest : (uint64_t)-1;
	biosdir->entries[idx].reset = amd_bios_table[i].reset;
	biosdir->entries[idx].copy = amd_bios_table[i].copy;
	biosdir->entries[idx].ro = amd_bios_table[i].ro;
	biosdir->entries[idx].compressed = amd_bios_table[i].zlib;
	biosdir->entries[idx].inst = amd_bios_table[i].inst;
	biosdir->entries[idx].subprog = amd_bios_table[i].subpr;
	biosdir->entries[idx].size = size;
	biosdir->entries[idx].source = source;
	biosdir->entries[idx].address_mode = SET_ADDR_MODE_BY_TABLE(biosdir);
}

static void integrate_bios_levels(context *ctx, amd_cb_config *cb_config)
{
	unsigned int count;
	uint32_t current_table_save;

	if (cb_config->recovery_ab) {
		add_psp_firmware_entry(ctx, ctx->pspdir2, ctx->biosdir2,
			AMD_FW_BIOS_TABLE, TABLE_L2_SIZE_MAX);
		if (ctx->pspdir2_b != NULL)
			add_psp_firmware_entry(ctx, ctx->pspdir2_b,
				ctx->biosdir2_b, AMD_FW_BIOS_TABLE,
				TABLE_L2_SIZE_MAX);
	} else if (ctx->biosdir2) {
		current_table_save = ctx->current_table;
		ctx->current_table = BUFF_TO_RUN_MODE(*ctx, ctx->biosdir, AMD_ADDR_REL_BIOS);
		count = ctx->biosdir->header.num_entries;
		assert_fw_entry(count, MAX_BIOS_ENTRIES, ctx);
		ctx->biosdir->entries[count].type = AMD_BIOS_L2_PTR;
		ctx->biosdir->entries[count].region_type = 0;
		ctx->biosdir->entries[count].size =
					+ MAX_BIOS_ENTRIES
					* sizeof(bios_directory_entry);
		ctx->biosdir->entries[count].source =
					BUFF_TO_RUN(*ctx, ctx->biosdir2);
		ctx->biosdir->entries[count].address_mode =
					SET_ADDR_MODE(ctx->biosdir, AMD_ADDR_REL_BIOS);
		ctx->biosdir->entries[count].subprog = 0;
		ctx->biosdir->entries[count].inst = 0;
		ctx->biosdir->entries[count].copy = 0;
		ctx->biosdir->entries[count].compressed = 0;
		ctx->biosdir->entries[count].dest = -1;
		ctx->biosdir->entries[count].reset = 0;
		ctx->biosdir->entries[count].ro = 0;
		count++;
		fill_dir_header(ctx->biosdir, count, ctx);
		ctx->current_table = current_table_save;
	}
}
static void integrate_bios_firmwares(context *ctx,
					amd_bios_entry *fw_table,
					uint32_t cookie,
					amd_cb_config *cb_config)
{
	ssize_t bytes;
	unsigned int i, count;
	int level;
	int apob_idx;
	uint32_t size;
	uint64_t source;
	uint32_t current_table_save;
	bios_directory_table *biosdir;

	biosdir = new_bios_dir(ctx, cb_config->multi_level, cookie);

	if (cookie == BHD_COOKIE)
		ctx->biosdir = biosdir;
	else if (cookie == BHDL2_COOKIE) {
		if (ctx->biosdir2 == NULL)
			ctx->biosdir2 = biosdir;
		else if (ctx->biosdir2_b == NULL)
			ctx->biosdir2_b = biosdir;
	}

	/* This function can create a primary table, a secondary table, or a
	 * flattened table which contains all applicable types.  These if-else
	 * statements infer what the caller intended.  If a 2nd-level cookie
	 * is passed, clearly a 2nd-level table is intended.  However, a
	 * 1st-level cookie may indicate level 1 or flattened.
	 */
	if (!cb_config->multi_level)
		level = BDT_BOTH;
	else if (cookie == BHDL2_COOKIE)
		level = BDT_LVL2;
	else if (cookie == BHD_COOKIE)
		level = BDT_LVL1;
	else
		level = BDT_BOTH;

	current_table_save = ctx->current_table;
	ctx->current_table = BUFF_TO_RUN_MODE(*ctx, biosdir, AMD_ADDR_REL_BIOS);
	adjust_current_pointer(ctx, 0, TABLE_ALIGNMENT);

	for (i = 0, count = 0; fw_table[i].type != AMD_BIOS_INVALID; i++) {
		if (!(fw_table[i].level & level))
			continue;
		if (fw_table[i].filename == NULL && (
				fw_table[i].type != AMD_BIOS_SIG &&
				fw_table[i].type != AMD_BIOS_APOB &&
				fw_table[i].type != AMD_BIOS_APOB_NV &&
				fw_table[i].type != AMD_BIOS_L2_PTR &&
				fw_table[i].type != AMD_BIOS_BIN &&
				fw_table[i].type != AMD_BIOS_PSP_SHARED_MEM))
			continue;

		/* BIOS Directory items may have additional requirements */

		/* SIG needs a size, else no choice but to skip */
		if (fw_table[i].type == AMD_BIOS_SIG && !fw_table[i].size)
			continue;

		/* Check APOB_NV requirements */
		if (fw_table[i].type == AMD_BIOS_APOB_NV) {
			if (!fw_table[i].size && !fw_table[i].src)
				continue; /* APOB_NV not used */
			if (fw_table[i].src && !fw_table[i].size) {
				fprintf(stderr, "Error: APOB NV address provided, but no size\n");
				amdfwtool_cleanup(ctx);
				exit(1);
			}
			/* If the APOB isn't used, APOB_NV isn't used either */
			apob_idx = find_bios_entry(AMD_BIOS_APOB);
			if (apob_idx < 0 || !fw_table[apob_idx].dest)
				continue; /* APOV NV not supported */
		}

		/* APOB_DATA needs destination */
		if (fw_table[i].type == AMD_BIOS_APOB && !fw_table[i].dest) {
			fprintf(stderr, "Error: APOB destination not provided\n");
			amdfwtool_cleanup(ctx);
			exit(1);
		}

		/* BIOS binary must have destination and uncompressed size.  If
		 * no filename given, then user must provide a source address.
		 */
		if (fw_table[i].type == AMD_BIOS_BIN) {
			if (!fw_table[i].dest || !fw_table[i].size) {
				fprintf(stderr, "Error: BIOS binary destination and uncompressed size are required\n");
				amdfwtool_cleanup(ctx);
				exit(1);
			}
			if (!fw_table[i].filename && !fw_table[i].src) {
				fprintf(stderr, "Error: BIOS binary assumed outside amdfw.rom but no source address given\n");
				amdfwtool_cleanup(ctx);
				exit(1);
			}
		}

		/* PSP_SHARED_MEM needs a destination and size */
		if (fw_table[i].type == AMD_BIOS_PSP_SHARED_MEM &&
				(!fw_table[i].dest || !fw_table[i].size))
			continue;
		assert_fw_entry(count, MAX_BIOS_ENTRIES, ctx);

		biosdir->entries[count].type = fw_table[i].type;
		biosdir->entries[count].region_type = fw_table[i].region_type;
		biosdir->entries[count].dest = fw_table[i].dest ?
					fw_table[i].dest : (uint64_t)-1;
		biosdir->entries[count].reset = fw_table[i].reset;
		biosdir->entries[count].copy = fw_table[i].copy;
		biosdir->entries[count].ro = fw_table[i].ro;
		biosdir->entries[count].compressed = fw_table[i].zlib;
		biosdir->entries[count].inst = fw_table[i].inst;
		biosdir->entries[count].subprog = fw_table[i].subpr;

		switch (fw_table[i].type) {
		case AMD_BIOS_SIG:
			/* Reserve size bytes within amdfw.rom */
			biosdir->entries[count].size = fw_table[i].size;
			biosdir->entries[count].source = RUN_CURRENT(*ctx);
			biosdir->entries[count].address_mode =
					SET_ADDR_MODE_BY_TABLE(biosdir);
			memset(BUFF_CURRENT(*ctx), 0xff,
							biosdir->entries[count].size);
			adjust_current_pointer(ctx, biosdir->entries[count].size, 0x100U);
			break;
		case AMD_BIOS_APOB:
			biosdir->entries[count].size = fw_table[i].size;
			biosdir->entries[count].source = fw_table[i].src;
			biosdir->entries[count].address_mode = SET_ADDR_MODE_BY_TABLE(biosdir);
			break;
		case AMD_BIOS_APOB_NV:
			if (has_apob_nv_quirk(cb_config->soc_id)) {
				/*
				 * once ROM3 mapping (>16MiB) is used on any SOC that
				 * has the apob quirk, this needs to be updated, since
				 * using an MMIO address is then not as simply as adding
				 * the SPI_ROM_BASE offset anymore.
				 */
				if (fw_table[i].src + fw_table[i].size >= 16*MiB) {
					fprintf(stderr,
						"APOB_NV location too high (0x%lx + 0x%lx)\n",
						fw_table[i].src, fw_table[i].size);
					amdfwtool_cleanup(ctx);
					exit(1);
				}
				biosdir->entries[count].source = fw_table[i].src + SPI_ROM_BASE; // convert to MMIO address
				biosdir->entries[count].address_mode = AMD_ADDR_PHYSICAL;
			} else {
				biosdir->entries[count].source = fw_table[i].src;
				biosdir->entries[count].address_mode =
					SET_ADDR_MODE(biosdir, AMD_ADDR_REL_BIOS);
			}
			biosdir->entries[count].size = fw_table[i].size;
			break;
		case AMD_BIOS_BIN:
			/* Don't make a 2nd copy, point to the same one */
			if ((level == BDT_LVL1 && locate_bdt_bios(ctx->biosdir2, &source, &size)) ||
				(level == BDT_LVL2 && locate_bdt_bios(ctx->biosdir, &source, &size))) {
				biosdir->entries[count].source = source;
				biosdir->entries[count].address_mode =
						SET_ADDR_MODE(biosdir, AMD_ADDR_REL_BIOS);
				biosdir->entries[count].size = size;
				break;
			}

			/* Level 2 and no copy found in level 1, or level 1 and no copy found in level 2 */
			biosdir->entries[count].source = fw_table[i].src;
			biosdir->entries[count].address_mode =
						SET_ADDR_MODE(biosdir, AMD_ADDR_REL_BIOS);
			biosdir->entries[count].dest = fw_table[i].dest;
			biosdir->entries[count].size = fw_table[i].size;

			if (!fw_table[i].filename)
				break;

			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes <= 0) {
				amdfwtool_cleanup(ctx);
				exit(1);
			}

			biosdir->entries[count].source =
				RUN_CURRENT(*ctx);
			biosdir->entries[count].address_mode =
				SET_ADDR_MODE_BY_TABLE(biosdir);

			adjust_current_pointer(ctx, bytes, 0x100U);
			break;
		case AMD_BIOS_PSP_SHARED_MEM:
			biosdir->entries[count].dest = fw_table[i].dest;
			biosdir->entries[count].size = fw_table[i].size;
			break;

		default: /* everything else is copied from input */
			if (fw_table[i].type == AMD_BIOS_APCB ||
					fw_table[i].type == AMD_BIOS_APCB_BK)
				adjust_current_pointer(ctx, 0, ERASE_ALIGNMENT);
			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes <= 0) {
				amdfwtool_cleanup(ctx);
				exit(1);
			}

			biosdir->entries[count].size = (uint32_t)bytes;
			biosdir->entries[count].source = RUN_CURRENT(*ctx);
			biosdir->entries[count].address_mode = SET_ADDR_MODE_BY_TABLE(biosdir);

			adjust_current_pointer(ctx, bytes, 0x100U);
			if (fw_table[i].type == AMD_BIOS_APCB && !cb_config->have_apcb_bk) {
				size = biosdir->entries[count].size;
				source = biosdir->entries[count].source;
				count++;
				add_bios_apcb_bk_entry(biosdir, count, fw_table[i].inst, size, source);
			}
			break;
		}

		count++;
	}

	fill_dir_header(biosdir, count, ctx);
	ctx->current_table = current_table_save;
}

static void add_combo_entry(void *combo_dir, void *dir, uint32_t combo_index,
			context *ctx, amd_cb_config *cb_config)
{
	psp_combo_directory *cdir = combo_dir;
	assert_fw_entry(combo_index, MAX_COMBO_ENTRIES, ctx);
	/* 0 -Compare PSP ID, 1 -Compare chip family ID */
	cdir->entries[combo_index].id_sel = 0;
	cdir->entries[combo_index].id = get_psp_id(cb_config->soc_id);
	cdir->entries[combo_index].lvl2_addr =
		BUFF_TO_RUN_MODE(*ctx, dir, AMD_ADDR_REL_BIOS);

	fill_dir_header(combo_dir, combo_index + 1, ctx);
}

static int set_efs_table(uint8_t soc_id, amd_cb_config *cb_config,
			 embedded_firmware *amd_romsig)
{
	if ((cb_config->efs_spi_readmode == 0xFF) || (cb_config->efs_spi_speed == 0xFF)) {
		fprintf(stderr, "Error: EFS read mode and SPI speed must be set\n");
		return 1;
	}

	/* amd_romsig->efs_gen introduced after RAVEN/PICASSO.
	 * Leave as 0xffffffff for first gen */
	if (cb_config->second_gen) {
		amd_romsig->efs_gen.gen = EFS_SECOND_GEN;
		amd_romsig->efs_gen.reserved = 0;
	} else {
		amd_romsig->efs_gen.gen = EFS_BEFORE_SECOND_GEN;
		amd_romsig->efs_gen.reserved = ~0;
	}

	switch (soc_id) {
	case PLATFORM_CARRIZO:
	case PLATFORM_STONEYRIDGE:
		amd_romsig->spi_readmode_f15_mod_60_6f = cb_config->efs_spi_readmode;
		amd_romsig->fast_speed_new_f15_mod_60_6f = cb_config->efs_spi_speed;
		break;
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
		amd_romsig->spi_readmode_f17_mod_00_2f = cb_config->efs_spi_readmode;
		amd_romsig->spi_fastspeed_f17_mod_00_2f = cb_config->efs_spi_speed;
		switch (cb_config->efs_spi_micron_flag) {
		case 0:
			amd_romsig->qpr_dummy_cycle_f17_mod_00_2f = 0xff;
			break;
		case 1:
			amd_romsig->qpr_dummy_cycle_f17_mod_00_2f = 0xa;
			break;
		default:
			fprintf(stderr, "Error: EFS Micron flag must be correctly set.\n\n");
			return 1;
		}
		break;
	case PLATFORM_RENOIR:
	case PLATFORM_LUCIENNE:
	case PLATFORM_CEZANNE:
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_GLINDA:
	case PLATFORM_GENOA:
	case PLATFORM_FAEGAN:
		amd_romsig->spi_readmode_f17_mod_30_3f = cb_config->efs_spi_readmode;
		amd_romsig->spi_fastspeed_f17_mod_30_3f = cb_config->efs_spi_speed;
		switch (cb_config->efs_spi_micron_flag) {
		case 0:
			amd_romsig->micron_detect_f17_mod_30_3f = 0xff;
			break;
		case 1:
			amd_romsig->micron_detect_f17_mod_30_3f = 0xaa;
			break;
		case 2:
			amd_romsig->micron_detect_f17_mod_30_3f = 0x55;
			break;
		default:
			fprintf(stderr, "Error: EFS Micron flag must be correctly set.\n\n");
			return 1;
		}
		break;
	case PLATFORM_UNKNOWN:
	default:
		fprintf(stderr, "Error: Invalid SOC name.\n\n");
		return 1;
	}
	return 0;
}

void open_process_config(char *config, amd_cb_config *cb_config)
{
	FILE *config_handle;

	if (config) {
		config_handle = fopen(config, "r");
		if (config_handle == NULL) {
			fprintf(stderr, "Can not open file %s for reading: %s\n",
				config, strerror(errno));
			exit(1);
		}
		if (process_config(config_handle, cb_config) == 0) {
			fprintf(stderr, "Configuration file %s parsing error\n",
					config);
			fclose(config_handle);
			exit(1);
		}
		fclose(config_handle);
	}

	/* For debug. */
	if (cb_config->debug) {
		dump_psp_firmwares(amd_psp_fw_table);
		dump_bdt_firmwares(amd_bios_table);
	}
}

static bool is_initial_alignment_required(enum platform soc_id)
{
	switch (soc_id) {
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_GLINDA:
	case PLATFORM_FAEGAN:
		return false;
	default:
		return true;
	}
}

static bool needs_new_combo_layout(enum platform soc_id)
{
	return needs_ish(soc_id);
}

int main(int argc, char **argv)
{
	int retval = 0;
	int targetfd;
	context ctx = { 0 };
	uint32_t romsig_offset;
	amd_cb_config cb_config = {
		.efs_spi_readmode = 0xff, .efs_spi_speed = 0xff, .efs_spi_micron_flag = 0xff
	};

	ctx.current_pointer_saved = 0xFFFFFFFF;

	retval = amdfwtool_getopt(argc, argv, &cb_config, &ctx);

	if (retval) {
		return retval;
	}

	if (cb_config.use_combo) {
		ctx.amd_psp_fw_table_clean = malloc(sizeof(amd_psp_fw_table));
		ctx.amd_bios_table_clean = malloc(sizeof(amd_bios_table));
		memcpy(ctx.amd_psp_fw_table_clean, amd_psp_fw_table, sizeof(amd_psp_fw_table));
		memcpy(ctx.amd_bios_table_clean, amd_bios_table, sizeof(amd_bios_table));
	}

	open_process_config(cb_config.config, &cb_config);

	if (cb_config.use_combo && needs_new_combo_layout(cb_config.soc_id))
		cb_config.combo_new_rab = true;

	ctx.rom = malloc(ctx.rom_size);
	if (!ctx.rom) {
		fprintf(stderr, "Error: Failed to allocate memory\n");
		return 1;
	}
	memset(ctx.rom, 0xFF, ctx.rom_size);

	romsig_offset = cb_config.efs_location ? cb_config.efs_location : AMD_ROMSIG_OFFSET;
	set_current_pointer(&ctx, romsig_offset);

	ctx.amd_romsig_ptr = BUFF_OFFSET(ctx, romsig_offset);
	ctx.amd_romsig_ptr->signature = EMBEDDED_FW_SIGNATURE;
	ctx.amd_romsig_ptr->imc_entry = 0;
	ctx.amd_romsig_ptr->gec_entry = 0;
	ctx.amd_romsig_ptr->xhci_entry = 0;

	if (cb_config.soc_id != PLATFORM_UNKNOWN) {
		retval = set_efs_table(cb_config.soc_id, &cb_config, ctx.amd_romsig_ptr);
		if (retval) {
			fprintf(stderr, "ERROR: Failed to initialize EFS table!\n");
			return retval;
		}
	} else {
		fprintf(stderr, "WARNING: No SOC name specified.\n");
	}

	if (cb_config.need_ish)
		ctx.address_mode = AMD_ADDR_REL_TAB;
	else if (cb_config.second_gen)
		ctx.address_mode = AMD_ADDR_REL_BIOS;
	else
		ctx.address_mode = AMD_ADDR_PHYSICAL;

	if (cb_config.efs_location != cb_config.body_location)
		set_current_pointer(&ctx, cb_config.body_location);
	else
		set_current_pointer(&ctx, romsig_offset + sizeof(embedded_firmware));

	integrate_firmwares(&ctx, ctx.amd_romsig_ptr, amd_fw_table);

	if (is_initial_alignment_required(cb_config.soc_id)) {
		/* TODO: Check for older platforms. */
		adjust_current_pointer(&ctx, 0, 0x10000U);
	}
	ctx.current_table = 0;

	/* If the tool is invoked with command-line options to keep the signed PSP
	   binaries separate, process the signed binaries first. */
	if (cb_config.signed_output_file && cb_config.signed_start_addr)
		process_signed_psp_firmwares(cb_config.signed_output_file,
				amd_psp_fw_table,
				cb_config.signed_start_addr,
				cb_config.soc_id);

	if (cb_config.use_combo && !cb_config.combo_new_rab) {
		ctx.psp_combo_dir = new_combo_dir(&ctx, PSP2_COOKIE);

		adjust_current_pointer(&ctx, 0, 0x1000U);

		if (!cb_config.recovery_ab)
			ctx.bhd_combo_dir = new_combo_dir(&ctx, BHD2_COOKIE);
	}

	ctx.combo_index = 0;
	if (cb_config.config)
		cb_config.combo_config[0] = cb_config.config;

	do {
		if (cb_config.use_combo && cb_config.debug)
			printf("Processing %dth combo entry\n", ctx.combo_index);

		/* The pspdir level 1 is special. For new combo layout, all the combo entries
		   share one pspdir L1. It should not be cleared at each iteration. */
		if (!cb_config.combo_new_rab || ctx.combo_index == 0) {
			ctx.pspdir = NULL;
			ctx.pspdir_bak = NULL;
		}
		ctx.pspdir2 = NULL;
		ctx.pspdir2_b = NULL;
		ctx.biosdir = NULL;
		ctx.biosdir2 = NULL;
		ctx.biosdir2_b = NULL;
		ctx.ish_a_dir = NULL;
		ctx.ish_b_dir = NULL;
		/* for non-combo image, combo_config[0] == config, and
		 *  it already is processed.  Actually "combo_index >
		 *  0" is enough. Put both of them here to make sure
		 *  and make it clear this will not affect non-combo
		 *  case.
		 */
		if (cb_config.use_combo && ctx.combo_index > 0) {
			/* Restore the table as clean data. */
			memcpy(amd_psp_fw_table, ctx.amd_psp_fw_table_clean,
				sizeof(amd_psp_fw_table));
			memcpy(amd_bios_table, ctx.amd_bios_table_clean,
				sizeof(amd_bios_table));
			assert_fw_entry(ctx.combo_index, MAX_COMBO_ENTRIES, &ctx);
			open_process_config(cb_config.combo_config[ctx.combo_index], &cb_config);

			/* In most cases, the address modes are same. */
			if (cb_config.need_ish)
				ctx.address_mode = AMD_ADDR_REL_TAB;
			else if (cb_config.second_gen)
				ctx.address_mode = AMD_ADDR_REL_BIOS;
			else
				ctx.address_mode = AMD_ADDR_PHYSICAL;

			register_apcb_combo(&cb_config, ctx.combo_index, &ctx);
		}

		if (cb_config.multi_level) {
			/* PSP L1 */
			integrate_psp_firmwares(&ctx,
					amd_psp_fw_table, PSP_COOKIE, &cb_config);
			/* PSP L2 & BIOS L2 (if AB recovery) */
			integrate_psp_firmwares(&ctx,
						amd_psp_fw_table, PSPL2_COOKIE, &cb_config);
			if (cb_config.recovery_ab) {
				integrate_bios_firmwares(&ctx,
						amd_bios_table, BHDL2_COOKIE, &cb_config);
				if (!cb_config.recovery_ab_single_copy) {
					integrate_psp_firmwares(&ctx,
						amd_psp_fw_table, PSPL2_COOKIE, &cb_config);
					integrate_bios_firmwares(&ctx,
						amd_bios_table, BHDL2_COOKIE, &cb_config);
				}
				integrate_bios_levels(&ctx, &cb_config);
			}
			integrate_psp_levels(&ctx, &cb_config);
		} else {
			/* flat: PSP 1 cookie and no pointer to 2nd table */
			integrate_psp_firmwares(&ctx,
					amd_psp_fw_table, PSP_COOKIE, &cb_config);
		}

		if (!cb_config.use_combo || (cb_config.combo_new_rab && ctx.combo_index == 0)) {
			/* For new combo layout, there is only 1 PSP level 1 directory. */
			fill_psp_directory_to_efs(ctx.amd_romsig_ptr, ctx.pspdir, &ctx, &cb_config);
			fill_psp_bak_directory_to_efs(ctx.amd_romsig_ptr, ctx.pspdir_bak, &ctx, &cb_config);
		} else if (cb_config.use_combo && !cb_config.combo_new_rab) {
			fill_psp_directory_to_efs(ctx.amd_romsig_ptr, ctx.psp_combo_dir, &ctx, &cb_config);
			add_combo_entry(ctx.psp_combo_dir, ctx.pspdir, ctx.combo_index, &ctx, &cb_config);
		}

		if (have_bios_tables(amd_bios_table) && !cb_config.recovery_ab) {
			if (cb_config.multi_level) {
				integrate_bios_firmwares(&ctx,
						amd_bios_table, BHD_COOKIE, &cb_config);
				integrate_bios_firmwares(&ctx,
						amd_bios_table, BHDL2_COOKIE, &cb_config);
				integrate_bios_levels(&ctx, &cb_config);
			} else {
				/* flat: BHD1 cookie and no pointer to 2nd table */
				integrate_bios_firmwares(&ctx,
						amd_bios_table, BHD_COOKIE, &cb_config);
			}
			if (!cb_config.use_combo) {
				fill_bios_directory_to_efs(ctx.amd_romsig_ptr, ctx.biosdir,
					&ctx, &cb_config);
			} else if (ctx.bhd_combo_dir != NULL) {
				/* In recovery A/B mode, there isn't a BHD combo directory.
				 * Instead, the BIOS tables level 2 are linked by PSP tables.
				 */
				fill_bios_directory_to_efs(ctx.amd_romsig_ptr, ctx.bhd_combo_dir,
					&ctx, &cb_config);
				add_combo_entry(ctx.bhd_combo_dir, ctx.biosdir, ctx.combo_index, &ctx, &cb_config);
			}
		}
		if (cb_config.debug)
			dump_image_addresses(&ctx);
	} while (cb_config.use_combo && ++ctx.combo_index < MAX_COMBO_ENTRIES &&
					cb_config.combo_config[ctx.combo_index] != NULL);

	targetfd = open(cb_config.output, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (targetfd >= 0) {
		uint32_t offset = cb_config.efs_location;
		uint32_t bytes = cb_config.efs_location == cb_config.body_location ?
				ctx.current - offset : sizeof(embedded_firmware);
		uint32_t ret_bytes;

		ret_bytes = write_from_buf_to_file(targetfd, BUFF_OFFSET(ctx, offset), bytes);
		if (bytes != ret_bytes) {
			fprintf(stderr, "Error: Writing to file %s failed\n", cb_config.output);
			retval = 1;
		}
		close(targetfd);
	} else {
		fprintf(stderr, "Error: could not open file: %s\n", cb_config.output);
		retval = 1;
	}

	if (cb_config.efs_location != cb_config.body_location) {
		ssize_t bytes;

		bytes = write_body(cb_config.output, BUFF_OFFSET(ctx, cb_config.body_location),
			ctx.current - cb_config.body_location);
		if (bytes != ctx.current - cb_config.body_location) {
			fprintf(stderr, "Error: Writing body\n");
			retval = 1;
		}
	}

	if (cb_config.manifest_file) {
		dump_blob_version(cb_config.manifest_file, amd_psp_fw_table);
	}

	amdfwtool_cleanup(&ctx);
	return retval;
}
