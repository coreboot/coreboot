/*
 * This file is part of msrtool.
 *
 * Copyright (c) 2008 Peter Stuge <peter@stuge.se>
 * Copyright (c) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef MSRTOOL_H
#define MSRTOOL_H

#include <stdio.h>
#include <stdint.h>
#if (defined(__MACH__) && defined(__APPLE__))
/* DirectHW is available here: https://www.coreboot.org/DirectHW */
#define __DARWIN__
#include <DirectHW/DirectHW.h>
#endif
#if defined(__FreeBSD__)
#include <sys/ioctl.h>
#include <sys/cpuctl.h>
#endif
#include <pci/pci.h>

#define HEXCHARS "0123456789abcdefABCDEF"

enum {
	MSRTYPE_RDONLY,
	MSRTYPE_RDWR,
	MSRTYPE_WRONLY,
	MSRTYPE_EOT
} MsrTypes;

enum {
	PRESENT_RSVD,
	PRESENT_DEC,
	PRESENT_BIN,
	PRESENT_OCT,
	PRESENT_HEX,
	PRESENT_HEXDEC,
	PRESENT_STR,
} PresentTypes;

struct msr {
	uint32_t hi;
	uint32_t lo;
};

struct msrbitvalues {
	const struct msr value;
	const char *text;
};

struct msrbits {
	const uint8_t start;
	const uint8_t size;
	const char *name;
	const char *desc;
	const uint8_t present;
	const struct msrbitvalues bitval[32];
};

struct msrdef {
	const uint32_t addr;
	const uint8_t type;
	const struct msr resetval;
	const char *symbol;
	const char *desc;
	const struct msrbits bits[65];
};

#define MSR1(lo) { 0, (lo) }
#define MSR2(hi,lo) { (hi), (lo) }

#define BITVAL_EOT .text = NULL
#define BITVAL_ISEOT(bv) (NULL == (bv).text)

#define BITS_EOT .size = 0
#define BITS_ISEOT(b) (0 == (b).size)

#define MSR_EOT .type = MSRTYPE_EOT
#define MSR_ISEOT(m) (MSRTYPE_EOT == (m).type)

#define NOBITS {{ BITVAL_EOT }}
#define RESERVED "RSVD", "Reserved", PRESENT_HEXDEC, NOBITS

#define MAX_CORES 8

typedef enum {
	VENDOR_INTEL = 0x756e6547,
	VENDOR_AMD = 0x68747541,
	VENDOR_CENTAUR = 0x746e6543,
} vendor_t;

struct cpuid_t {
	uint8_t family;
	uint8_t model;
	uint8_t stepping;
	uint8_t ext_family;
	uint8_t ext_model;
	vendor_t vendor;
};

struct targetdef {
	const char *name;
	const char *prettyname;
	int (*probe)(const struct targetdef *target, const struct cpuid_t *id);
	const struct msrdef *msrs;
};

#define TARGET_EOT .name = NULL
#define TARGET_ISEOT(t) (NULL == (t).name)


enum SysModes {
	SYS_RDONLY = 0,
	SYS_WRONLY,
	SYS_RDWR
};

struct sysdef {
	const char *name;
	const char *prettyname;
	int (*probe)(const struct sysdef *system);
	int (*open)(uint8_t cpu, enum SysModes mode);
	int (*close)(uint8_t cpu);
	int (*rdmsr)(uint8_t cpu, uint32_t addr, struct msr *val);
};

#define SYSTEM_EOT .name = NULL
#define SYSTEM_ISEOT(s) (NULL == (s).name)

extern const struct sysdef *sys;

extern uint8_t targets_found;
extern const struct targetdef **targets;

extern uint8_t reserved, verbose, quiet;

extern struct pci_access *pacc;

#define printf_quiet(x...) do { if (!quiet) fprintf(stderr,x); } while(0)
#define printf_verbose(x...) do { if (verbose && !quiet) fprintf(stderr,x); } while(0)

