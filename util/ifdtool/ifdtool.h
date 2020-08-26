/* ifdtool - dump Intel Firmware Descriptor information */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <stdbool.h>
#define IFDTOOL_VERSION "1.2"

enum ifd_version {
	IFD_VERSION_1,
	IFD_VERSION_2,
};

/* port from flashrom */
enum ich_chipset {
	CHIPSET_ICH_UNKNOWN,
	CHIPSET_ICH,
	CHIPSET_ICH2345,
	CHIPSET_ICH6,
	CHIPSET_POULSBO, /* SCH U* */
	CHIPSET_TUNNEL_CREEK, /* Atom E6xx */
	CHIPSET_CENTERTON, /* Atom S1220 S1240 S1260 */
	CHIPSET_ICH7,
	CHIPSET_ICH8,
	CHIPSET_ICH9,
	CHIPSET_ICH10,
	CHIPSET_PCH_UNKNOWN,
	CHIPSET_5_SERIES_IBEX_PEAK,
	CHIPSET_6_SERIES_COUGAR_POINT,
	CHIPSET_7_SERIES_PANTHER_POINT,
	CHIPSET_8_SERIES_LYNX_POINT,
	CHIPSET_BAYTRAIL, /* Actually all with Silvermont architecture:
			   * Bay Trail, Avoton/Rangeley
			   */
	CHIPSET_8_SERIES_LYNX_POINT_LP,
	CHIPSET_8_SERIES_WELLSBURG,
	CHIPSET_9_SERIES_WILDCAT_POINT,
	CHIPSET_9_SERIES_WILDCAT_POINT_LP,
	CHIPSET_N_J_SERIES, /* Gemini Lake: N5xxx, J5xxx, N4xxx, J4xxx */
	CHIPSET_100_200_SERIES_SUNRISE_POINT, /* 6th-7th gen Core i/o (LP) variants */
	CHIPSET_300_400_SERIES_CANNON_ICE_POINT, /* 8th-10th gen Core i/o (LP) variants */
	CHIPSET_500_SERIES_TIGER_POINT, /* 11th gen Core i/o (LP) variants onwards */
	CHIPSET_C620_SERIES_LEWISBURG,
};

enum platform {
	PLATFORM_APL,
	PLATFORM_CNL,
	PLATFORM_GLK,
	PLATFORM_ICL,
	PLATFORM_JSL,
	PLATFORM_SKLKBL,
	PLATFORM_TGL,
	PLATFORM_ADL,
};

#define LAYOUT_LINELEN 80

enum spi_frequency {
	SPI_FREQUENCY_20MHZ = 0,
	SPI_FREQUENCY_33MHZ = 1,
	SPI_FREQUENCY_48MHZ = 2,
	SPI_FREQUENCY_50MHZ_30MHZ = 4,
	SPI_FREQUENCY_17MHZ = 6,
};

enum component_density {
	COMPONENT_DENSITY_512KB = 0,
	COMPONENT_DENSITY_1MB   = 1,
	COMPONENT_DENSITY_2MB   = 2,
	COMPONENT_DENSITY_4MB   = 3,
	COMPONENT_DENSITY_8MB   = 4,
	COMPONENT_DENSITY_16MB  = 5,
	COMPONENT_DENSITY_32MB  = 6,
	COMPONENT_DENSITY_64MB  = 7,
	COMPONENT_DENSITY_UNUSED = 0xf
};

// flash descriptor
typedef struct {
	uint32_t flvalsig;
	uint32_t flmap0;
	uint32_t flmap1;
	uint32_t flmap2;
} __attribute__((packed)) fdbar_t;

// regions
#define MAX_REGIONS 9
#define MAX_REGIONS_OLD 5

enum flash_regions {
	REGION_DESC,
	REGION_BIOS,
	REGION_ME,
	REGION_GBE,
	REGION_PDR,
	REGION_EC = 8,
};

typedef struct {
	uint32_t flreg[MAX_REGIONS];
} __attribute__((packed)) frba_t;

// component section
typedef struct {
	uint32_t flcomp;
	uint32_t flill;
	uint32_t flpb;
} __attribute__((packed)) fcba_t;

// pch strap
#define MAX_PCHSTRP 1024

typedef struct {
	uint32_t pchstrp[MAX_PCHSTRP];
} __attribute__((packed)) fpsba_t;

/*
 * WR / RD bits start at different locations within the flmstr regs, but
 * otherwise have identical meaning.
 */
#define FLMSTR_WR_SHIFT_V1 24
#define FLMSTR_WR_SHIFT_V2 20
#define FLMSTR_RD_SHIFT_V1 16
#define FLMSTR_RD_SHIFT_V2 8

// master
typedef struct {
	uint32_t flmstr1;
	uint32_t flmstr2;
	uint32_t flmstr3;
	uint32_t flmstr4;
	uint32_t flmstr5;
} __attribute__((packed)) fmba_t;

// processor strap
typedef struct {
	uint32_t data[8];
} __attribute__((packed)) fmsba_t;

// ME VSCC
typedef struct {
	uint32_t jid;
	uint32_t vscc;
} vscc_t;

typedef struct {
	// Actual number of entries specified in vtl
	/* FIXME: Rationale for the limit of 8.
	 *        AFAICT it's 127, cf. flashrom's ich_descriptors_tool). */
	vscc_t entry[8];
} vtba_t;

typedef struct {
	int base, limit, size;
} region_t;

struct region_name {
	const char *pretty;
	const char *terse;
	const char *filename;
	const char *fmapname;
};
