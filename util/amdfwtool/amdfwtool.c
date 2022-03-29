/* SPDX-License-Identifier: GPL-2.0-only */

/*
 *  ROMSIG At ROMBASE + 0x20000:
 *  0            4               8                C
 *  +------------+---------------+----------------+------------+
 *  | 0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
 *  +------------+---------------+----------------+------------+
 *  | PSPDIR ADDR|PSPDIR ADDR    |<-- Field 0x14 could be either
 *  +------------+---------------+   2nd PSP directory or PSP COMBO directory
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
 *  |             Other PSP Firmware                           |
 *  |                                                          |
 *  |                                                          |
 *  +------------+---------------+----------------+------------+
 *
 *  PSP Combo directory
 *  +------------+---------------+----------------+------------+
 *  | 'PSP2'     | Fletcher      |    Count       |Look up mode|
 *  +------------+---------------+----------------+------------+
 *  |            R e s e r v e d                               |
 *  +------------+---------------+----------------+------------+
 *  | ID-Sel     | PSP ID        |   PSPDIR ADDR  |            | 2nd PSP directory
 *  +------------+---------------+----------------+------------+
 *  | ID-Sel     | PSP ID        |   PSPDIR ADDR  |            | 3rd PSP directory
 *  +------------+---------------+----------------+------------+
 *  |                                                          |
 *  |        Other PSP                                         |
 *  |                                                          |
 *  +------------+---------------+----------------+------------+
 *
 */

#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <stdint.h>

#include "amdfwtool.h"

#define AMD_ROMSIG_OFFSET	0x20000
#define MIN_ROM_KB		256

#define ALIGN(val, by) (((val) + (by) - 1) & ~((by) - 1))
#define _MAX(A, B) (((A) > (B)) ? (A) : (B))
#define ERASE_ALIGNMENT 0x1000U
#define TABLE_ALIGNMENT 0x1000U
#define BLOB_ALIGNMENT 0x100U
#define TABLE_ERASE_ALIGNMENT _MAX(TABLE_ALIGNMENT, ERASE_ALIGNMENT)
#define BLOB_ERASE_ALIGNMENT _MAX(BLOB_ALIGNMENT, ERASE_ALIGNMENT)

#define DEFAULT_SOFT_FUSE_CHAIN "0x1"

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
 * regardless of the type of directory used.  The --combo-capable
 * argument enforces this placement.
 *
 * TODO: Future work may require fully implementing the PSP_COMBO feature.
 */
#define PSP_COMBO 0

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

static void usage(void)
{
	printf("amdfwtool: Create AMD Firmware combination\n");
	printf("Usage: amdfwtool [options] --flashsize <size> --output <filename>\n");
	printf("--xhci <FILE>                  Add XHCI blob\n");
	printf("--imc <FILE>                   Add IMC blob\n");
	printf("--gec <FILE>                   Add GEC blob\n");

	printf("\nPSP options:\n");
	printf("--combo-capable                Place PSP directory pointer at Embedded\n");
	printf("                               Firmware\n");
	printf("                               offset able to support combo directory\n");
	printf("--multilevel                   Generate primary and secondary tables\n");
	printf("--nvram <FILE>                 Add nvram binary\n");
	printf("--soft-fuse                    Set soft fuse\n");
	printf("--token-unlock                 Set token unlock\n");
	printf("--whitelist                    Set if there is a whitelist\n");
	printf("--use-pspsecureos              Set if psp secure OS is needed\n");
	printf("--load-mp2-fw                  Set if load MP2 firmware\n");
	printf("--load-s0i3                    Set if load s0i3 firmware\n");
	printf("--verstage <FILE>              Add verstage\n");
	printf("--verstage_sig                 Add verstage signature\n");
	printf("--recovery-ab                  Use the recovery A/B layout\n");
	printf("\nBIOS options:\n");
	printf("--instance <number>            Sets instance field for the next BIOS\n");
	printf("                               firmware\n");
	printf("--apcb <FILE>                  Add AGESA PSP customization block\n");
	printf("--apob-base <HEX_VAL>          Destination for AGESA PSP output block\n");
	printf("--apob-nv-base <HEX_VAL>       Location of S3 resume data\n");
	printf("--apob-nv-size <HEX_VAL>       Size of S3 resume data\n");
	printf("--ucode <FILE>                 Add microcode patch\n");
	printf("--bios-bin <FILE>              Add compressed image; auto source address\n");
	printf("--bios-bin-src <HEX_VAL>       Address in flash of source if -V not used\n");
	printf("--bios-bin-dest <HEX_VAL>      Destination for uncompressed BIOS\n");
	printf("--bios-uncomp-size <HEX>       Uncompressed size of BIOS image\n");
	printf("--output <filename>            output filename\n");
	printf("--flashsize <HEX_VAL>          ROM size in bytes\n");
	printf("                               size must be larger than %dKB\n",
		MIN_ROM_KB);
	printf("                               and must a multiple of 1024\n");
	printf("--location                     Location of Directory\n");
	printf("--anywhere                     Use any 64-byte aligned addr for Directory\n");
	printf("--sharedmem                    Location of PSP/FW shared memory\n");
	printf("--sharedmem-size               Maximum size of the PSP/FW shared memory\n");
	printf("                               area\n");
	printf("--soc-name <socname>           Specify SOC name. Supported names are\n");
	printf("                               Stoneyridge, Raven, Picasso, Renoir, Cezanne\n");
	printf("                               or Lucienne\n");
	printf("\nEmbedded Firmware Structure options used by the PSP:\n");
	printf("--spi-speed <HEX_VAL>          SPI fast speed to place in EFS Table\n");
	printf("                               0x0 66.66Mhz\n");
	printf("                               0x1 33.33MHz\n");
	printf("                               0x2 22.22MHz\n");
	printf("                               0x3 16.66MHz\n");
	printf("                               0x4 100MHz\n");
	printf("                               0x5 800KHz\n");
	printf("--spi-read-mode <HEX_VAL>      SPI read mode to place in EFS Table\n");
	printf("                               0x0 Normal Read (up to 33M)\n");
	printf("                               0x1 Reserved\n");
	printf("                               0x2 Dual IO (1-1-2)\n");
	printf("                               0x3 Quad IO (1-1-4)\n");
	printf("                               0x4 Dual IO (1-2-2)\n");
	printf("                               0x5 Quad IO (1-4-4)\n");
	printf("                               0x6 Normal Read (up to 66M)\n");
	printf("                               0x7 Fast Read\n");
	printf("--spi-micron-flag <HEX_VAL>    Micron SPI part support for RV and later SOC\n");
	printf("                               0x0 Micron parts are not used\n");
	printf("                               0x1 Micron parts are always used\n");
	printf("                               0x2 Micron parts optional, this option is only\n");
	printf("                                   supported with RN/LCN SOC\n");
	printf("\nGeneral options:\n");
	printf("-c|--config <config file>      Config file\n");
	printf("-d|--debug                     Print debug message\n");
	printf("-l|--list                      List out the firmware files\n");
	printf("-h|--help                      Show this help\n");
}

