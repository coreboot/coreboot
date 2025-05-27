/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "amdfwtool.h"

#define MIN_ROM_KB		256
#define MAX_MAPPED_WINDOW	(16 * MiB)
#define MAX_MAPPED_WINDOW_MASK	(MAX_MAPPED_WINDOW - 1)

#define DEFAULT_SOFT_FUSE_CHAIN "0x1"

enum {
	AMDFW_OPT_CONFIG =	'c',
	AMDFW_OPT_DEBUG =	'd',
	AMDFW_OPT_HELP =	'h',

	AMDFW_OPT_XHCI = 128,
	AMDFW_OPT_IMC,
	AMDFW_OPT_GEC,
	AMDFW_OPT_RECOVERY_AB,
	AMDFW_OPT_RECOVERY_AB_SINGLE_COPY,
	AMDFW_OPT_USE_COMBO,
	AMDFW_OPT_COMBO1_CONFIG,
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
	AMDFW_OPT_OUTPUT_MANIFEST,

	AMDFW_OPT_INSTANCE,
	AMDFW_OPT_APCB,
	AMDFW_OPT_APCB_COMBO1,
	AMDFW_OPT_APOBBASE,
	AMDFW_OPT_BIOSBIN,
	AMDFW_OPT_BIOSBIN_SOURCE,
	AMDFW_OPT_BIOSBIN_DEST,
	AMDFW_OPT_BIOS_UNCOMP_SIZE,
	AMDFW_OPT_BIOSBIN_UNCOMP,
	AMDFW_OPT_UCODE,
	AMDFW_OPT_APOB_NVBASE,
	AMDFW_OPT_APOB_NVSIZE,

	AMDFW_OPT_OUTPUT,
	AMDFW_OPT_FLASHSIZE,
	AMDFW_OPT_LOCATION,
	AMDFW_OPT_ANYWHERE,
	AMDFW_OPT_SHAREDMEM,
	AMDFW_OPT_SHAREDMEM_SIZE,
	AMDFW_OPT_SIGNED_OUTPUT,
	AMDFW_OPT_SIGNED_ADDR,
	AMDFW_OPT_BODY_LOCATION,
	/* begin after ASCII characters */
	LONGOPT_SPI_READ_MODE	= 256,
	LONGOPT_SPI_SPEED	= 257,
	LONGOPT_SPI_MICRON_FLAG	= 258,
	LONGOPT_BIOS_SIG	= 259,
	LONGOPT_NVRAM_BASE	= 260,
	LONGOPT_NVRAM_SIZE	= 261,
	LONGOPT_RPMC_NVRAM_BASE	= 262,
	LONGOPT_RPMC_NVRAM_SIZE	= 263,
};

static const char optstring[] = {AMDFW_OPT_CONFIG, ':',
	AMDFW_OPT_DEBUG, AMDFW_OPT_HELP
};

