/*
 * Initial LinuxBIOS ACPI Support - headers and defines.
 * 
 * written by Stefan Reinauer <stepan@openbios.org>
 * (C) 2004 SUSE LINUX AG
 *
 * The ACPI table structs are based on the Linux kernel sources.
 * 
 */


#ifndef __ASM_ACPI_H
#define __ASM_ACPI_H

#if HAVE_ACPI_TABLES==1

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define RSDP_NAME               "RSDP"
#define RSDP_SIG                "RSD PTR "  /* RSDT Pointer signature */

/* ACPI 2.0 table RSDP */

typedef struct acpi_rsdp {
	char  signature[8];
	u8    checksum;
	char  oem_id[6];
	u8    revision;
	u32   rsdt_address;
	u32   length;
	u64   xsdt_address;
	u8    ext_checksum;
	u8    reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

/* Generic Address Container */

typedef struct acpi_gen_regaddr {
	u8  space_id;
	u8  bit_width;
	u8  bit_offset;
	u8  resv;
	u32 addrl;
	u32 addrh;
} __attribute__ ((packed)) acpi_addr_t;

/* Generic ACPI Header, provided by (almost) all tables */

typedef struct acpi_table_header         /* ACPI common table header */
{
	char signature [4];          /* ACPI signature (4 ASCII characters) */\
	u32  length;                 /* Length of table, in bytes, including header */\
	u8   revision;               /* ACPI Specification minor version # */\
	u8   checksum;               /* To make sum of entire table == 0 */\
	char oem_id [6];             /* OEM identification */\
	char oem_table_id [8];       /* OEM table identification */\
	u32  oem_revision;           /* OEM revision number */\
	char asl_compiler_id [4];    /* ASL compiler vendor ID */\
	u32  asl_compiler_revision;  /* ASL compiler revision number */
} __attribute__ ((packed)) acpi_header_t;

/* RSDT */

typedef struct acpi_rsdt {
	struct acpi_table_header header;
	u32 entry[8];
} __attribute__ ((packed)) acpi_rsdt_t;

/* HPET TIMERS */

typedef struct acpi_hpet {
	struct acpi_table_header header;
	u32 id;
	struct acpi_gen_regaddr addr;
	u8 number;
	u16 min_tick;
	u8 attributes;
} __attribute__ ((packed)) acpi_hpet_t;


unsigned long write_acpi_tables(unsigned long addr);

#else // HAVE_ACPI_TABLES

#define write_acpi_tables(start) (start)

#endif

#endif
