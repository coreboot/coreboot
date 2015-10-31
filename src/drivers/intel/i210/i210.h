/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _INTEL_I210_H_
#define _INTEL_I210_H_

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

#endif /* _INTEL_I210_H_ */