static struct option long_options[] = {
	{"xhci",             required_argument, 0, AMDFW_OPT_XHCI },
	{"imc",              required_argument, 0, AMDFW_OPT_IMC },
	{"gec",              required_argument, 0, AMDFW_OPT_GEC },
	/* PSP Directory Table items */
	{"recovery-ab",            no_argument, 0, AMDFW_OPT_RECOVERY_AB },
	{"recovery-ab-single-copy", no_argument, 0, AMDFW_OPT_RECOVERY_AB_SINGLE_COPY },
	{"use-combo",              no_argument, 0, AMDFW_OPT_USE_COMBO },
	{"combo-config1",    required_argument, 0, AMDFW_OPT_COMBO1_CONFIG },
	{"multilevel",             no_argument, 0, AMDFW_OPT_MULTILEVEL },
	{"nvram",            required_argument, 0, AMDFW_OPT_NVRAM },
	{"nvram-base",       required_argument, 0, LONGOPT_NVRAM_BASE },
	{"nvram-size",       required_argument, 0, LONGOPT_NVRAM_SIZE },
	{"rpmc-nvram-base",  required_argument, 0, LONGOPT_RPMC_NVRAM_BASE },
	{"rpmc-nvram-size",  required_argument, 0, LONGOPT_RPMC_NVRAM_SIZE },
	{"soft-fuse",        required_argument, 0, AMDFW_OPT_FUSE },
	{"token-unlock",           no_argument, 0, AMDFW_OPT_UNLOCK },
	{"whitelist",        required_argument, 0, AMDFW_OPT_WHITELIST },
	{"use-pspsecureos",        no_argument, 0, AMDFW_OPT_USE_PSPSECUREOS },
	{"load-mp2-fw",            no_argument, 0, AMDFW_OPT_LOAD_MP2FW },
	{"load-s0i3",              no_argument, 0, AMDFW_OPT_LOAD_S0I3 },
	{"spl-table",        required_argument, 0, AMDFW_OPT_SPL_TABLE },
	{"verstage",         required_argument, 0, AMDFW_OPT_VERSTAGE },
	{"verstage_sig",     required_argument, 0, AMDFW_OPT_VERSTAGE_SIG },
	{"output-manifest",  required_argument, 0, AMDFW_OPT_OUTPUT_MANIFEST },
	/* BIOS Directory Table items */
	{"instance",         required_argument, 0, AMDFW_OPT_INSTANCE },
	{"apcb",             required_argument, 0, AMDFW_OPT_APCB },
	{"apcb-combo1",      required_argument, 0, AMDFW_OPT_APCB_COMBO1 },
	{"apob-base",        required_argument, 0, AMDFW_OPT_APOBBASE },
	{"bios-bin",         required_argument, 0, AMDFW_OPT_BIOSBIN },
	{"bios-bin-src",     required_argument, 0, AMDFW_OPT_BIOSBIN_SOURCE },
	{"bios-bin-dest",    required_argument, 0, AMDFW_OPT_BIOSBIN_DEST },
	{"bios-uncomp-size", required_argument, 0, AMDFW_OPT_BIOS_UNCOMP_SIZE },
	{"bios-bin-uncomp",        no_argument, 0, AMDFW_OPT_BIOSBIN_UNCOMP },
	{"bios-sig-size",    required_argument, 0, LONGOPT_BIOS_SIG },
	{"ucode",            required_argument, 0, AMDFW_OPT_UCODE },
	{"apob-nv-base",     required_argument, 0, AMDFW_OPT_APOB_NVBASE },
	{"apob-nv-size",     required_argument, 0, AMDFW_OPT_APOB_NVSIZE },
	/* Embedded Firmware Structure items*/
	{"spi-read-mode",    required_argument, 0, LONGOPT_SPI_READ_MODE },
	{"spi-speed",        required_argument, 0, LONGOPT_SPI_SPEED },
	{"spi-micron-flag",  required_argument, 0, LONGOPT_SPI_MICRON_FLAG },
	{"body-location",     required_argument, 0, AMDFW_OPT_BODY_LOCATION },
	/* other */
	{"output",           required_argument, 0, AMDFW_OPT_OUTPUT },
	{"flashsize",        required_argument, 0, AMDFW_OPT_FLASHSIZE },
	{"location",         required_argument, 0, AMDFW_OPT_LOCATION },
	{"anywhere",         no_argument,       0, AMDFW_OPT_ANYWHERE },
	{"sharedmem",        required_argument, 0, AMDFW_OPT_SHAREDMEM },
	{"sharedmem-size",   required_argument, 0, AMDFW_OPT_SHAREDMEM_SIZE },

	{"signed-output",           required_argument, 0, AMDFW_OPT_SIGNED_OUTPUT },
	{"signed-addr",           required_argument, 0, AMDFW_OPT_SIGNED_ADDR },

	{"config",           required_argument, 0, AMDFW_OPT_CONFIG },
	{"debug",            no_argument,       0, AMDFW_OPT_DEBUG },
	{"help",             no_argument,       0, AMDFW_OPT_HELP },
	{NULL,               0,                 0,  0  }
};

