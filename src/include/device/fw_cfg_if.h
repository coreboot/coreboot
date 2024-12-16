/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * These are the qemu firmware config interface defines and structs.
 * Copied over from qemu source tree,
 * include/standard-headers/linux/qemu_fw_cfg.h and modified accordingly.
 */
#ifndef FW_CFG_IF_H
#define FW_CFG_IF_H

#include <stdint.h>

enum fw_cfg_enum {
	FW_CFG_SIGNATURE,
	FW_CFG_ID,
	FW_CFG_UUID,
	FW_CFG_RAM_SIZE,
	FW_CFG_NOGRAPHIC,
	FW_CFG_NB_CPUS,
	FW_CFG_MACHINE_ID,
	FW_CFG_KERNEL_ADDR,
	FW_CFG_KERNEL_SIZE,
	FW_CFG_KERNEL_CMDLINE,
	FW_CFG_INITRD_ADDR,
	FW_CFG_INITRD_SIZE,
	FW_CFG_BOOT_DEVICE,
	FW_CFG_NUMA,
	FW_CFG_BOOT_MENU,
	FW_CFG_MAX_CPUS,
	FW_CFG_KERNEL_ENTRY,
	FW_CFG_KERNEL_DATA,
	FW_CFG_INITRD_DATA,
	FW_CFG_CMDLINE_ADDR,
	FW_CFG_CMDLINE_SIZE,
	FW_CFG_CMDLINE_DATA,
	FW_CFG_SETUP_ADDR,
	FW_CFG_SETUP_SIZE,
	FW_CFG_SETUP_DATA,
	FW_CFG_FILE_DIR
};

#define FW_CFG_FILE_FIRST       0x20
#define FW_CFG_FILE_SLOTS       0x10
#define FW_CFG_MAX_ENTRY        (FW_CFG_FILE_FIRST+FW_CFG_FILE_SLOTS)

#define FW_CFG_WRITE_CHANNEL    0x4000
#define FW_CFG_ARCH_LOCAL       0x8000
#define FW_CFG_ENTRY_MASK       ~(FW_CFG_WRITE_CHANNEL | FW_CFG_ARCH_LOCAL)

#define FW_CFG_ACPI_TABLES      (FW_CFG_ARCH_LOCAL + 0)
#define FW_CFG_SMBIOS_ENTRIES   (FW_CFG_ARCH_LOCAL + 1)
#define FW_CFG_IRQ0_OVERRIDE    (FW_CFG_ARCH_LOCAL + 2)
#define FW_CFG_E820_TABLE       (FW_CFG_ARCH_LOCAL + 3)
#define FW_CFG_HPET             (FW_CFG_ARCH_LOCAL + 4)

#define FW_CFG_INVALID          0xffff

/* width in bytes of fw_cfg control register */
#define FW_CFG_CTL_SIZE         0x02

/* fw_cfg "file name" is up to 56 characters (including terminating nul) */
#define FW_CFG_MAX_FILE_PATH    56

/* size in bytes of fw_cfg signature */
#define FW_CFG_SIG_SIZE         4

typedef struct FWCfgFile {
    uint32_t  size;        /* file size */
    uint16_t  select;      /* write this to 0x510 to read it */
    uint16_t  reserved;
    char      name[FW_CFG_MAX_FILE_PATH];
} __packed FWCfgFile;

typedef struct FWCfgFiles {
    uint32_t  count;
    FWCfgFile f[];
} __packed FWCfgFiles;

typedef struct FwCfgE820Entry {
    uint64_t address;
    uint64_t length;
    uint32_t type;
} __packed FwCfgE820Entry __attribute((__aligned__(4)));


#define SMBIOS_FIELD_ENTRY 0
#define SMBIOS_TABLE_ENTRY 1

typedef struct FwCfgSmbios {
	uint16_t length;
	uint8_t  headertype;
	uint8_t  tabletype;
	uint16_t fieldoffset;
} __packed FwCfgSmbios;

/* FW_CFG_ID bits */
#define FW_CFG_VERSION          0x01
#define FW_CFG_VERSION_DMA      0x02

/* FW_CFG_DMA_CONTROL bits */
#define FW_CFG_DMA_CTL_ERROR    0x01
#define FW_CFG_DMA_CTL_READ     0x02
#define FW_CFG_DMA_CTL_SKIP     0x04
#define FW_CFG_DMA_CTL_SELECT   0x08
#define FW_CFG_DMA_CTL_WRITE    0x10

#define FW_CFG_DMA_SIGNATURE    0x51454d5520434647ULL /* "QEMU CFG" */

/* Control as first field allows for different structures selected by this
 * field, which might be useful in the future
 */
typedef struct FwCfgDmaAccess {
	uint32_t control;
	uint32_t length;
	uint64_t address;
} __packed FwCfgDmaAccess;

#endif /* FW_CFG_IF_H */
