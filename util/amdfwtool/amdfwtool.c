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

#ifndef CONFIG_ROM_SIZE
#define CONFIG_ROM_SIZE 0x400000
#endif

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

#define EMBEDDED_FW_SIGNATURE 0x55aa55aa
#define PSP_COOKIE 0x50535024		/* 'PSP$' */
#define PSPL2_COOKIE 0x324c5024		/* '2LP$' */
#define PSP2_COOKIE 0x50535032		/* 'PSP2' */
#define BDT1_COOKIE 0x44484224		/* 'DHB$ */
#define BDT2_COOKIE 0x324c4224		/* '2LB$ */

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

#if defined(__GLIBC__)
typedef unsigned long long int uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
#endif

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
	printf("Usage: amdfwtool [options] -f <size> -o <filename>\n");
	printf("-x | --xhci <FILE>             Add XHCI blob\n");
	printf("-i | --imc <FILE>              Add IMC blob\n");
	printf("-g | --gec <FILE>              Add GEC blob\n");

	printf("\nPSP options:\n");
	printf("-A | --combo-capable           Place PSP directory pointer at Embedded Firmware\n");
	printf("                               offset able to support combo directory\n");
	printf("-M | --multilevel              Generate primary and secondary tables\n");
	printf("-p | --pubkey <FILE>           Add pubkey\n");
	printf("-b | --bootloader <FILE>       Add bootloader\n");
	printf("-S | --subprogram <number>     Sets subprogram field for the next firmware\n");
	printf("-s | --smufirmware <FILE>      Add smufirmware\n");
	printf("-r | --recovery <FILE>         Add recovery\n");
	printf("-k | --rtmpubkey <FILE>        Add rtmpubkey\n");
	printf("-c | --secureos <FILE>         Add secureos\n");
	printf("-n | --nvram <FILE>            Add nvram\n");
	printf("-d | --securedebug <FILE>      Add securedebug\n");
	printf("-t | --trustlets <FILE>        Add trustlets\n");
	printf("-u | --trustletkey <FILE>      Add trustletkey\n");
	printf("-w | --smufirmware2 <FILE>     Add smufirmware2\n");
	printf("-m | --smuscs <FILE>           Add smuscs\n");
	printf("-T | --soft-fuse <HEX_VAL>     Override default soft fuse values\n");
	printf("-z | --abl-image <FILE>        Add AGESA Binary\n");
	printf("-J | --sec-gasket <FILE>       Add security gasket\n");
	printf("-B | --mp2-fw <FILE>           Add MP2 firmware\n");
	printf("-N | --secdebug <FILE>         Add secure unlock image\n");
	printf("-U | --token-unlock            Reserve space for debug token\n");
	printf("-K | --drv-entry-pts <FILE>    Add PSP driver entry points\n");
	printf("-L | --ikek <FILE>             Add Wrapped iKEK\n");
	printf("-Y | --s0i3drv <FILE>          Add s0i3 driver\n");
	printf("-Z | --verstage <FILE>         Add verstage\n");
	printf("\nBIOS options:\n");
	printf("-I | --instance <number>       Sets instance field for the next BIOS firmware\n");
	printf("-a | --apcb <FILE>             Add AGESA PSP customization block\n");
	printf("-Q | --apob-base <HEX_VAL>     Destination for AGESA PSP output block\n");
	printf("-F | --apob-nv-base <HEX_VAL>  Location of S3 resume data\n");
	printf("-H | --apob-nv-size <HEX_VAL>  Size of S3 resume data\n");
	printf("-y | --pmu-inst <FILE>         Add PMU firmware instruction portion\n");
	printf("-G | --pmu-data <FILE>         Add PMU firmware data portion\n");
	printf("-O | --ucode <FILE>            Add microcode patch\n");
	printf("-X | --mp2-config <FILE>       Add MP2 configuration\n");
	printf("-V | --bios-bin <FILE>         Add compressed image; auto source address\n");
	printf("-e | --bios-bin-src <HEX_VAL>  Address in flash of source if -V not used\n");
	printf("-v | --bios-bin-dest <HEX_VAL> Destination for uncompressed BIOS\n");
	printf("-j | --bios-uncomp-size <HEX>  Uncompressed size of BIOS image\n");
	printf("\n-o | --output <filename>     output filename\n");
	printf("-f | --flashsize <HEX_VAL>     ROM size in bytes\n");
	printf("                               size must be larger than %dKB\n",
		MIN_ROM_KB);
	printf("                               and must a multiple of 1024\n");
	printf("-l | --location                Location of Directory\n");
	printf("-q | --anywhere                Use any 64-byte aligned addr for Directory\n");
	printf("-R | --sharedmem               Location of PSP/FW shared memory\n");
	printf("-P | --sharedmem-size          Maximum size of the PSP/FW shared memory area\n");
	printf("-C | --soc-name <socname>      Specify SOC name. Supported names are\n");
	printf("                               Stoneyridge, Raven, Picasso, Renoir or Lucienne");
	printf("-h | --help                    show this help\n");
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
}

typedef enum _amd_bios_type {
	AMD_BIOS_APCB = 0x60,
	AMD_BIOS_APOB = 0x61,
	AMD_BIOS_BIN = 0x62,
	AMD_BIOS_APOB_NV = 0x63,
	AMD_BIOS_PMUI = 0x64,
	AMD_BIOS_PMUD = 0x65,
	AMD_BIOS_UCODE = 0x66,
	AMD_BIOS_APCB_BK = 0x68,
	AMD_BIOS_MP2_CFG = 0x6a,
	AMD_BIOS_PSP_SHARED_MEM = 0x6b,
	AMD_BIOS_L2_PTR =  0x70,
	AMD_BIOS_INVALID,
} amd_bios_type;