static void usage(void)
{
	printf("amdfwtool: Create AMD Firmware combination\n");
	printf("Usage: amdfwtool [options] --flashsize <size> --output <filename>\n");
	printf("--xhci <FILE>                  Add XHCI blob\n");
	printf("--imc <FILE>                   Add IMC blob\n");
	printf("--gec <FILE>                   Add GEC blob\n");

	printf("\nPSP options:\n");
	printf("--use-combo                    Use the COMBO layout\n");
	printf("--combo-config1 <config file>  Config for 1st combo entry\n");
	printf("--multilevel                   Generate primary and secondary tables\n");
	printf("--nvram <FILE>                 Add nvram binary\n");
	printf("--soft-fuse                    Set soft fuse\n");
	printf("--token-unlock                 Set token unlock\n");
	printf("--nvram-base <HEX_VAL>         Base address of nvram\n");
	printf("--nvram-size <HEX_VAL>         Size of nvram\n");
	printf("--rpmc-nvram-base <HEX_VAL>    Base address of RPMC nvram\n");
	printf("--rpmc-nvram-size <HEX_VAL>    Size of RPMC nvram\n");
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
	printf("--apcb-combo1 <FILE>           Add APCB for 1st combo\n");
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
	printf("--output-manifest <FILE>       Writes a manifest with the blobs versions\n");
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
	printf("-h|--help                      Show this help\n");
}

extern amd_fw_entry amd_psp_fw_table[];
extern amd_bios_entry amd_bios_table[];
extern amd_fw_entry amd_fw_table[];

static void register_amd_psp_fw_addr(amd_fw_type type, int sub,
					char *dst_str, char *size_str)
{
	unsigned int i;

	for (i = 0; amd_psp_fw_table[i].type != AMD_FW_INVALID; i++) {
		if (amd_psp_fw_table[i].type != type)
			continue;

		if (amd_psp_fw_table[i].subprog == sub) {
			if (dst_str)
				amd_psp_fw_table[i].dest = strtoull(dst_str, NULL, 16);
			if (size_str)
				amd_psp_fw_table[i].size = strtoul(size_str, NULL, 16);
			return;
		}
	}
}

