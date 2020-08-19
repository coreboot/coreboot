/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * EC communication interface for COMPAL ENE932 Embedded Controller.
 */

#ifndef _EC_COMPAL_ENE932_EC_H
#define _EC_COMPAL_ENE932_EC_H

#include <stdint.h>

#define EC_IO 0xfd60 /* Mainboard specific. Could be Kconfig option */
#define EC_IO_HIGH EC_IO + 1
#define EC_IO_LOW  EC_IO + 2
#define EC_IO_DATA EC_IO + 3

/* ENE EC internal address space */
#define REG_SPI_DATA    0xfeab
#define REG_SPI_COMMAND 0xfeac
#define REG_SPI_CONFIG  0xfead
#define CFG_CSn_FORCE_LOW        (1 << 4)
#define CFG_COMMAND_WRITE_ENABLE (1 << 3)
#define CFG_STATUS               (1 << 1)

#define KBD_DATA	0x60
#define KBD_COMMAND	0x64
#define KBD_STATUS	0x64
#define   KBD_IBF	(1 << 1) // 1: input buffer full (data ready for ec)
#define   KBD_OBF	(1 << 0) // 1: output buffer full (data ready for host)

/* Wait 400ms for keyboard controller answers */
#define KBC_TIMEOUT_IN_MS 400

u8 ec_kbc_read_ob(void);
void ec_kbc_write_cmd(u8 cmd);
void ec_kbc_write_ib(u8 data);
int kbc_cleanup_buffers(void);

#endif /* _EC_COMPAL_ENE932_EC_H */