#define BDT_LVL1 0x1
#define BDT_LVL2 0x2
#define BDT_BOTH (BDT_LVL1 | BDT_LVL2)
typedef struct _amd_bios_entry {
	amd_bios_type type;
	int region_type;
	int reset;
	int copy;
	int ro;
	int zlib;
	int inst;
	int subpr;
	uint64_t src;
	uint64_t dest;
	size_t size;
	char *filename;
	int level;
} amd_bios_entry;

typedef enum _amd_fw_type {
	AMD_FW_PSP_PUBKEY = 0,
	AMD_FW_PSP_BOOTLOADER = 1,
	AMD_FW_PSP_SMU_FIRMWARE = 8,
	AMD_FW_PSP_RECOVERY = 3,
	AMD_FW_PSP_RTM_PUBKEY = 5,
	AMD_FW_PSP_SECURED_OS = 2,
	AMD_FW_PSP_NVRAM = 4,
	AMD_FW_PSP_SECURED_DEBUG = 9,
	AMD_FW_PSP_TRUSTLETS = 12,
	AMD_FW_PSP_TRUSTLETKEY = 13,
	AMD_FW_PSP_SMU_FIRMWARE2 = 18,
	AMD_PSP_FUSE_CHAIN = 11,
	AMD_FW_PSP_SMUSCS = 95,
	AMD_DEBUG_UNLOCK = 0x13,
	AMD_WRAPPED_IKEK = 0x21,
	AMD_TOKEN_UNLOCK = 0x22,
	AMD_SEC_GASKET = 0x24,
	AMD_MP2_FW = 0x25,
	AMD_DRIVER_ENTRIES = 0x28,
	AMD_S0I3_DRIVER = 0x2d,
	AMD_ABL0 = 0x30,
	AMD_ABL1 = 0x31,
	AMD_ABL2 = 0x32,
	AMD_ABL3 = 0x33,
	AMD_ABL4 = 0x34,
	AMD_ABL5 = 0x35,
	AMD_ABL6 = 0x36,
	AMD_ABL7 = 0x37,
	AMD_FW_PSP_WHITELIST = 0x3a,
	AMD_FW_L2_PTR = 0x40,
	AMD_FW_PSP_VERSTAGE = 0x52,
	AMD_FW_VERSTAGE_SIG = 0x53,
	AMD_FW_IMC,
	AMD_FW_GEC,
	AMD_FW_XHCI,
	AMD_FW_INVALID,
} amd_fw_type;

#define PSP_LVL1 0x1
#define PSP_LVL2 0x2
#define PSP_BOTH (PSP_LVL1 | PSP_LVL2)
typedef struct _amd_fw_entry {
	amd_fw_type type;
	uint8_t subprog;
	char *filename;
	int level;
	uint64_t other;
} amd_fw_entry;

static amd_fw_entry amd_psp_fw_table[] = {
	{ .type = AMD_FW_PSP_PUBKEY, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_BOOTLOADER, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 0, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_RECOVERY, .level = PSP_LVL1 },
	{ .type = AMD_FW_PSP_RTM_PUBKEY, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SECURED_OS, .level = PSP_LVL2 },
	{ .type = AMD_FW_PSP_NVRAM, .level = PSP_LVL2 },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 2, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SECURED_DEBUG, .level = PSP_LVL2 },
	{ .type = AMD_FW_PSP_TRUSTLETS, .level = PSP_LVL2 },
	{ .type = AMD_FW_PSP_TRUSTLETKEY, .level = PSP_LVL2 },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 2, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 1, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 1, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMUSCS, .level = PSP_BOTH  },
	{ .type = AMD_PSP_FUSE_CHAIN, .level = PSP_LVL2 },
	{ .type = AMD_DEBUG_UNLOCK, .level = PSP_LVL2 },
	{ .type = AMD_WRAPPED_IKEK, .level = PSP_BOTH },
	{ .type = AMD_TOKEN_UNLOCK, .level = PSP_BOTH },
	{ .type = AMD_SEC_GASKET, .subprog = 2, .level = PSP_BOTH },
	{ .type = AMD_SEC_GASKET, .subprog = 1, .level = PSP_BOTH },
	{ .type = AMD_MP2_FW, .subprog = 2, .level = PSP_LVL2 },
	{ .type = AMD_MP2_FW, .subprog = 1, .level = PSP_LVL2 },
	{ .type = AMD_DRIVER_ENTRIES, .level = PSP_LVL2 },
	{ .type = AMD_S0I3_DRIVER, .level = PSP_LVL2 },
	{ .type = AMD_ABL0, .level = PSP_BOTH },
	{ .type = AMD_ABL1, .level = PSP_BOTH },
	{ .type = AMD_ABL2, .level = PSP_BOTH },
	{ .type = AMD_ABL3, .level = PSP_BOTH },
	{ .type = AMD_ABL4, .level = PSP_BOTH },
	{ .type = AMD_ABL5, .level = PSP_BOTH },
	{ .type = AMD_ABL6, .level = PSP_BOTH },
	{ .type = AMD_ABL7, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE, .subprog = 1, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_SMU_FIRMWARE2, .subprog = 1, .level = PSP_BOTH },
	{ .type = AMD_FW_PSP_WHITELIST, .level = PSP_LVL2 },
	{ .type = AMD_FW_PSP_VERSTAGE, .level = PSP_BOTH },
	{ .type = AMD_FW_VERSTAGE_SIG, .level = PSP_BOTH },
	{ .type = AMD_FW_INVALID },
};