static void register_bios_fw_addr(amd_bios_type type, char *src_str,
					char *dst_str, char *size_str)
{
	uint32_t i;
	for (i = 0; amd_bios_table[i].type != AMD_BIOS_INVALID; i++) {
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

static void register_fw_token_unlock(void)
{
	uint32_t i;

	for (i = 0; amd_psp_fw_table[i].type != AMD_FW_INVALID; i++) {
		if (amd_psp_fw_table[i].type != AMD_TOKEN_UNLOCK)
			continue;

		amd_psp_fw_table[i].other = 1;
		return;
	}
}

static void register_fw_filename(amd_fw_type type, uint8_t sub, char filename[])
{
	unsigned int i;

	for (i = 0; amd_fw_table[i].type != AMD_FW_INVALID; i++) {
		if (amd_fw_table[i].type == type) {
			amd_fw_table[i].filename = filename;
			return;
		}
	}

	for (i = 0; amd_psp_fw_table[i].type != AMD_FW_INVALID; i++) {
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

	for (i = 0; amd_bios_table[i].type != AMD_BIOS_INVALID; i++) {
		if (amd_bios_table[i].type == type
					&& amd_bios_table[i].inst == ins
					&& amd_bios_table[i].subpr == sub) {
			amd_bios_table[i].filename = name;
			return;
		}
	}
}

static void register_fw_fuse(char *str)
{
	uint32_t i;

	for (i = 0; amd_psp_fw_table[i].type != AMD_FW_INVALID; i++) {
		if (amd_psp_fw_table[i].type != AMD_PSP_FUSE_CHAIN)
			continue;

		amd_psp_fw_table[i].other = strtoull(str, NULL, 16);
		return;
	}
}

void register_apcb_combo(amd_cb_config *cb_config, int combo_index, context *ctx)
{
	if (ctx->combo_apcb[combo_index].filename != NULL) {
		register_bdt_data(AMD_BIOS_APCB,
			ctx->combo_apcb[combo_index].sub,
			ctx->combo_apcb[combo_index].ins & 0xF,
			ctx->combo_apcb[combo_index].filename);
		if (cb_config->have_apcb_bk)
			register_bdt_data(AMD_BIOS_APCB_BK,
				ctx->combo_apcb_bk[combo_index].sub,
				ctx->combo_apcb_bk[combo_index].ins & 0xF,
				ctx->combo_apcb_bk[combo_index].filename);
	} else {
		/* Use main APCB if no Combo APCB is provided */
		register_bdt_data(AMD_BIOS_APCB, ctx->combo_apcb[0].sub,
			ctx->combo_apcb[0].ins & 0xF, ctx->combo_apcb[0].filename);
		if (cb_config->have_apcb_bk)
			register_bdt_data(AMD_BIOS_APCB_BK,
				ctx->combo_apcb_bk[0].sub,
				ctx->combo_apcb_bk[0].ins & 0xF,
				ctx->combo_apcb_bk[0].filename);
	}
}

int amdfwtool_getopt(int argc, char *argv[], amd_cb_config *cb_config, context *ctx)
{
	int c;
	/* Values cleared after each firmware or parameter, regardless if N/A */
	uint8_t sub = 0, instance = 0;
	char *tmp;
	int retval = 0;
	bool any_location = 0;
	int fuse_defined = 0;

	while (1) {
		int optindex = 0;
		int bios_tbl_index = -1;

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
		case AMDFW_OPT_RECOVERY_AB:
			cb_config->recovery_ab = true;
			break;
		case AMDFW_OPT_RECOVERY_AB_SINGLE_COPY:
			cb_config->recovery_ab = true;
			cb_config->recovery_ab_single_copy = true;
			break;
		case AMDFW_OPT_USE_COMBO:
			cb_config->use_combo = true;
			break;
		case AMDFW_OPT_COMBO1_CONFIG:
			cb_config->use_combo = true;
			/* assert_fw_entry(1, MAX_COMBO_ENTRIES, &ctx); */
			cb_config->combo_config[1] = optarg;
			break;
		case AMDFW_OPT_MULTILEVEL:
			cb_config->multi_level = true;
			break;
		case AMDFW_OPT_UNLOCK:
			register_fw_token_unlock();
			cb_config->unlock_secure = true;
			sub = instance = 0;
			break;
		case AMDFW_OPT_USE_PSPSECUREOS:
			cb_config->use_secureos = true;
			break;
		case AMDFW_OPT_INSTANCE:
			instance = strtoul(optarg, &tmp, 16);
			break;
		case AMDFW_OPT_LOAD_MP2FW:
			cb_config->load_mp2_fw = true;
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
			if ((instance & 0xF0) == 0) {
				register_bdt_data(AMD_BIOS_APCB, sub, instance & 0xF, optarg);
				ctx->combo_apcb[0].filename = optarg;
				ctx->combo_apcb[0].ins = instance;
				ctx->combo_apcb[0].sub = sub;
			} else {
				register_bdt_data(AMD_BIOS_APCB_BK, sub,
					instance & 0xF, optarg);
				ctx->combo_apcb_bk[0].filename = optarg;
				ctx->combo_apcb_bk[0].ins = instance;
				ctx->combo_apcb_bk[0].sub = sub;
				cb_config->have_apcb_bk = 1;
			}
			sub = instance = 0;
			break;
		case AMDFW_OPT_APCB_COMBO1:
			/* assert_fw_entry(1, MAX_COMBO_ENTRIES, &ctx); */
			if ((instance & 0xF0) == 0) {
				ctx->combo_apcb[1].filename = optarg;
				ctx->combo_apcb[1].ins = instance;
				ctx->combo_apcb[1].sub = sub;
			} else {
				ctx->combo_apcb_bk[1].filename = optarg;
				ctx->combo_apcb_bk[1].ins = instance;
				ctx->combo_apcb_bk[1].sub = sub;
				cb_config->have_apcb_bk = 1;
			}
			sub = instance = 0;
			break;
		case AMDFW_OPT_APOBBASE:
			/* APOB destination */
			register_bios_fw_addr(AMD_BIOS_APOB, 0, optarg, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_APOB_NVBASE:
			/* APOB NV source */
			register_bios_fw_addr(AMD_BIOS_APOB_NV, optarg, 0, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_APOB_NVSIZE:
			/* APOB NV size */
			register_bios_fw_addr(AMD_BIOS_APOB_NV, 0, 0, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOSBIN:
			register_bdt_data(AMD_BIOS_BIN, sub, instance, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOSBIN_SOURCE:
			/* BIOS source */
			register_bios_fw_addr(AMD_BIOS_BIN, optarg, 0, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOSBIN_DEST:
			/* BIOS destination */
			register_bios_fw_addr(AMD_BIOS_BIN, 0, optarg, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOS_UNCOMP_SIZE:
			/* BIOS destination size */
			register_bios_fw_addr(AMD_BIOS_BIN, 0, 0, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_BIOSBIN_UNCOMP:
			bios_tbl_index = find_bios_entry(AMD_BIOS_BIN);
			if (bios_tbl_index != -1)
				amd_bios_table[bios_tbl_index].zlib = 0;
			break;
		case LONGOPT_BIOS_SIG:
			/* BIOS signature size */
			register_bios_fw_addr(AMD_BIOS_SIG, 0, 0, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_UCODE:
			register_bdt_data(AMD_BIOS_UCODE, sub,
				instance, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_LOAD_S0I3:
			cb_config->s0i3 = true;
			break;
		case AMDFW_OPT_SPL_TABLE:
			register_fw_filename(AMD_FW_SPL, sub, optarg);
			sub = instance = 0;
			cb_config->have_mb_spl = true;
			break;
		case AMDFW_OPT_WHITELIST:
			register_fw_filename(AMD_FW_PSP_WHITELIST, sub, optarg);
			sub = instance = 0;
			cb_config->have_whitelist = true;
			break;
		case AMDFW_OPT_VERSTAGE:
			register_fw_filename(AMD_FW_PSP_VERSTAGE, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_VERSTAGE_SIG:
			register_fw_filename(AMD_FW_VERSTAGE_SIG, sub, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_OUTPUT_MANIFEST:
			cb_config->manifest_file = optarg;
			break;
		case AMDFW_OPT_SIGNED_OUTPUT:
			cb_config->signed_output_file = optarg;
			sub = instance = 0;
			break;
		case AMDFW_OPT_SIGNED_ADDR:
			cb_config->signed_start_addr = strtoull(optarg, NULL, 16);
			sub = instance = 0;
			break;
		case LONGOPT_SPI_READ_MODE:
			cb_config->efs_spi_readmode = strtoull(optarg, NULL, 16);
			sub = instance = 0;
			break;
		case LONGOPT_SPI_SPEED:
			cb_config->efs_spi_speed = strtoull(optarg, NULL, 16);
			sub = instance = 0;
			break;
		case LONGOPT_SPI_MICRON_FLAG:
			cb_config->efs_spi_micron_flag = strtoull(optarg, NULL, 16);
			sub = instance = 0;
			break;
		case AMDFW_OPT_OUTPUT:
			cb_config->output = optarg;
			break;
		case AMDFW_OPT_FLASHSIZE:
			ctx->rom_size = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				fprintf(stderr, "Error: ROM size specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;
		case AMDFW_OPT_LOCATION:
			cb_config->efs_location = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				fprintf(stderr, "Error: Directory Location specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			if (cb_config->body_location == 0)
				cb_config->body_location = cb_config->efs_location;
			break;
		case AMDFW_OPT_ANYWHERE:
			any_location = 1;
			break;
		case AMDFW_OPT_SHAREDMEM:
			/* shared memory destination */
			register_bios_fw_addr(AMD_BIOS_PSP_SHARED_MEM, 0, optarg, 0);
			sub = instance = 0;
			break;
		case AMDFW_OPT_SHAREDMEM_SIZE:
			/* shared memory size */
			register_bios_fw_addr(AMD_BIOS_PSP_SHARED_MEM, NULL, NULL, optarg);
			sub = instance = 0;
			break;
		case LONGOPT_NVRAM_BASE:
			/* PSP NV base */
			register_amd_psp_fw_addr(AMD_FW_PSP_NVRAM, sub, optarg, 0);
			sub = instance = 0;
			break;
		case LONGOPT_NVRAM_SIZE:
			/* PSP NV size */
			register_amd_psp_fw_addr(AMD_FW_PSP_NVRAM, sub, 0, optarg);
			sub = instance = 0;
			break;
		case LONGOPT_RPMC_NVRAM_BASE:
			/* PSP RPMC NV base */
			register_amd_psp_fw_addr(AMD_RPMC_NVRAM, sub, optarg, 0);
			sub = instance = 0;
			break;
		case LONGOPT_RPMC_NVRAM_SIZE:
			/* PSP RPMC NV size */
			register_amd_psp_fw_addr(AMD_RPMC_NVRAM, sub, 0, optarg);
			sub = instance = 0;
			break;
		case AMDFW_OPT_CONFIG:
			cb_config->config = optarg;
			break;
		case AMDFW_OPT_DEBUG:
			cb_config->debug = 1;
			break;
		case AMDFW_OPT_HELP:
			usage();
			return 1;
		case AMDFW_OPT_BODY_LOCATION:
			cb_config->body_location = (uint32_t)strtoul(optarg, &tmp, 16);
			if (*tmp != '\0') {
				fprintf(stderr, "Error: Body Location specified"
					" incorrectly (%s)\n\n", optarg);
				retval = 1;
			}
			break;

		default:
			break;
		}
	}

	if (!fuse_defined)
		register_fw_fuse(DEFAULT_SOFT_FUSE_CHAIN);

	if (!cb_config->output) {
		fprintf(stderr, "Error: Output value is not specified.\n\n");
		retval = 1;
	}

	if (ctx->rom_size % 1024 != 0) {
		fprintf(stderr, "Error: ROM Size (%d bytes) should be a multiple of"
			" 1024 bytes.\n\n", ctx->rom_size);
		retval = 1;
	}

	if (ctx->rom_size < MIN_ROM_KB * 1024) {
		fprintf(stderr, "Error: ROM Size (%dKB) must be at least %dKB.\n\n",
			ctx->rom_size / 1024, MIN_ROM_KB);
		retval = 1;
	}

	printf("    AMDFWTOOL  Using ROM size of %dKB\n", ctx->rom_size / 1024);

	/* If the flash size is larger than 16M, we assume the given
	   addresses are already relative ones. Otherwise we print error.*/
	if (cb_config->efs_location && cb_config->efs_location > ctx->rom_size) {
		fprintf(stderr, "Error: EFS/Directory location outside of ROM.\n\n");
		return 1;
	}
	if (cb_config->body_location && cb_config->body_location > ctx->rom_size) {
		fprintf(stderr, "Error: Body location outside of ROM.\n\n");
		return 1;
	}

	if (!cb_config->efs_location && cb_config->body_location) {
		fprintf(stderr, "Error AMDFW body location specified without EFS location.\n");
		return 1;
	}

	if (cb_config->body_location != cb_config->efs_location &&
			cb_config->body_location <
				ALIGN(cb_config->efs_location + sizeof(embedded_firmware),
					BLOB_ALIGNMENT)) {
		fprintf(stderr, "Error: Insufficient space between EFS and Blobs.\n");
		fprintf(stderr, "  Require safe spacing of 256 bytes\n");
		return 1;
	}

	if (any_location) {
		if ((cb_config->body_location & 0x3f) || (cb_config->efs_location & 0x3f)) {
			fprintf(stderr, "Error: Invalid Directory/EFS location.\n");
			fprintf(stderr, "  Valid locations are 64-byte aligned\n");
			return 1;
		}
	} else {
		/* efs_location is relative address now. */
		switch (cb_config->efs_location) {
		case 0:
		case 0xFA0000:
		case 0xF20000:
		case 0xE20000:
		case 0xC20000:
		case 0x820000:
		case 0x020000:
			break;
		case 0x7A0000:
		case 0x720000:
		case 0x620000:
		case 0x420000:
			/* Special cases for 8M. */
			if (ctx->rom_size != 0x800000) {
				fprintf(stderr, "Error: Invalid Directory location.\n");
				fprintf(stderr, "%x is only for 8M image size.", cb_config->efs_location);
				return 1;
			}
			break;
		case 0x3A0000:
		case 0x320000:
		case 0x220000:
			/* Special cases for 4M. */
			if (ctx->rom_size != 0x400000) {
				fprintf(stderr, "Error: Invalid Directory location.\n");
				fprintf(stderr, "%x is only for 4M image size.", cb_config->efs_location);
				return 1;
			}
			break;
		default:
			fprintf(stderr, "Error: Invalid Directory location.\n");
			fprintf(stderr, "  Valid locations are 0xFFFA0000, 0xFFF20000,\n");
			fprintf(stderr, "  0xFFE20000, 0xFFC20000, 0xFF820000, 0xFF020000\n");
			fprintf(stderr, "  0xFA0000, 0xF20000, 0xE20000, 0xC20000,\n");
			fprintf(stderr, "  0x820000, 0x020000\n");
			return 1;
		}
	}

	printf("    AMDFWTOOL  Using firmware directory location of address: 0x%08x",
			cb_config->efs_location);
	if (cb_config->body_location != cb_config->efs_location)
		printf(" with a split body at: 0x%08x\n", cb_config->body_location);
	else
		printf("\n");

	if (retval) {
		usage();
		return retval;
	}

	return 0;
}
