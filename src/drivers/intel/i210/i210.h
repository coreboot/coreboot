/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _INTEL_I210_H_
#define _INTEL_I210_H_

#include <types.h>
#include <device/device.h>

#define I210_PCI_MEM_BAR_OFFSET	0x10
#define I210_REG_EECTRL		0x12010	  /* Offset for EEPROM control reg */
#define  I210_FLUPD		0x800000  /* Start flash update bit */
#define  I210_FLUDONE		0x4000000 /* Flash update done indicator */
#define I210_REG_EEREAD		0x12014   /* Offset for EEPROM read reg */
#define I210_REG_EEWRITE	0x12018   /* Offset for EEPROM write reg */
#define  I210_CMDV		0x01      /* command valid bit */
#define  I210_DONE		0x02      /* command done bit */
#define I210_TARGET_CHECKSUM	0xBABA    /* resulting checksum */

/*define some other useful values here */
#define I210_POLL_TIMEOUT_US	300000    /* 300 ms */
/*Define some error states here*/
#define I210_SUCCESS		0x00000000
#define I210_INVALID_PARAM	0x00000001
#define I210_NOT_READY		0x00000002
#define I210_READ_ERROR		0x00000004
#define I210_WRITE_ERROR	0x00000008
#define I210_CHECKSUM_ERROR	0x00000010
#define I210_FLASH_UPDATE_ERROR	0x00000020

#define MAC_ADDR_LEN		6

/* We need one function we can call to get a MAC address to use */
enum cb_err mainboard_get_mac_address(struct device *dev, uint8_t mac[MAC_ADDR_LEN]);

#endif /* _INTEL_I210_H_ */