amd_fw_entry amd_psp_fw_table[] = {
	{ .type = AMD_FW_PSP_PUBKEY, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_BOOTLOADER, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_RECOVERY, .level = PSP_LVL1 },
	{ .type = AMD_FW_PSP_RTM_PUBKEY, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SECURED_OS, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_NVRAM, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SECURED_DEBUG, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_TRUSTLETS, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_TRUSTLETKEY, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMUSCS, .level = PSP_BOTH },
	{ .type = AMD_PSP_FUSE_CHAIN, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_DEBUG_UNLOCK, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_HW_IPCFG, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_WRAPPED_IKEK, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_TOKEN_UNLOCK, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SEC_GASKET, .subprog = 0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SEC_GASKET, .subprog = 2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_SEC_GASKET, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_MP2_FW, .subprog = 2, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_MP2_FW, .subprog = 1, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_MP2_FW, .subprog = 0, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_DRIVER_ENTRIES, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_KVM_IMAGE, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_S0I3_DRIVER, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_VBIOS_BTLOADER, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_TOS_SEC_POLICY, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_USB_PHY, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_DRTM_TA, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_KEYDB_BL, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_KEYDB_TOS, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_SPL, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_DMCU_ERAM, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_DMCU_ISR, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_MSMU, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_DMCUB, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_SPIROM_CFG, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_RPMC_NVRAM, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_BOOTLOADER_AB, .level = PSP_LVL2 | PSP_LVL2_AB },
	{ .type = AMD_ABL0, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL2, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL3, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL4, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL5, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL6, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_ABL7, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 1, .level = PSP_BOTH | PSP_LVL2_AB },
	{ .type = AMD_FW_PSP_WHITELIST, .level = PSP_LVL2 },
	{ .type = AMD_FW_PSP_VERSTAGE, .level = PSP_BOTH | PSP_BOTH_AB },
	{ .type = AMD_FW_VERSTAGE_SIG, .level = PSP_BOTH | PSP_BOTH_AB },
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
			.reset = 1, .copy = 1, .zlib = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_APOB_NV, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_PMUI, .inst = 1, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 1, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 2, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 2, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 4, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 4, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 1, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 1, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 2, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 2, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 4, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 4, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_UCODE, .inst = 0, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 1, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 2, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_MP2_CFG, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_PSP_SHARED_MEM, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_INVALID },
};


#define MAX_BIOS_ENTRIES 0x2f

typedef struct _context {
	char *rom;		/* target buffer, size of flash device */
	uint32_t rom_size;	/* size of flash device */
	uint32_t address_mode;	/* 0:abs address; 1:relative to flash; 2: relative to table */
	uint32_t current;	/* pointer within flash & proxy buffer */
	uint32_t current_table;
} context;

#define ADDRESS_MODE_0_PHY		0
#define ADDRESS_MODE_1_REL_BIOS		1
#define ADDRESS_MODE_2_REL_TAB		2
#define ADDRESS_MODE_3_REL_SLOT		3

#define RUN_BASE(ctx) (0xFFFFFFFF - (ctx).rom_size + 1)
#define RUN_OFFSET_MODE(ctx, offset, mode) \
		((mode) == ADDRESS_MODE_0_PHY ? RUN_BASE(ctx) + (offset) :	\
		((mode) == ADDRESS_MODE_1_REL_BIOS ? (offset) :	\
		((mode) == ADDRESS_MODE_2_REL_TAB ? (offset) - ctx.current_table : (offset))))
#define RUN_OFFSET(ctx, offset) RUN_OFFSET_MODE((ctx), (offset), (ctx).address_mode)
#define RUN_TO_OFFSET(ctx, run) ((ctx).address_mode == ADDRESS_MODE_0_PHY ?	\
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
/* Only set the address mode in entry if the table is mode 2. */
#define SET_ADDR_MODE(table, mode) \
		((table)->header.additional_info_fields.address_mode ==	\
		ADDRESS_MODE_2_REL_TAB ? (mode) : 0)
#define SET_ADDR_MODE_BY_TABLE(table) \
		SET_ADDR_MODE((table), (table)->header.additional_info_fields.address_mode)

void assert_fw_entry(uint32_t count, uint32_t max, context *ctx)
{
	if (count >= max) {
		fprintf(stderr, "Error: BIOS entries (%d) exceeds max allowed items "
			"(%d)\n", count, max);
		free(ctx->rom);
		exit(1);
	}
}

static void *new_psp_dir(context *ctx, int multi)
{
	void *ptr;

	/*
	 * Force both onto boundary when multi.  Primary table is after
	 * updatable table, so alignment ensures primary can stay intact
	 * if secondary is reprogrammed.
	 */
	if (multi)
		ctx->current = ALIGN(ctx->current, TABLE_ERASE_ALIGNMENT);
	else
		ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);

	ptr = BUFF_CURRENT(*ctx);
	((psp_directory_header *)ptr)->num_entries = 0;
	((psp_directory_header *)ptr)->additional_info = 0;
	((psp_directory_header *)ptr)->additional_info_fields.address_mode = ctx->address_mode;
	ctx->current += sizeof(psp_directory_header)
			+ MAX_PSP_ENTRIES * sizeof(psp_directory_entry);
	return ptr;
}

static void *new_ish_dir(context *ctx)
{
	void *ptr;
	ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	ctx->current += TABLE_ALIGNMENT;
	return ptr;
}

#if PSP_COMBO
static void *new_combo_dir(context *ctx)
{
	void *ptr;

	ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	ctx->current += sizeof(psp_combo_header)
			+ MAX_COMBO_ENTRIES * sizeof(psp_combo_entry);
	return ptr;
}
#endif

static void fill_dir_header(void *directory, uint32_t count, uint32_t cookie, context *ctx)
{
	psp_combo_directory *cdir = directory;
	psp_directory_table *dir = directory;
	bios_directory_table *bdir = directory;
	uint32_t table_size = 0;

	if (!count)
		return;
	if (ctx == NULL || directory == NULL) {
		fprintf(stderr, "Calling %s with NULL pointers\n", __func__);
		return;
	}

	/* The table size needs to be 0x1000 aligned. So align the end of table. */
	ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);

	switch (cookie) {
	case PSP2_COOKIE:
		/* caller is responsible for lookup mode */
		cdir->header.cookie = cookie;
		cdir->header.num_entries = count;
		cdir->header.reserved[0] = 0;
		cdir->header.reserved[1] = 0;
		/* checksum everything that comes after the Checksum field */
		cdir->header.checksum = fletcher32(&cdir->header.num_entries,
					count * sizeof(psp_combo_entry)
					+ sizeof(cdir->header.num_entries)
					+ sizeof(cdir->header.lookup)
					+ 2 * sizeof(cdir->header.reserved[0]));
		break;
	case PSP_COOKIE:
	case PSPL2_COOKIE:
		table_size = ctx->current - ctx->current_table;
		if ((table_size % TABLE_ALIGNMENT) != 0) {
			fprintf(stderr, "The PSP table size should be 4K aligned\n");
			exit(1);
		}
		dir->header.cookie = cookie;
		dir->header.num_entries = count;
		dir->header.additional_info_fields.dir_size = table_size / TABLE_ALIGNMENT;
		dir->header.additional_info_fields.spi_block_size = 1;
		dir->header.additional_info_fields.base_addr = 0;
		/* checksum everything that comes after the Checksum field */
		dir->header.checksum = fletcher32(&dir->header.num_entries,
					count * sizeof(psp_directory_entry)
					+ sizeof(dir->header.num_entries)
					+ sizeof(dir->header.additional_info));
		break;
	case BDT1_COOKIE:
	case BDT2_COOKIE:
		table_size = ctx->current - ctx->current_table;
		if ((table_size % TABLE_ALIGNMENT) != 0) {
			fprintf(stderr, "The BIOS table size should be 4K aligned\n");
			exit(1);
		}
		bdir->header.cookie = cookie;
		bdir->header.num_entries = count;
		bdir->header.additional_info_fields.dir_size = table_size / TABLE_ALIGNMENT;
		bdir->header.additional_info_fields.spi_block_size = 1;
		bdir->header.additional_info_fields.base_addr = 0;
		/* checksum everything that comes after the Checksum field */
		bdir->header.checksum = fletcher32(&bdir->header.num_entries,
					count * sizeof(bios_directory_entry)
					+ sizeof(bdir->header.num_entries)
					+ sizeof(bdir->header.additional_info));
		break;
	}

}

