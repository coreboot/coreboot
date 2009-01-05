/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

#ifndef CK804_SMBUS_H
#define CK804_SMBUS_H

#include <device/smbus_def.h>
#include <io.h>

#define SMBHSTSTAT  0x1
#define SMBHSTPRTCL 0x0
#define SMBHSTCMD   0x3
#define SMBXMITADD  0x2
#define SMBHSTDAT0  0x4
#define SMBHSTDAT1  0x5

/*
 * Between 1-10 seconds, We should never timeout normally.
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT (100 * 1000 * 10)

int do_smbus_write_byte(unsigned smbus_io_base, unsigned device,
			unsigned address, unsigned char val);

int do_smbus_read_byte(unsigned smbus_io_base, unsigned device,
			      unsigned address);

#endif