#define SYSERROR(call, addr) do { \
	const struct msrdef *m = findmsrdef(addr); \
	if (m) \
		fprintf(stderr, "%s: " #call "(0x%08x) %s: %s\n", __func__, addr, m->symbol, strerror(errno)); \
	else \
		fprintf(stderr, "%s: " #call "(0x%08x): %s\n", __func__, addr, strerror(errno)); \
} while (0);

/* sys.c */
struct cpuid_t *cpuid(void);
struct pci_dev *pci_dev_find(uint16_t vendor, uint16_t device);

/* msrutils.c */
void hexprint(FILE *f, const struct msr val, const uint8_t bits);
void strprint(FILE *f, const struct msr val, const uint8_t bits);
int msr_eq(const struct msr a, const struct msr b);
struct msr msr_shl(const struct msr a, const uint8_t bits);
struct msr msr_shr(const struct msr a, const uint8_t bits);
void msr_and(struct msr *a, const struct msr b);
const struct msrdef *findmsrdef(const uint32_t addr);
uint32_t msraddrbyname(const char *name);
void dumpmsrdefs(const struct targetdef *t);
int dumpmsrdefsvals(FILE *f, const struct targetdef *t, const uint8_t cpu);
uint8_t str2msr(char *str, struct msr *msr, char **endptr);
void decodemsr(const uint8_t cpu, const uint32_t addr, const struct msr val);
uint8_t diff_msr(FILE *fout, const uint32_t addr, const struct msr a, const struct msr b);



/** system externs **/

/* linux.c */
extern int linux_probe(const struct sysdef *system);
extern int linux_open(uint8_t cpu, enum SysModes mode);
extern int linux_close(uint8_t cpu);
extern int linux_rdmsr(uint8_t cpu, uint32_t addr, struct msr *val);

/* darwin.c */
extern int darwin_probe(const struct sysdef *system);
extern int darwin_open(uint8_t cpu, enum SysModes mode);
extern int darwin_close(uint8_t cpu);
extern int darwin_rdmsr(uint8_t cpu, uint32_t addr, struct msr *val);

/* freebsd.c */
extern int freebsd_probe(const struct sysdef *system);
extern int freebsd_open(uint8_t cpu, enum SysModes mode);
extern int freebsd_close(uint8_t cpu);
extern int freebsd_rdmsr(uint8_t cpu, uint32_t addr, struct msr *val);

/** target externs **/

/* geodegx2.c */
extern int geodegx2_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef geodegx2_msrs[];

/* geodelx.c */
extern int geodelx_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef geodelx_msrs[];

/* cs5536.c */
extern int cs5536_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef cs5536_msrs[];

/* k8.c */
extern int k8_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef k8_msrs[];

/* via_c7.c */
extern int via_c7_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef via_c7_msrs[];

/* intel_pentium3_early.c */
extern int intel_pentium3_early_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_pentium3_early_msrs[];

/* intel_pentium3.c */
extern int intel_pentium3_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_pentium3_msrs[];

/* intel_core1.c */
extern int intel_core1_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_core1_msrs[];

/* intel_core2_early.c */
extern int intel_core2_early_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_core2_early_msrs[];

/* intel_core2_later.c */
extern int intel_core2_later_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_core2_later_msrs[];

/* intel_pentium4_early.c */
extern int intel_pentium4_early_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_pentium4_early_msrs[];

/* intel_pentium4_later.c */
extern int intel_pentium4_later_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_pentium4_later_msrs[];

/* intel_pentium_d.c */
extern int intel_pentium_d_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_pentium_d_msrs[];

/* intel_nehalem.c */
extern int intel_nehalem_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_nehalem_msrs[];

/* intel_atom.c */
extern int intel_atom_probe(const struct targetdef *t, const struct cpuid_t *id);
extern const struct msrdef intel_atom_msrs[];

#endif /* MSRTOOL_H */