static ssize_t copy_blob(void *dest, const char *src_file, size_t room)
{
	int fd;
	struct stat fd_stat;
	ssize_t bytes;

	fd = open(src_file, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error opening file: %s: %s\n",
		       src_file, strerror(errno));
		return -1;
	}

	if (fstat(fd, &fd_stat)) {
		fprintf(stderr, "fstat error: %s\n", strerror(errno));
		close(fd);
		return -2;
	}

	if ((size_t)fd_stat.st_size > room) {
		fprintf(stderr, "Error: %s will not fit.  Exiting.\n", src_file);
		close(fd);
		return -3;
	}

	bytes = read(fd, dest, (size_t)fd_stat.st_size);
	close(fd);
	if (bytes != (ssize_t)fd_stat.st_size) {
		fprintf(stderr, "Error while reading %s\n", src_file);
		return -4;
	}

	return bytes;
}

enum platform {
	PLATFORM_UNKNOWN,
	PLATFORM_STONEYRIDGE,
	PLATFORM_RAVEN,
	PLATFORM_PICASSO,
	PLATFORM_RENOIR,
	PLATFORM_CEZANNE,
	PLATFORM_MENDOCINO,
	PLATFORM_LUCIENNE,
	PLATFORM_SABRINA,
};

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
	case PLATFORM_SABRINA:
		psp_id = 0xBC0D0900;
		break;
	case PLATFORM_STONEYRIDGE:
		psp_id = 0x10220B00;
		break;
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

	ctx->current += sizeof(embedded_firmware);
	ctx->current = ALIGN(ctx->current, BLOB_ALIGNMENT);

	for (i = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (fw_table[i].filename != NULL) {
			switch (fw_table[i].type) {
			case AMD_FW_IMC:
				ctx->current = ALIGN(ctx->current, 0x10000U);
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
				free(ctx->rom);
				exit(1);
			}

			ctx->current = ALIGN(ctx->current + bytes,
							BLOB_ALIGNMENT);
		}
	}
}

/* For debugging */
static void dump_psp_firmwares(amd_fw_entry *fw_table)
{
	amd_fw_entry *index;

	printf("PSP firmware components:");
	for (index = fw_table; index->type != AMD_FW_INVALID; index++) {
		if (index->filename)
			printf("  %2x: %s\n", index->type, index->filename);
	}
}

static void dump_bdt_firmwares(amd_bios_entry *fw_table)
{
	amd_bios_entry *index;

	printf("BIOS Directory Table (BDT) components:");
	for (index = fw_table; index->type != AMD_BIOS_INVALID; index++) {
		if (index->filename)
			printf("  %2x: %s\n", index->type, index->filename);
	}
}

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
				index->type != AMD_BIOS_APCB_BK)
			free(index->filename);
	}
}

static void integrate_psp_ab(context *ctx, psp_directory_table *pspdir,
		psp_directory_table *pspdir2, ish_directory_table *ish,
		amd_fw_type ab, enum platform soc_id)
{
	uint32_t count;
	uint32_t current_table_save;

	current_table_save = ctx->current_table;
	ctx->current_table = (char *)pspdir - ctx->rom;
	count = pspdir->header.num_entries;
	assert_fw_entry(count, MAX_PSP_ENTRIES, ctx);
	pspdir->entries[count].type = (uint8_t)ab;
	pspdir->entries[count].subprog = 0;
	pspdir->entries[count].rsvd = 0;
	if (ish != NULL) {
		ish->pl2_location = BUFF_TO_RUN_MODE(*ctx, pspdir2, ADDRESS_MODE_1_REL_BIOS);
		ish->boot_priority = ab == AMD_FW_RECOVERYAB_A ? 0xFFFFFFFF : 1;
		ish->update_retry_count = 2;
		ish->glitch_retry_count = 0;
		ish->psp_id = get_psp_id(soc_id);
		ish->checksum = fletcher32(&ish->boot_priority,
				sizeof(ish_directory_table) - sizeof(uint32_t));
		pspdir->entries[count].addr =
				BUFF_TO_RUN_MODE(*ctx, ish, ADDRESS_MODE_1_REL_BIOS);
		pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, ADDRESS_MODE_1_REL_BIOS);
		pspdir->entries[count].size = TABLE_ALIGNMENT;
	} else {
		pspdir->entries[count].addr =
				BUFF_TO_RUN_MODE(*ctx, pspdir2, ADDRESS_MODE_1_REL_BIOS);
		pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, ADDRESS_MODE_1_REL_BIOS);
		pspdir->entries[count].size = pspdir2->header.num_entries *
				sizeof(psp_directory_entry) +
				sizeof(psp_directory_header);
	}

	count++;
	pspdir->header.num_entries = count;
	ctx->current_table = current_table_save;
}

static void integrate_psp_firmwares(context *ctx,
					psp_directory_table *pspdir,
					psp_directory_table *pspdir2,
					psp_directory_table *pspdir2_b,
					amd_fw_entry *fw_table,
					uint32_t cookie,
					enum platform soc_id,
					amd_cb_config *cb_config)
{
	ssize_t bytes;
	unsigned int i, count;
	int level;
	uint32_t current_table_save;
	bool recovery_ab = cb_config->recovery_ab;
	ish_directory_table *ish_a_dir = NULL, *ish_b_dir = NULL;

	/* This function can create a primary table, a secondary table, or a
	 * flattened table which contains all applicable types.  These if-else
	 * statements infer what the caller intended.  If a 2nd-level cookie
	 * is passed, clearly a 2nd-level table is intended.  However, a
	 * 1st-level cookie may indicate level 1 or flattened.  If the caller
	 * passes a pointer to a 2nd-level table, then assume not flat.
	 */
	if (!cb_config->multi_level)
		level = PSP_BOTH;
	else if (cookie == PSPL2_COOKIE)
		level = PSP_LVL2;
	else if (pspdir2)
		level = PSP_LVL1;
	else
		level = PSP_BOTH;

	if (recovery_ab) {
		if (cookie == PSPL2_COOKIE)
			level = PSP_LVL2_AB;
		else if (pspdir2)
			level = PSP_LVL1_AB;
		else
			level = PSP_BOTH_AB;
	}
	current_table_save = ctx->current_table;
	ctx->current_table = (char *)pspdir - ctx->rom;
	ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);

	for (i = 0, count = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (!(fw_table[i].level & level))
			continue;

		assert_fw_entry(count, MAX_PSP_ENTRIES, ctx);

		if (fw_table[i].type == AMD_TOKEN_UNLOCK) {
			if (!fw_table[i].other)
				continue;
			ctx->current = ALIGN(ctx->current, ERASE_ALIGNMENT);
			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].size = 4096; /* TODO: doc? */
			pspdir->entries[count].addr = RUN_CURRENT(*ctx);
			pspdir->entries[count].address_mode = SET_ADDR_MODE_BY_TABLE(pspdir);
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			ctx->current = ALIGN(ctx->current + 4096, 0x100U);
			count++;
		} else if (fw_table[i].type == AMD_PSP_FUSE_CHAIN) {
			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].size = 0xFFFFFFFF;
			pspdir->entries[count].addr = fw_table[i].other;
			pspdir->entries[count].address_mode = 0;
			count++;
		} else if (fw_table[i].type == AMD_FW_PSP_NVRAM) {
			if (fw_table[i].filename == NULL)
				continue;
			/* TODO: Add a way to reserve for NVRAM without
			 * requiring a filename.  This isn't a feature used
			 * by coreboot systems, so priority is very low.
			 */
			ctx->current = ALIGN(ctx->current, ERASE_ALIGNMENT);
			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes <= 0) {
				free(ctx->rom);
				exit(1);
			}

			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].size = ALIGN(bytes,
							ERASE_ALIGNMENT);
			pspdir->entries[count].addr =
				RUN_CURRENT_MODE(*ctx, ADDRESS_MODE_1_REL_BIOS);
			pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, ADDRESS_MODE_1_REL_BIOS);

			ctx->current = ALIGN(ctx->current + bytes,
							BLOB_ERASE_ALIGNMENT);
			count++;
		} else if (fw_table[i].filename != NULL) {
			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes < 0) {
				free(ctx->rom);
				exit(1);
			}

			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].subprog = fw_table[i].subprog;
			pspdir->entries[count].rsvd = 0;
			pspdir->entries[count].size = (uint32_t)bytes;
			pspdir->entries[count].addr = RUN_CURRENT(*ctx);
			pspdir->entries[count].address_mode = SET_ADDR_MODE_BY_TABLE(pspdir);

			ctx->current = ALIGN(ctx->current + bytes,
							BLOB_ALIGNMENT);
			count++;
		} else {
			/* This APU doesn't have this firmware. */
		}
	}

	if (recovery_ab && (pspdir2 != NULL)) {
		if (cb_config->need_ish) {	/* Need ISH */
			ish_a_dir = new_ish_dir(ctx);
			if (pspdir2_b != NULL)
				ish_b_dir = new_ish_dir(ctx);
		}
		pspdir->header.num_entries = count;
		integrate_psp_ab(ctx, pspdir, pspdir2, ish_a_dir,
			AMD_FW_RECOVERYAB_A, soc_id);
		if (pspdir2_b != NULL)
			integrate_psp_ab(ctx, pspdir, pspdir2_b, ish_b_dir,
				AMD_FW_RECOVERYAB_B, soc_id);
		count = pspdir->header.num_entries;
	} else if (pspdir2 != NULL) {
		assert_fw_entry(count, MAX_PSP_ENTRIES, ctx);
		pspdir->entries[count].type = AMD_FW_L2_PTR;
		pspdir->entries[count].subprog = 0;
		pspdir->entries[count].rsvd = 0;
		pspdir->entries[count].size = sizeof(pspdir2->header)
					+ pspdir2->header.num_entries
					* sizeof(psp_directory_entry);

		pspdir->entries[count].addr =
				BUFF_TO_RUN_MODE(*ctx, pspdir2, ADDRESS_MODE_1_REL_BIOS);
		pspdir->entries[count].address_mode =
				SET_ADDR_MODE(pspdir, ADDRESS_MODE_1_REL_BIOS);
		count++;
	}

	fill_dir_header(pspdir, count, cookie, ctx);
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
	ctx->current_table = (char *)pspdir - ctx->rom;

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

	pspdir->header.num_entries = count;
	pspdir->header.checksum = fletcher32(&pspdir->header.num_entries,
					count * sizeof(psp_directory_entry)
					+ sizeof(pspdir->header.num_entries)
					+ sizeof(pspdir->header.additional_info));

	ctx->current_table = current_table_save;
}