static amd_fw_entry amd_fw_table[] = {
	{ .type = AMD_FW_XHCI },
	{ .type = AMD_FW_IMC },
	{ .type = AMD_FW_GEC },
	{ .type = AMD_FW_INVALID },
};

static amd_bios_entry amd_bios_table[] = {
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
	{ .type = AMD_BIOS_PMUI, .inst = 4, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 4, .subpr = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 1, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 1, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUI, .inst = 4, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_PMUD, .inst = 4, .subpr = 1, .level = BDT_BOTH },
	{ .type = AMD_BIOS_UCODE, .inst = 0, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 1, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_UCODE, .inst = 2, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_MP2_CFG, .level = BDT_LVL2 },
	{ .type = AMD_BIOS_PSP_SHARED_MEM, .inst = 0, .level = BDT_BOTH },
	{ .type = AMD_BIOS_INVALID },
};

typedef struct _embedded_firmware {
	uint32_t signature; /* 0x55aa55aa */
	uint32_t imc_entry;
	uint32_t gec_entry;
	uint32_t xhci_entry;
	uint32_t psp_entry;
	uint32_t comboable;
	uint32_t bios0_entry; /* todo: add way to select correct entry */
	uint32_t bios1_entry;
	uint32_t bios2_entry;
	uint32_t second_gen_efs;
	uint32_t bios3_entry;
	uint32_t reserved_2Ch;
	uint32_t promontory_fw_ptr;
	uint32_t lp_promontory_fw_ptr;
	uint32_t reserved_38h;
	uint32_t reserved_3Ch;
	uint8_t spi_readmode_f15_mod_60_6f;
	uint8_t fast_speed_new_f15_mod_60_6f;
	uint8_t reserved_42h;
	uint8_t spi_readmode_f17_mod_00_2f;
	uint8_t spi_fastspeed_f17_mod_00_2f;
	uint8_t qpr_dummy_cycle_f17_mod_00_2f;
	uint8_t reserved_46h;
	uint8_t spi_readmode_f17_mod_30_3f;
	uint8_t spi_fastspeed_f17_mod_30_3f;
	uint8_t micron_detect_f17_mod_30_3f;
	uint8_t reserved_4Ah;
	uint8_t reserved_4Bh;
	uint32_t reserved_4Ch;
} __attribute__((packed, aligned(16))) embedded_firmware;

typedef struct _psp_directory_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t reserved;
} __attribute__((packed, aligned(16))) psp_directory_header;

typedef struct _psp_directory_entry {
	uint8_t type;
	uint8_t subprog;
	uint16_t rsvd;
	uint32_t size;
	uint64_t addr; /* or a value in some cases */
} __attribute__((packed)) psp_directory_entry;

typedef struct _psp_directory_table {
	psp_directory_header header;
	psp_directory_entry entries[];
} __attribute__((packed)) psp_directory_table;

#define MAX_PSP_ENTRIES 0x1f

typedef struct _psp_combo_header {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t lookup;
	uint64_t reserved[2];
} __attribute__((packed, aligned(16))) psp_combo_header;

typedef struct _psp_combo_entry {
	uint32_t id_sel;
	uint32_t id;
	uint64_t lvl2_addr;
} __attribute__((packed)) psp_combo_entry;

typedef struct _psp_combo_directory {
	psp_combo_header header;
	psp_combo_entry entries[];
} __attribute__((packed)) psp_combo_directory;

#define MAX_COMBO_ENTRIES 1

typedef struct _bios_directory_hdr {
	uint32_t cookie;
	uint32_t checksum;
	uint32_t num_entries;
	uint32_t reserved;
} __attribute__((packed, aligned(16))) bios_directory_hdr;

typedef struct _bios_directory_entry {
	uint8_t type;
	uint8_t region_type;
	int reset:1;
	int copy:1;
	int ro:1;
	int compressed:1;
	int inst:4;
	uint8_t subprog; /* b[7:3] reserved */
	uint32_t size;
	uint64_t source;
	uint64_t dest;
} __attribute__((packed)) bios_directory_entry;

typedef struct _bios_directory_table {
	bios_directory_hdr header;
	bios_directory_entry entries[];
} bios_directory_table;

#define MAX_BIOS_ENTRIES 0x2f

typedef struct _context {
	char *rom;		/* target buffer, size of flash device */
	uint32_t rom_size;	/* size of flash device */
	uint32_t current;	/* pointer within flash & proxy buffer */
} context;

#define RUN_BASE(ctx) (0xFFFFFFFF - (ctx).rom_size + 1)
#define RUN_OFFSET(ctx, offset) (RUN_BASE(ctx) + (offset))
#define RUN_CURRENT(ctx) RUN_OFFSET((ctx), (ctx).current)
#define BUFF_OFFSET(ctx, offset) ((void *)((ctx).rom + (offset)))
#define BUFF_CURRENT(ctx) BUFF_OFFSET((ctx), (ctx).current)
#define BUFF_TO_RUN(ctx, ptr) RUN_OFFSET((ctx), ((char *)(ptr) - (ctx).rom))
#define BUFF_ROOM(ctx) ((ctx).rom_size - (ctx).current)

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
	ctx->current += sizeof(psp_directory_header)
			+ MAX_PSP_ENTRIES * sizeof(psp_directory_entry);
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

