#include <string.h>
#include <console/console.h>
#include <arch/io.h>

#include "fw_cfg.h"

/* ----------------------------------------------------------------- */
/* from qemu soure tree, include/hw/nvram/fw_cfg.h */

#define FW_CFG_SIGNATURE        0x00
#define FW_CFG_ID               0x01
#define FW_CFG_UUID             0x02
#define FW_CFG_RAM_SIZE         0x03
#define FW_CFG_NOGRAPHIC        0x04
#define FW_CFG_NB_CPUS          0x05
#define FW_CFG_MACHINE_ID       0x06
#define FW_CFG_KERNEL_ADDR      0x07
#define FW_CFG_KERNEL_SIZE      0x08
#define FW_CFG_KERNEL_CMDLINE   0x09
#define FW_CFG_INITRD_ADDR      0x0a
#define FW_CFG_INITRD_SIZE      0x0b
#define FW_CFG_BOOT_DEVICE      0x0c
#define FW_CFG_NUMA             0x0d
#define FW_CFG_BOOT_MENU        0x0e
#define FW_CFG_MAX_CPUS         0x0f
#define FW_CFG_KERNEL_ENTRY     0x10
#define FW_CFG_KERNEL_DATA      0x11
#define FW_CFG_INITRD_DATA      0x12
#define FW_CFG_CMDLINE_ADDR     0x13
#define FW_CFG_CMDLINE_SIZE     0x14
#define FW_CFG_CMDLINE_DATA     0x15
#define FW_CFG_SETUP_ADDR       0x16
#define FW_CFG_SETUP_SIZE       0x17
#define FW_CFG_SETUP_DATA       0x18
#define FW_CFG_FILE_DIR         0x19

#define FW_CFG_FILE_FIRST       0x20
#define FW_CFG_FILE_SLOTS       0x10
#define FW_CFG_MAX_ENTRY        (FW_CFG_FILE_FIRST+FW_CFG_FILE_SLOTS)

#define FW_CFG_WRITE_CHANNEL    0x4000
#define FW_CFG_ARCH_LOCAL       0x8000
#define FW_CFG_ENTRY_MASK       ~(FW_CFG_WRITE_CHANNEL | FW_CFG_ARCH_LOCAL)

#define FW_CFG_INVALID          0xffff

typedef struct FWCfgFile {
    uint32_t  size;        /* file size */
    uint16_t  select;      /* write this to 0x510 to read it */
    uint16_t  reserved;
    char      name[56];
} FWCfgFile;

typedef struct FWCfgFiles {
    uint32_t  count;
    FWCfgFile f[];
} FWCfgFiles;

/* ----------------------------------------------------------------- */

#define FW_CFG_PORT_CTL       0x0510
#define FW_CFG_PORT_DATA      0x0511

static unsigned char fw_cfg_detected = 0xff;

static int fw_cfg_present(void)
{
	static const char qsig[] = "QEMU";
	unsigned char sig[4];

	if (fw_cfg_detected == 0xff) {
		fw_cfg_get(FW_CFG_SIGNATURE, sig, sizeof(sig));
		fw_cfg_detected = (memcmp(sig, qsig, 4) == 0) ? 1 : 0;
		printk(BIOS_INFO, "QEMU: firmware config interface %s\n",
		       fw_cfg_detected ? "detected" : "not found");
	}
	return fw_cfg_detected;
}

void fw_cfg_get(int entry, void *dst, int dstlen)
{
	outw(entry, FW_CFG_PORT_CTL);
	insb(FW_CFG_PORT_DATA, dst, dstlen);
}

int fw_cfg_max_cpus(void)
{
	unsigned short max_cpus;

	if (!fw_cfg_present())
		return -1;

	fw_cfg_get(FW_CFG_MAX_CPUS, &max_cpus, sizeof(max_cpus));
	return max_cpus;
}