static void *new_bios_dir(context *ctx, bool multi)
{
	void *ptr;

	/*
	 * Force both onto boundary when multi.  Primary table is after
	 * updatable table, so alignment ensures primary can stay intact
	 * if secondary is reprogrammed.
	 */
	if (multi)
		ctx->current = ALIGN(ctx->current, TABLE_ERASE_ALIGNMENT);
	else
		ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);
	ptr = BUFF_CURRENT(*ctx);
	((bios_directory_hdr *) ptr)->additional_info = 0;
	((bios_directory_hdr *) ptr)->additional_info_fields.address_mode = ctx->address_mode;
	ctx->current_table = ctx->current;
	ctx->current += sizeof(bios_directory_hdr)
			+ MAX_BIOS_ENTRIES * sizeof(bios_directory_entry);
	return ptr;
}

static int locate_bdt2_bios(bios_directory_table *level2,
					uint64_t *source, uint32_t *size)
{
	uint32_t i;

	*source = 0;
	*size = 0;
	if (!level2)
		return 0;

	for (i = 0 ; i < level2->header.num_entries ; i++) {
		if (level2->entries[i].type == AMD_BIOS_BIN) {
			*source = level2->entries[i].source;
			*size = level2->entries[i].size;
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

static int find_bios_entry(amd_bios_type type)
{
	int i;

	for (i = 0; amd_bios_table[i].type != AMD_BIOS_INVALID; i++) {
		if (amd_bios_table[i].type == type)
			return i;
	}
	return -1;
}

static void integrate_bios_firmwares(context *ctx,
					bios_directory_table *biosdir,
					bios_directory_table *biosdir2,
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

	/* This function can create a primary table, a secondary table, or a
	 * flattened table which contains all applicable types.  These if-else
	 * statements infer what the caller intended.  If a 2nd-level cookie
	 * is passed, clearly a 2nd-level table is intended.  However, a
	 * 1st-level cookie may indicate level 1 or flattened.  If the caller
	 * passes a pointer to a 2nd-level table, then assume not flat.
	 */
	if (!cb_config->multi_level)
		level = BDT_BOTH;
	else if (cookie == BDT2_COOKIE)
		level = BDT_LVL2;
	else if (biosdir2)
		level = BDT_LVL1;
	else
		level = BDT_BOTH;

	ctx->current = ALIGN(ctx->current, TABLE_ALIGNMENT);

	for (i = 0, count = 0; fw_table[i].type != AMD_BIOS_INVALID; i++) {
		if (!(fw_table[i].level & level))
			continue;
		if (fw_table[i].filename == NULL && (
				fw_table[i].type != AMD_BIOS_APOB &&
				fw_table[i].type != AMD_BIOS_APOB_NV &&
				fw_table[i].type != AMD_BIOS_L2_PTR &&
				fw_table[i].type != AMD_BIOS_BIN &&
				fw_table[i].type != AMD_BIOS_PSP_SHARED_MEM))
			continue;

		/* BIOS Directory items may have additional requirements */

		/* Check APOB_NV requirements */
		if (fw_table[i].type == AMD_BIOS_APOB_NV) {
			if (!fw_table[i].size && !fw_table[i].src)
				continue; /* APOB_NV not used */
			if (fw_table[i].src && !fw_table[i].size) {
				fprintf(stderr, "Error: APOB NV address provided, but no size\n");
				free(ctx->rom);
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
			free(ctx->rom);
			exit(1);
		}

		/* BIOS binary must have destination and uncompressed size.  If
		 * no filename given, then user must provide a source address.
		 */
		if (fw_table[i].type == AMD_BIOS_BIN) {
			if (!fw_table[i].dest || !fw_table[i].size) {
				fprintf(stderr, "Error: BIOS binary destination and uncompressed size are required\n");
				free(ctx->rom);
				exit(1);
			}
			if (!fw_table[i].filename && !fw_table[i].src) {
				fprintf(stderr, "Error: BIOS binary assumed outside amdfw.rom but no source address given\n");
				free(ctx->rom);
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
		case AMD_BIOS_APOB:
			biosdir->entries[count].size = fw_table[i].size;
			biosdir->entries[count].source = fw_table[i].src;
			biosdir->entries[count].address_mode = SET_ADDR_MODE_BY_TABLE(biosdir);
			break;
		case AMD_BIOS_APOB_NV:
			if (fw_table[i].src) {
				/* If source is given, use that and its size */
				biosdir->entries[count].source = fw_table[i].src;
				biosdir->entries[count].address_mode =
						SET_ADDR_MODE(biosdir, ADDRESS_MODE_1_REL_BIOS);
				biosdir->entries[count].size = fw_table[i].size;
			} else {
				/* Else reserve size bytes within amdfw.rom */
				ctx->current = ALIGN(ctx->current, ERASE_ALIGNMENT);
				biosdir->entries[count].source = RUN_CURRENT(*ctx);
				biosdir->entries[count].address_mode =
						SET_ADDR_MODE(biosdir, ADDRESS_MODE_1_REL_BIOS);
				biosdir->entries[count].size = ALIGN(
						fw_table[i].size, ERASE_ALIGNMENT);
				memset(BUFF_CURRENT(*ctx), 0xff,
						biosdir->entries[count].size);
				ctx->current = ctx->current
						+ biosdir->entries[count].size;
			}
			break;
		case AMD_BIOS_BIN:
			/* Don't make a 2nd copy, point to the same one */
			if (level == BDT_LVL1 && locate_bdt2_bios(biosdir2, &source, &size)) {
				biosdir->entries[count].source = source;
				biosdir->entries[count].address_mode =
						SET_ADDR_MODE(biosdir, ADDRESS_MODE_1_REL_BIOS);
				biosdir->entries[count].size = size;
				break;
			}

			/* level 2, or level 1 and no copy found in level 2 */
			biosdir->entries[count].source = fw_table[i].src;
			biosdir->entries[count].address_mode =
						SET_ADDR_MODE(biosdir, ADDRESS_MODE_1_REL_BIOS);
			biosdir->entries[count].dest = fw_table[i].dest;
			biosdir->entries[count].size = fw_table[i].size;

			if (!fw_table[i].filename)
				break;

			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes <= 0) {
				free(ctx->rom);
				exit(1);
			}

			biosdir->entries[count].source =
				RUN_CURRENT_MODE(*ctx, ADDRESS_MODE_1_REL_BIOS);
			biosdir->entries[count].address_mode =
				SET_ADDR_MODE(biosdir, ADDRESS_MODE_1_REL_BIOS);

			ctx->current = ALIGN(ctx->current + bytes, 0x100U);
			break;
		case AMD_BIOS_PSP_SHARED_MEM:
			biosdir->entries[count].dest = fw_table[i].dest;
			biosdir->entries[count].size = fw_table[i].size;
			break;

		default: /* everything else is copied from input */
			if (fw_table[i].type == AMD_BIOS_APCB ||
					fw_table[i].type == AMD_BIOS_APCB_BK)
				ctx->current = ALIGN(
						ctx->current, ERASE_ALIGNMENT);
			bytes = copy_blob(BUFF_CURRENT(*ctx),
					fw_table[i].filename, BUFF_ROOM(*ctx));
			if (bytes <= 0) {
				free(ctx->rom);
				exit(1);
			}

			biosdir->entries[count].size = (uint32_t)bytes;
			biosdir->entries[count].source = RUN_CURRENT(*ctx);
			biosdir->entries[count].address_mode = SET_ADDR_MODE_BY_TABLE(biosdir);

			ctx->current = ALIGN(ctx->current + bytes, 0x100U);
			break;
		}

		count++;
	}

	if (biosdir2) {
		assert_fw_entry(count, MAX_BIOS_ENTRIES, ctx);
		biosdir->entries[count].type = AMD_BIOS_L2_PTR;
		biosdir->entries[count].region_type = 0;
		biosdir->entries[count].size =
					+ MAX_BIOS_ENTRIES
					* sizeof(bios_directory_entry);
		biosdir->entries[count].source =
					BUFF_TO_RUN(*ctx, biosdir2);
		biosdir->entries[count].address_mode =
					SET_ADDR_MODE(biosdir, ADDRESS_MODE_1_REL_BIOS);
		biosdir->entries[count].subprog = 0;
		biosdir->entries[count].inst = 0;
		biosdir->entries[count].copy = 0;
		biosdir->entries[count].compressed = 0;
		biosdir->entries[count].dest = -1;
		biosdir->entries[count].reset = 0;
		biosdir->entries[count].ro = 0;
		count++;
	}

	fill_dir_header(biosdir, count, cookie, ctx);
}

enum {
	AMDFW_OPT_CONFIG =	'c',
	AMDFW_OPT_DEBUG =	'd',
	AMDFW_OPT_HELP =	'h',
	AMDFW_OPT_LIST_DEPEND =	'l',

	AMDFW_OPT_XHCI = 128,
	AMDFW_OPT_IMC,
	AMDFW_OPT_GEC,
	AMDFW_OPT_COMBO,
	AMDFW_OPT_RECOVERY_AB,
	AMDFW_OPT_MULTILEVEL,
	AMDFW_OPT_NVRAM,

	AMDFW_OPT_FUSE,
	AMDFW_OPT_UNLOCK,
	AMDFW_OPT_WHITELIST,
	AMDFW_OPT_USE_PSPSECUREOS,
	AMDFW_OPT_LOAD_MP2FW,
	AMDFW_OPT_LOAD_S0I3,
	AMDFW_OPT_SPL_TABLE,
	AMDFW_OPT_VERSTAGE,
	AMDFW_OPT_VERSTAGE_SIG,

	AMDFW_OPT_INSTANCE,
	AMDFW_OPT_APCB,
	AMDFW_OPT_APOBBASE,
	AMDFW_OPT_BIOSBIN,
	AMDFW_OPT_BIOSBIN_SOURCE,
	AMDFW_OPT_BIOSBIN_DEST,
	AMDFW_OPT_BIOS_UNCOMP_SIZE,
	AMDFW_OPT_UCODE,
	AMDFW_OPT_APOB_NVBASE,
	AMDFW_OPT_APOB_NVSIZE,

	AMDFW_OPT_OUTPUT,
	AMDFW_OPT_FLASHSIZE,
	AMDFW_OPT_LOCATION,
	AMDFW_OPT_ANYWHERE,
	AMDFW_OPT_SHAREDMEM,
	AMDFW_OPT_SHAREDMEM_SIZE,
	AMDFW_OPT_SOC_NAME,
	/* begin after ASCII characters */
	LONGOPT_SPI_READ_MODE	= 256,
	LONGOPT_SPI_SPEED	= 257,
	LONGOPT_SPI_MICRON_FLAG	= 258,
};

static char const optstring[] = {AMDFW_OPT_CONFIG, ':',
	AMDFW_OPT_DEBUG, AMDFW_OPT_HELP, AMDFW_OPT_LIST_DEPEND
};

static struct option long_options[] = {
	{"xhci",             required_argument, 0, AMDFW_OPT_XHCI },
	{"imc",              required_argument, 0, AMDFW_OPT_IMC },
	{"gec",              required_argument, 0, AMDFW_OPT_GEC },
	/* PSP Directory Table items */
	{"combo-capable",          no_argument, 0, AMDFW_OPT_COMBO },
	{"recovery-ab",            no_argument, 0, AMDFW_OPT_RECOVERY_AB },
	{"multilevel",             no_argument, 0, AMDFW_OPT_MULTILEVEL },
	{"nvram",            required_argument, 0, AMDFW_OPT_NVRAM },
	{"soft-fuse",        required_argument, 0, AMDFW_OPT_FUSE },
	{"token-unlock",           no_argument, 0, AMDFW_OPT_UNLOCK },
	{"whitelist",        required_argument, 0, AMDFW_OPT_WHITELIST },
	{"use-pspsecureos",        no_argument, 0, AMDFW_OPT_USE_PSPSECUREOS },
	{"load-mp2-fw",            no_argument, 0, AMDFW_OPT_LOAD_MP2FW },
	{"load-s0i3",              no_argument, 0, AMDFW_OPT_LOAD_S0I3 },
	{"spl-table",        required_argument, 0, AMDFW_OPT_SPL_TABLE },
	{"verstage",         required_argument, 0, AMDFW_OPT_VERSTAGE },
	{"verstage_sig",     required_argument, 0, AMDFW_OPT_VERSTAGE_SIG },
	/* BIOS Directory Table items */
	{"instance",         required_argument, 0, AMDFW_OPT_INSTANCE },
	{"apcb",             required_argument, 0, AMDFW_OPT_APCB },
	{"apob-base",        required_argument, 0, AMDFW_OPT_APOBBASE },
	{"bios-bin",         required_argument, 0, AMDFW_OPT_BIOSBIN },
	{"bios-bin-src",     required_argument, 0, AMDFW_OPT_BIOSBIN_SOURCE },
	{"bios-bin-dest",    required_argument, 0, AMDFW_OPT_BIOSBIN_DEST },
	{"bios-uncomp-size", required_argument, 0, AMDFW_OPT_BIOS_UNCOMP_SIZE },
	{"ucode",            required_argument, 0, AMDFW_OPT_UCODE },
	{"apob-nv-base",     required_argument, 0, AMDFW_OPT_APOB_NVBASE },
	{"apob-nv-size",     required_argument, 0, AMDFW_OPT_APOB_NVSIZE },
	/* Embedded Firmware Structure items*/
	{"spi-read-mode",    required_argument, 0, LONGOPT_SPI_READ_MODE },
	{"spi-speed",        required_argument, 0, LONGOPT_SPI_SPEED },
	{"spi-micron-flag",  required_argument, 0, LONGOPT_SPI_MICRON_FLAG },
	/* other */
	{"output",           required_argument, 0, AMDFW_OPT_OUTPUT },
	{"flashsize",        required_argument, 0, AMDFW_OPT_FLASHSIZE },
	{"location",         required_argument, 0, AMDFW_OPT_LOCATION },
	{"anywhere",         no_argument,       0, AMDFW_OPT_ANYWHERE },
	{"sharedmem",        required_argument, 0, AMDFW_OPT_SHAREDMEM },
	{"sharedmem-size",   required_argument, 0, AMDFW_OPT_SHAREDMEM_SIZE },
	{"soc-name",         required_argument, 0, AMDFW_OPT_SOC_NAME },

	{"config",           required_argument, 0, AMDFW_OPT_CONFIG },
	{"debug",            no_argument,       0, AMDFW_OPT_DEBUG },
	{"help",             no_argument,       0, AMDFW_OPT_HELP },
	{"list",             no_argument,       0, AMDFW_OPT_LIST_DEPEND },
	{NULL,               0,                 0,  0  }
};

void register_fw_fuse(char *str)
{
	uint32_t i;

	for (i = 0; i < sizeof(amd_psp_fw_table) / sizeof(amd_fw_entry); i++) {
		if (amd_psp_fw_table[i].type != AMD_PSP_FUSE_CHAIN)
			continue;

		amd_psp_fw_table[i].other = strtoull(str, NULL, 16);
		return;
	}
}

static void register_fw_token_unlock(void)
{
	uint32_t i;

	for (i = 0; i < sizeof(amd_psp_fw_table) / sizeof(amd_fw_entry); i++) {
		if (amd_psp_fw_table[i].type != AMD_TOKEN_UNLOCK)
			continue;

		amd_psp_fw_table[i].other = 1;
		return;
	}
}

static void register_fw_filename(amd_fw_type type, uint8_t sub, char filename[])
{
	unsigned int i;

	for (i = 0; i < sizeof(amd_fw_table) / sizeof(amd_fw_entry); i++) {
		if (amd_fw_table[i].type == type) {
			amd_fw_table[i].filename = filename;
			return;
		}
	}

	for (i = 0; i < sizeof(amd_psp_fw_table) / sizeof(amd_fw_entry); i++) {
		if (amd_psp_fw_table[i].type != type)
			continue;

		if (amd_psp_fw_table[i].subprog == sub) {
			amd_psp_fw_table[i].filename = filename;
			return;
		}
	}
}

static void register_bdt_data(amd_bios_type type, int sub, int ins, char name[])
{
	uint32_t i;

	for (i = 0; i < sizeof(amd_bios_table) / sizeof(amd_bios_entry); i++) {
		if (amd_bios_table[i].type == type
					&& amd_bios_table[i].inst == ins
					&& amd_bios_table[i].subpr == sub) {
			amd_bios_table[i].filename = name;
			return;
		}
	}
}

static void register_fw_addr(amd_bios_type type, char *src_str,
					char *dst_str, char *size_str)
{
	uint32_t i;
	for (i = 0; i < sizeof(amd_bios_table) / sizeof(amd_bios_entry); i++) {
		if (amd_bios_table[i].type != type)
			continue;

		if (src_str)
			amd_bios_table[i].src = strtoull(src_str, NULL, 16);
		if (dst_str)
			amd_bios_table[i].dest = strtoull(dst_str, NULL, 16);
		if (size_str)
			amd_bios_table[i].size = strtoul(size_str, NULL, 16);

		return;
	}
}

static int set_efs_table(uint8_t soc_id, embedded_firmware *amd_romsig,
			 uint8_t efs_spi_readmode, uint8_t efs_spi_speed,
			 uint8_t efs_spi_micron_flag)
{
	if ((efs_spi_readmode == 0xFF) || (efs_spi_speed == 0xFF)) {
		fprintf(stderr, "Error: EFS read mode and SPI speed must be set\n");
		return 1;
	}
	switch (soc_id) {
	case PLATFORM_STONEYRIDGE:
		amd_romsig->spi_readmode_f15_mod_60_6f = efs_spi_readmode;
		amd_romsig->fast_speed_new_f15_mod_60_6f = efs_spi_speed;
		break;
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
		/* amd_romsig->efs_gen introduced after RAVEN/PICASSO.
		 * Leave as 0xffffffff for first gen */
		amd_romsig->spi_readmode_f17_mod_00_2f = efs_spi_readmode;
		amd_romsig->spi_fastspeed_f17_mod_00_2f = efs_spi_speed;
		switch (efs_spi_micron_flag) {
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
	case PLATFORM_SABRINA:
		amd_romsig->efs_gen.gen = EFS_SECOND_GEN;
		amd_romsig->spi_readmode_f17_mod_30_3f = efs_spi_readmode;
		amd_romsig->spi_fastspeed_f17_mod_30_3f = efs_spi_speed;
		switch (efs_spi_micron_flag) {
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

static int identify_platform(char *soc_name)
{
	if (!strcasecmp(soc_name, "Stoneyridge"))
		return PLATFORM_STONEYRIDGE;
	else if (!strcasecmp(soc_name, "Raven"))
		return PLATFORM_RAVEN;
	else if (!strcasecmp(soc_name, "Picasso"))
		return PLATFORM_PICASSO;
	else if (!strcasecmp(soc_name, "Cezanne"))
		return PLATFORM_CEZANNE;
	else if (!strcasecmp(soc_name, "Mendocino"))
		return PLATFORM_MENDOCINO;
	else if (!strcasecmp(soc_name, "Renoir"))
		return PLATFORM_RENOIR;
	else if (!strcasecmp(soc_name, "Lucienne"))
		return PLATFORM_LUCIENNE;
	else if (!strcasecmp(soc_name, "Sabrina"))
		return PLATFORM_SABRINA;
	else
		return PLATFORM_UNKNOWN;

}

int main(int argc, char **argv)
{
	int c;
	int retval = 0;
	char *tmp;
	char *rom = NULL;
	embedded_firmware *amd_romsig;
	psp_directory_table *pspdir = NULL;
	psp_directory_table *pspdir2 = NULL;
	psp_directory_table *pspdir2_b = NULL;
	bool comboable = false;
	int fuse_defined = 0;
	int targetfd;
	char *output = NULL, *config = NULL;
	FILE *config_handle;
	context ctx = { 0 };
	/* Values cleared after each firmware or parameter, regardless if N/A */
	uint8_t sub = 0, instance = 0;
	uint32_t dir_location = 0;
	bool any_location = 0;
	uint32_t romsig_offset;
	uint32_t rom_base_address;
	uint8_t soc_id = PLATFORM_UNKNOWN;
	uint8_t efs_spi_readmode = 0xff;
	uint8_t efs_spi_speed = 0xff;
	uint8_t efs_spi_micron_flag = 0xff;

	amd_cb_config cb_config;
	int debug = 0;
	int list_deps = 0;

	cb_config.have_whitelist = false;
	cb_config.unlock_secure = false;
	cb_config.use_secureos = false;
	cb_config.load_mp2_fw = false;
	cb_config.s0i3 = false;
	cb_config.multi_level = false;
	cb_config.recovery_ab = false;
	cb_config.need_ish = false;

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case AMDFW_OPT_XHCI:
			register_fw_filename(AMD_FW_XHCI, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_IMC:
			register_fw_filename(AMD_FW_IMC, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_GEC:
			register_fw_filename(AMD_FW_GEC, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_COMBO:
			comboable = true;
			break;
		case AMDFW_OPT_RECOVERY_AB:
			cb_config.recovery_ab = true;
			break;
		case AMDFW_OPT_MULTILEVEL:
			cb_config.multi_level = true;
			break;
		case AMDFW_OPT_UNLOCK:
			register_fw_token_unlock();
			cb_config.unlock_secure = true;
			sub = instance = 0;
			break;
		case AMDFW_OPT_USE_PSPSECUREOS:
			cb_config.use_secureos = true;
			break;
		case AMDFW_OPT_INSTANCE:
			instance = strtoul(optarg, &tmp, 16);
			break;
		case AMDFW_OPT_LOAD_MP2FW:
			cb_config.load_mp2_fw = true;
			break;
		case AMDFW_OPT_NVRAM:
			register_fw_filename(AMD_FW_PSP_NVRAM, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_FUSE:
			register_fw_fuse(optarg);
			fuse_defined = 1;
			sub = 0;
			break;
		case AMDFW_OPT_APCB:
			if ((instance & 0xF0) == 0)
				register_bdt_data(AMD_BIOS_APCB, sub, instance & 0xF, optarg);
			else
				register_bdt_data(AMD_BIOS_APCB_BK, sub,
							instance & 0xF, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_APOBBASE:
			/* APOB destination */
			register_fw_addr(AMD_BIOS_APOB, 0, optarg, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_APOB_NVBASE:
			/* APOB NV source */
			register_fw_addr(AMD_BIOS_APOB_NV, optarg, 0, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_APOB_NVSIZE:
			/* APOB NV size */
			register_fw_addr(AMD_BIOS_APOB_NV, 0, 0, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOSBIN:
			register_bdt_data(AMD_BIOS_BIN, sub, instance, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOSBIN_SOURCE:
			/* BIOS source */
			register_fw_addr(AMD_BIOS_BIN, optarg, 0, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOSBIN_DEST:
			/* BIOS destination */
			register_fw_addr(AMD_BIOS_BIN, 0, optarg, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOS_UNCOMP_SIZE:
			/* BIOS destination size */
			register_fw_addr(AMD_BIOS_BIN, 0, 0, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_UCODE:
			register_bdt_data(AMD_BIOS_UCODE, sub,
							instance, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_LOAD_S0I3:
			cb_config.s0i3 = true;
			break;
		case AMDFW_OPT_SPL_TABLE:
			register_fw_filename(AMD_FW_SPL, sub, optarg);
			sub = instance = 0;
			cb_config.have_mb_spl = true;
			break;
		case AMDFW_OPT_WHITELIST:
			register_fw_filename(AMD_FW_PSP_WHITELIST, sub, optarg);
			sub = instance = 0;
			cb_config.have_whitelist = true;
			break;
		case AMDFW_OPT_VERSTAGE:
			register_fw_filename(AMD_FW_PSP_VERSTAGE, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_VERSTAGE_SIG:
			register_fw_filename(AMD_FW_VERSTAGE_SIG, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_SOC_NAME:
			soc_id = identify_platform(optarg);
			if (soc_id == PLATFORM_UNKNOWN) {
				fprintf(stderr, "Error: Invalid SOC name specified\n\n");
				retval = 1;
			}
			sub = instance = 0;
			break;
		case LONGOPT_SPI_READ_MODE:
			efs_spi_readmode = strtoull(optarg, NULL, 16);
			sub = instance = 0;
			break;
		case LONGOPT_SPI_SPEED:
			efs_spi_speed = strtoull(optarg, NULL, 16);
			sub = instance = 0;
			break;
		case LONGOPT_SPI_MICRON_FLAG:
			efs_spi_micron_flag = strtoull(optarg, NULL, 16);
			sub = instance = 0;
			break;
		case AMDFW_OPT_OUTPUT:
			output = optarg;
			break;
		case AMDFW_OPT_FLASHSIZE:
			ctx.rom_size = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				fprintf(stderr, "Error: ROM size specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;
		case AMDFW_OPT_LOCATION:
			dir_location = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				fprintf(stderr, "Error: Directory Location specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;
		case AMDFW_OPT_ANYWHERE:
			any_location = 1;
			break;
		case AMDFW_OPT_SHAREDMEM:
			/* shared memory destination */
			register_fw_addr(AMD_BIOS_PSP_SHARED_MEM, 0, optarg, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_SHAREDMEM_SIZE:
			/* shared memory size */
			register_fw_addr(AMD_BIOS_PSP_SHARED_MEM, NULL, NULL, optarg);
			sub = instance = 0;
			break;

		case AMDFW_OPT_CONFIG:
			config = optarg;
			break;
		case AMDFW_OPT_DEBUG:
			debug = 1;
			break;
		case AMDFW_OPT_HELP:
			usage();
			return 0;
		case AMDFW_OPT_LIST_DEPEND:
			list_deps = 1;
			break;
		default:
			break;
		}
	}

	if (cb_config.need_ish)
		cb_config.recovery_ab = true;

	if (cb_config.recovery_ab)
		cb_config.multi_level = true;

	if (config) {
		config_handle = fopen(config, "r");
		if (config_handle == NULL) {
			fprintf(stderr, "Can not open file %s for reading: %s\n",
				config, strerror(errno));
			exit(1);
		}
		if (process_config(config_handle, &cb_config, list_deps) == 0) {
			fprintf(stderr, "Configuration file %s parsing error\n", config);
			fclose(config_handle);
			exit(1);
		}
		fclose(config_handle);
	}
	/* For debug. */
	if (debug) {
		dump_psp_firmwares(amd_psp_fw_table);
		dump_bdt_firmwares(amd_bios_table);
	}

	if (!fuse_defined)
		register_fw_fuse(DEFAULT_SOFT_FUSE_CHAIN);

	if (!output && !list_deps) {
		fprintf(stderr, "Error: Output value is not specified.\n\n");
		retval = 1;
	}

	if ((ctx.rom_size % 1024 != 0)  && !list_deps) {
		fprintf(stderr, "Error: ROM Size (%d bytes) should be a multiple of"
			" 1024 bytes.\n\n", ctx.rom_size);
		retval = 1;
	}

	if ((ctx.rom_size < MIN_ROM_KB * 1024)  && !list_deps) {
		fprintf(stderr, "Error: ROM Size (%dKB) must be at least %dKB.\n\n",
			ctx.rom_size / 1024, MIN_ROM_KB);
		retval = 1;
	}

	if (retval) {
		usage();
		return retval;
	}

	if (list_deps) {
		return retval;
	}

	printf("    AMDFWTOOL  Using ROM size of %dKB\n", ctx.rom_size / 1024);

	rom_base_address = 0xFFFFFFFF - ctx.rom_size + 1;
	if (dir_location && (dir_location < rom_base_address)) {
		fprintf(stderr, "Error: Directory location outside of ROM.\n\n");
		return 1;
	}

	if (any_location) {
		if (dir_location & 0x3f) {
			fprintf(stderr, "Error: Invalid Directory location.\n");
			fprintf(stderr, "  Valid locations are 64-byte aligned\n");
			return 1;
		}
	} else {
		switch (dir_location) {
		case 0:          /* Fall through */
		case 0xFFFA0000: /* Fall through */
		case 0xFFF20000: /* Fall through */
		case 0xFFE20000: /* Fall through */
		case 0xFFC20000: /* Fall through */
		case 0xFF820000: /* Fall through */
		case 0xFF020000: /* Fall through */
			break;
		default:
			fprintf(stderr, "Error: Invalid Directory location.\n");
			fprintf(stderr, "  Valid locations are 0xFFFA0000, 0xFFF20000,\n");
			fprintf(stderr, "  0xFFE20000, 0xFFC20000, 0xFF820000, 0xFF020000\n");
			return 1;
		}
	}
	ctx.rom = malloc(ctx.rom_size);
	if (!ctx.rom) {
		fprintf(stderr, "Error: Failed to allocate memory\n");
		return 1;
	}
	memset(ctx.rom, 0xFF, ctx.rom_size);

	if (dir_location)
		romsig_offset = ctx.current = dir_location - rom_base_address;
	else
		romsig_offset = ctx.current = AMD_ROMSIG_OFFSET;

	amd_romsig = BUFF_OFFSET(ctx, romsig_offset);
	amd_romsig->signature = EMBEDDED_FW_SIGNATURE;
	amd_romsig->imc_entry = 0;
	amd_romsig->gec_entry = 0;
	amd_romsig->xhci_entry = 0;
	amd_romsig->efs_gen.reserved = 0;

	if (soc_id != PLATFORM_UNKNOWN) {
		retval = set_efs_table(soc_id, amd_romsig, efs_spi_readmode,
					efs_spi_speed, efs_spi_micron_flag);
		if (retval) {
			fprintf(stderr, "ERROR: Failed to initialize EFS table!\n");
			return retval;
		}
	} else {
		fprintf(stderr, "WARNING: No SOC name specified.\n");
	}

	if (cb_config.need_ish)
		ctx.address_mode = ADDRESS_MODE_2_REL_TAB;
	else if (amd_romsig->efs_gen.gen == EFS_SECOND_GEN)
		ctx.address_mode = ADDRESS_MODE_1_REL_BIOS;
	else
		ctx.address_mode = ADDRESS_MODE_0_PHY;
	printf("    AMDFWTOOL  Using firmware directory location of %s address: 0x%08x\n",
			ctx.address_mode == ADDRESS_MODE_0_PHY ? "absolute" : "relative",
			RUN_CURRENT(ctx));

	integrate_firmwares(&ctx, amd_romsig, amd_fw_table);

	ctx.current = ALIGN(ctx.current, 0x10000U); /* TODO: is it necessary? */
	ctx.current_table = 0;

	if (cb_config.multi_level) {
		/* Do 2nd PSP directory followed by 1st */
		pspdir2 = new_psp_dir(&ctx, cb_config.multi_level);
		integrate_psp_firmwares(&ctx, pspdir2, NULL, NULL,
					amd_psp_fw_table, PSPL2_COOKIE, soc_id, &cb_config);
		if (cb_config.recovery_ab) {
			/* B is same as above directories for A */
			/* Skip creating pspdir2_b here to save flash space. Related
			 * biosdir2_b will be skipped automatically. */
			pspdir2_b = new_psp_dir(&ctx, cb_config.multi_level);
			integrate_psp_firmwares(&ctx, pspdir2_b, NULL, NULL,
					amd_psp_fw_table, PSPL2_COOKIE, soc_id, &cb_config);
		} else {
			pspdir2_b = NULL; /* More explicitly */
		}
		pspdir = new_psp_dir(&ctx, cb_config.multi_level);
		integrate_psp_firmwares(&ctx, pspdir, pspdir2, pspdir2_b,
					amd_psp_fw_table, PSP_COOKIE, soc_id, &cb_config);
	} else {
		/* flat: PSP 1 cookie and no pointer to 2nd table */
		pspdir = new_psp_dir(&ctx, cb_config.multi_level);
		integrate_psp_firmwares(&ctx, pspdir, NULL, NULL,
					amd_psp_fw_table, PSP_COOKIE, soc_id, &cb_config);
	}

	if (comboable)
		amd_romsig->new_psp_directory = BUFF_TO_RUN(ctx, pspdir);
	else
		amd_romsig->psp_directory = BUFF_TO_RUN(ctx, pspdir);

#if PSP_COMBO
	psp_combo_directory *combo_dir = new_combo_dir(&ctx);
	amd_romsig->combo_psp_directory = BUFF_TO_RUN(ctx, combo_dir);
	/* 0 -Compare PSP ID, 1 -Compare chip family ID */
	combo_dir->entries[0].id_sel = 0;
	combo_dir->entries[0].id = get_psp_id(soc_id);
	combo_dir->entries[0].lvl2_addr = BUFF_TO_RUN(ctx, pspdir);

	combo_dir->header.lookup = 1;
	fill_dir_header(combo_dir, 1, PSP2_COOKIE, &ctx);
#endif

	if (have_bios_tables(amd_bios_table)) {
		bios_directory_table *biosdir = NULL;
		if (cb_config.multi_level) {
			/* Do 2nd level BIOS directory followed by 1st */
			bios_directory_table *biosdir2 = NULL;
			bios_directory_table *biosdir2_b = NULL;

			biosdir2 = new_bios_dir(&ctx, cb_config.multi_level);

			integrate_bios_firmwares(&ctx, biosdir2, NULL,
						amd_bios_table, BDT2_COOKIE, &cb_config);
			if (cb_config.recovery_ab) {
				if (pspdir2_b != NULL) {
					biosdir2_b = new_bios_dir(&ctx, cb_config.multi_level);
					integrate_bios_firmwares(&ctx, biosdir2_b, NULL,
						amd_bios_table, BDT2_COOKIE, &cb_config);
				}
				add_psp_firmware_entry(&ctx, pspdir2, biosdir2,
					AMD_FW_BIOS_TABLE, TABLE_ALIGNMENT);
				if (pspdir2_b != NULL)
					add_psp_firmware_entry(&ctx, pspdir2_b, biosdir2_b,
						AMD_FW_BIOS_TABLE, TABLE_ALIGNMENT);
			} else {
				biosdir = new_bios_dir(&ctx, cb_config.multi_level);
				integrate_bios_firmwares(&ctx, biosdir, biosdir2,
						amd_bios_table, BDT1_COOKIE, &cb_config);
			}
		} else {
			/* flat: BDT1 cookie and no pointer to 2nd table */
			biosdir = new_bios_dir(&ctx, cb_config.multi_level);
			integrate_bios_firmwares(&ctx, biosdir, NULL,
						amd_bios_table, BDT1_COOKIE, &cb_config);
		}
		switch (soc_id) {
		case PLATFORM_RENOIR:
		case PLATFORM_LUCIENNE:
		case PLATFORM_CEZANNE:
			if (!cb_config.recovery_ab)
				amd_romsig->bios3_entry = BUFF_TO_RUN(ctx, biosdir);
			break;
		case PLATFORM_MENDOCINO:
		case PLATFORM_SABRINA:
			break;
		case PLATFORM_STONEYRIDGE:
		case PLATFORM_RAVEN:
		case PLATFORM_PICASSO:
		default:
			amd_romsig->bios1_entry = BUFF_TO_RUN(ctx, biosdir);
			break;
		}
	}

	/* Free the filename. */
	free_psp_firmware_filenames(amd_psp_fw_table);
	free_bdt_firmware_filenames(amd_bios_table);

	targetfd = open(output, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (targetfd >= 0) {
		ssize_t bytes;
		bytes = write(targetfd, amd_romsig, ctx.current - romsig_offset);
		if (bytes != ctx.current - romsig_offset) {
			fprintf(stderr, "Error: Writing to file %s failed\n", output);
			retval = 1;
		}
		close(targetfd);
	} else {
		fprintf(stderr, "Error: could not open file: %s\n", output);
		retval = 1;
	}

	free(rom);
	return retval;
}