static void fill_dir_header(void *directory, uint32_t count, uint32_t cookie)
{
	psp_combo_directory *cdir = directory;
	psp_directory_table *dir = directory;
	bios_directory_table *bdir = directory;

	if (!count)
		return;

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
		dir->header.cookie = cookie;
		dir->header.num_entries = count;
		dir->header.reserved = 0;
		/* checksum everything that comes after the Checksum field */
		dir->header.checksum = fletcher32(&dir->header.num_entries,
					count * sizeof(psp_directory_entry)
					+ sizeof(dir->header.num_entries)
					+ sizeof(dir->header.reserved));
		break;
	case BDT1_COOKIE:
	case BDT2_COOKIE:
		bdir->header.cookie = cookie;
		bdir->header.num_entries = count;
		bdir->header.reserved = 0;
		/* checksum everything that comes after the Checksum field */
		bdir->header.checksum = fletcher32(&bdir->header.num_entries,
					count * sizeof(bios_directory_entry)
					+ sizeof(bdir->header.num_entries)
					+ sizeof(bdir->header.reserved));
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
		printf("Error opening file: %s: %s\n",
		       src_file, strerror(errno));
		return -1;
	}

	if (fstat(fd, &fd_stat)) {
		printf("fstat error: %s\n", strerror(errno));
		close(fd);
		return -2;
	}

	if ((size_t)fd_stat.st_size > room) {
		printf("Error: %s will not fit.  Exiting.\n", src_file);
		close(fd);
		return -3;
	}

	bytes = read(fd, dest, (size_t)fd_stat.st_size);
	close(fd);
	if (bytes != (ssize_t)fd_stat.st_size) {
		printf("Error while reading %s\n", src_file);
		return -4;
	}

	return bytes;
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

static void integrate_psp_firmwares(context *ctx,
					psp_directory_table *pspdir,
					psp_directory_table *pspdir2,
					amd_fw_entry *fw_table,
					uint32_t cookie)
{
	ssize_t bytes;
	unsigned int i, count;
	int level;

	/* This function can create a primary table, a secondary table, or a
	 * flattened table which contains all applicable types.  These if-else
	 * statements infer what the caller intended.  If a 2nd-level cookie
	 * is passed, clearly a 2nd-level table is intended.  However, a
	 * 1st-level cookie may indicate level 1 or flattened.  If the caller
	 * passes a pointer to a 2nd-level table, then assume not flat.
	 */
	if (cookie == PSPL2_COOKIE)
		level = PSP_LVL2;
	else if (pspdir2)
		level = PSP_LVL1;
	else
		level = PSP_BOTH;

	ctx->current = ALIGN(ctx->current, BLOB_ALIGNMENT);

	for (i = 0, count = 0; fw_table[i].type != AMD_FW_INVALID; i++) {
		if (!(fw_table[i].level & level))
			continue;

		if (fw_table[i].type == AMD_TOKEN_UNLOCK) {
			if (!fw_table[i].other)
				continue;
			ctx->current = ALIGN(ctx->current, ERASE_ALIGNMENT);
			pspdir->entries[count].type = fw_table[i].type;
			pspdir->entries[count].size = 4096; /* TODO: doc? */
			pspdir->entries[count].addr = RUN_CURRENT(*ctx);
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
			pspdir->entries[count].addr = RUN_CURRENT(*ctx);

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

			ctx->current = ALIGN(ctx->current + bytes,
							BLOB_ALIGNMENT);
			count++;
		} else {
			/* This APU doesn't have this firmware. */
		}
	}

	if (pspdir2) {
		pspdir->entries[count].type = AMD_FW_L2_PTR;
		pspdir->entries[count].subprog = 0;
		pspdir->entries[count].rsvd = 0;
		pspdir->entries[count].size = sizeof(pspdir2->header)
					+ pspdir2->header.num_entries
					* sizeof(psp_directory_entry);

		pspdir->entries[count].addr = BUFF_TO_RUN(*ctx, pspdir2);
		count++;
	}

	if (count > MAX_PSP_ENTRIES) {
		printf("Error: PSP entries exceed max allowed items\n");
		free(ctx->rom);
		exit(1);
	}

	fill_dir_header(pspdir, count, cookie);
}

