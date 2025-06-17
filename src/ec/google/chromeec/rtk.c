/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <stdint.h>

#include "ec.h"

/* For RTK, access ranges 0x800 thru 0x9ff using EMI interface instead of LPC */
#define EMI_RANGE_START EC_HOST_CMD_REGION0
#define EMI_RANGE_END   (EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SIZE)

#define HOSTCMD_PARAM_MEM_BASE CONFIG_EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_BASE
#define ACPI_MEM_BASE          (HOSTCMD_PARAM_MEM_BASE + 0x100)

#define SIO_CONFIG_PORT 0x2e
#define SIO_DATA_PORT   0x2f
#define SIO_LDN         0x07
#define EMI0_LDN        0x06
#define EMI1_LDN        0x07 /* EMI1 LDN */

#define EMI_ADDR3 0xf0 /* The EMI base address 31-24*/
#define EMI_ADDR2 0xf1 /* The EMI base address 23-16*/
#define EMI_ADDR1 0xf2 /* The EMI base address 15-8*/
#define EMI_CTRL  0x30

static bool is_emi_inited;

static inline void sio_write_config(uint8_t reg, uint8_t value)
{
	outb(reg, SIO_CONFIG_PORT);
	outb(value, SIO_DATA_PORT);
}

static void host_emi_init(void)
{
	/*
	 * Due the hardware design, the RTS5915 EMI should be initiated by host sio command,
	 * The EMI range is 256 bytes, chromeec needs two region for host command and ACPI
	 * shared memory.
	 */
	if (is_emi_inited)
		return;

	printk(BIOS_INFO, "RTS5915 EMI: start init ...\n");

	/* Configure the EMI0 to 0xfe0b0000 for host command parameters */
	sio_write_config(SIO_LDN, EMI0_LDN);
	sio_write_config(EMI_ADDR3, HOSTCMD_PARAM_MEM_BASE >> 24 & 0xff);
	sio_write_config(EMI_ADDR2, HOSTCMD_PARAM_MEM_BASE >> 16 & 0xff);
	sio_write_config(EMI_ADDR1, HOSTCMD_PARAM_MEM_BASE >> 8 & 0xff);
	sio_write_config(EMI_CTRL, 0x01); /* Enable EMI */

	/* Configure the EMI1 to 0xfe0b0100 for ACPI shared memory */
	sio_write_config(SIO_LDN, EMI1_LDN);
	sio_write_config(EMI_ADDR3, ACPI_MEM_BASE >> 24 & 0xff);
	sio_write_config(EMI_ADDR2, ACPI_MEM_BASE >> 16 & 0xff);
	sio_write_config(EMI_ADDR1, ACPI_MEM_BASE >> 8 & 0xff);
	sio_write_config(EMI_CTRL, 0x01); /* Enable EMI */

	printk(BIOS_INFO, "RTS5915 EMI: done\n");

	is_emi_inited = true;
}

bool chipset_emi_read_bytes(u16 port, size_t length, u8 *dest, u8 *csum)
{
	size_t i;

	host_emi_init();

	printk(BIOS_DEBUG, "RTS5915: read port 0x%x, size %zu\n", port, length);

	if (port >= EMI_RANGE_START && port <= EMI_RANGE_END) {
		uint8_t *p = (uint8_t *)(HOSTCMD_PARAM_MEM_BASE + (port - EMI_RANGE_START));
		for (i = 0; i < length; ++i) {
			dest[i] = p[i];
			if (csum)
				*csum += dest[i];
		}
		return true;
	}
	return false;
}

bool chipset_emi_write_bytes(u16 port, size_t length, u8 *msg, u8 *csum)
{
	size_t i;

	host_emi_init();

	printk(BIOS_DEBUG, "RTS5915: write port 0x%x, size %zu\n", port, length);

	if (port >= EMI_RANGE_START && port <= EMI_RANGE_END) {
		uint8_t *p = (uint8_t *)(HOSTCMD_PARAM_MEM_BASE + (port - EMI_RANGE_START));
		for (i = 0; i < length; ++i) {
			p[i] = msg[i];
			if (csum)
				*csum += msg[i];
		}
		return true;
	}

	return false;
}