static void *new_bios_dir(context *ctx, int multi)
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
					uint32_t cookie)
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
	if (cookie == BDT2_COOKIE)
		level = BDT_LVL2;
	else if (biosdir2)
		level = BDT_LVL1;
	else
		level = BDT_BOTH;

	ctx->current = ALIGN(ctx->current, BLOB_ALIGNMENT);

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
				printf("Error: APOB NV address provided, but no size\n");
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
			printf("Error: APOB destination not provided\n");
			free(ctx->rom);
			exit(1);
		}

		/* BIOS binary must have destination and uncompressed size.  If
		 * no filename given, then user must provide a source address.
		 */
		if (fw_table[i].type == AMD_BIOS_BIN) {
			if (!fw_table[i].dest || !fw_table[i].size) {
				printf("Error: BIOS binary destination and uncompressed size are required\n");
				free(ctx->rom);
				exit(1);
			}
			if (!fw_table[i].filename && !fw_table[i].src) {
				printf("Error: BIOS binary assumed outside amdfw.rom but no source address given\n");
				free(ctx->rom);
				exit(1);
			}
		}

		/* PSP_SHARED_MEM needs a destination and size */
		if (fw_table[i].type == AMD_BIOS_PSP_SHARED_MEM &&
				(!fw_table[i].dest || !fw_table[i].size))
			continue;

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
			break;
		case AMD_BIOS_APOB_NV:
			if (fw_table[i].src) {
				/* If source is given, use that and its size */
				biosdir->entries[count].source = fw_table[i].src;
				biosdir->entries[count].size = fw_table[i].size;
			} else {
				/* Else reserve size bytes within amdfw.rom */
				ctx->current = ALIGN(ctx->current, ERASE_ALIGNMENT);
				biosdir->entries[count].source = RUN_CURRENT(*ctx);
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
				biosdir->entries[count].size = size;
				break;
			}

			/* level 2, or level 1 and no copy found in level 2 */
			biosdir->entries[count].source = fw_table[i].src;
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

			biosdir->entries[count].source = RUN_CURRENT(*ctx);

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

			ctx->current = ALIGN(ctx->current + bytes, 0x100U);
			break;
		}

		count++;
	}

	if (biosdir2) {
		biosdir->entries[count].type = AMD_BIOS_L2_PTR;
		biosdir->entries[count].size =
					+ MAX_BIOS_ENTRIES
					* sizeof(bios_directory_entry);
		biosdir->entries[count].source =
					BUFF_TO_RUN(*ctx, biosdir2);
		biosdir->entries[count].subprog = 0;
		biosdir->entries[count].inst = 0;
		biosdir->entries[count].copy = 0;
		biosdir->entries[count].compressed = 0;
		biosdir->entries[count].dest = -1;
		biosdir->entries[count].reset = 0;
		biosdir->entries[count].ro = 0;
		count++;
	}

	if (count > MAX_BIOS_ENTRIES) {
		printf("Error: BIOS entries (%d) exceeds max allowed items "
			"(%d)\n", count, MAX_BIOS_ENTRIES);
		free(ctx->rom);
		exit(1);
	}

	fill_dir_header(biosdir, count, cookie);
}

enum {
	/* begin after ASCII characters */
	LONGOPT_SPI_READ_MODE	= 256,
	LONGOPT_SPI_SPEED	= 257,
	LONGOPT_SPI_MICRON_FLAG	= 258,
};

// Unused values: D
static const char *optstring  = "x:i:g:AMS:p:b:s:r:k:c:n:d:t:u:w:m:T:z:J:B:K:L:Y:N:UW:I:a:Q:V:e:v:j:y:G:O:X:F:H:o:f:l:hZ:qR:P:C:E:";

static struct option long_options[] = {
	{"xhci",             required_argument, 0, 'x' },
	{"imc",              required_argument, 0, 'i' },
	{"gec",              required_argument, 0, 'g' },
	/* PSP Directory Table items */
	{"combo-capable",          no_argument, 0, 'A' },
	{"multilevel",             no_argument, 0, 'M' },
	{"subprogram",       required_argument, 0, 'S' },
	{"pubkey",           required_argument, 0, 'p' },
	{"bootloader",       required_argument, 0, 'b' },
	{"smufirmware",      required_argument, 0, 's' },
	{"recovery",         required_argument, 0, 'r' },
	{"rtmpubkey",        required_argument, 0, 'k' },
	{"secureos",         required_argument, 0, 'c' },
	{"nvram",            required_argument, 0, 'n' },
	{"securedebug",      required_argument, 0, 'd' },
	{"trustlets",        required_argument, 0, 't' },
	{"trustletkey",      required_argument, 0, 'u' },
	{"smufirmware2",     required_argument, 0, 'w' },
	{"smuscs",           required_argument, 0, 'm' },
	{"soft-fuse",        required_argument, 0, 'T' },
	{"abl-image",        required_argument, 0, 'z' },
	{"sec-gasket",       required_argument, 0, 'J' },
	{"mp2-fw",           required_argument, 0, 'B' },
	{"drv-entry-pts",    required_argument, 0, 'K' },
	{"ikek",             required_argument, 0, 'L' },
	{"s0i3drv",          required_argument, 0, 'Y' },
	{"secdebug",         required_argument, 0, 'N' },
	{"token-unlock",           no_argument, 0, 'U' },
	{"whitelist",        required_argument, 0, 'W' },
	{"verstage",         required_argument, 0, 'Z' },
	{"verstage_sig",     required_argument, 0, 'E' },
	/* BIOS Directory Table items */
	{"instance",         required_argument, 0, 'I' },
	{"apcb",             required_argument, 0, 'a' },
	{"apob-base",        required_argument, 0, 'Q' },
	{"bios-bin",         required_argument, 0, 'V' },
	{"bios-bin-src",     required_argument, 0, 'e' },
	{"bios-bin-dest",    required_argument, 0, 'v' },
	{"bios-uncomp-size", required_argument, 0, 'j' },
	{"pmu-inst",         required_argument, 0, 'y' },
	{"pmu-data",         required_argument, 0, 'G' },
	{"ucode",            required_argument, 0, 'O' },
	{"mp2-config",       required_argument, 0, 'X' },
	{"apob-nv-base",     required_argument, 0, 'F' },
	{"apob-nv-size",     required_argument, 0, 'H' },
	/* Embedded Firmware Structure items*/
	{"spi-read-mode",    required_argument, 0, LONGOPT_SPI_READ_MODE },
	{"spi-speed",        required_argument, 0, LONGOPT_SPI_SPEED },
	{"spi-micron-flag",  required_argument, 0, LONGOPT_SPI_MICRON_FLAG },
	/* other */
	{"output",           required_argument, 0, 'o' },
	{"flashsize",        required_argument, 0, 'f' },
	{"location",         required_argument, 0, 'l' },
	{"anywhere",         no_argument,       0, 'q' },
	{"sharedmem",        required_argument, 0, 'R' },
	{"sharedmem-size",   required_argument, 0, 'P' },
	{"soc-name",         required_argument, 0, 'C' },
	{"help",             no_argument,       0, 'h' },
	{NULL,               0,                 0,  0  }
};

static void register_fw_fuse(char *str)
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

enum platform {
	PLATFORM_UNKNOWN,
	PLATFORM_STONEYRIDGE,
	PLATFORM_RAVEN,
	PLATFORM_PICASSO,
	PLATFORM_RENOIR,
	PLATFORM_LUCIENNE,
};

static int set_efs_table(uint8_t soc_id, embedded_firmware *amd_romsig,
			 uint8_t efs_spi_readmode, uint8_t efs_spi_speed,
			 uint8_t efs_spi_micron_flag)
{
	if ((efs_spi_readmode == 0xFF) || (efs_spi_speed == 0xFF)) {
		printf("Error: EFS read mode and SPI speed must be set\n");
		return 1;
	}
	switch (soc_id) {
	case PLATFORM_STONEYRIDGE:
		amd_romsig->second_gen_efs = 0;
		amd_romsig->spi_readmode_f15_mod_60_6f = efs_spi_readmode;
		amd_romsig->fast_speed_new_f15_mod_60_6f = efs_spi_speed;
		break;
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
		amd_romsig->second_gen_efs = 0;
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
			printf("Error: EFS Micron flag must be correctly set.\n\n");
			return 1;
		}
		break;
	case PLATFORM_RENOIR:
	case PLATFORM_LUCIENNE:
		amd_romsig->second_gen_efs = 1;
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
			printf("Error: EFS Micron flag must be correctly set.\n\n");
			return 1;
		}
		break;
	case PLATFORM_UNKNOWN:
	default:
		printf("Error: Invalid SOC name.\n\n");
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
	else if (!strcasecmp(soc_name, "Renoir"))
		return PLATFORM_RENOIR;
	else if (!strcasecmp(soc_name, "Lucienne"))
		return PLATFORM_LUCIENNE;
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
	psp_directory_table *pspdir;
	int comboable = 0;
	int fuse_defined = 0;
	int targetfd;
	char *output = NULL;
	context ctx = {
		.rom_size = CONFIG_ROM_SIZE,
	};
	/* Values cleared after each firmware or parameter, regardless if N/A */
	uint8_t sub = 0, instance = 0;
	int abl_image = 0;
	uint32_t dir_location = 0;
	bool any_location = 0;
	uint32_t romsig_offset;
	uint32_t rom_base_address;
	uint8_t soc_id = PLATFORM_UNKNOWN;
	uint8_t efs_spi_readmode = 0xff;
	uint8_t efs_spi_speed = 0xff;
	uint8_t efs_spi_micron_flag = 0xff;

	int multi = 0;

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'x':
			register_fw_filename(AMD_FW_XHCI, sub, optarg);
			sub = instance = 0;
			break;
		case 'i':
			register_fw_filename(AMD_FW_IMC, sub, optarg);
			sub = instance = 0;
			break;
		case 'g':
			register_fw_filename(AMD_FW_GEC, sub, optarg);
			sub = instance = 0;
			break;
		case 'A':
			comboable = 1;
			break;
		case 'M':
			multi = 1;
			break;
		case 'U':
			register_fw_token_unlock();
			sub = instance = 0;
			break;
		case 'S':
			sub = (uint8_t)strtoul(optarg, &tmp, 16);
			break;
		case 'I':
			instance = strtoul(optarg, &tmp, 16);
			break;
		case 'p':
			register_fw_filename(AMD_FW_PSP_PUBKEY, sub, optarg);
			sub = instance = 0;
			break;
		case 'b':
			register_fw_filename(AMD_FW_PSP_BOOTLOADER,
								sub, optarg);
			sub = instance = 0;
			break;
		case 's':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE,
								sub, optarg);
			sub = instance = 0;
			break;
		case 'r':
			register_fw_filename(AMD_FW_PSP_RECOVERY, sub, optarg);
			sub = instance = 0;
			break;
		case 'k':
			register_fw_filename(AMD_FW_PSP_RTM_PUBKEY,
								sub, optarg);
			sub = instance = 0;
			break;
		case 'c':
			register_fw_filename(AMD_FW_PSP_SECURED_OS,
								sub, optarg);
			sub = instance = 0;
			break;
		case 'n':
			register_fw_filename(AMD_FW_PSP_NVRAM, sub, optarg);
			sub = instance = 0;
			break;
		case 'd':
			register_fw_filename(AMD_FW_PSP_SECURED_DEBUG,
								sub, optarg);
			sub = instance = 0;
			break;
		case 't':
			register_fw_filename(AMD_FW_PSP_TRUSTLETS, sub, optarg);
			sub = instance = 0;
			break;
		case 'u':
			register_fw_filename(AMD_FW_PSP_TRUSTLETKEY,
								sub, optarg);
			sub = instance = 0;
			break;
		case 'w':
			register_fw_filename(AMD_FW_PSP_SMU_FIRMWARE2,
								sub, optarg);
			sub = instance = 0;
			break;
		case 'm':
			register_fw_filename(AMD_FW_PSP_SMUSCS, sub, optarg);
			sub = instance = 0;
			break;
		case 'T':
			register_fw_fuse(optarg);
			fuse_defined = 1;
			sub = 0;
			break;
		case 'a':
			register_bdt_data(AMD_BIOS_APCB, sub, instance, optarg);
			register_bdt_data(AMD_BIOS_APCB_BK, sub,
							instance, optarg);
			sub = instance = 0;
			break;
		case 'Q':
			/* APOB destination */
			register_fw_addr(AMD_BIOS_APOB, 0, optarg, 0);
			sub = instance = 0;
			break;
		case 'F':
			/* APOB NV source */
			register_fw_addr(AMD_BIOS_APOB_NV, optarg, 0, 0);
			sub = instance = 0;
			break;
		case 'H':
			/* APOB NV size */
			register_fw_addr(AMD_BIOS_APOB_NV, 0, 0, optarg);
			sub = instance = 0;
			break;
		case 'V':
			register_bdt_data(AMD_BIOS_BIN, sub, instance, optarg);
			sub = instance = 0;
			break;
		case 'e':
			/* BIOS source */
			register_fw_addr(AMD_BIOS_BIN, optarg, 0, 0);
			sub = instance = 0;
			break;
		case 'v':
			/* BIOS destination */
			register_fw_addr(AMD_BIOS_BIN, 0, optarg, 0);
			sub = instance = 0;
			break;
		case 'j':
			/* BIOS destination size */
			register_fw_addr(AMD_BIOS_BIN, 0, 0, optarg);
			sub = instance = 0;
			break;
		case 'y':
			register_bdt_data(AMD_BIOS_PMUI, sub, instance, optarg);
			sub = instance = 0;
			break;
		case 'G':
			register_bdt_data(AMD_BIOS_PMUD, sub, instance, optarg);
			sub = instance = 0;
			break;
		case 'O':
			register_bdt_data(AMD_BIOS_UCODE, sub,
							instance, optarg);
			sub = instance = 0;
			break;
		case 'J':
			register_fw_filename(AMD_SEC_GASKET, sub, optarg);
			sub = instance = 0;
			break;
		case 'B':
			register_fw_filename(AMD_MP2_FW, sub, optarg);
			sub = instance = 0;
			break;
		case 'z':
			register_fw_filename(AMD_ABL0 + abl_image++,
								sub, optarg);
			sub = instance = 0;
			break;
		case 'X':
			register_bdt_data(AMD_BIOS_MP2_CFG, sub,
							instance, optarg);
			sub = instance = 0;
			break;
		case 'K':
			register_fw_filename(AMD_DRIVER_ENTRIES, sub, optarg);
			sub = instance = 0;
			break;
		case 'L':
			register_fw_filename(AMD_WRAPPED_IKEK, sub, optarg);
			sub = instance = 0;
			break;
		case 'Y':
			register_fw_filename(AMD_S0I3_DRIVER, sub, optarg);
			sub = instance = 0;
			break;
		case 'N':
			register_fw_filename(AMD_DEBUG_UNLOCK, sub, optarg);
			sub = instance = 0;
			break;
		case 'W':
			register_fw_filename(AMD_FW_PSP_WHITELIST, sub, optarg);
			sub = instance = 0;
			break;
		case 'Z':
			register_fw_filename(AMD_FW_PSP_VERSTAGE, sub, optarg);
			sub = instance = 0;
			break;
		case 'E':
			register_fw_filename(AMD_FW_VERSTAGE_SIG, sub, optarg);
			sub = instance = 0;
			break;
		case 'C':
			soc_id = identify_platform(optarg);
			if (soc_id == PLATFORM_UNKNOWN) {
				printf("Error: Invalid SOC name specified\n\n");
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
		case 'o':
			output = optarg;
			break;
		case 'f':
			ctx.rom_size = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				printf("Error: ROM size specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;
		case 'l':
			dir_location = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				printf("Error: Directory Location specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;
		case 'q':
			any_location = 1;
			break;
		case 'R':
			/* shared memory destination */
			register_fw_addr(AMD_BIOS_PSP_SHARED_MEM, 0, optarg, 0);
			sub = instance = 0;
			break;
		case 'P':
			/* shared memory size */
			register_fw_addr(AMD_BIOS_PSP_SHARED_MEM, NULL, NULL, optarg);
			sub = instance = 0;
			break;

		case 'h':
			usage();
			return 0;
		default:
			break;
		}
	}

	if (!fuse_defined)
		register_fw_fuse(DEFAULT_SOFT_FUSE_CHAIN);

	if (!output) {
		printf("Error: Output value is not specified.\n\n");
		retval = 1;
	}

	if (ctx.rom_size % 1024 != 0) {
		printf("Error: ROM Size (%d bytes) should be a multiple of"
			" 1024 bytes.\n\n", ctx.rom_size);
		retval = 1;
	}

	if (ctx.rom_size < MIN_ROM_KB * 1024) {
		printf("Error: ROM Size (%dKB) must be at least %dKB.\n\n",
			ctx.rom_size / 1024, MIN_ROM_KB);
		retval = 1;
	}

	if (retval) {
		usage();
		return retval;
	}

	printf("    AMDFWTOOL  Using ROM size of %dKB\n", ctx.rom_size / 1024);

	rom_base_address = 0xFFFFFFFF - ctx.rom_size + 1;
	if (dir_location && (dir_location < rom_base_address)) {
		printf("Error: Directory location outside of ROM.\n\n");
		return 1;
	}

	if (any_location) {
		if (dir_location & 0x3f) {
			printf("Error: Invalid Directory location.\n");
			printf("  Valid locations are 64-byte aligned\n");
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
			printf("Error: Invalid Directory location.\n");
			printf("  Valid locations are 0xFFFA0000, 0xFFF20000,\n");
			printf("  0xFFE20000, 0xFFC20000, 0xFF820000, 0xFF020000\n");
			return 1;
		}
	}

	ctx.rom = malloc(ctx.rom_size);
	if (!ctx.rom) {
		printf("Error: Failed to allocate memory\n");
		return 1;
	}
	memset(ctx.rom, 0xFF, ctx.rom_size);

	if (dir_location)
		romsig_offset = ctx.current = dir_location - rom_base_address;
	else
		romsig_offset = ctx.current = AMD_ROMSIG_OFFSET;
	printf("    AMDFWTOOL  Using firmware directory location of 0x%08x\n",
			RUN_CURRENT(ctx));

	amd_romsig = BUFF_OFFSET(ctx, romsig_offset);
	amd_romsig->signature = EMBEDDED_FW_SIGNATURE;
	amd_romsig->imc_entry = 0;
	amd_romsig->gec_entry = 0;
	amd_romsig->xhci_entry = 0;

	if (soc_id != PLATFORM_UNKNOWN) {
		retval = set_efs_table(soc_id, amd_romsig, efs_spi_readmode,
					efs_spi_speed, efs_spi_micron_flag);
		if (retval) {
			printf("ERROR: Failed to initialize EFS table!\n");
			return retval;
		}
	} else {
		printf("WARNING: No SOC name specified.\n");
	}

	integrate_firmwares(&ctx, amd_romsig, amd_fw_table);

	ctx.current = ALIGN(ctx.current, 0x10000U); /* TODO: is it necessary? */

	if (multi) {
		/* Do 2nd PSP directory followed by 1st */
		psp_directory_table *pspdir2 = new_psp_dir(&ctx, multi);
		integrate_psp_firmwares(&ctx, pspdir2, 0,
						amd_psp_fw_table, PSPL2_COOKIE);

		pspdir = new_psp_dir(&ctx, multi);
		integrate_psp_firmwares(&ctx, pspdir, pspdir2,
						amd_psp_fw_table, PSP_COOKIE);
	} else {
		/* flat: PSP 1 cookie and no pointer to 2nd table */
		pspdir = new_psp_dir(&ctx, multi);
		integrate_psp_firmwares(&ctx, pspdir, 0,
						amd_psp_fw_table, PSP_COOKIE);
	}

	if (comboable)
		amd_romsig->comboable = BUFF_TO_RUN(ctx, pspdir);
	else
		amd_romsig->psp_entry = BUFF_TO_RUN(ctx, pspdir);

#if PSP_COMBO
	psp_combo_directory *combo_dir = new_combo_dir(&ctx);
	amd_romsig->comboable = BUFF_TO_RUN(ctx, combo_dir);
	/* 0 -Compare PSP ID, 1 -Compare chip family ID */
	combo_dir->entries[0].id_sel = 0;
	/* TODO: PSP ID. Documentation is needed. */
	combo_dir->entries[0].id = 0x10220B00;
	combo_dir->entries[0].lvl2_addr = BUFF_TO_RUN(ctx, pspdir);

	combo_dir->header.lookup = 1;
	fill_dir_header(combo_dir, 1, PSP2_COOKIE);
#endif

	if (have_bios_tables(amd_bios_table)) {
		bios_directory_table *biosdir;
		if (multi) {
			/* Do 2nd level BIOS directory followed by 1st */
			bios_directory_table *biosdir2 =
						new_bios_dir(&ctx, multi);
			integrate_bios_firmwares(&ctx, biosdir2, 0,
						amd_bios_table, BDT2_COOKIE);

			biosdir = new_bios_dir(&ctx, multi);
			integrate_bios_firmwares(&ctx, biosdir, biosdir2,
						amd_bios_table, BDT1_COOKIE);
		} else {
			/* flat: BDT1 cookie and no pointer to 2nd table */
			biosdir = new_bios_dir(&ctx, multi);
			integrate_bios_firmwares(&ctx, biosdir, 0,
						amd_bios_table, BDT1_COOKIE);
		}
		amd_romsig->bios1_entry = BUFF_TO_RUN(ctx, biosdir);
	}

	targetfd = open(output, O_RDWR | O_CREAT | O_TRUNC, 0666);
	if (targetfd >= 0) {
		write(targetfd, amd_romsig, ctx.current - romsig_offset);
		close(targetfd);
	} else {
		printf("Error: could not open file: %s\n", output);
		retval = 1;
	}

	free(rom);
	return retval;
}
